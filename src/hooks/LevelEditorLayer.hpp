#pragma once
#include <Geode/modify/LevelEditorLayer.hpp>
#include "../AudioSource.hpp"

class $modify(HookedLevelEditorLayer, LevelEditorLayer) {
    struct Fields {
        std::shared_ptr<AudioSource> m_currentSong;
        cocos2d::CCLabelBMFont* m_songPopupLabel;
    };

    bool init(GJGameLevel* p0, bool p1);
    void postUpdate(float dt);
};
