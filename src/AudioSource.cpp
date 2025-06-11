#include "AudioSource.hpp"
#include <fmod_errors.h>
#include "log.hpp"

AudioSource::AudioSource(std::filesystem::path path)
    : m_path(path)
    , m_isLoadingAudio(false)
    , m_hasLoadedAudio(false)
    , m_sound(nullptr)
    , m_name("<UNLOADED>")
    , m_artist("<UNLOADED>")
    , m_albumCover(nullptr)
    , m_length(0) {}

// handled by texture cache now
// AudioSource::~AudioSource() {
//     if (m_albumCover) m_albumCover->release();
// }

void AudioSource::loadAudioThreaded() {
    std::thread(&AudioSource::loadAudio, this).detach();
}

void AudioSource::loadAudio() {
    geode::utils::thread::setName(fmt::format("Audio Loading: {}", m_name));
    m_isLoadingAudio = true;
	auto ret = FMODAudioEngine::sharedEngine()->m_system->createStream(geode::utils::string::pathToString(m_path).c_str(), FMOD_CREATESTREAM | FMOD_ACCURATETIME, nullptr, &m_sound);
    if (ret != FMOD_OK) em::log::warn("FMOD error (2): {} (0x{:02X})", FMOD_ErrorString(ret), (int)ret);
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
