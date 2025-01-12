#include "AudioManager.hpp"
#include "ui/SongInfoPopup.hpp"
#include <Geode/fmod/fmod_errors.h>
#include <regex>
#include <locale>
#include "simdutf.h"

AudioManager& AudioManager::get() {
    static AudioManager instance;
    return instance;
}

AudioManager::AudioManager() 
    : m_channel(nullptr)
    , m_system(FMODAudioEngine::sharedEngine()->m_system)
    , m_volume(1.f)

    , m_queue({})
    , m_history({})
    , m_queueLength(30)
    , m_historyLength(15)
    , m_songs({})
    , m_hasZeroSongs(false)
    
    , m_isInEditor(false)
    , m_isEditorAudioPlaying(false)
    , m_isPaused(false)
    , m_isQueueBeingPopulated(false)
    
    , m_playCurrentSongQueuedForLoad(false)
    
    , m_lowPassStrength(0)
    , m_lowPassEasedCutoff(0)
    , m_lowPassFilter(nullptr)
    
    , m_easers({}) {}

void AudioManager::init() {
    geode::log::debug("AudioManager::init()");
    populateSongs();
    m_system->createDSPByType(FMOD_DSP_TYPE_LOWPASS, &m_lowPassFilter);
	m_lowPassFilter->setParameterFloat(FMOD_DSP_LOWPASS_RESONANCE, 0.f);

    std::locale::global(std::locale{".utf-8"});
    std::setlocale(LC_ALL, ".utf-8");
}

void AudioManager::populateSongs() {
    m_isQueueBeingPopulated = true;
    m_queue.clear();
    m_songs.clear();
    m_history.clear();
    if (m_channel) m_channel->stop();

    m_preloadUI = PreloadUI::create();
    m_preloadUI->addToSceneAndAnimate();
    std::thread(&AudioManager::populateSongsThread, this).detach();
}

void AudioManager::populateSongsThread() {
    geode::utils::thread::setName("EditorMusic Song Loading");
    geode::log::info("Populating songs...");

    geode::log::pushNest();
    setupPreloadUIFromPath(geode::Mod::get()->getConfigDir());
    setupPreloadUIFromPath(geode::Mod::get()->getSettingValue<std::filesystem::path>("extra-songs-path"));
    populateSongsFromPath(geode::Mod::get()->getConfigDir());
    populateSongsFromPath(geode::Mod::get()->getSettingValue<std::filesystem::path>("extra-songs-path"));
    geode::log::popNest();

    geode::Loader::get()->queueInMainThread([this]{
        m_preloadUI->runCompleteAnimationAndRemove();
        m_preloadUI = nullptr;
    });

    geode::log::info("Finished populating songs!");
    if (m_songs.size() < m_queueLength) m_queueLength = m_songs.size();
    m_isQueueBeingPopulated = false;
}

void AudioManager::setupPreloadUIFromPath(std::filesystem::path path) {
    if (!std::filesystem::exists(path)) return;
    geode::log::debug("Populating song total from path...");
    for (const auto& file : std::filesystem::directory_iterator(path.string())) {
        if (std::filesystem::is_regular_file(file)) m_preloadUI->m_totalSongs++;
        if (std::filesystem::is_directory(file)) setupPreloadUIFromPath(file);
    }
}

void AudioManager::populateSongsFromPath(std::filesystem::path path) {
    if (!std::filesystem::exists(path)) return;
    geode::log::debug("Populating songs from path...");

    for (const auto& file : std::filesystem::directory_iterator(path.string())) {
        if (std::filesystem::is_directory(file)) {
            // stack overflow from the person with 10000 folders inside each other incoming
            populateSongsFromPath(file);
            continue;
        }

        auto source = std::make_shared<AudioSource>(file.path());
        populateAudioSourceInfo(source);
        m_songs.push_back(source);
        geode::Loader::get()->queueInMainThread([this, source]{
            m_preloadUI->increment(source->m_name);
        });
	}
}

