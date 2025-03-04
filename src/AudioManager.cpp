#include "AudioManager.hpp"
#include "ui/SongInfoPopup.hpp"
#include <Geode/fmod/fmod_errors.h>
#include <regex>
#include "simdutf.h"
#include "utils.hpp"
#include "log.hpp"
#include "eclipse.hpp"

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
    , m_queueLength(50)
    , m_historyLength(25)
    , m_songs({})
    
    , m_isInEditor(false)
    , m_isEditorAudioPlaying(false)
    , m_isPaused(false)
    , m_isQueueBeingPopulated(false)
    
    , m_playCurrentSongQueuedForLoad(false)
    
    , m_lowPassStrength(0)
    , m_lowPassEasedCutoff(0)
    , m_lowPassFilter(nullptr)
    
    , m_easers({})
    
    , m_gen(std::random_device{}())
    , m_randomSongGenerator(0, 0) {}

void AudioManager::init() {
    m_system->createDSPByType(FMOD_DSP_TYPE_LOWPASS, &m_lowPassFilter);
    m_lowPassFilter->setParameterFloat(FMOD_DSP_LOWPASS_RESONANCE, 0.f);
}

void AudioManager::populateSongs() {
    if (m_isQueueBeingPopulated) return;

    m_isQueueBeingPopulated = true;
    m_queue.clear();
    m_songs.clear();
    m_history.clear();
    if (m_channel) m_channel->stop();
    if (auto pop = SongInfoPopup::get()) pop->close();

    m_preloadUI = PreloadUI::create();
    m_preloadUI->addToSceneAndAnimate();
    std::thread(&AudioManager::populateSongsThread, this).detach();
}

void AudioManager::populateSongsThread() {
    geode::utils::thread::setName("EditorMusic Song Loading");
    em::log::info("Populating songs...");

    geode::log::pushNest();
    setupPreloadUIFromPath(geode::Mod::get()->getConfigDir());
    setupPreloadUIFromPath(geode::Mod::get()->getSettingValue<std::filesystem::path>("extra-songs-path"));
    populateSongsFromPath(geode::Mod::get()->getConfigDir());
    populateSongsFromPath(geode::Mod::get()->getSettingValue<std::filesystem::path>("extra-songs-path"));
    geode::log::popNest();

    geode::Loader::get()->queueInMainThread([this] {
        m_preloadUI->runCompleteAnimationAndRemove();
        m_preloadUI = nullptr;
    });

    em::log::info("Finished populating songs!");

    if (m_songs.size() < 50) m_queueLength = m_songs.size();
    else m_queueLength = 50;

    if (m_songs.size() > 0) m_randomSongGenerator = std::uniform_int_distribution<int>(0, m_songs.size() - 1);

    if (m_isInEditor) {
        checkQueueLength();
        checkSongPreload();
    }

    em::rift_labels::set(em::rift_labels::g_labelNumSongs, (int64_t)m_songs.size());

    m_isQueueBeingPopulated = false;
}

void AudioManager::setupPreloadUIFromPath(std::filesystem::path path) {
    if (!std::filesystem::exists(path)) return;
    em::log::debug("Populating song total from path...");
    for (const auto& file : std::filesystem::directory_iterator(path.string())) {
        if (std::filesystem::is_regular_file(file) && isValidAudioFile(file)) m_preloadUI->m_totalSongs++;
        if (std::filesystem::is_directory(file)) setupPreloadUIFromPath(file);
    }
}

void AudioManager::populateSongsFromPath(std::filesystem::path path) {
    if (!std::filesystem::exists(path)) return;
    em::log::debug("Populating songs from path...");

    for (const auto& file : std::filesystem::directory_iterator(path.string())) {
        if (std::filesystem::is_directory(file)) {
            // stack overflow from the person with 10000 folders inside each other incoming
            populateSongsFromPath(file);
            continue;
        }

        if (!isValidAudioFile(file)) continue;

        auto source = std::make_shared<AudioSource>(file.path());
        populateAudioSourceInfo(source);
        m_songs.push_back(source);
        geode::Loader::get()->queueInMainThread([this, source] {
            m_preloadUI->increment(source->m_name);
        });
    }
}

