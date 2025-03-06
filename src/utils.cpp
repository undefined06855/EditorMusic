#include "utils.hpp"

namespace em::utils {

std::string formatTime(unsigned int ms) {
    int rawSeconds = ms / 1000;
    
    int minutes = (rawSeconds / 60) % 60;
    int hours = (rawSeconds / 60) / 60;
    int seconds = rawSeconds % 60;

    if (minutes == 0 && hours == 0) return fmt::format("00:{:02}", seconds);
    if (hours == 0) return fmt::format("{:02}:{:02}", minutes, seconds);
    return fmt::format("{:02}:{:02}:{:02}", hours, minutes, seconds);
}

cocos2d::CCImage::EImageFormat mimeTypeToFormat(std::string mimeType) {
    std::unordered_map<std::string, cocos2d::CCImage::EImageFormat> formatToMap = {
        { "image/png", cocos2d::CCImage::EImageFormat::kFmtPng },
        { "image/jpg", cocos2d::CCImage::EImageFormat::kFmtJpg },
        { "image/jpeg", cocos2d::CCImage::EImageFormat::kFmtJpg },
        { "image/tiff", cocos2d::CCImage::EImageFormat::kFmtTiff },
        { "image/tif", cocos2d::CCImage::EImageFormat::kFmtTiff }
    };

    auto ret = formatToMap.find(mimeType);
    if (ret == formatToMap.end()) return cocos2d::CCImage::EImageFormat::kFmtUnKnown;
    return ret->second;
}

bool isMusicPlayerCentered() {
    auto format = geode::Mod::get()->getSettingValue<std::string>("player-format");
    return format == "centered no album" || format == "centered with album";
}

bool showMusicPlayerAlbumCover() {
    auto format = geode::Mod::get()->getSettingValue<std::string>("player-format");
    return format == "default" || format == "centered with album";
}

}
