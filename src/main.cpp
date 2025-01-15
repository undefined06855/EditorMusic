#include "AudioManager.hpp"
#include "ui/SongInfoPopup.hpp"
#include "settings/ReloadSongSetting.hpp"
#include "eclipse.hpp"

$on_mod(Loaded) {
    if (!geode::Mod::get()->getSettingValue<bool>("menulayer-load")) AudioManager::get().init();

    geode::listenForSettingChanges("low-pass", [](bool value) {
        auto am = AudioManager::get();

        if (value) am.updateLowPassFilter();
        else       am.m_channel->removeDSP(am.m_lowPassFilter);
    });

    geode::listenForSettingChanges("player-format", [](std::string value) {
        auto popup = SongInfoPopup::get();
        if (popup) popup->updateCustomizableUI();
    });

    geode::listenForSettingChanges("round-corners", [](std::string value) {
        auto popup = SongInfoPopup::get();
        if (popup) popup->updateCustomizableUI();
    });

    geode::listenForSettingChanges("extra-songs-path", [](std::filesystem::path value) {
        AudioManager::get().populateSongs();
    });

    (void)geode::Mod::get()->registerCustomSettingType("reload-song-button", &ReloadSongSetting::parse);

    em::rift_labels::g_isEclipseLoaded = geode::Loader::get()->isModLoaded("eclipse.eclipse-menu");
    if (em::rift_labels::g_isEclipseLoaded) geode::log::debug("hi eclipse nice to meet you i have some variables for you");
}


// PreloadUI testing stuff
// #include <Geode/modify/MenuLayer.hpp>
// class $modify(MenuLayer) {
//     struct Fields {
//         PreloadUI* m_preloadUI = nullptr;
//         int m_lastSongID = 0;
//     };

//     bool init() {
//         if (!MenuLayer::init()) return false;
//         m_fields->m_preloadUI = PreloadUI::create();
//         m_fields->m_preloadUI->addToSceneAndAnimate();
//         return true;
//     }

//     void onDaily(cocos2d::CCObject*) {
//         m_fields->m_preloadUI->m_totalSongs++;
//     }

//     void onStats(cocos2d::CCObject*) {
//         std::array<std::string, 6> songNames = {
//             "robert topala.mp3",
//             "Back on Track",
//             "idk song name",
//             "besyfursgligojrvnger",
//             "imagine this is a song name but like really long and really silly",
//             "a"
//         };

//         m_fields->m_preloadUI->increment(songNames[m_fields->m_lastSongID++]);
//         if (m_fields->m_lastSongID > 5) m_fields->m_lastSongID = 0;
//     }

//     void onGarage(cocos2d::CCObject*) {
//         m_fields->m_preloadUI->runCompleteAnimationAndRemove();
//     }
// };
