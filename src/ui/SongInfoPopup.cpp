#include "SongInfoPopup.hpp"
#include <Geode/ui/GeodeUI.hpp>
#include "../AudioManager.hpp"
#include "../utils.hpp"

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

    m_albumCoverSprite = nullptr;

    m_currentSongLabel = cocos2d::CCLabelBMFont::create("...", "bigFont.fnt");
    m_currentSongLabel->setID("current-song-label");
    m_currentSongLabel->setAnchorPoint({ 0.f, .5f });
    m_mainLayer->addChildAtPosition(m_currentSongLabel, geode::Anchor::Left, { 87.f, -6.5f });

    m_currentSongArtistLabel = cocos2d::CCLabelBMFont::create("...", "bigFont.fnt");
    m_currentSongArtistLabel->setID("current-song-artist-label");
    m_currentSongArtistLabel->setAnchorPoint({ 0.f, .5f });
    m_mainLayer->addChildAtPosition(m_currentSongArtistLabel, geode::Anchor::Left, { 87.f, -20.f });

    // TODO: allow hiding album cover - probably by moving m_currentSongLabel
    // and artist label to the centre and making them centred? maybe also allow
    // for that but also with the album cover by putting the album cover behind
    // the labels

    m_progressBar = ProgressBar::create(345.f);
    m_progressBar->setID("song-progress-bar");
    m_mainLayer->addChildAtPosition(m_progressBar, geode::Anchor::Bottom, { 0.f, 27.f });

    m_progressLabel = cocos2d::CCLabelBMFont::create("00:00/00:00", "chatFont.fnt");
    m_progressLabel->setID("song-progress-label");
    m_progressLabel->setScale(.61f);
    m_progressLabel->setAnchorPoint({0.f, .5f});
    m_mainLayer->addChildAtPosition(m_progressLabel, geode::Anchor::BottomLeft, { 7.5f, 13.f });


    auto playPauseButton = CCMenuItemToggler::create(
        cocos2d::CCSprite::createWithSpriteFrameName("GJ_pauseBtn_001.png"),
        cocos2d::CCSprite::createWithSpriteFrameName("GJ_playMusicBtn_001.png"),
        this, menu_selector(SongInfoPopup::onPlayPause)
    );
    playPauseButton->setID("play-pause-btn");
    playPauseButton->toggle(AudioManager::get().m_isPaused);
    m_buttonMenu->addChildAtPosition(playPauseButton, geode::Anchor::Bottom);

    auto prevSprite = geode::CircleButtonSprite::createWithSpriteFrameName("edit_leftBtn2_001.png");
    auto leftSpr = prevSprite->getTopNode();
    leftSpr->setPositionX(leftSpr->getPositionX() - 2.f);
    leftSpr->setScale(leftSpr->getScale() * 0.84);
    prevSprite->setScale(.6f);
    auto prevButton = CCMenuItemSpriteExtra::create(prevSprite, this, menu_selector(SongInfoPopup::onPrev));
    prevButton->setID("prev-button");
    m_buttonMenu->addChildAtPosition(prevButton, geode::Anchor::Bottom, { -33.f, 0.f });

    auto nextSprite = geode::CircleButtonSprite::createWithSpriteFrameName("edit_rightBtn2_001.png");
    auto rightSpr = nextSprite->getTopNode();
    rightSpr->setPositionX(rightSpr->getPositionX() + 2.f);
    rightSpr->setScale(rightSpr->getScale() * 0.84);
    nextSprite->setScale(.6f);
    auto nextButton = CCMenuItemSpriteExtra::create(nextSprite, this, menu_selector(SongInfoPopup::onNext));
    nextButton->setID("next-button");
    m_buttonMenu->addChildAtPosition(nextButton, geode::Anchor::Bottom, { 33.f, 0.f });

    auto rwTopSprite = cocos2d::CCSprite::createWithSpriteFrameName("edit_ccwBtn_001.png");
    auto rwTopLabel = cocos2d::CCLabelBMFont::create("10s", "bigFont.fnt");
    rwTopLabel->setScale(.3f);
    rwTopSprite->addChildAtPosition(rwTopLabel, geode::Anchor::Center);
    auto rwSprite = geode::CircleButtonSprite::create(rwTopSprite, geode::CircleBaseColor::Green, geode::CircleBaseSize::Small);
    rwSprite->setScale(.575f);
    auto rwButton = CCMenuItemSpriteExtra::create(rwSprite, this, menu_selector(SongInfoPopup::onRewind));
    rwButton->setID("rewind-button");
    m_buttonMenu->addChildAtPosition(rwButton, geode::Anchor::Bottom, { -72.f, 0.f });

    auto ffTopSprite = cocos2d::CCSprite::createWithSpriteFrameName("edit_cwBtn_001.png");
    auto ffTopLabel = cocos2d::CCLabelBMFont::create("10s", "bigFont.fnt");
    ffTopLabel->setScale(.3f);
    ffTopSprite->addChildAtPosition(ffTopLabel, geode::Anchor::Center);
    auto ffSprite = geode::CircleButtonSprite::create(ffTopSprite, geode::CircleBaseColor::Green, geode::CircleBaseSize::Small);
    ffSprite->setScale(.575f);
    auto ffButton = CCMenuItemSpriteExtra::create(ffSprite, this, menu_selector(SongInfoPopup::onFastForward));
    ffButton->setID("fast-forward-button");
    m_buttonMenu->addChildAtPosition(ffButton, geode::Anchor::Bottom, { 72.f, 0.f });


    auto settingsSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
    settingsSprite->setScale(.6f);
    auto settingsButton = CCMenuItemSpriteExtra::create(settingsSprite, this, menu_selector(SongInfoPopup::onSettings));
    settingsButton->setID("settings-btn");
    m_buttonMenu->addChildAtPosition(settingsButton, geode::Anchor::TopRight);



    m_loadingCircle = LoadingCircleSprite::create(1.f);
    m_loadingCircle->setID("loading-circle");
    m_mainLayer->addChildAtPosition(m_loadingCircle, geode::Anchor::Center);

    // our layouting fucks this up so we need to remove and readd
    // might as well add ids here as well
    m_closeBtn->removeFromParent();
    m_closeBtn->setID("close-btn");
    m_buttonMenu->addChildAtPosition(m_closeBtn, geode::Anchor::TopLeft);
    m_buttonMenu->setID("button-menu");
    m_title->setID("title");

    update(0.f);
    scheduleUpdate();

    return true;
}