void AudioManager::populateAudioSourceInfo(std::shared_ptr<AudioSource> source) {
    // load song, get info, deload song
    geode::log::debug("Populating audio source info...");
    geode::log::pushNest();

    // FMOD_OPENONLY loads it but does not allocate memory for the actual sound data
    // so is great for just reading the metadata
    FMOD::Sound* sound;
	m_system->createSound(source->m_path.string().c_str(), FMOD_OPENONLY, nullptr, &sound);

    // try catch doesnt work on android sometimes but might as well try
    try {

    // figure out the metadata tag and if it exists
    FMOD_TAG nameTag;
    std::map<std::string, std::string> nameExtensionMap = {
        { ".mp3", "TIT2" },
        { ".wav", "INAM" },
        { ".ogg", "TITLE" },
        { ".flac", "TITLE" }
    };
    FMOD_RESULT nameRes = sound->getTag(nameExtensionMap[source->m_path.extension().string()].c_str(), 0, &nameTag);

    // ... if it doesnt exist
    if (nameRes == FMOD_ERR_TAGNOTFOUND) {
        geode::log::debug("Using fallback, metadata not found");
        source->m_name = figureOutFallbackName(source->m_path);
    } else {
        geode::log::debug("Song has a name in metadata");

        // simdutf::autodetect_encoding does exist but there's no point using it
        // because it crashes for me trying to read the BOM or whatever and also
        // since the type is embedded into the tag data anyway as long as
        // whatever's writing the tag doesnt lie about the data type then it
        // should be fine

        // in the utf16 checks it looks like adding one seems to remove the
        // entire bom even though it's two bytes long? or at least it should be
        // but well this works
        // sometimes there's still a null byte even though other songs wont so
        // in filterSongNameThruRegex there's a null byte remover
        std::string songName;
        switch (nameTag.datatype) {
            case FMOD_TAGDATATYPE_STRING_UTF16: {
                geode::log::debug("Song name is in utf16");
                // subtract 2 for bom and null byte, divide by two to get utf8 length
                int length = nameTag.datalen / 2 - 1;
                songName.resize(length);
                const char16_t* tagDataExceptBOM = (char16_t*)nameTag.data + 1;

                simdutf::result res = simdutf::convert_utf16le_to_utf8_with_errors(tagDataExceptBOM, length, songName.data());
                if (res.error != simdutf::error_code::SUCCESS) {
                    geode::log::warn("Conversion failed at char {}", res.count);
                    songName = figureOutFallbackName(source->m_path);
                }

                break;
            }

            case FMOD_TAGDATATYPE_STRING_UTF16BE: {
                geode::log::debug("Song name is in utf16 but big endian (very silly)");
                // subtract 2 for bom and null byte, divide by two to get utf8 length
                int length = nameTag.datalen / 2 - 1;
                songName.resize(length);
                const char16_t* tagDataExceptBOM = (char16_t*)nameTag.data + 1;

                simdutf::result res = simdutf::convert_utf16be_to_utf8_with_errors(tagDataExceptBOM, length, songName.data());
                if (res.error != simdutf::error_code::SUCCESS) {
                    geode::log::warn("Conversion failed at char {}", res.count);
                    songName = figureOutFallbackName(source->m_path);
                }

                break;
            }

            case FMOD_TAGDATATYPE_STRING:
            case FMOD_TAGDATATYPE_STRING_UTF8: {
                geode::log::debug("Song name is in utf8");
                songName = std::string((char*)nameTag.data);
                break;
            }
            
            default: {
                // uhhhh
                geode::log::debug("Song name not in known format, using fallback");
                songName = figureOutFallbackName(source->m_path);
            }
        }

        source->m_name = filterNameThruRegex(songName);
    }

    } catch(...) {
        geode::log::warn("Song name failed to be read, but caught");
        source->m_name = figureOutFallbackName(source->m_path);
    }

    // only mp3 supported for now
    FMOD_TAG albumCoverTag;
    if (source->m_path.extension().string() == ".mp3") {
        FMOD_RESULT albumRes = sound->getTag("APIC", 0, &albumCoverTag);
        if (albumRes != FMOD_ERR_TAGNOTFOUND) populateAlbumCover(source, albumCoverTag);
    }

	sound->getLength(&source->m_length, FMOD_TIMEUNIT_MS);

    geode::log::popNest();
    geode::log::info("Loaded song info for {}!", source->m_name);
    sound->release();
}

