#include "LevelEditorLayer.hpp"
#ifndef GEODE_IS_MACOS
#include <geode.custom-keybinds/include/Keybinds.hpp>
#endif
#include "../AudioManager.hpp"
#include "../log.hpp"
#include "../ui/SongInfoPopup.hpp"

bool HookedLevelEditorLayer::init(GJGameLevel* p0, bool p1) {
    if (!LevelEditorLayer::init(p0, p1)) return false;

    auto fields = m_fields.self();
    fields->m_currentSong = nullptr;
    fields->m_songPopupLabel = cocos2d::CCLabelBMFont::create("...", "bigFont.fnt");
    fields->m_songPopupLabel->setID("song-popup-label"_spr);
    fields->m_songPopupLabel->setScale(.35f);
    fields->m_songPopupLabel->setZOrder(120);
    fields->m_songPopupLabel->setPosition({
        cocos2d::CCScene::get()->getContentWidth() / 2.f,
        cocos2d::CCScene::get()->getContentHeight()
    });
    m_editorUI->addChild(fields->m_songPopupLabel);

    // smh
    bool isLikelyObjectWorkshop = \
           geode::Loader::get()->getLoadedMod("firee.object-workshop")
        && cocos2d::CCScene::get()->getChildByType<ProfilePage>(0);

    if (!isLikelyObjectWorkshop) AudioManager::get().enterEditor();
    else em::log::debug("prevented enterEditor because likely object workshop!");
    
#ifndef GEODE_IS_MACOS
    addEventListener<keybinds::InvokeBindFilter>([](keybinds::InvokeBindEvent* event) {
        if (event->isDown()) {
            em::log::debug("Keybind: Pause");
            AudioManager::get().togglePause();
        }

        return geode::ListenerResult::Propagate;
    }, "pause-song"_spr);

    addEventListener<keybinds::InvokeBindFilter>([](keybinds::InvokeBindEvent* event) {
        if (event->isDown()) {
            em::log::debug("Keybind: Next song");
            AudioManager::get().nextSong();
        }

        return geode::ListenerResult::Propagate;
    }, "next-song"_spr);

    addEventListener<keybinds::InvokeBindFilter>([](keybinds::InvokeBindEvent* event) {
        if (event->isDown()) {
            em::log::debug("Keybind: Prev song");
            AudioManager::get().prevSong();
        }

        return geode::ListenerResult::Propagate;
    }, "prev-song"_spr);
#endif

    return true;
}

void HookedLevelEditorLayer::updateEditor(float dt) {
    LevelEditorLayer::updateEditor(dt);

    auto currentSong = AudioManager::get().getCurrentSong();
    auto fields = m_fields.self();
    if (currentSong == fields->m_currentSong) return;
    fields->m_currentSong = currentSong;

    if (!geode::Mod::get()->getSettingValue<bool>("next-song-notif") || SongInfoPopup::get()) return;

    // song just changed, set label + run anim
    fields->m_songPopupLabel->setString(fmt::format("Now Playing: {}", currentSong->getCombinedSongName()).c_str());

    // using this isntead of numberOfRunningActions because it;s either inlined or
    // not found for imac and i cant be bothered to wait
    if (fields->m_songPopupLabel->m_pActionManager->numberOfRunningActionsInTarget(fields->m_songPopupLabel) > 0) return;
    fields->m_songPopupLabel->runAction(
        cocos2d::CCSequence::create(
            cocos2d::CCSpawn::createWithTwoActions(
                cocos2d::CCFadeIn::create(.5f),
                cocos2d::CCEaseExponentialOut::create(cocos2d::CCMoveBy::create(.5f, { 0.f, -15.f }))
            ),
            cocos2d::CCDelayTime::create(.7f),
            cocos2d::CCSpawn::createWithTwoActions(
                cocos2d::CCFadeOut::create(.5f),
                cocos2d::CCEaseExponentialIn::create(cocos2d::CCMoveBy::create(.5f, { 0.f, 15.f }))
            ),
            NULL
        )
    );
}
