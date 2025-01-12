#pragma once
#include <Geode/Geode.hpp>

;class AudioSource {
public:
    AudioSource(std::filesystem::path path);
    ~AudioSource();

    std::filesystem::path m_path;

    bool m_isLoadingAudio;
    bool m_hasLoadedAudio;
    FMOD::Sound* m_sound;
    std::string m_name;
    cocos2d::CCTexture2D* m_albumCover;
    unsigned int m_length;

    void loadAudioThreaded();
    void loadAudio();
};
