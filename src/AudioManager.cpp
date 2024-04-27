#include "AudioManager.hpp"
#include <codecvt>
#include <regex>

void AudioManager::setupFromOneFolder(ghc::filesystem::path path) {
	for (const auto& file : std::filesystem::directory_iterator(path.string())) {
		std::filesystem::path path = file.path();

		if (this->extensions.contains(path.extension().string())) {
			// create the sounds!
			FMOD::Sound* sound;
			this->system->createSound(path.string().c_str(), FMOD_LOOP_NORMAL, nullptr, &sound);
			sound->setLoopCount(0);

			this->sounds.push_back(sound);
			log::info("Sound created for {}", path.filename().string());

			FMOD_TAG tag;
			FMOD_RESULT res;
			if (path.extension().string() == ".mp3") {
				res = sound->getTag("TIT2", 0, &tag);
			}
			else if (path.extension().string() == ".wav") {
				res = sound->getTag("INAM", 0, &tag);
			}
			else if (path.extension().string() == ".ogg" || path.extension().string() == ".flac") {
				res = sound->getTag("TITLE", 0, &tag);
			}

			if (res == FMOD_ERR_TAGNOTFOUND) {
				log::warn("Name tag not found for song, using fallback");
				std::string songName;

				if (Mod::get()->getSettingValue<bool>("unnamed-song-fallback")) songName = path.filename().string();
				else songName = Mod::get()->getSettingValue<std::string>("unnamed-song-fallback-custom");

				this->songNames.push_back(songName);
				continue;
			}

			std::string songName;
			const char* songNameAsChar = reinterpret_cast<const char*>(tag.data);

			if (tag.datatype == FMOD_TAGDATATYPE_STRING_UTF16) {
				log::info("Song name is in utf16");
				std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
				songName = converter.to_bytes(reinterpret_cast<const char16_t*>(songNameAsChar));
			}
			else if (tag.datatype == FMOD_TAGDATATYPE_STRING_UTF16BE) {
				log::info("Song name is in utf16 but big endian (very silly)");
				// silly big endian
				std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
				std::u16string utf16String(reinterpret_cast<const char16_t*>(songNameAsChar), tag.datalen / sizeof(char16_t));
				// reverse reverse
				for (size_t i = 0; i < utf16String.size(); ++i) {
					utf16String[i] = (utf16String[i] << 8) | (utf16String[i] >> 8);
				}
				// cha cha real smooth
				songName = converter.to_bytes(utf16String);
			}
			else {
				log::info("Song name is in utf8");
				songName = std::string(songNameAsChar, tag.datalen);
			}

			// filter out stuff like the poorly parsed BOM
			log::info("Song name before: {}", songName);
			std::regex charsInBigFont("[^\u0020\u0021\u0022\u0023\u0024\u0025\u0026\u0027\u0028\u0029\u002a\u002b\u002c\u002d\u002e\u002f\u0030\u0031\u0032\u0033\u0034\u0035\u0036\u0037\u0038\u0039\u003a\u003b\u003c\u003d\u003e\u003f\u0040\u0041\u0042\u0043\u0044\u0045\u0046\u0047\u0048\u0049\u004a\u004b\u004c\u004d\u004e\u004f\u0050\u0051\u0052\u0053\u0054\u0055\u0056\u0057\u0058\u0059\u005a\u005b\u005c\u005d\u005e\u005f\u0060\u0061\u0062\u0063\u0064\u0065\u0066\u0067\u0068\u0069\u006a\u006b\u006c\u006d\u006e\u006f\u0070\u0071\u0072\u0073\u0074\u0075\u0076\u0077\u0078\u0079\u007a\u007b\u007c\u007d\u007e\u2022]+");
			songName = std::regex_replace(songName, charsInBigFont, "");

			this->songNames.push_back(songName);
			log::info("Song name after: {}", songName);
		}
		else {
			log::warn("Unsupported file extension found in config dir: {} (from {})", path.extension().string(), path.filename().string());
		}
	}
}

void AudioManager::setup() {
	this->setupFromOneFolder(Mod::get()->getConfigDir());
	// todo: multiple directories

	log::info("Sounds created! songs={}", this->sounds.size());

	if (this->sounds.size() == 0) {
		// oh there's no songs, disable everything
		this->hasNoSongs = true;
		return;
	}

	this->songID = rand() % this->sounds.size();

	this->channel->setMode(FMOD_LOOP_OFF);
}

void AudioManager::playAudio(bool newSong) {
	if (this->hasNoSongs) return;
	if (this->isRunning) return;

	int id;
	if (newSong) {
		// im sure this will be fine, right?
		do {
			id = rand() % this->sounds.size();
		} while (id == this->songID && this->sounds.size() > 1);

		this->startOffset = 0;
		log::info("new song! id={}", id);
		this->songID = id;
	} else id = this->songID;

	auto sound = this->sounds.at(id);

	this->system->playSound(sound, nullptr, false, &(this->channel));
	this->turnUpMusic();
	this->isRunning = true;
	log::info("offset: {}ms", startOffset);
	this->channel->setPosition(startOffset, FMOD_TIMEUNIT_MS);

	this->currentSongName = this->songNames.at(id);
	log::info("current song name: {}", this->currentSongName);
}

void AudioManager::stopAudio() {
	if (this->hasNoSongs) return;

	// PLEASE can someone submit a pr that adds fading to this thanks
	if (!this->isRunning) return;

	this->channel->getPosition(&(this->startOffset), FMOD_TIMEUNIT_MS);
	log::info("new start offset: {}", this->startOffset);

	this->channel->stop();
	this->isRunning = false;
}

void AudioManager::turnDownMusic() const {
	this->channel->setVolume(0.3f * Mod::get()->getSettingValue<double>("volume"));
}

void AudioManager::turnUpMusic() const {
	this->channel->setVolume(Mod::get()->getSettingValue<double>("volume"));
}

void AudioManager::onExitEditor() {
	this->startOffset = 0;
}

void AudioManager::tick(float dt) {
	if (this->hasNoSongs) return;

	// if fading needs to be added it can be added here
	// and i probably will but i can't be bothered

	// check if needs to go to next song
	bool isPlaying;
	this->channel->isPlaying(&isPlaying);
	// so if it's not playing but it should be...
	if (!isPlaying && this->isRunning) {
		log::info("song finished?");
		this->isRunning = false;
		this->playAudio(true);
	}
}

void AudioManager::nextSong() {
	  if (this->hasNoSongs) return;

	  this->stopAudio();
	  this->playAudio(true);
}