bool AudioManager::isValidAudioFile(std::filesystem::path path) {
    if (!std::filesystem::is_regular_file(path)) return false;
    
    static const std::array<std::string, 4> validFiles = { ".mp3", ".wav", ".ogg", ".flac" };
    return std::find(validFiles.begin(), validFiles.end(), path.extension()) != validFiles.end();
}

void AudioManager::populateAudioSourceInfo(std::shared_ptr<AudioSource> source) {
    // load song, get info, deload song
    em::log::debug("Populating audio source info...");
    geode::log::pushNest();

    // FMOD_OPENONLY loads it but does not allocate memory for the actual sound data
    // so is great for just reading the metadata
    FMOD::Sound* sound;
    m_system->createSound(source->m_path.string().c_str(), FMOD_OPENONLY, nullptr, &sound);

    // figure out metadata for name and if it exists
    std::map<std::string, std::string> nameExtensionMap = {
        { ".mp3", "TIT2" },
        { ".wav", "INAM" },
        { ".ogg", "TITLE" },
        { ".flac", "TITLE" }
    };
    FMOD_TAG nameTag;
    if (sound->getTag(nameExtensionMap[source->m_path.extension().string()].c_str(), 0, &nameTag) == FMOD_ERR_TAGNOTFOUND) {
        em::log::debug("Using fallback, title not found");
        source->m_name = figureOutFallbackName(source->m_path);
    } else source->m_name = populateStringTag(nameTag, true, source);

    // figure out metadata for artist and if it exists
    std::map<std::string, std::string> artistExtensionMap = {
        { ".mp3", "TPE1" },
        { ".wav", "IART" },
        { ".ogg", "ARTIST" },
        { ".flac", "ARTIST" }
    };
    FMOD_TAG artistTag;
    if (sound->getTag(artistExtensionMap[source->m_path.extension().string()].c_str(), 0, &artistTag) == FMOD_ERR_TAGNOTFOUND) {
        em::log::debug("Using fallback, artist not found");
        source->m_artist = "Unknown";
    } else source->m_artist = populateStringTag(artistTag, false);

    // only mp3 covers supported for now
    FMOD_TAG albumCoverTag;
    if (source->m_path.extension().string() == ".mp3") {
        FMOD_RESULT albumRes = sound->getTag("APIC", 0, &albumCoverTag);
        if (albumRes != FMOD_ERR_TAGNOTFOUND) populateAlbumCover(source, albumCoverTag);
    }

	sound->getLength(&source->m_length, FMOD_TIMEUNIT_MS);

    geode::log::popNest();
    em::log::info("Loaded song info for {}!", source->m_name);
    sound->release();
}

std::string AudioManager::populateStringTag(FMOD_TAG tag, bool useTitleFallback, std::shared_ptr<AudioSource> sourceForFallback) {
    em::log::debug("Populating string tag for {}", tag.name);
    geode::log::NestScope scope;

    // simdutf::autodetect_encoding does exist but there's no point using it
    // because it crashes for me trying to read the BOM or whatever and also
    // since the type is embedded into the tag data anyway as long as
    // whatever's writing the tag doesnt lie about the data type then it should
    // be fine

    // in the utf16 checks it looks like adding one seems to remove the
    // entire bom even though it's two bytes long? or at least it should be
    // but well this works
    // sometimes there's still a null byte even though other songs wont so
    // in filterSongNameThruRegex there's a null byte remover
    switch (tag.datatype) {
        case FMOD_TAGDATATYPE_STRING_UTF16: {
            em::log::debug("Tag is in utf16");
            // subtract 2 for bom and null byte, divide by two to get utf8 length
            int length = tag.datalen / 2 - 1;
            const char16_t* tagDataExceptBOM = (char16_t*)tag.data + 1;

            std::string ret;
            ret.resize(length);
            simdutf::result res = simdutf::convert_utf16le_to_utf8_with_errors(tagDataExceptBOM, length, ret.data());
            if (res.error != simdutf::error_code::SUCCESS) {
                em::log::warn("Conversion failed at char {}", res.count);
                return useTitleFallback ? figureOutFallbackName(sourceForFallback->m_path) : "Unknown";
            }

            return filterNameThruRegex(ret);
        }

        case FMOD_TAGDATATYPE_STRING_UTF16BE: {
            em::log::debug("Tag is in utf16 but big endian (very silly)");
            // subtract 2 for bom and null byte, divide by two to get utf8 length
            int length = tag.datalen / 2 - 1;
            const char16_t* tagDataExceptBOM = (char16_t*)tag.data + 1;

            std::string ret;
            ret.resize(length);
            simdutf::result res = simdutf::convert_utf16be_to_utf8_with_errors(tagDataExceptBOM, length, ret.data());
            if (res.error != simdutf::error_code::SUCCESS) {
                em::log::warn("Conversion failed at char {}", res.count);
                return useTitleFallback ? figureOutFallbackName(sourceForFallback->m_path) : "Unknown";
            }

            return filterNameThruRegex(ret);
        }

        case FMOD_TAGDATATYPE_STRING:
        case FMOD_TAGDATATYPE_STRING_UTF8: {
            em::log::debug("Tag is in utf8");
            return filterNameThruRegex(std::string((char*)tag.data));
        }
        
        default: {
            // uhhhh
            em::log::debug("Tag not in known format, using fallback");
            return useTitleFallback ? figureOutFallbackName(sourceForFallback->m_path) : "Unknown";
        }
    }
}

