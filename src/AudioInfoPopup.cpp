#include <Geode/ui/GeodeUI.hpp>
#include "AudioInfoPopup.hpp"
#include "AudioManager.hpp"
#include "Utils.hpp"

bool AudioInfoPopup::setup() {
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    this->setTitle("Current song:");

    this->btnMenu = CCMenu::create();
    this->m_mainLayer->addChild(this->btnMenu);

    // also gets set in tick
    auto label = CCLabelBMFont::create("", "bigFont.fnt");
    label->setID("song-label"_spr);
    label->setPosition(CCPoint{ winSize.width / 2, (winSize.height / 2) + 15.f });
    label->setScale(AudioManager::get().desiredPopupScale);
    this->m_mainLayer->addChild(label);

    // this gets set in tick
    auto lengthLabel = CCLabelBMFont::create("", "chatFont.fnt");
    lengthLabel->setID("length-label"_spr);
    lengthLabel->setPosition(CCPoint{ winSize.width / 2, (winSize.height / 2) - 10.f });
    lengthLabel->setScale(.725f);
    lengthLabel->setColor(ccColor3B{ 40, 40, 40 });
    this->m_mainLayer->addChild(lengthLabel);
    

    auto prevSongSprite = CCSprite::createWithSpriteFrameName("navArrowBtn_001.png");
    prevSongSprite->setScale(.69f);
    prevSongSprite->setFlipX(true);

    auto prevSongBtn = CCMenuItemSpriteExtra::create(prevSongSprite, this, menu_selector(AudioInfoPopup::onPrevBtn));
    prevSongBtn->setID("prev-song-btn"_spr);
    prevSongBtn->setPosition(CCPoint{ -150.f, 15.f });
    this->btnMenu->addChild(prevSongBtn);


    auto nextSongSprite = CCSprite::createWithSpriteFrameName("navArrowBtn_001.png");
    nextSongSprite->setScale(.69f);

    auto nextSongBtn = CCMenuItemSpriteExtra::create(nextSongSprite, this, menu_selector(AudioInfoPopup::onNextBtn));
    nextSongBtn->setID("next-song-btn"_spr);
    nextSongBtn->setPosition(CCPoint{ 150.f, 15.f });
    this->btnMenu->addChild(nextSongBtn);


    auto settingsSprite = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
    settingsSprite->setScale(152.f / 203.f); // 152 = pause button sprite height, 203 = options button height

    auto settingsBtn = CCMenuItemSpriteExtra::create(settingsSprite, this, menu_selector(AudioInfoPopup::onSettingsButton));
    settingsBtn->setID("settings-btn"_spr);
    settingsBtn->setPosition(CCPoint{ 153.f, -40.f });
    this->btnMenu->addChild(settingsBtn);


    auto playSprite = CCSprite::createWithSpriteFrameName("GJ_playMusicBtn_001.png");
    auto pauseSprite = CCSprite::createWithSpriteFrameName("GJ_pauseBtn_001.png");
    
    bool amIsPaused;
    AudioManager::get().channel->getPaused(&amIsPaused);
    auto playPauseBtn = CCMenuItemSpriteExtra::create(amIsPaused ? playSprite : pauseSprite, this, menu_selector(AudioInfoPopup::onPlayPause));
    playPauseBtn->setID("play-pause-btn"_spr);
    playPauseBtn->setPosition(CCPoint{ 0, -40 });
    this->btnMenu->addChild(playPauseBtn);


    auto restartSprite = CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png");
    restartSprite->setScale(152.f / 184.f); // see above

    auto restartButton = CCMenuItemSpriteExtra::create(restartSprite, this, menu_selector(AudioInfoPopup::onSongRestart));
    restartButton->setID("restart-btn"_spr);
    restartButton->setPosition(CCPoint{ -153.f, -40.f });
    this->btnMenu->addChild(restartButton);


    auto rewindSprite = CCSprite::createWithSpriteFrameName("edit_leftBtn2_001.png");
    rewindSprite->setAnchorPoint(CCPoint{ 0.f, 0.f });
    auto rewindLabel = CCLabelBMFont::create("10s", "bigFont.fnt");
    rewindLabel->setPosition(CCPoint{ 13.5f, 10.f });
    rewindLabel->setScale(.2f);
    rewindSprite->addChild(rewindLabel);

    auto rewindButton = CCMenuItemSpriteExtra::create(rewindSprite, this, menu_selector(AudioInfoPopup::onRw10Sec));
    rewindButton->setID("rewind-btn"_spr);
    rewindButton->setPosition(CCPoint{ -40.f, -40.f });
    this->btnMenu->addChild(rewindButton);


    auto fastfSprite = CCSprite::createWithSpriteFrameName("edit_rightBtn2_001.png");
    fastfSprite->setAnchorPoint(CCPoint{ 0.f, 0.f });
    auto fastfLabel = CCLabelBMFont::create("10s", "bigFont.fnt");
    fastfLabel->setPosition(CCPoint{ 11.f, 10.f });
    fastfLabel->setScale(.2f);
    fastfSprite->addChild(fastfLabel);

    auto fastfButton = CCMenuItemSpriteExtra::create(fastfSprite, this, menu_selector(AudioInfoPopup::onFF10Sec));
    fastfButton->setID("fast-forward-btn"_spr);
    fastfButton->setPosition(CCPoint{ 40.f, -40.f });
    this->btnMenu->addChild(fastfButton);


    this->setID("audio-info-popup"_spr);
    return true;
}

