#include "LevelEditorLayer.hpp"
#ifdef GEODE_IS_WINDOWS
#include <geode.custom-keybinds/include/Keybinds.hpp>
#endif
#include "../AudioManager.hpp"
#include "../log.hpp"

bool HookedLevelEditorLayer::init(GJGameLevel* p0, bool p1) {
    if (!LevelEditorLayer::init(p0, p1)) return false;

    // smh
    bool isLikelyObjectWorkshop = \
           geode::Loader::get()->getLoadedMod("firee.object-workshop")
        && cocos2d::CCScene::get()->getChildByType<ProfilePage>(0);

    if (!isLikelyObjectWorkshop) AudioManager::get().enterEditor();
    else em::log::debug("prevented enterEditor because likely object workshop!");
    
#ifndef GEODE_IS_MACOS
    addEventListener<keybinds::InvokeBindFilter>([](keybinds::InvokeBindEvent* event) {
        if (event->isDown()) {
            em::log::debug("Keybind: Pause");
            AudioManager::get().togglePause();
        }

        return geode::ListenerResult::Propagate;
    }, "pause-song"_spr);

    addEventListener<keybinds::InvokeBindFilter>([](keybinds::InvokeBindEvent* event) {
        if (event->isDown()) {
            em::log::debug("Keybind: Next song");
            AudioManager::get().nextSong();
        }

        return geode::ListenerResult::Propagate;
    }, "next-song"_spr);

    addEventListener<keybinds::InvokeBindFilter>([](keybinds::InvokeBindEvent* event) {
        if (event->isDown()) {
            em::log::debug("Keybind: Prev song");
            AudioManager::get().prevSong();
        }

        return geode::ListenerResult::Propagate;
    }, "prev-song"_spr);
#endif

    return true;
}
