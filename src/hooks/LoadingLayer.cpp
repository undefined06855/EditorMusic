#include "LoadingLayer.hpp"
#include "../AudioManager.hpp"
#include "window.hpp"

bool HookedLoadingLayer::init(bool isReloading) {
    if (!LoadingLayer::init(isReloading)) return false;

    if (!isReloading) {
        // setup low pass filters on first game launch
        AudioManager::get().init();
#ifdef GEODE_IS_WINDOWS
        em::window::modifyWindowProc();
#endif
    }

    // dont ask
    geode::Loader::get()->queueInMainThread([] {
        geode::Loader::get()->queueInMainThread([] {
            AudioManager::get().populateSongs();
        });
    });

    return true;
}
