#include "utils.hpp"

namespace em::utils {

std::string formatTime(unsigned int ms) {
    int seconds = ms / 1000;
    int minutes = (seconds / 60) % 60;
    int hours = minutes / 60;
    int formattedSeconds = seconds % 60;

    if (minutes == 0) {
        return fmt::format("00:{:02}", formattedSeconds);
    } else if (hours == 0) {
        return fmt::format("{:02}:{:02}", minutes, formattedSeconds);
    } else {
        return fmt::format("{:02}:{:02}:{:02}", hours, minutes, formattedSeconds);
    }
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
