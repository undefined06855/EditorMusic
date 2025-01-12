#pragma once
#include <Geode/Geode.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>

class $modify(HookedEditorPauseLayer, EditorPauseLayer) {
    static void onModify(auto& self);
    
    bool init(LevelEditorLayer* editor);
    void onExitEditor(cocos2d::CCObject* sender);
    void onSaveAndExit(cocos2d::CCObject* sender);
    void onSaveAndPlay(cocos2d::CCObject* sender);
}
