#pragma once
#include "AudioSource.hpp"
#include "Easer.hpp"
#include "ui/PreloadUI.hpp"
#include <random>

struct LoadIssue {
    std::string m_message;
    FMOD_RESULT m_type;
};

class AudioManager : cocos2d::CCObject {
    AudioManager();
public:
    static AudioManager& get();
    void init();

    bool m_shownLoadIssues;
    std::vector<LoadIssue> m_loadIssues;

    std::string m_playlist;

    std::deque<std::shared_ptr<AudioSource>> m_queue;
    std::deque<std::shared_ptr<AudioSource>> m_history;
    int m_queueLength;
    int m_historyLength;
    std::vector<std::shared_ptr<AudioSource>> m_songs;

    FMOD::Channel* m_channel;
    FMOD::System* m_system;
    float m_volume;

    bool m_isPaused;
    bool m_isInEditor;
    bool m_isEditorAudioPlaying;
    bool m_isQueueBeingPopulated;

    bool m_playCurrentSongQueuedForLoad;

    int m_lowPassStrength;
    FMOD::DSP* m_lowPassFilter;
    float m_lowPassEasedCutoff;

    std::vector<Easer> m_easers;

    PreloadUI* m_preloadUI;

    std::mt19937 m_gen;
    std::uniform_int_distribution<int> m_randomSongGenerator;

    std::unordered_map<uint32_t, cocos2d::CCTexture2D*> m_textureCache;

    void populateSongs();
    void populateSongsThread();
    std::filesystem::path pathForPlaylistFile(geode::ZStringView name);
    std::vector<std::filesystem::path> searchPlaylistFile(geode::ZStringView name);
    void setupPreloadUIFromPath(const std::filesystem::path& path);
    void populateSongsFromPath(const std::filesystem::path& path);
    bool populateAudioSourceInfo(std::shared_ptr<AudioSource> source);
    std::string populateStringTag(FMOD_TAG tag, bool useTitleFallback, std::shared_ptr<AudioSource> sourceForFallback = nullptr);
    void populateAlbumCover(std::shared_ptr<AudioSource> source, FMOD_TAG tag);
    std::string figureOutFallbackName(const std::filesystem::path& path);
    std::string filterNameThruRegex(geode::ZStringView songName);
    std::string formatArtistString(geode::ZStringView artists);
    bool isValidAudioFile(const std::filesystem::path& path);

    bool playlistFileExists(geode::ZStringView name);
    geode::Result<> writePlaylistFile(geode::ZStringView name, std::vector<std::shared_ptr<AudioSource>> songs);
    geode::Result<std::string> addToPlaylistFile(geode::ZStringView name, std::vector<std::shared_ptr<AudioSource>> songs);
    geode::Result<std::string> removeFromPlaylistFile(geode::ZStringView name, std::vector<std::shared_ptr<AudioSource>> songs);

    void checkQueueLength();

    void nextSong();
    void prevSong();
    void rewind();
    void fastForward();
    void togglePause();
    void setPaused(bool value);

    void update(float dt);
    void startPlayingCurrentSong();
    void checkSongPreload();

    std::shared_ptr<AudioSource> getCurrentSong();
    unsigned int getCurrentSongPosition();
    unsigned int getCurrentSongLength();
    bool shouldSongBePlaying();
    bool shouldAllowAudioFunctions();

    std::shared_ptr<AudioSource> getRandomSong();

    void updateLowPassFilter();

    void enterEditor();
    void exitEditor();

    void onCloseGDWindow();

    void showLoadErrors(cocos2d::CCScene* scene);
};
