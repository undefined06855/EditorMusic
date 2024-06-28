#include <Geode/Geode.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/CCScheduler.hpp>
#include <Geode/loader/SettingEvent.hpp>
#include "AudioManager.hpp"
#include "AudioInfoPopup.hpp"
#include <fmt/core.h>
#include <fmt/color.h>

using namespace geode::prelude;

class $modify(LevelEditorLayer) {
	bool init(GJGameLevel * p0, bool p1) {
		if (!LevelEditorLayer::init(p0, p1)) return false;

		log::info("start music (entered editor)\nisInEditor=true");
		AudioManager::get().playNewSong();
		AudioManager::get().isInEditor = true;

		return true;
	}
};

class $modify(FunkyEditorPauseLayer, EditorPauseLayer) {
	void onExitEditor(CCObject* sender) {
		log::info("isInEditor=false");
		AudioManager::get().isInEditor = false;

		EditorPauseLayer::onExitEditor(sender);
	}

	/*
	// this (obviously) gets called before the flalertlayer so it's probably not of much use
	void onExitNoSave(CCObject* sender) {
		log::info("stop music (exit)");
		AudioManager::get()._pause();

		EditorPauseLayer::onExitNoSave(sender);
	}
	*/

	void onSaveAndExit(CCObject* sender) {
		log::info("isInEditor=false");
		AudioManager::get().isInEditor = false;

		EditorPauseLayer::onSaveAndExit(sender);
	}

	void onSaveAndPlay(CCObject* sender) {
		log::info("isInEditor=false");
		AudioManager::get().isInEditor = false;

		EditorPauseLayer::onSaveAndPlay(sender);
	}

	// shortcuts in pause menu
	bool init(LevelEditorLayer* p0) {
		if (!EditorPauseLayer::init(p0)) return false;
		auto smallActionsMenu = this->getChildByID("small-actions-menu");
		if (!smallActionsMenu) return true;

		// skip song
		if (Mod::get()->getSettingValue<bool>("skip-song")) {
			auto spr = ButtonSprite::create(
				"Next\nSong", 30, 0, .4f, true,
				"bigFont.fnt", "GJ_button_04.png", 30.f
			);
			spr->setScale(.8f);

			auto nextSongButton = CCMenuItemSpriteExtra::create(
				spr, this, menu_selector(FunkyEditorPauseLayer::onNextSong)
			);
			nextSongButton->setID("next-song-btn"_spr);
			smallActionsMenu->insertAfter(nextSongButton, nullptr);

			smallActionsMenu->updateLayout();
		}

		// prev song
		if (Mod::get()->getSettingValue<bool>("prev-song")) {
			auto spr = ButtonSprite::create(
				"Prev.\nSong", 30, 0, .4f, true,
				"bigFont.fnt", "GJ_button_04.png", 30.f
			);
			spr->setScale(.8f);

			auto nextSongButton = CCMenuItemSpriteExtra::create(
				spr, this, menu_selector(FunkyEditorPauseLayer::onPrevSong)
			);
			nextSongButton->setID("next-song-btn"_spr);
			smallActionsMenu->insertAfter(nextSongButton, nullptr);

			smallActionsMenu->updateLayout();
		}

		// current song
		if (Mod::get()->getSettingValue<bool>("current-song")) {
			auto topMenu = this->getChildByIDRecursive("top-menu");
			if (!topMenu) return true;
			
			auto audioManager = AudioManager::get();

			auto layer = CCLayer::create();
			layer->setID("current-song"_spr);

			auto songTitle = CCLabelBMFont::create(audioManager.currentSongName.c_str(), "bigFont.fnt");
			songTitle->setScale(audioManager.desiredScale);
			songTitle->setID("current-song-title"_spr);
			songTitle->setPositionX(topMenu->getContentWidth() / 2);
			songTitle->setPositionY(35.f);
			layer->addChild(songTitle);

			auto songHeader = CCLabelBMFont::create("Current song:", "bigFont.fnt");
			songHeader->setScale(.27f);
			songHeader->setID("current-song-header"_spr);
			songHeader->setPositionX(topMenu->getContentWidth() / 2);
			songHeader->setPositionY(46.f);
			layer->addChild(songHeader);

			auto bg = CCScale9Sprite::create("GJ_square01.png");
			bg->setContentSize(CCSize{ 215.f, 60.f });
			bg->setID("current-song-bg"_spr);
			bg->setPositionX(topMenu->getContentWidth() / 2);
			bg->setPositionY(45.f);
			bg->setZOrder(-1);
			layer->addChild(bg);

			auto settingsMenu = CCMenu::create();
			settingsMenu->setID("editormusic-menu"_spr);
			settingsMenu->setPosition(CCPoint{0.f, 0.f});
			layer->addChild(settingsMenu);

			auto settingsSprite = CCSprite::createWithSpriteFrameName("GJ_menuBtn_001.png");
			settingsSprite->setScale(.325f);

			auto settingsButton = CCMenuItemSpriteExtra::create(settingsSprite, this, menu_selector(FunkyEditorPauseLayer::onSettings));
			settingsButton->setID("editormusic-settings"_spr);
			settingsButton->setPosition(CCPoint{ 235.f, 45.f });
			settingsMenu->addChild(settingsButton);

			topMenu->addChild(layer);
		}

		return true;
	}

