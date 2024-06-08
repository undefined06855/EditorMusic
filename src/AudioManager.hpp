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
	bool isRunning = false;
	std::string currentSongName = "";
	unsigned int currentSongLength = 0;
	
	float desiredScale = 1.f;
	float desiredPopupScale = 1.f;

	void setup();
	void setupFromOneFolder(std::filesystem::path path);

	void turnDownMusic() const;
	void turnUpMusic() const;

	void tick(float dt);

	void pause();
	void play();
	void playNewSong();
	void playSongID(int id);
	void nextSong();
	void prevSong();

	float getSongPercentage();
	int getSongMS();
	void setSongPercentage(float);
};
