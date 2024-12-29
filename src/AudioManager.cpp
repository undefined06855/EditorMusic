#include "AudioManager.hpp"
#include <codecvt>
#include <regex>
#include "Ease.hpp"

AudioManager& AudioManager::get() {
	static AudioManager instance;
	return instance;
}

void AudioManager::setupFromOneFolder(std::filesystem::path path) {
	for (const auto& file : std::filesystem::directory_iterator(path.string())) {
		std::filesystem::path path = file.path();

		if (this->extensions.contains(path.extension().string()) && std::filesystem::is_regular_file(path)) {
			UnloadedAudio song;
			song.path = path;
			this->songs.push_back(song);
		} else {
			log::warn("Unsupported file extension or folder found in config dir: {} (from {})", path.extension().string(), path.filename().string());
		}
	}
}

void AudioManager::setup() {
	this->setupFromOneFolder(Mod::get()->getConfigDir());
	std::filesystem::path customPath = Mod::get()->getSettingValue<std::filesystem::path>("extra-songs-path");

	if (customPath.string() != "(none)") {
		if (std::filesystem::exists(customPath)) {
			log::info("Custom path exists! Loading from it...");
			this->setupFromOneFolder(customPath);
		} else {
			log::warn("Custom path doesn't exist!");
			this->customPathDoesntExist = true;
		}
	}

	log::info("Sounds created! songs={}", this->songs.size());

	if (this->songs.size() == 0) {
		// oh there's no songs, disable everything
		this->hasNoSongs = true;
		return;
	}

	this->songID = rand() % this->songs.size();

	this->channel->setMode(FMOD_LOOP_OFF);

	this->system->createDSPByType(FMOD_DSP_TYPE_LOWPASS, &this->lowPassFilterDSP);
	this->lowPassFilterDSP->setParameterFloat(FMOD_DSP_LOWPASS_RESONANCE, 0.f);
}

void AudioManager::tick(float dt) {
	if (this->hasNoSongs) return;

	// set playtestmusicisplaying
	this->playtestMusicIsPlaying = FMODAudioEngine::sharedEngine()->isMusicPlaying(0);
	//log::info("{}", this->playtestMusicIsPlaying);
	// pause channel if needed
	this->channel->setPaused(this->isPaused || this->playtestMusicIsPlaying || !this->isInEditor);
	this->channel->setVolume(Mod::get()->getSettingValue<double>("volume") * this->volume);

	//log::info("low pass: {}/{}, paused: {}, playtesting: {}, notInEditor: {}", this->lowPassStrength, this->actualLowPassCutoff, this->isPaused, this->playtestMusicIsPlaying, !this->isInEditor);

	if (!this->isInEditor) return;

	// check for low pass filter
	int nodesAlreadyPresent = SceneManager::get()->getPersistedNodes().size();
	int lowPassStrengthBefore = this->lowPassStrength;
	this->lowPassStrength = CCScene::get()->getChildrenCount() - nodesAlreadyPresent;
	if (this->lowPassStrength < 0) this->lowPassStrength = 0;
	if (LevelEditorLayer::get()->getChildByID("EditorPauseLayer")) {
		this->lowPassStrength++;
	}

	// and add filter or remove if needed
	if (this->lowPassStrength != lowPassStrengthBefore) {
		this->updateLowPassFilter();
	}
	this->lowPassFilterDSP->setParameterFloat(FMOD_DSP_LOWPASS_CUTOFF, this->actualLowPassCutoff);


	// check if needs to go to next song
	bool channelIsPlaying;
	this->channel->isPlaying(&channelIsPlaying);
	// so if it's not playing and not paused (i.e it should be playing)...
	if (!channelIsPlaying && !this->isPaused) {
		log::info("song is probably finished");
		this->playNewSong();
	}
}

std::string AudioManager::figureOutFallbackName(UnloadedAudio unloadedSong) {
	std::string songName;
	if (Mod::get()->getSettingValue<bool>("unnamed-song-fallback")) songName = unloadedSong.path.filename().string();
	else songName = Mod::get()->getSettingValue<std::string>("unnamed-song-fallback-custom");
	return songName;
}

