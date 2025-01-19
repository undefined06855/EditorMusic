#include "PreloadUI.hpp"

PreloadUI* PreloadUI::create() {
    auto ret = new PreloadUI;
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

bool PreloadUI::init() {
    if (!CCNode::init()) return false;
    setID("PreloadUI"_spr);

    m_totalSongs = 0;
    m_songsLoaded = 0;

    setContentSize({230, 55});
    setAnchorPoint({0.5f, 0.5f});
    setCascadeOpacityEnabled(true);    

    auto bg = cocos2d::extension::CCScale9Sprite::create("GJ_square01.png");
    bg->setID("background");
    bg->setContentSize(getContentSize());
    addChildAtPosition(bg, geode::Anchor::Center);

    auto title = cocos2d::CCLabelBMFont::create("Loading songs...", "bigFont.fnt");
    title->setID("title");
    title->setScale(0.35f);
    addChildAtPosition(title, geode::Anchor::Top, { 0.f, -10.f });

    m_progressLabel = cocos2d::CCLabelBMFont::create("...", "bigFont.fnt");
    m_progressLabel->setID("loading-progress");
    m_progressLabel->setScale(0.75f); // initial scale, will be overwritten later
    addChildAtPosition(m_progressLabel, geode::Anchor::Center);

    m_progressBar = ProgressBar::create(200.f);
    m_progressBar->setID("loading-progress-bar");
    addChildAtPosition(m_progressBar, geode::Anchor::Bottom, { 0.f, 11.f });

    m_subtitleLabel = cocos2d::CCLabelBMFont::create("...", "bigFont.fnt");
    m_subtitleLabel->setID("loading-progress-subtitle");
    m_subtitleLabel->setScale(0.4f);
    addChildAtPosition(m_subtitleLabel, geode::Anchor::Bottom, { 0.f, 12.f });

    float pad = 10.f;
    float sceneTop = cocos2d::CCScene::get()->getContentHeight();
    float left = pad + getContentWidth() / 2.f;
    m_animUpperPos = ccp(left, sceneTop + pad + getContentHeight() / 2.f);
    m_animLowerPos = ccp(left, sceneTop - pad - getContentHeight() / 2.f);

    return true;
}

void PreloadUI::addToSceneAndAnimate() {
    auto scene = cocos2d::CCScene::get();
    scene->addChild(this);
    geode::SceneManager::get()->keepAcrossScenes(this);

    setPosition(m_animUpperPos);
    runAction(
        cocos2d::CCSpawn::createWithTwoActions(
            cocos2d::CCFadeIn::create(.5f),
            cocos2d::CCEaseExponentialOut::create(cocos2d::CCMoveTo::create(.5f, m_animLowerPos))
        )
    );
}

void PreloadUI::increment(std::string lastSongLoadedName) {
    m_songsLoaded++;
    m_progressLabel->setString(lastSongLoadedName.c_str());
    m_progressLabel->limitLabelWidth(190.f, .5f, 0.1f);
    m_subtitleLabel->setString(fmt::format("{}/{}", m_songsLoaded, m_totalSongs).c_str());
    m_progressBar->setValue((float)m_songsLoaded / (float)m_totalSongs);
}

void PreloadUI::runCompleteAnimationAndRemove() {
    if (m_totalSongs > 0) {
        m_progressLabel->setString("Song loading complete!");
        m_progressLabel->limitLabelWidth(190.f, .7f, 0.1f);
        m_subtitleLabel->setString(fmt::format("{}/{}", m_songsLoaded, m_totalSongs).c_str());
    } else {
        m_progressLabel->setString("No songs loaded!");
        m_progressLabel->limitLabelWidth(190.f, .7f, 0.1f);
        m_subtitleLabel->setString("");
    }

    geode::SceneManager::get()->forget(this);

    if (!cocos2d::CCScene::get()) {
        removeFromParent();
        return;
    }

    runAction(
        cocos2d::CCSequence::create(
            cocos2d::CCDelayTime::create(1.f),
            cocos2d::CCSpawn::createWithTwoActions(
                cocos2d::CCFadeOut::create(.5f),
                cocos2d::CCEaseExponentialIn::create(cocos2d::CCMoveTo::create(.5f, m_animUpperPos))
            ),
            cocos2d::CCDelayTime::create(.5f),
            cocos2d::CCRemoveSelf::create(),
            NULL
        )
    );
}
