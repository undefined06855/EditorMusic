#pragma once
#include <Geode/Geode.hpp>
#include "../AudioSource.hpp"
#include "ProgressBar.hpp"

class SongInfoPopup : public geode::Popup<> {
public:
    static SongInfoPopup* create();
    bool setup();

    cocos2d::CCLabelBMFont* m_currentSongLabel;
    cocos2d::CCLabelBMFont* m_currentSongArtistLabel;
    cocos2d::CCSprite* m_albumCoverSprite;

    ProgressBar* m_progressBar;
    cocos2d::CCLabelBMFont* m_progressLabel;

    std::shared_ptr<AudioSource> m_currentSong;
    LoadingCircleSprite* m_loadingCircle;

    cocos2d::CCMenu* m_layoutedButtonMenu;

    void update(float dt);
    void updateAlbumCover();

    void onPrev(cocos2d::CCObject* sender);
    void onNext(cocos2d::CCObject* sender);
    void onPlayPause(cocos2d::CCObject* sender);
    void onRewind(cocos2d::CCObject* sender);
    void onFastForward(cocos2d::CCObject* sender);

    void onSettings(cocos2d::CCObject* sender);
};
