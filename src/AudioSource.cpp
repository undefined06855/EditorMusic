#include "AudioSource.hpp"

AudioSource::AudioSource(std::filesystem::path path)
    : m_path(path)
    , m_isLoadingAudio(false)
    , m_hasLoadedAudio(false)
    , m_sound(nullptr)
    , m_name("<UNLOADED>")
    , m_artist("<UNLOADED>")
    , m_albumCover(nullptr)
    , m_length(0) {}

AudioSource::~AudioSource() {
    if (m_albumCover) m_albumCover->release();
}

void AudioSource::loadAudioThreaded() {
    std::thread(&AudioSource::loadAudio, this).detach();
}

void AudioSource::loadAudio() {
    geode::utils::thread::setName(fmt::format("Audio Loading: {}", m_name));
    m_isLoadingAudio = true;
	FMODAudioEngine::sharedEngine()->m_system->createStream(m_path.string().c_str(), FMOD_CREATESTREAM, nullptr, &m_sound);
    m_sound->setLoopCount(0);
    m_hasLoadedAudio = true;
    m_isLoadingAudio = false;
}

std::string AudioSource::getCombinedSongName() {
    if (m_artist != "Unknown") {
        return fmt::format("{} - {}", m_name, m_artist);
    }

    return m_name;
}