void AudioManager::populateAlbumCover(std::shared_ptr<AudioSource> source, FMOD_TAG tag) {
    em::log::debug("Reading album cover info...");
    geode::log::NestScope scope;
    // oh baby
    // this assumes reading the tag and everything is fine
    // and only supports mp3 APIC tags for now

    // https://id3.org/id3v2.4.0-frames 4.14 Attached Picture
    char* data = (char*)tag.data;
    int i = 0;

    char textEncoding = data[i++];
    std::string mimeType = std::string((const char*)(data + i)); i += mimeType.length();
    char pictureType = data[i++];
    std::string description;
    if (textEncoding == 0x01 || textEncoding == 0x02) {
        description = "(not read)";
        i += std::char_traits<char16_t>::length((const char16_t*)(data + i)) * 2 + 2;
    } else {
        description = std::string((const char*)(data + i));
        i += description.length();
    }
    if (description.length() == 0) i++; // fixes some of my album covers
    void* imageData = data + i + 1;
    bool imageDataIsURL = false;

    if (mimeType == "-->") {
        // stupid
        imageDataIsURL = true;
    }

    em::log::debug("Album cover info:");
    geode::log::pushNest();
        em::log::debug("Encoding: 0x{:02X}", textEncoding);
        em::log::debug("MIME type: \"{}\"", mimeType);
        em::log::debug("Picture type 0x{:02X}", pictureType);
        em::log::debug("Description: \"{}\"", description);
        em::log::debug("Image data is url: {}", imageDataIsURL);
    geode::log::popNest();

    if (imageDataIsURL) {
        em::log::debug("Album cover is in URL format!");
        return;
    }

    // if there is no image/ prefix, image/ is implied - add it
    if (mimeType.find("image/") != 0) { mimeType = "image/" + mimeType; }

    auto format = em::utils::mimeTypeToFormat(mimeType);

    if (format == cocos2d::CCImage::EImageFormat::kFmtUnKnown) {
        em::log::debug("Album cover is in unsupported format: {}!", mimeType);
        return;
    }

    // finally read it??
    int length = tag.datalen - i - 1; // minus one for null terminator
    auto image = new cocos2d::CCImage;
    // this ignores the last four params if type is a png
    if (!image->initWithImageData(imageData, length, format, 0, 0, 0, 0)) {
        em::log::warn("Error creating CCImage from image data for album cover of {}!", source->m_name);
        delete image;
        return;
    }

    em::log::debug("CCImage created, queue cctexture in main thread...");
    geode::Loader::get()->queueInMainThread([source, image] {
        auto texture = new cocos2d::CCTexture2D;
        if (!texture->initWithImage(image)) {
            em::log::warn("Error creating CCTexture2D from CCImage for album cover of {}!", source->m_name);
            delete texture;
            image->release();
            return;
        }

        image->release();

        em::log::debug("Album cover cctexture loaded for {}!", source->m_name);
        source->m_albumCover = texture;
    });
}

std::string AudioManager::figureOutFallbackName(std::filesystem::path path) {
    // stem is filename without the extension
    return filterNameThruRegex(path.stem().string());
}

