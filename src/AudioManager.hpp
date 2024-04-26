#pragma once
#include <Geode/Geode.hpp>
using namespace geode::prelude;

class AudioManager {
protected:
	bool isRunning = false;
	std::unordered_set<std::string> extensions = {
		".mp3",
		".wav",
		".ogg"
	};

public:
	std::vector<FMOD::Sound*> sounds;
	FMOD::Channel* channel;
	FMOD::System* system = FMODAudioEngine::sharedEngine()->m_system;
	unsigned int startOffset = 0;
	int songID;
	bool hasNoSongs = false;

	void setup() {
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

	void playAudio(bool newSong) {
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
		}
		else id = this->songID;

		auto sound = this->sounds.at(id);

		this->system->playSound(sound, nullptr, false, &(this->channel));
		this->turnUpMusic();
		this->isRunning = true;
		log::info("offset: {}ms", startOffset);
		this->channel->setPosition(startOffset, FMOD_TIMEUNIT_MS);
	}

	void stopAudio() {
		if (this->hasNoSongs) return;

		// PLEASE can someone submit a pr that adds fading to this thanks
		if (!this->isRunning) return;

		this->channel->getPosition(&(this->startOffset), FMOD_TIMEUNIT_MS);
		log::info("new start offset: {}", this->startOffset);

		this->channel->stop();
		this->isRunning = false;
	}

	void turnDownMusic() {
		this->channel->setVolume(.4f * Mod::get()->getSettingValue<double>("volume"));
	}

	void turnUpMusic() {
		this->channel->setVolume(Mod::get()->getSettingValue<double>("volume"));
	}

	void onExitEditor() {
		this->startOffset = 0;
	}

	void tick(float dt) {
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

	void nextSong() {
		if (this->hasNoSongs) return;

		this->stopAudio();
		this->playAudio(true);
	}
};
