#pragma once

namespace em::utils {

std::string formatTime(unsigned int ms);
cocos2d::CCImage::EImageFormat mimeTypeToFormat(std::string mimeType);
bool isMusicPlayerCentered();
bool showMusicPlayerAlbumCover();

}
