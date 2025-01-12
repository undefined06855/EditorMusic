#include "CurrentSongNode.hpp"
#include "SongInfoPopup.hpp"
#include "../AudioManager.hpp"
#include "../utils.hpp"

CurrentSongNode* CurrentSongNode::create() {
    auto ret = new CurrentSongNode;
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

bool CurrentSongNode::init() {
    if (!CCNode::init()) return false;

    m_lastSongName = "if this is a song name that you actually have a song for this will break editormusic i think";
    m_currentSongButton = nullptr;

    setContentSize({200, 40});
    setAnchorPoint({.5f, 1.f});
    setID("CurrentSongNode"_spr);

    auto audioManager = AudioManager::get();

    m_mainLayer = cocos2d::CCLayer::create();
    m_mainLayer->setID("main-layer");
    m_mainLayer->setAnchorPoint({ 0.5f, 0.f });
    m_mainLayer->setContentSize(getContentSize());
    addChildAtPosition(m_mainLayer, geode::Anchor::Bottom);

    auto bg = cocos2d::extension::CCScale9Sprite::create("GJ_square01.png");
    bg->setID("background");
    bg->setContentSize({ getContentWidth(), getContentHeight() * 2.f});
    m_mainLayer->addChildAtPosition(bg, geode::Anchor::Top);

    auto label = cocos2d::CCLabelBMFont::create("Current Song:", "bigFont.fnt");
    label->setID("label");
    label->setScale(0.27f);
    m_mainLayer->addChildAtPosition(label, geode::Anchor::Top, {0.f, -6.f});

    m_buttonMenu = cocos2d::CCMenu::create();
    m_buttonMenu->setID("button-menu");
    m_buttonMenu->ignoreAnchorPointForPosition(false);
    m_buttonMenu->setContentSize(getContentSize());
    m_mainLayer->addChildAtPosition(m_buttonMenu, geode::Anchor::Center);

    // TODO: allow using original format of a menu button to access song info

    updateLabelStuffs();

    m_hintSprite = cocos2d::CCSprite::create("click-for-song-info.png"_spr);
    m_hintSprite->setID("song-info-hint");
    m_hintSprite->setVisible(!geode::Mod::get()->getSavedValue("hide-song-info-hint", false));
    m_hintSprite->setScale(.8f);
    m_mainLayer->addChildAtPosition(m_hintSprite, geode::Anchor::BottomRight);

    scheduleUpdate();

    return true;
}

void CurrentSongNode::onButton(cocos2d::CCObject* sender) {
    geode::Mod::get()->setSavedValue("hide-song-info-hint", true);
    SongInfoPopup::create()->show();
}

void CurrentSongNode::update(float dt) {
    updateLabelStuffs();
}

void CurrentSongNode::updateLabelStuffs() {
    auto currentSongName = AudioManager::get().getCurrentSong()->m_name;
    if (m_lastSongName == currentSongName) return;

    if (m_currentSongButton) m_currentSongButton->removeFromParent();

    m_currentSongLabel = cocos2d::CCLabelBMFont::create(currentSongName.c_str(), "bigFont.fnt");
    m_currentSongLabel->limitLabelWidth(178.f, 1.f, .1f);
    m_currentSongLabel->setID("current-song-label");

    m_currentSongButton = CCMenuItemSpriteExtra::create(m_currentSongLabel, this, menu_selector(CurrentSongNode::onButton));
    m_currentSongButton->setID("current-song-button");
    m_buttonMenu->addChildAtPosition(m_currentSongButton, geode::Anchor::Center);

    m_lastSongName = currentSongName;
}
