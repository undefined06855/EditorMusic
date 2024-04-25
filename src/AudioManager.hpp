#pragma once
#include <Geode/Geode.hpp>
using namespace geode::prelude;

class AudioManager {
protected:
	bool isRunning = false;

public:
	FMOD::Sound* sound;
	FMOD::Channel* channel;
	FMOD::System* system = FMODAudioEngine::sharedEngine()->m_system;
	unsigned int startOffset = 0;

	void setup() {
		auto music = (Mod::get()->getConfigDir() / "music.mp3").string();
		this->system->createSound(music.c_str(), FMOD_LOOP_NORMAL, nullptr, &(this->sound));
		log::info("Sound created");
	}

	void playAudio() {
		log::info("isRunning (start): {}", (this->isRunning ? "yes" : "no"));

		if (this->isRunning) return;

		if (!this->sound) {
			log::info("Sound not created!");
			return;
		}

		this->sound->setLoopCount(-1);
		this->system->playSound(this->sound, nullptr, false, &(this->channel));
		this->turnUpMusic();
		this->isRunning = true;
		log::info("offset: {}ms", startOffset);
		this->channel->setPosition(startOffset, FMOD_TIMEUNIT_MS);
	}

	void stopAudio() {
		// PLEASE can someone submit a pr that adds fading to this thanks
		log::info("isRunning (stop): {}", (this->isRunning ? "yes" : "no"));
		if (!this->isRunning) return;

		this->channel->getPosition(&(this->startOffset), FMOD_TIMEUNIT_MS);
		log::info("new start offset: {}", this->startOffset);

		this->channel->stop();
		this->isRunning = false;
	}

	void turnDownMusic() {
		this->channel->setVolume(GameManager::get()->m_bgVolume * .4f);
	}

	void turnUpMusic() {
		this->channel->setVolume(GameManager::get()->m_bgVolume * .8f);
	}

	void onExitEditor() {
		this->startOffset = 0;
	}
};
