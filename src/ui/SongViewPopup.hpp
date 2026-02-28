#pragma once
#include "../AudioSource.hpp"

class SongUserObject : public cocos2d::CCObject {
public:
    std::shared_ptr<AudioSource> m_song;
};

class SongViewPopup : public geode::Popup {
public:
    static SongViewPopup* create();
    bool init();

    geode::TextInput* m_playlistName;
    cocos2d::CCLabelBMFont* m_songCountLabel;
    std::vector<std::shared_ptr<AudioSource>> m_selected;
    geode::ScrollLayer* m_scrollLayer;

    void updateSongCount();
    void onFolder(cocos2d::CCObject* sender);
    void onToggleSong(cocos2d::CCObject* sender);
    void onLoadPlaylist(cocos2d::CCObject* sender);
    void onCreatePlaylist(cocos2d::CCObject* sender);
    void onAddToPlaylist(cocos2d::CCObject* sender);
    void onRemoveFromPlaylist(cocos2d::CCObject* sender);
    void onInfo(cocos2d::CCObject* sender);
};