void AudioManager::populateAlbumCover(std::shared_ptr<AudioSource> source, FMOD_TAG tag) {
    geode::log::debug("Reading album cover info...");
    geode::log::NestScope scope();
    // oh baby
    // this assumes reading the tag and everything is fine
    // and only supports mp3 APIC tags for now

    // https://id3.org/id3v2.4.0-frames 4.14 Attached Picture
    char* data = (char*)tag.data;
    int i = 0;

    char textEncoding = data[i++];
    std::string mimeType = std::string((const char*)(data + i)); i += mimeType.length();
    char pictureType = data[i++];
    std::string description = std::string((const char*)(data + i)); i += description.length();
    char* imageData = data + i;
    bool imageDataIsURL = false;

    if (mimeType == "-->") {
        // stupid
        imageDataIsURL = true;
    }

    geode::log::debug("Album cover info:");
    geode::log::pushNest();
        geode::log::debug("Encoding: 0x{:02X}", textEncoding);
        geode::log::debug("MIME type: \"{}\"", mimeType);
        geode::log::debug("Picture type 0x{:02X}", pictureType);
        geode::log::debug("Description: \"{}\"", mimeType);
        geode::log::debug("Image data is url: {}", imageDataIsURL);
    geode::log::popNest();

    if (!imageDataIsURL && mimeType.find("image/") != 0) { mimeType = "image/" + mimeType; }

    if (mimeType != "image/png") {
        geode::log::debug("Album cover is in unsupported format: {}!", mimeType);
        return;
    }

    if (imageDataIsURL) {
        geode::log::debug("Album cover is in URL format!");
        return;
    }

    // finally read it??
    int length = tag.datalen - i;
    auto image = new cocos2d::CCImage;
    // this ignores the last four params if type is a png
    bool ret = image->initWithImageData(imageData, length, cocos2d::CCImage::EImageFormat::kFmtPng, 0, 0, 0, 0);
    
    if (!ret) {
        geode::log::debug("Error creating CCImage from image data!");
        delete image;
        return;
    }

    geode::Loader::get()->queueInMainThread([source, image]{
        auto texture = new cocos2d::CCTexture2D;
        if (!texture->initWithImage(image)) {
            geode::log::info("Error creating CCTexture2D from CCImage!");
            delete texture;
            return;
        }

        source->m_albumCover = texture;
    });
}

std::string AudioManager::figureOutFallbackName(std::filesystem::path path) {
    // stem is filename without the extension
    return filterNameThruRegex(path.stem().string());
}

std::string AudioManager::filterNameThruRegex(std::string songName) {
    size_t potentialNullByte = songName.find('\0');
    if (potentialNullByte != std::string::npos) {
        songName.erase(potentialNullByte);
    }

    // regex of chars that are in bigFont.fnt
    // any character from space to ~ and also bullet point (u2022)
    // this matches any character that isnt that and replaces it with a question mark
    return std::regex_replace(songName, std::regex("[^ -~•]+"), "?");
}


// this is false advertising it also checks history length
void AudioManager::checkQueueLength() {
    while (m_queue.size() < m_queueLength) {
        // get random song and shove on the end
        m_queue.push_back(getRandomSong());
    }

    while (m_history.size() > m_historyLength) {
        m_history.pop_front();
    }
}

void AudioManager::nextSong() {
    geode::log::info("next song...");
    std::shared_ptr<AudioSource> nextSong;

    if (m_queue.size() == 1) nextSong = getCurrentSong();
    else nextSong = m_queue[1];

    goToSongFromQueue(nextSong);
}

