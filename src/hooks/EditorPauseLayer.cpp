#include "EditorPauseLayer.hpp"
#include "../AudioManager.hpp"
#include "../ui/CurrentSongNode.hpp"
#include "../log.hpp"

void HookedEditorPauseLayer::onModify(auto& self) {
    if (!self.setHookPriorityAfterPost("EditorPauseLayer::init", "hjfod.betteredit")) {
        em::log::warn("Hook priority could not be set - will cause incompats with betteredit!");
    }
}

void HookedEditorPauseLayer::onExitEditor(cocos2d::CCObject* sender) {
    em::log::info("Exited editor, onExitEditor");
    AudioManager::get().exitEditor();
    EditorPauseLayer::onExitEditor(sender);
}

void HookedEditorPauseLayer::onSaveAndExit(cocos2d::CCObject* sender) {
    em::log::info("Exited editor, onSaveAndExit");
    AudioManager::get().exitEditor();
    EditorPauseLayer::onSaveAndExit(sender);
}

void HookedEditorPauseLayer::onSaveAndPlay(cocos2d::CCObject* sender) {
    em::log::info("Exited editor, onSaveAndPlay");
    AudioManager::get().exitEditor();
    EditorPauseLayer::onSaveAndPlay(sender);
}

bool HookedEditorPauseLayer::init(LevelEditorLayer* editor) {
    if (!EditorPauseLayer::init(editor)) return false;

    auto layer = cocos2d::CCLayer::create();
    layer->setID("current-song-layer"_spr);
    layer->setPosition(getContentSize() / 2);
    addChild(layer);

    layer->addChildAtPosition(CurrentSongNode::create(), geode::Anchor::Top);

    // make sure our ui doesnt obscure anything
    // this doesnt actually seem to work but whatever
    auto menu = getChildByID("info-menu");

    auto objLabel = static_cast<cocos2d::CCLabelBMFont*>(menu->getChildByID("object-count-label"));
    auto lengthLabel = static_cast<cocos2d::CCLabelBMFont*>(menu->getChildByID("length-label"));
    if (objLabel && lengthLabel) {
        objLabel->limitLabelWidth(175.f, 0.5f, .1f);
        lengthLabel->limitLabelWidth(175.f, 0.5f, .1f);
    }

    return true;
}
