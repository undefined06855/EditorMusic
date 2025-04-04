#pragma once
#include "../AudioSource.hpp"
#include "ProgressBar.hpp"
#include <Geode/ui/LoadingSpinner.hpp>

class SongInfoPopup : public geode::Popup<> {
public:
    static SongInfoPopup* create();
    bool setup();
    static SongInfoPopup* get();

    cocos2d::CCLabelBMFont* m_currentSongLabel;
    cocos2d::CCLabelBMFont* m_currentSongArtistLabel;
    cocos2d::CCSprite* m_albumCoverSprite;
    cocos2d::extension::CCScale9Sprite* m_albumCoverClipSprite;

    ProgressBar* m_progressBar;
    cocos2d::CCLabelBMFont* m_progressLabel;

    std::shared_ptr<AudioSource> m_currentSong;
    geode::LoadingSpinner* m_loadingCircle;

    cocos2d::CCMenu* m_layoutedButtonMenu;
    CCMenuItemToggler* m_playPauseButton;

    bool m_isInStrippedBackMode;

    void update(float dt);
    void updateAlbumCover();

    void updateCustomizableUI();

    void onPrev(cocos2d::CCObject* sender);
    void onNext(cocos2d::CCObject* sender);
    void onPlayPause(cocos2d::CCObject* sender);
    void onRewind(cocos2d::CCObject* sender);
    void onFastForward(cocos2d::CCObject* sender);

    void onSettings(cocos2d::CCObject* sender);
    void onNoSongsRefresh(cocos2d::CCObject* sender);

    cocos2d::CCSprite* getRandomSpriteForNoSongPopup();

    void close();
};
