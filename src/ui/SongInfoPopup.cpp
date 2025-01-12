#include "SongInfoPopup.hpp"
#include "../AudioManager.hpp"

SongInfoPopup* SongInfoPopup::create() {
    auto ret = new SongInfoPopup;
    if (ret->initAnchored(360.f, 140.f)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

bool SongInfoPopup::setup() {
    setTitle("Current song:");
    setID("SongInfoPopup"_spr);

    m_currentSongLabel = cocos2d::CCLabelBMFont::create("...", "bigFont.fnt");
    m_currentSongLabel->setID("current-song-label");
    m_mainLayer->addChildAtPosition(m_currentSongLabel, geode::Anchor::Center);

    m_albumCoverSprite = cocos2d::CCSprite::create("dialogIcon_018.png");
    m_albumCoverSprite->setID("album-cover");
    m_mainLayer->addChildAtPosition(m_albumCoverSprite, geode::Anchor::Left);

    scheduleUpdate();

    return true;
}

void SongInfoPopup::update(float dt) {
    auto currentSong = AudioManager::get().getCurrentSong();
    if (currentSong != m_lastSongDisplayed) {
        m_currentSongLabel->setString(currentSong->m_name.c_str());
        m_currentSongLabel->limitLabelWidth(300.f, 1.f, .1f);

        m_albumCoverSprite->removeFromParent();
        if (currentSong->m_albumCover) {
            geode::log::info("Album cover found!!");
            m_albumCoverSprite = cocos2d::CCSprite::createWithTexture(currentSong->m_albumCover);
        } else {
            geode::log::debug("No album cover found, using fallback...");
            m_albumCoverSprite = cocos2d::CCSprite::create("dialogIcon_018.png");
        }

        m_albumCoverSprite->setID("album-cover");
        m_mainLayer->addChildAtPosition(m_albumCoverSprite, geode::Anchor::Left);

        m_lastSongDisplayed = currentSong;
    }

}
