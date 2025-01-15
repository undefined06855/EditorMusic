#pragma once
#include "../AudioSource.hpp"

class CurrentSongNode : public cocos2d::CCNode {
public:
    static CurrentSongNode* create();
    bool init();

    cocos2d::CCLayer* m_mainLayer;
    CCMenuItemSpriteExtra* m_currentSongButton;
    cocos2d::CCLabelBMFont* m_currentSongLabel;
    cocos2d::CCSprite* m_hintSprite;
    cocos2d::CCMenu* m_buttonMenu;
    std::shared_ptr<AudioSource> m_currentSong;

    void onButton(cocos2d::CCObject* sender);
    void update(float dt);
};