#pragma once
#include <Geode/modify/EditorPauseLayer.hpp>

class $modify(HookedEditorPauseLayer, EditorPauseLayer) {
    static void onModify(auto& self);

    bool init(LevelEditorLayer* editor);
    void onResume(cocos2d::CCObject* sender);
};
