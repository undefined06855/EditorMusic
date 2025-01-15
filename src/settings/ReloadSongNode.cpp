#include "ReloadSongNode.hpp"
#include "ReloadSongSetting.hpp"
#include "../AudioManager.hpp"

ReloadSongNode* ReloadSongNode::create(std::shared_ptr<ReloadSongSetting> setting, float width) {
    auto ret = new ReloadSongNode;
    if (ret->init(setting, width)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

bool ReloadSongNode::init(std::shared_ptr<ReloadSongSetting> setting, float width) {
    if (!SettingNodeV3::init(setting, width)) return false;
    
    auto spr = cocos2d::CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png");
    spr->setScale(.5f);
    auto btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(ReloadSongNode::onButton));
    getButtonMenu()->addChildAtPosition(btn, geode::Anchor::Center);

    updateState(nullptr);

    return true;
}

void ReloadSongNode::onButton(cocos2d::CCObject* sender) {
    AudioManager::get().populateSongs();
}

void ReloadSongNode::onCommit() {}
void ReloadSongNode::onResetToDefault() {}
bool ReloadSongNode::hasUncommittedChanges() const { return false; }
bool ReloadSongNode::hasNonDefaultValue() const { return false; }
