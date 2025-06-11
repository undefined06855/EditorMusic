#pragma once

namespace em::utils {

extern const uint32_t g_crc32_4[4][256];

std::string formatTime(unsigned int ms);
cocos2d::CCImage::EImageFormat mimeTypeToFormat(std::string mimeType);
bool isMusicPlayerCentered();
bool showMusicPlayerAlbumCover();
uint32_t crc32(const uint8_t* bytes, size_t len, uint32_t initial);

}
