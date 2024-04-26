#include "AudioManager.hpp"

void AudioManager::setup() {
	std::vector<std::string> files = {};

	for (const auto& file : std::filesystem::directory_iterator(Mod::get()->getConfigDir().string())) {
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
			} else if (path.extension().string() == ".wav") {
				res = sound->getTag("INAM", 0, &tag);
			} else if (path.extension().string() == ".ogg") {
				res = sound->getTag("TITLE", 0, &tag);
			}

			if (res == FMOD_ERR_TAGNOTFOUND) {
				log::info("Name tag not found for song, using fallback");
				std::string songName;

				if (Mod::get()->getSettingValue<bool>("unnamed-song-fallback")) songName = path.filename().string();
				else songName = Mod::get()->getSettingValue<std::string>("unnamed-song-fallback-custom");

				this->songNames.push_back(songName);
				continue;
			}

			auto songName = std::string(reinterpret_cast<const char*>(tag.data), tag.datalen);
			this->songNames.push_back(songName);
			log::info("Song name is {}", songName);
		} else {
			log::info("Unsupported file extension found in config dir: {}", path.filename().string());
		}
	}

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
		if (this->sounds.size() > 1) {
			do {
				id = rand() % this->sounds.size();
			} while (id == this->songID);
		}

		this->startOffset = 0;
		log::info("new song!");
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
