#pragma once
#include "external/labels.hpp"

namespace em::rift_labels {

inline const std::string g_labelNumSongs = "editorSongCount";
inline const std::string g_labelCurrentSong = "currentEditorSong";
inline const std::string g_labelCurrentSongArtist = "currentEditorSongArtist";
inline const std::string g_labelEditorSongDuration = "editorSongDuration";
inline const std::string g_labelEditorSongTimestamp = "editorSongTimestamp";

inline bool g_isEclipseLoaded = false;

template <eclipse::label::SupportedType T>
inline void set(std::string labelName, T value) {
    if (!g_isEclipseLoaded) return;
    eclipse::label::setVariable(labelName, value);
}

}