AudioInfoPopup* AudioInfoPopup::create() {
    auto ret = new AudioInfoPopup();
    if (ret && ret->init(360.f, 140.f)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

void AudioInfoPopup::onPrevBtn(CCObject* sender) {
    log::info("prev");
    AudioManager::get().prevSong();
}

void AudioInfoPopup::onNextBtn(CCObject* sender) {
    log::info("next");
    AudioManager::get().nextSong();
}

void AudioInfoPopup::onSettingsButton(CCObject* sender) {
    log::info("Settings");
    openSettingsPopup(Mod::get());
}

void AudioInfoPopup::onPlayPause(CCObject* sender) {
    log::info("play / pause");
    bool amIsPaused;
    AudioManager::get().channel->getPaused(&amIsPaused);

    auto btn = static_cast<CCMenuItemSpriteExtra*>(this->btnMenu->getChildByID("play-pause-btn"_spr));

    if (amIsPaused) {
        log::info("was paused");
        AudioManager::get().play();
        btn->setSprite(CCSprite::createWithSpriteFrameName("GJ_pauseBtn_001.png"));
    } else {
        log::info("was playing");
        AudioManager::get().pause();
        btn->setSprite(CCSprite::createWithSpriteFrameName("GJ_playMusicBtn_001.png"));
    }

    btn->updateSprite();
}

void AudioInfoPopup::onSongRestart(CCObject* sender) {
    log::info("restartt song");
    AudioManager::get().channel->setPosition(0.0f, FMOD_TIMEUNIT_MS);
}

void AudioInfoPopup::onRw10Sec(CCObject* sender) {
    log::info("it's rewind time");
    unsigned int curPos;
    AudioManager::get().channel->getPosition(&curPos, FMOD_TIMEUNIT_MS);
    if (curPos < 100000) AudioManager::get().prevSong();
    else                 AudioManager::get().channel->setPosition(curPos - 10000, FMOD_TIMEUNIT_MS);
}

void AudioInfoPopup::onFF10Sec(CCObject* sender) {
    log::info("fast forward");
    unsigned int curPos;
    AudioManager::get().channel->getPosition(&curPos, FMOD_TIMEUNIT_MS);
    if (curPos > AudioManager::get().currentSongLength - 10000) AudioManager::get().playNewSong();
    else                                                        AudioManager::get().channel->setPosition(curPos + 10000, FMOD_TIMEUNIT_MS);
}

void AudioInfoPopup::tick() {
    auto songTitle = static_cast<CCLabelBMFont*>(this->m_mainLayer->getChildByID("song-label"_spr));
    songTitle->setString(AudioManager::get().currentSongName.c_str());
    songTitle->setScale(AudioManager::get().desiredPopupScale);

    auto lengthString = static_cast<CCLabelBMFont*>(this->m_mainLayer->getChildByID("length-label"_spr));
    lengthString->setString(
        fmt::format(
            "{}/{}",
            Utils::formatTime(static_cast<int>(AudioManager::get().getSongMS() / 1000)).c_str(),
            Utils::formatTime(static_cast<int>(AudioManager::get().currentSongLength / 1000)).c_str()
        ).c_str()
    );
}
