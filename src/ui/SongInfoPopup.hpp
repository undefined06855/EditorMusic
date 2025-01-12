#pragma once
#include <Geode/Geode.hpp>
#include "../AudioSource.hpp"

class SongInfoPopup : public geode::Popup<> {
public:
    static SongInfoPopup* create();
    bool setup();

    cocos2d::CCLabelBMFont* m_currentSongLabel;
    cocos2d::CCSprite* m_albumCoverSprite;
    std::shared_ptr<AudioSource> m_lastSongDisplayed;

    void update(float dt);

    void onPrev(cocos2d::CCObject* sender);
    void onNext(cocos2d::CCObject* sender);
};
