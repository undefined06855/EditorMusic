#pragma once
#include "AudioSource.hpp"
#include "Easer.hpp"
#include "ui/PreloadUI.hpp"
#include <random>

class AudioManager {
    AudioManager();
public:
    static AudioManager& get();
    void init();

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

    void populateSongs();
    void populateSongsThread();
    void setupPreloadUIFromPath(std::filesystem::path path);
    void populateSongsFromPath(std::filesystem::path path);
    void populateAudioSourceInfo(std::shared_ptr<AudioSource> source);
    std::string populateStringTag(FMOD_TAG tag, bool useTitleFallback, std::shared_ptr<AudioSource> sourceForFallback = nullptr);
    void populateAlbumCover(std::shared_ptr<AudioSource> source, FMOD_TAG tag);
    std::string figureOutFallbackName(std::filesystem::path path);
    std::string filterNameThruRegex(std::string songName);
    std::string formatArtistString(std::string artists);
    bool isValidAudioFile(std::filesystem::path path);

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
};