void AudioManager::prevSong() {
    geode::log::info("previous song...");

    auto prevSong = m_history.back();
    m_history.pop_back();
    goToSongFromHistory(prevSong);
}


void AudioManager::goToSongFromQueue(std::shared_ptr<AudioSource> source) {
    geode::log::info("going to song {} (from queue)", source->m_name);
    
    // get current song and put in history
    m_history.push_back(source);
    
    // then pop every other song before it so that it becomes the new song 0 in
    // m_queue

    auto iterator = std::find(m_queue.begin(), m_queue.end(), source);
    if (iterator != m_queue.end()) {
        for (auto i = m_queue.begin(); i != iterator; ++i) {
            m_history.push_back(source);
            m_queue.pop_front();
        }
    }

    // and repopulate queue
    checkQueueLength();
    checkSongPreload();
    startPlayingCurrentSong();
}

void AudioManager::goToSongAndRemakeQueue(std::shared_ptr<AudioSource> source) {
    geode::log::debug("going to song {} (remake queue)", source->m_name);

    // just go to it
    // this is for if you click on a song in the entire playlist
    // so clear everything, plonk this at the start of m_queue and remake everything

    m_history.clear();
    m_queue.clear();
    m_queue.push_back(source);
    checkQueueLength();
    checkSongPreload();
    startPlayingCurrentSong();
}

void AudioManager::goToSongFromHistory(std::shared_ptr<AudioSource> source) {
    geode::log::debug("going to song {} (from history)", source->m_name);

    // go to it but this is a song in history
    // so DONT clear history and DONT remake queue
    // just shove this at the start and let checkQueueLength remove the last one
    // DONT add the current song to history either

    m_queue.push_front(source);
    checkQueueLength();
    checkSongPreload();
    startPlayingCurrentSong();
}


void AudioManager::update(float dt) {
    // if we're waiting for a song to load, check again
    if (m_playCurrentSongQueuedForLoad) startPlayingCurrentSong();

    // update easings
    std::vector<Easer*> easersToRemove = {};
    for (auto& easer : m_easers) {
        easer.update(dt);
        if (easer.m_finished) easersToRemove.push_back(&easer);
    }
    for (auto* easer : easersToRemove) { m_easers.erase(std::remove(m_easers.begin(), m_easers.end(), *easer), m_easers.end()); }

    bool channelIsPlaying = false;
    m_channel->isPlaying(&channelIsPlaying);
    if (!channelIsPlaying && shouldSongBePlaying()) {
        geode::log::debug("channel isnt playing but should be, song ended?");
        nextSong();
    }

    if (m_isInEditor) {
        int persistedNodes = geode::SceneManager::get()->getPersistedNodes().size();
        int lowPassStrength = cocos2d::CCScene::get()->getChildrenCount() - persistedNodes - 1;
        if (LevelEditorLayer::get()->getChildByID("EditorPauseLayer")) lowPassStrength++;
        if (cocos2d::CCScene::get()->getChildByID("thesillydoggo.qolmod/QOLModButton")) lowPassStrength--;
        if (cocos2d::CCScene::get()->getChildByType<SongInfoPopup>(0)) lowPassStrength = 0;

        if (lowPassStrength != m_lowPassStrength && geode::Mod::get()->getSettingValue<bool>("low-pass")) {
            m_lowPassStrength = lowPassStrength;
            updateLowPassFilter();
        }
    }
    
    m_channel->setPaused(!shouldSongBePlaying());
    m_channel->setVolume(geode::Mod::get()->getSettingValue<double>("volume"));
    m_lowPassFilter->setParameterFloat(FMOD_DSP_LOWPASS_CUTOFF, m_lowPassEasedCutoff);
}