	void onNextSong(CCObject* sender) {
		AudioManager::get().playNewSong();
	}

	void onPrevSong(CCObject* sender) {
		AudioManager::get().prevSong();
	}

	void onSettings(CCObject* sender) {
		AudioInfoPopup::create()->show();
	}
};

class $modify(CCScheduler) {
	void update(float dt) {
		CCScheduler::update(dt);

		if (AudioManager::get().hasNoSongs) return;
		AudioManager::get().tick(dt);

		// damn this is a lot of indentation
		if (auto levelEditor = LevelEditorLayer::get()) {
			if (auto pauseLayer = levelEditor->getChildByID("EditorPauseLayer")) {
				if (auto topMenu = pauseLayer->getChildByID("top-menu")) {
					if (auto songTitleWrapper = topMenu->getChildByID("current-song"_spr)) {
						if (auto songTitle = static_cast<CCLabelBMFont*>(songTitleWrapper->getChildByID("current-song-title"_spr))) {
							// the check here was broken so it was removed if anyone remembers one here

							songTitle->setString(AudioManager::get().currentSongName.c_str());
							songTitle->setScale(AudioManager::get().desiredScale);
						}
					}
				}
			}
		}

		if (auto infoPopup = static_cast<AudioInfoPopup*>(CCScene::get()->getChildByID("audio-info-popup"_spr))) {
			infoPopup->tick();
		}
	}
};

class $modify(MenuLayer) {
	bool init() {
		if (!MenuLayer::init()) return false;

		if (AudioManager::get().hasNoSongs) {
			auto alert = FLAlertLayer::create(
				"Message from EditorMusic",
				"You have no songs in your config folder! 
 See the mod description for more details on how to access the music folder (or set your own custom folder)",
				"OK"
			);

			alert->m_scene = this;
			alert->show();
		}

		if (AudioManager::get().customPathDoesntExist) {
			auto alert = FLAlertLayer::create(
				"Message from EditorMusic",
				"The custom path you provided does not exist! If you don't have a custom path, set the setting to \"(none)\"",
				"OK"
			);

			alert->m_scene = this;
			alert->show();
		}

		return true;
	}
};

$execute {
	// so silly
	log::info("{}", fmt::styled("=============== AudioManager loading!! ===============", fg(fmt::rgb(0x4287f5)) | bg(fmt::rgb(0xFF0000))));
	AudioManager::get().setup();

	listenForSettingChanges("low-pass", +[](bool value) {
		AudioManager::get().updateLowPassFilter();
	});
}


// filter out debug logs
// also breaks stuff but ignore that

//void vlogImpl(Severity severity, Mod* mod, fmt::string_view format, fmt::format_args args) {
//	if (severity == Severity::Debug) return;
//	std::cout << mod->getID() << std::endl;
//	geode::log::vlogImpl(severity, mod, format, args);
//}
//
//$on_mod(Loaded) {
//	(void)geode::Mod::get()->hook(
//		(void*)geode::addresser::getNonVirtual(&geode::log::vlogImpl),
//		&vlogImpl, "geode::log::vlogImpl", tulip::hook::TulipConvention::Cdecl
//	);
//}