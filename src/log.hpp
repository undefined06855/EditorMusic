#pragma once

// lazy
#define LOG_ENABLE_CHECK geode::Mod::get()->getSettingValue<bool>("enable-log")

namespace em::log {

template <typename... Args>
inline void trace(geode::format::FmtStr<Args...> str, Args&&... args) {
    if (geode::Mod::get()->getSettingValue<bool>("enable-log")) geode::log::trace(str, std::forward<Args>(args)...);
}

template <typename... Args>
inline void debug(geode::format::FmtStr<Args...> str, Args&&... args) {
    if (geode::Mod::get()->getSettingValue<bool>("enable-log")) geode::log::debug(str, std::forward<Args>(args)...);
}

template <typename... Args>
inline void info(geode::format::FmtStr<Args...> str, Args&&... args) {
    if (geode::Mod::get()->getSettingValue<bool>("enable-log")) geode::log::info(str, std::forward<Args>(args)...);
}

template <typename... Args>
inline void warn(geode::format::FmtStr<Args...> str, Args&&... args) {
    if (geode::Mod::get()->getSettingValue<bool>("enable-log")) geode::log::warn(str, std::forward<Args>(args)...);
}

template <typename... Args>
inline void error(geode::format::FmtStr<Args...> str, Args&&... args) {
    if (geode::Mod::get()->getSettingValue<bool>("enable-log")) geode::log::error(str, std::forward<Args>(args)...);
}

}
