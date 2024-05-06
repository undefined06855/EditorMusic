#pragma once
#include <Geode/Geode.hpp>
using namespace geode::prelude;

class AudioManager {
private:
	AudioManager();
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
	unsigned int startOffset = 0;
	int songID;
	std::vector<int> history = {};
	bool hasNoSongs = false;
	bool customPathDoesntExist = false;
	bool isRunning = false;
	std::string currentSongName = "";

	void setup();
	void setupFromOneFolder(ghc::filesystem::path path);
	void playAudio(bool newSong);
	void playAudio(int id);
	void stopAudio();
	void turnDownMusic() const;
	void turnUpMusic() const;
	void onExitEditor();
	void tick(float dt);
	void nextSong();
	void prevSong();
};
