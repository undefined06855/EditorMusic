#include "EditorPauseLayer.hpp"
#include "../AudioManager.hpp"
#include "../ui/CurrentSongNode.hpp"
#include "../ui/SongInfoPopup.hpp"
#include "../log.hpp"

void HookedEditorPauseLayer::onModify(auto& self) {
    if (!self.setHookPriorityAfterPost("EditorPauseLayer::init", "hjfod.betteredit")) {
        em::log::warn("Hook priority could not be set - will cause incompats with betteredit!");
    }
}

void HookedEditorPauseLayer::onResume(cocos2d::CCObject* sender) {
    // dont allow exiting if the song info popup is on screen
    // in fact this will be when the user presses space, so toggle pause
    if (auto pop = SongInfoPopup::get()) {
        pop->m_playPauseButton->activate();
        return;
    }

    EditorPauseLayer::onResume(sender);
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


    // move label from song stopped at
    // id isnt prefixed with modid but checking both just in case it does in the
    // future

    auto node = getChildByID("song-stopped-at");
    if (!node) {
        node = getChildByID("aceinetx.song_stopped_at/song-stopped-at");
    }

    if (node) {
        node->setPositionY(269.f);
    }

    return true;
}
