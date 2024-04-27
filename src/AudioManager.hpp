#pragma once
#include <Geode/Geode.hpp>
using namespace geode::prelude;

class AudioManager {
protected:
	bool isRunning = false;
	std::unordered_set<std::string> extensions = {
		".mp3",
		".wav",
		".ogg",
		".flac"
	};

public:
	std::vector<FMOD::Sound*> sounds;
	std::vector<std::string> songNames;

	FMOD::Channel* channel;
	FMOD::System* system = FMODAudioEngine::sharedEngine()->m_system;
	unsigned int startOffset = 0;
	int songID;
	bool hasNoSongs = false;
	std::string currentSongName = "";

	void setup();
	void setupFromOneFolder(ghc::filesystem::path path);
	void playAudio(bool newSong);
	void stopAudio();
	void turnDownMusic() const;
	void turnUpMusic() const;
	void onExitEditor();
	void tick(float dt);
	void nextSong();
};
