#include "LevelEditorLayer.hpp"
#include <geode.custom-keybinds/include/Keybinds.hpp>
#include "../AudioManager.hpp"
#include "../log.hpp"

bool HookedLevelEditorLayer::init(GJGameLevel* p0, bool p1) {
    if (!LevelEditorLayer::init(p0, p1)) return false;

    AudioManager::get().enterEditor();

    addEventListener<keybinds::InvokeBindFilter>([](keybinds::InvokeBindEvent* event) {
        if (event->isDown()) {
            em::log::debug("Keybind: Pause");
            AudioManager::get().m_isPaused = !AudioManager::get().m_isPaused;
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

    return true;
}