void AudioManager::playSongID(int id) {
	// filter for stuff like the BOM or whatever goofy unicode characters artists put in the song titles (WHY DOES MDK PUT A MUSIC NOTE?? IT'S NOT NEEDED)
	std::regex charsInBigFont("[^\u0020\u0021\u0022\u0023\u0024\u0025\u0026\u0027\u0028\u0029\u002a\u002b\u002c\u002d\u002e\u002f\u0030\u0031\u0032\u0033\u0034\u0035\u0036\u0037\u0038\u0039\u003a\u003b\u003c\u003d\u003e\u003f\u0040\u0041\u0042\u0043\u0044\u0045\u0046\u0047\u0048\u0049\u004a\u004b\u004c\u004d\u004e\u004f\u0050\u0051\u0052\u0053\u0054\u0055\u0056\u0057\u0058\u0059\u005a\u005b\u005c\u005d\u005e\u005f\u0060\u0061\u0062\u0063\u0064\u0065\u0066\u0067\u0068\u0069\u006a\u006b\u006c\u006d\u006e\u006f\u0070\u0071\u0072\u0073\u0074\u0075\u0076\u0077\u0078\u0079\u007a\u007b\u007c\u007d\u007e\u2022]+");

	log::info("playing song with id {}", id);
	this->songID = id;
	this->history.push_back(id);
	auto usong = this->songs.at(id); // usong means UnloadedSong*

	// load the song
	log::info("loading song...");
	this->system->createSound(usong.path.string().c_str(), FMOD_LOOP_NORMAL, nullptr, &this->song.sound);
	this->song.sound->setLoopCount(0);

	// figure out the metadata tag and if it exists
	log::info("figuring out metadata...");
	FMOD_TAG tag;
	FMOD_RESULT res;
	std::string tagName;
	std::string extension = usong.path.extension().string();
	if (extension == ".mp3") {
		tagName = "TIT2";
	} else if (extension == ".wav") {
		tagName = "INAM";
	} else if (extension == ".ogg" || extension == ".flac") {
		tagName = "TITLE";
	}
	res = this->song.sound->getTag(tagName.c_str(), 0, &tag);

	// ... if it doesnt exist:
	log::info("checking existence...");
	if (res == FMOD_ERR_TAGNOTFOUND) {
		log::warn("Name tag not found for song, using fallback");

		this->song.name = this->figureOutFallbackName(usong);
		log::info("Loaded song {}!", this->song.name);
	}

	log::debug("Song has a name in metadata");

	// get the song name from metadata
	std::string songName;
	const char* songNameAsChar = reinterpret_cast<const char*>(tag.data);

	switch (tag.datatype) {
		case FMOD_TAGDATATYPE_STRING_UTF16: {
			log::debug("Song name is in utf16");
			std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
			songName = converter.to_bytes(reinterpret_cast<const char16_t*>(songNameAsChar));
			break;
		}

		case FMOD_TAGDATATYPE_STRING_UTF16BE: {
			log::debug("Song name is in utf16 but big endian (very silly)");
			// silly big endian
			std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
			std::u16string utf16String(reinterpret_cast<const char16_t*>(songNameAsChar), tag.datalen / sizeof(char16_t));
			for (size_t i = 0; i < utf16String.size(); ++i) {
				utf16String[i] = (utf16String[i] << 8) | (utf16String[i] >> 8);
			}
			songName = converter.to_bytes(utf16String);
			break;
		}

		case FMOD_TAGDATATYPE_STRING:
			log::debug("uhh i think this is in utf8");
		case FMOD_TAGDATATYPE_STRING_UTF8:
			log::debug("Song name is in utf8 (thanks)");
			log::debug("raw: {}, length: (datalen:{}, strlen:{})", songNameAsChar, tag.datalen, strlen(songNameAsChar));
			songName = std::string(songNameAsChar, tag.datalen);
			break;
		default:
			// uhhhh
			log::info("wtf the song name isnt a string (using fallback)");
			songName = this->figureOutFallbackName(usong);
	}

	this->song.name = songName;
	log::info("Loaded song {}!", songName);

	// set length
	this->song.sound->getLength(&this->song.length, FMOD_TIMEUNIT_MS);
	log::info("curSongLength: {}", this->song.length);

	// play the actual sound
	this->channel->stop(); // stop current sound
	this->system->playSound(this->song.sound, nullptr, false, &(this->channel));
	this->updateLowPassFilter(); // gets removed after playSound for whatever reason

	// replace invalid chars
	log::debug("Song name before: {}", this->song.name);
	this->song.name = std::regex_replace(this->song.name, charsInBigFont, Mod::get()->getSettingValue<std::string>("unsupported-characters-fallback"));
	log::debug("Song name after: {}", this->song.name);

	new Ease(0.f, 1.f, 0.5f, &this->volume);
}

// this calls AudioManager::playSongID (with a random id)
void AudioManager::playNewSong() {
	if (this->hasNoSongs) return;

	// im sure this will be fine, right?
	int id;
	if (this->songs.size() == 1) id = 0;
	else do {
		id = rand() % this->songs.size();
	} while (id == this->songID);

	this->playSongID(id);
}

void AudioManager::nextSong() {
	if (this->hasNoSongs) return;
	new Ease(1.f, 0.f, .5f, &this->volume, [this]() {
		this->playNewSong();
	});
}

void AudioManager::prevSong() {
	if (this->hasNoSongs) return;
	log::info("size: {}", this->history.size());
	if (this->history.size() <= 1) return; // this looks wrong but it's not dw

	new Ease(1.f, 0.f, .5f, &this->volume, [this]() {
		this->history.pop_back();
		this->playSongID(this->history.back());
		this->history.pop_back(); // call pop_back again bc previous song gets added again in this->playSongID
	});
}

void AudioManager::updateLowPassFilter() {
	if (!Mod::get()->getSettingValue<bool>("low-pass")) {
		this->channel->removeDSP(this->lowPassFilterDSP);
		this->actualLowPassCutoff = 1650.f;
		return;
	}
	log::info("updating low pass");

	if (this->lowPassStrength > 1) {
		// add it
		log::info("adding filter");
		this->channel->addDSP(0, this->lowPassFilterDSP);
		new Ease(this->actualLowPassCutoff, 1650.f - this->lowPassStrength * 250, .5f, &this->actualLowPassCutoff);
	} else {
		log::info("removing filter");
		// remove it
		this->channel->removeDSP(this->lowPassFilterDSP);
		this->actualLowPassCutoff = 1650.f;
	}
}

// used for sliders
float AudioManager::getSongPercentage() {
	unsigned int curPos;
	this->channel->getPosition(&curPos, FMOD_TIMEUNIT_MS);
	return curPos / (float)this->song.length;
}

int AudioManager::getSongMS() {
	unsigned int curPos;
	this->channel->getPosition(&curPos, FMOD_TIMEUNIT_MS);
	return curPos;
}

// also used for sliders
void AudioManager::setSongPercentage(float percentage) {
	this->channel->setPosition(static_cast<unsigned int>(percentage * this->song.length), FMOD_TIMEUNIT_MS);
}
