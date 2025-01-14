#include "MenuLayer.hpp"

bool g_firstTime = true;

bool HookedMenuLayer::init() {
    if (!MenuLayer::init()) return false;

    if (g_firstTime && geode::Mod::get()->getSettingValue<bool>("menulayer-load")) {
        AudioManager::get().init();
    }

    g_firstTime = false;

    return true;
}
