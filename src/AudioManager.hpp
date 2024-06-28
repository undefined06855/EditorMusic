#pragma once
#include <Geode/Geode.hpp>
using namespace geode::prelude;

struct UnloadedAudio {
	std::filesystem::path path;
};

struct LoadedAudio {
	FMOD::Sound* sound;
	std::string name;
	unsigned int length;
};

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

	std::vector<UnloadedAudio> songs;
	LoadedAudio song;

	FMOD::Channel* channel;
	FMOD::System* system = FMODAudioEngine::sharedEngine()->m_system;
	int songID;
	std::vector<int> history = {};
	bool hasNoSongs = false;
	bool customPathDoesntExist = false;
	bool isPaused = false;
	bool playtestMusicIsPlaying = false;
	bool isInEditor = false;
	int lowPassStrength = 0;
	float actualLowPassCutoff = 0;
	float volume = 0; // volume should start at zero so it fades in

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
