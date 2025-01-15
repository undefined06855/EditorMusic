#pragma once

// lazy
#define LOG_ENABLE_CHECK geode::Mod::get()->getSettingValue<bool>("enable-log")

namespace em::log {

    template <typename... Args>
    inline void debug(geode::log::impl::FmtStr<Args...> str, Args&&... args) {
        if (LOG_ENABLE_CHECK) geode::log::debug(str, std::forward<Args>(args)...);
    }

    template <typename... Args>
    inline void info(geode::log::impl::FmtStr<Args...> str, Args&&... args) {
        if (LOG_ENABLE_CHECK) geode::log::info(str, std::forward<Args>(args)...);
    }

    template <typename... Args>
    inline void warn(geode::log::impl::FmtStr<Args...> str, Args&&... args) {
        if (LOG_ENABLE_CHECK) geode::log::warn(str, std::forward<Args>(args)...);
    }

    template <typename... Args>
    inline void error(geode::log::impl::FmtStr<Args...> str, Args&&... args) {
        if (LOG_ENABLE_CHECK) geode::log::error(str, std::forward<Args>(args)...);
    }

}