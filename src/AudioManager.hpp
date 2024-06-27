#pragma once
#include <Geode/Geode.hpp>
using namespace geode::prelude;

class AudioManager {
private:
	AudioManager() {};
protected:
	std::unordered_set<std::string> extensions = {
		".mp3",
		".wav",
		".ogg",
		".flac"
	};
public:
	static AudioManager& get();

	std::vector<FMOD::Sound*> sounds;
	std::vector<std::string> songNames;

	FMOD::Channel* channel;
	FMOD::System* system = FMODAudioEngine::sharedEngine()->m_system;
	int songID;
	std::vector<int> history = {};
	bool hasNoSongs = false;
	bool customPathDoesntExist = false;
	bool isPaused = false;
	bool playtestMusicIsPlaying = false;
	bool isInEditor = false;
	std::string currentSongName = "";
	unsigned int currentSongLength = 0;
	int lowPassStrength = 0;

	FMOD::DSP* lowPassFilterDSP;

	float desiredScale = 1.f;
	float desiredPopupScale = 1.f;

	void setup();
	void setupFromOneFolder(std::filesystem::path path);

	void tick(float dt);

	void updateLowPassFilter();

	void playNewSong();
	void playSongID(int id);
	void nextSong();
	void prevSong();

	float getSongPercentage();
	int getSongMS();
	void setSongPercentage(float);
};