std::string AudioManager::filterNameThruRegex(std::string songName) {
    size_t potentialNullByte = songName.find('\0');
    if (potentialNullByte != std::string::npos) songName.erase(potentialNullByte);

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
    if (!shouldAllowAudioFunctions()) return;

    em::log::info("next song...");
    std::shared_ptr<AudioSource> nextSong;

    if (m_queue.size() == 1) nextSong = getCurrentSong();
    else nextSong = m_queue[1];

    m_history.push_back(getCurrentSong());
    if (m_queue.size() > 1) m_queue.pop_front();

    checkQueueLength();
    checkSongPreload();
    startPlayingCurrentSong();
}

void AudioManager::prevSong() {
    if (!shouldAllowAudioFunctions()) return;

    em::log::info("previous song...");
    if (m_history.size() == 0) {
        em::log::info("no songs left");
        geode::Notification::create("No songs left!", geode::NotificationIcon::None, .1f)->show();
        return;
    }

    auto prevSong = m_history.back();
    m_queue.push_front(prevSong);
    m_history.pop_back();

    checkQueueLength();
    checkSongPreload();
    startPlayingCurrentSong();
}

void AudioManager::rewind() {
    if (!shouldAllowAudioFunctions()) return;

    int pos = getCurrentSongPosition();
    if (pos < 12000) {
        prevSong();
        return;
    }

    m_channel->setPosition(pos - 10000, FMOD_TIMEUNIT_MS);
}

void AudioManager::fastForward() {
    if (!shouldAllowAudioFunctions()) return;

    int pos = getCurrentSongPosition();
    int len = getCurrentSongLength();
    if (pos > len - 12000) {
        nextSong();
        return;
    }

    m_channel->setPosition(pos + 10000, FMOD_TIMEUNIT_MS);
}

void AudioManager::togglePause() {
    setPaused(!m_isPaused);
}

void AudioManager::setPaused(bool value) {
    if (!shouldAllowAudioFunctions()) return;
    m_isPaused = value;
    geode::Mod::get()->setSavedValue<bool>("is-paused", m_isPaused);
}

void AudioManager::update(float dt) {
    // if we're waiting for a song to load, check again
    if (m_playCurrentSongQueuedForLoad) startPlayingCurrentSong();

    if (m_isInEditor && !LevelEditorLayer::get()) {
        // probably object toolbox smh
        em::log::warn("potential m_isInEditor desync! prematurely running exitEditor...");
        exitEditor();
        return;
    }

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
        em::log::debug("channel isnt playing but should be, song ended?");
        nextSong();
    }

    if (m_isInEditor) {
        int persistedNodes = geode::SceneManager::get()->getPersistedNodes().size();
        int lowPassStrength = cocos2d::CCScene::get()->getChildrenCount() - persistedNodes - 1;
        if (LevelEditorLayer::get()->getChildByID("EditorPauseLayer")) lowPassStrength++;
        if (cocos2d::CCScene::get()->getChildByID("thesillydoggo.qolmod/QOLModButton")) lowPassStrength--;
        if (cocos2d::CCScene::get()->getChildByID("hjfod.quick-volume-controls/overlay")) lowPassStrength--;
        if (SongInfoPopup::get()) lowPassStrength = 0;

        if (lowPassStrength != m_lowPassStrength && geode::Mod::get()->getSettingValue<bool>("low-pass")) {
            m_lowPassStrength = lowPassStrength;
            updateLowPassFilter();
        }
    }
    
    // our channel id is more than likely one, but there is a certain scenario where
    // it is two, but nobody will ever find it and only I know how so it should be fine
    m_isEditorAudioPlaying = FMODAudioEngine::sharedEngine()->isMusicPlaying(0) || FMODAudioEngine::sharedEngine()->isMusicPlaying(2);
    m_channel->setPaused(!shouldSongBePlaying());
    m_channel->setVolume(geode::Mod::get()->getSettingValue<double>("volume"));
    m_lowPassFilter->setParameterFloat(FMOD_DSP_LOWPASS_CUTOFF, m_lowPassEasedCutoff);

    if (shouldAllowAudioFunctions()) {
        em::rift_labels::set(em::rift_labels::g_labelEditorSongDuration, em::utils::formatTime(getCurrentSongPosition()));
        em::rift_labels::set(em::rift_labels::g_labelEditorSongTimestamp, em::utils::formatTime(getCurrentSongLength()));
    }
}

