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

	void setup();
	void playAudio(bool newSong);
	void stopAudio();
	void turnDownMusic() const;
	void turnUpMusic() const;
	void onExitEditor();
	void tick(float dt);
	void nextSong();
};