void SongInfoPopup::update(float dt) {
    auto audioManager = AudioManager::get();

    m_loadingCircle->setVisible(audioManager.m_playCurrentSongQueuedForLoad);

    m_progressBar->setVisible(audioManager.getCurrentSongLength() != 0);
    if (m_progressBar->isVisible()) m_progressBar->setValue((float)audioManager.getCurrentSongPosition() / (float)audioManager.getCurrentSongLength());

    auto left = em::utils::formatTime(audioManager.getCurrentSongPosition());
    auto right = em::utils::formatTime(audioManager.getCurrentSongLength());
    m_progressLabel->setString(fmt::format("{}/{}", left, right).c_str());

    // anything below here will only run every time the song gets updated and not
    // every frame
    auto currentSong = audioManager.getCurrentSong();
    if (currentSong == m_currentSong) return;
    m_currentSong = currentSong;


    m_currentSongLabel->setString(m_currentSong->m_name.c_str());
    m_currentSongLabel->limitLabelWidth(250.f, .5f, 0.f);

    m_currentSongArtistLabel->setString(m_currentSong->m_artist.c_str());
    m_currentSongArtistLabel->limitLabelWidth(150.f, .4f, 0.f);

    updateAlbumCover();
}

void SongInfoPopup::updateAlbumCover() {
    if (m_albumCoverSprite) m_albumCoverSprite->removeFromParent();

    if (m_currentSong->m_albumCover) {
        geode::log::debug("Album cover found!!");
        m_albumCoverSprite = cocos2d::CCSprite::createWithTexture(m_currentSong->m_albumCover);
    } else {
        geode::log::debug("No album cover found, using fallback...");
        m_albumCoverSprite = cocos2d::CCSprite::create("album-placeholder.png"_spr);
    }

    if (m_albumCoverSprite->getContentWidth() >= m_albumCoverSprite->getContentHeight()) {
        auto width = m_albumCoverSprite->getContentWidth();
        m_albumCoverSprite->setScale(70.f / width);
    } else {
        auto height = m_albumCoverSprite->getContentHeight();
        m_albumCoverSprite->setScale(70.f / height);
    }

    m_albumCoverSprite->setID("album-cover");
    m_albumCoverSprite->setAnchorPoint({ 0.f, 0.5f });
    m_mainLayer->addChildAtPosition(m_albumCoverSprite, geode::Anchor::Left, { 10.f, 10.f });
}

void SongInfoPopup::onPlayPause(cocos2d::CCObject* sender) { AudioManager::get().m_isPaused = !AudioManager::get().m_isPaused; }
void SongInfoPopup::onPrev(cocos2d::CCObject* sender) { AudioManager::get().prevSong(); }
void SongInfoPopup::onNext(cocos2d::CCObject* sender) { AudioManager::get().nextSong(); }
void SongInfoPopup::onRewind(cocos2d::CCObject* sender) { AudioManager::get().rewind(); }
void SongInfoPopup::onFastForward(cocos2d::CCObject* sender) { AudioManager::get().fastForward(); }

void SongInfoPopup::onSettings(cocos2d::CCObject* sender) { geode::openSettingsPopup(geode::Mod::get(), false); }