void AudioManager::startPlayingCurrentSong() {
    em::log::debug("start playing m_queue[0]");
    if (m_channel) m_channel->stop();

    if (!getCurrentSong()->m_hasLoadedAudio) {
        // uh oh
        em::log::debug("uh oh not loaded audio yet, m_playCurrentSongQueuedForLoad = {}", m_playCurrentSongQueuedForLoad);

        getCurrentSong()->loadAudioThreaded();
        m_playCurrentSongQueuedForLoad = true;
        return;
    }

    // now has loaded audio at this point
    m_playCurrentSongQueuedForLoad = false;
    auto ret = m_system->playSound(getCurrentSong()->m_sound, nullptr, m_isPaused, &m_channel); 
    updateLowPassFilter();
    if (ret != FMOD_OK) em::log::warn("FMOD error: {} (0x{:02X})", FMOD_ErrorString(ret), (int)ret);
    em::log::debug("Music time!");

    em::rift_labels::set(em::rift_labels::g_labelCurrentSong, getCurrentSong()->m_name);
    em::rift_labels::set(em::rift_labels::g_labelCurrentSongArtist, getCurrentSong()->m_artist);
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

unsigned int AudioManager::getCurrentSongLength() {
    return getCurrentSong()->m_length;
}


bool AudioManager::shouldSongBePlaying() {
    return !m_isPaused && m_isInEditor && !m_isEditorAudioPlaying && !m_playCurrentSongQueuedForLoad && !m_songs.empty() && !m_isQueueBeingPopulated;
}

bool AudioManager::shouldAllowAudioFunctions() {
    return m_isInEditor && !m_playCurrentSongQueuedForLoad && !m_songs.empty() && !m_isQueueBeingPopulated;
}


std::shared_ptr<AudioSource> AudioManager::getRandomSong() {
    // gets a random song that isnt in the queue currently
    std::shared_ptr<AudioSource> ret;

    do {
        ret = m_songs[m_randomSongGenerator(m_gen)];
    } while (std::find(m_queue.begin(), m_queue.end(), ret) != m_queue.end());

    return ret;
}


void AudioManager::updateLowPassFilter() {
    em::log::debug("updating low pass filter...");

    if (m_lowPassStrength <= 0) {
        em::log::debug("removing low pass filter, m_lowPassStrength={}", m_lowPassStrength);
        m_channel->removeDSP(m_lowPassFilter);
        return;
    }

    // wont work if there already is one
    m_channel->addDSP(0, m_lowPassFilter);
    float cutoff = 1700.f - m_lowPassStrength * 250;
    em::log::debug("setting low pass filter (m_lowPassStrength={}, calculated cutoff={})", m_lowPassStrength, cutoff);
    m_easers.push_back(Easer(&m_lowPassEasedCutoff, m_lowPassEasedCutoff, cutoff, .5f));
}


void AudioManager::enterEditor() {
    m_isInEditor = true;
    if (m_songs.empty()) return;

    m_isPaused = geode::Mod::get()->getSavedValue<bool>("is-paused", false);

    checkQueueLength();
    checkSongPreload();
    startPlayingCurrentSong();
}

void AudioManager::exitEditor() {
    m_isInEditor = false;
    if (m_songs.empty()) return;

    m_channel->stop(); // will cause it to be freed
    m_channel = nullptr;
    m_queue.clear();
    m_history.clear();
}

void AudioManager::checkSongPreload() {
    em::log::debug("Check song preload...");

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

void AudioManager::onCloseGDWindow() {
    em::log::debug("GD window closed, stop music");
    exitEditor();
    // will cause all songs to be deleted and their album covers (caused an
    // opengl memory leak before)
    // not like it really matters though because the game is about to close
    // also like a half dozen or so textures still dont get freed and I dont
    // know why but whatever I guess i dont even need to do this
    m_songs.clear();

    // there's a chance an extra frame gets rendered and SongInfoPopup crashes
    if (auto pop = SongInfoPopup::get()) pop->close();
}