void AudioManager::startPlayingCurrentSong() {
    geode::log::debug("start playing m_queue[0]");
    if (m_channel) m_channel->stop();

    if (!getCurrentSong()->m_hasLoadedAudio) {
        // uh oh
        geode::log::debug("uh oh not loaded audio yet, m_playCurrentSongQueuedForLoad = {}", m_playCurrentSongQueuedForLoad);

        getCurrentSong()->loadAudioThreaded();
        m_playCurrentSongQueuedForLoad = true;
        return;
    }

    // now has loaded audio at this point
    m_playCurrentSongQueuedForLoad = false;
    auto ret = m_system->playSound(getCurrentSong()->m_sound, nullptr, false, &m_channel); 
    updateLowPassFilter();
    if (ret != FMOD_OK) geode::log::warn("FMOD error: {} (0x{:02X})", FMOD_ErrorString(ret), (int)ret);
    geode::log::debug("Music time!");
}

std::shared_ptr<AudioSource> AudioManager::getCurrentSong() {
    if (m_queue.empty()) return m_songs.front(); // fallback
    return m_queue.front();
}

unsigned int AudioManager::getCurrentSongPosition() {
    unsigned int curPos;
	m_channel->getPosition(&curPos, FMOD_TIMEUNIT_MS);
	return curPos;
}


bool AudioManager::shouldSongBePlaying() {
    return !m_isPaused && m_isInEditor && !m_isEditorAudioPlaying && !m_playCurrentSongQueuedForLoad && m_songs.size() != 0;
}


std::shared_ptr<AudioSource> AudioManager::getRandomSong() {
    // gets a random song that isnt in the queue currently
    std::shared_ptr<AudioSource> ret;

    do {
        ret = m_songs[rand() % m_songs.size()];
    } while (std::find(m_queue.begin(), m_queue.end(), ret) != m_queue.end());

    return ret;
}


void AudioManager::updateLowPassFilter() {
    geode::log::debug("updating low pass filter...");

    if (m_lowPassStrength <= 0) {
        geode::log::debug("removing low pass filter, m_lowPassStrength={}", m_lowPassStrength);
        m_channel->removeDSP(m_lowPassFilter);
        return;
    }

    // wont work if there already is one
    m_channel->addDSP(0, m_lowPassFilter);
    float cutoff = 1700.f - m_lowPassStrength * 250;
    geode::log::debug("setting low pass filter (m_lowPassStrength={}, calculated cutoff={})", m_lowPassStrength, cutoff);
    m_easers.push_back(Easer(&m_lowPassEasedCutoff, m_lowPassEasedCutoff, cutoff, .5f));
}


void AudioManager::enterEditor() {
    m_isInEditor = true;
    if (m_songs.size() == 0) return;
    
    checkQueueLength();
    startPlayingCurrentSong();
}

void AudioManager::exitEditor() {
    m_isInEditor = false;
    if (m_songs.size() == 0) return;

    m_channel->stop(); // will cause it to be freed
    m_channel = nullptr;
    m_queue.clear();
    m_history.clear();
}

void AudioManager::checkSongPreload() {
    geode::log::debug("Check song preload...");

    // go through all songs and see...
    for (auto song : m_songs) {
        // check if it's either in the first three songs in the queue or first
        // three songs in history, if so it should be loaded
        // ternary used for readability
        // just dont worry about this it's simple and there's not a similarly
        // performant way to rewrite this without it being massive
        bool shouldBeLoaded = \
            (m_queue.size() < 3 ? true : (m_queue[0] == song || m_queue[1] == song || m_queue[2] == song)) ||
            (m_history.size() < 3 ? true : (m_history[0] == song || m_history[1] == song || m_history[2] == song));

        // see if we need to delete or load the sound
        if (song->m_hasLoadedAudio && !shouldBeLoaded) {
            // delete
            song->m_sound->release();
            song->m_sound = nullptr;
            song->m_hasLoadedAudio = false;
        } else if (!song->m_hasLoadedAudio && shouldBeLoaded) {
            // load
            song->loadAudioThreaded();
        }
    }
}
