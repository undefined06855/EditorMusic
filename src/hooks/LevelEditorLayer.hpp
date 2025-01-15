#ifdef GEODE_IS_WINDOWS
#include <Geode/modify/LevelEditorLayer.hpp>

class $modify(HookedLevelEditorLayer, LevelEditorLayer) {
    bool init(GJGameLevel* p0, bool p1);
};
#endif