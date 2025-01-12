#include "LevelEditorLayer.hpp"
#include "../AudioManager.hpp"

bool HookedLevelEditorLayer::init(GJGameLevel* p0, bool p1) {
    if (!LevelEditorLayer::init(p0, p1)) return false;
    AudioManager::get().enterEditor();
    return true;
}
