#include "SongInfoPopup.hpp"
#include <Geode/ui/GeodeUI.hpp>
#include "../AudioManager.hpp"
#include "../utils.hpp"
#include "../log.hpp"

SongInfoPopup* SongInfoPopup::create() {
    auto ret = new SongInfoPopup;
    if (ret->initAnchored(360.f, 140.f)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

SongInfoPopup* SongInfoPopup::get() {
    return cocos2d::CCScene::get()->getChildByType<SongInfoPopup>(0);
}

bool SongInfoPopup::setup() {
    setTitle("Current song:");
    setID("SongInfoPopup"_spr);

    m_isInStrippedBackMode = false;

    auto am = AudioManager::get();
    if (am.m_songs.empty()) {
        m_isInStrippedBackMode = true;

        auto noticeLabel = cocos2d::CCLabelBMFont::create("You have no songs :(", "bigFont.fnt");
        noticeLabel->setScale(.8f);
        noticeLabel->setID("no-songs-notice-label");
        m_mainLayer->addChildAtPosition(noticeLabel, geode::Anchor::Center, { 0.f, 33.f });

        auto secondaryNoticeLabel = cocos2d::CCLabelBMFont::create("You have no songs in either your config or custom folder! Go to the mod settings to open the config folder to place songs in (or to set a custom folder), then press the reload button, or restart the game.", "bigFont.fnt", 240.f);
        secondaryNoticeLabel->setID("no-songs-secondary-notice-label");
        secondaryNoticeLabel->setScale(.3f);
        m_mainLayer->addChildAtPosition(secondaryNoticeLabel, geode::Anchor::Center, { 0.f, -3.f });

        auto reloadButton = CCMenuItemSpriteExtra::create(ButtonSprite::create("Reload"), this, menu_selector(SongInfoPopup::onNoSongsRefresh));
        reloadButton->setID("no-songs-reload-button");
        m_buttonMenu->addChildAtPosition(reloadButton, geode::Anchor::Bottom, { 0.f, 24.f });

        m_closeBtn->removeFromParent();
        m_buttonMenu->addChildAtPosition(m_closeBtn, geode::Anchor::TopLeft);

        auto spr = getRandomSpriteForNoSongPopup();
        spr->setID("no-songs-random-sprite");
        spr->setScale(.5f);
        m_mainLayer->addChildAtPosition(spr, geode::Anchor::Left, { 20.f, 0.f });

        return true;
    }

    if (am.m_isQueueBeingPopulated) {
        m_isInStrippedBackMode = true;

        auto noticeLabel = cocos2d::CCLabelBMFont::create("Songs are being loaded! Come back later once it's finished.", "bigFont.fnt", 300.f);
        noticeLabel->setScale(.5f);
        noticeLabel->setID("songs-populating-notice-label");
        m_mainLayer->addChildAtPosition(noticeLabel, geode::Anchor::Center, { 0.f, 20.f });

        auto silly = cocos2d::CCSprite::create("dialogIcon_018.png");
        silly->setID("songs-populating-silly-image");
        silly->setScale(.6f);
        m_mainLayer->addChildAtPosition(silly, geode::Anchor::Center, { 33.f, -15.f });

        return true;
    }

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

    m_loadingCircle = geode::LoadingSpinner::create(1.f);
    m_loadingCircle->setID("loading-circle");
    m_mainLayer->addChildAtPosition(m_loadingCircle, geode::Anchor::Center);

    // our layouting fucks this up so we need to remove and readd
    // might as well add ids here as well
    m_closeBtn->removeFromParent();
    m_closeBtn->setID("close-btn");
    m_buttonMenu->addChildAtPosition(m_closeBtn, geode::Anchor::TopLeft);
    m_buttonMenu->setID("button-menu");
    m_title->setID("title");
    m_bgSprite->setID("bg-sprite");
    m_bgSprite->setZOrder(-10);

    // adds the rest of the labels
    // can be called from elsewhere to be updated after changing settings
    // which is why its in a different function
    // setting nullptr here to ensure the removing checks correctly work and dont
    // remove nodes that dont exist!
    m_currentSongLabel = nullptr;
    m_currentSongArtistLabel = nullptr;
    m_albumCoverSprite = nullptr;
    updateCustomizableUI();

    scheduleUpdate();

    return true;
}

void SongInfoPopup::updateCustomizableUI() {
    if (m_isInStrippedBackMode) return;
    if (m_currentSongLabel) m_currentSongLabel->removeFromParent();
    if (m_currentSongArtistLabel) m_currentSongArtistLabel->removeFromParent();
    
    m_currentSongLabel = cocos2d::CCLabelBMFont::create("...", "bigFont.fnt");
    m_currentSongLabel->setID("current-song-label");

    m_currentSongArtistLabel = cocos2d::CCLabelBMFont::create("...", "bigFont.fnt");
    m_currentSongArtistLabel->setID("current-song-artist-label");

    if (em::utils::isMusicPlayerCentered()) {
        m_mainLayer->addChildAtPosition(m_currentSongLabel, geode::Anchor::Center, { 0.f, 8.f });
        m_mainLayer->addChildAtPosition(m_currentSongArtistLabel, geode::Anchor::Center, { 0.f, -7.25f });
    } else {
        m_currentSongLabel->setAnchorPoint({ 0.f, .5f });
        m_currentSongArtistLabel->setAnchorPoint({ 0.f, .5f });
        m_mainLayer->addChildAtPosition(m_currentSongLabel, geode::Anchor::Left, { 84.f, -3.f });
        m_mainLayer->addChildAtPosition(m_currentSongArtistLabel, geode::Anchor::Left, { 84.f, -17.25f });
    }

    m_currentSong = nullptr;
    update(0.f);
}

void SongInfoPopup::update(float dt) {
    if (m_isInStrippedBackMode) return;
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
    m_currentSongLabel->limitLabelWidth(265.f, em::utils::isMusicPlayerCentered() ? .7f : .5f, 0.f);

    m_currentSongArtistLabel->setString(m_currentSong->m_artist.c_str());
    m_currentSongArtistLabel->limitLabelWidth(150.f, .4f, 0.f);

    updateAlbumCover();
}

void SongInfoPopup::updateAlbumCover() {
    if (m_isInStrippedBackMode) return;
    if (m_albumCoverSprite) m_albumCoverSprite->removeFromParent();

    if (m_currentSong->m_albumCover) {
        em::log::debug("Album cover found for {}", m_currentSong->m_name);
        m_albumCoverSprite = cocos2d::CCSprite::createWithTexture(m_currentSong->m_albumCover);
    } else {
        em::log::debug("No album cover found for {}, using fallback...", m_currentSong->m_name);
        m_albumCoverSprite = cocos2d::CCSprite::create("album-placeholder.png"_spr);
    }

    // resize to be 70x70 max
    if (m_albumCoverSprite->getContentWidth() >= m_albumCoverSprite->getContentHeight()) {
        auto width = m_albumCoverSprite->getContentWidth();
        m_albumCoverSprite->setScale(70.f / width);
    } else {
        auto height = m_albumCoverSprite->getContentHeight();
        m_albumCoverSprite->setScale(70.f / height);
    }

    m_albumCoverSprite->setID("album-cover");
    m_albumCoverSprite->setZOrder(-1);
    m_albumCoverSprite->setVisible(em::utils::showMusicPlayerAlbumCover());

    if (em::utils::isMusicPlayerCentered()) {
        m_mainLayer->addChildAtPosition(m_albumCoverSprite, geode::Anchor::Center, { 0.f, 3.f });
    } else {
        m_albumCoverSprite->setAnchorPoint({ 0.f, 0.5f });
        m_mainLayer->addChildAtPosition(m_albumCoverSprite, geode::Anchor::Left, { 10.f, 10.f });
    }
}

cocos2d::CCSprite* SongInfoPopup::getRandomSpriteForNoSongPopup() {
    std::array<std::string, 7> potentialSpriteNames = {
        "dialogIcon_029.png", // confused
        "dialogIcon_028.png", // robtop
        "dialogIcon_034.png", // eyes
        "dialogIcon_043.png", // diamond guy
        "dialogIcon_052.png", // choppa
        "dialogIcon_054.png", // wraith
        "dialogIcon_023.png" // potbor annoyed
    };

    return cocos2d::CCSprite::create(potentialSpriteNames[rand() % 7].c_str());
}

void SongInfoPopup::onPlayPause(cocos2d::CCObject* sender) { AudioManager::get().m_isPaused = !AudioManager::get().m_isPaused; }
void SongInfoPopup::onPrev(cocos2d::CCObject* sender) { AudioManager::get().prevSong(); }
void SongInfoPopup::onNext(cocos2d::CCObject* sender) { AudioManager::get().nextSong(); }
void SongInfoPopup::onRewind(cocos2d::CCObject* sender) { AudioManager::get().rewind(); }
void SongInfoPopup::onFastForward(cocos2d::CCObject* sender) { AudioManager::get().fastForward(); }

void SongInfoPopup::onSettings(cocos2d::CCObject* sender) { geode::openSettingsPopup(geode::Mod::get(), false); }

void SongInfoPopup::onNoSongsRefresh(cocos2d::CCObject* sender) {
    AudioManager::get().populateSongs();
    onClose(sender);
}
