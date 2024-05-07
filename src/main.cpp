#include <Geode/Geode.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/CCScheduler.hpp>
#include <Geode/modify/LoadingLayer.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include "AudioManager.hpp"
#include <fmt/core.h>
#include <fmt/color.h>

using namespace geode::prelude;

class $modify(LevelEditorLayer) {
	void onPlaytest() {
		LevelEditorLayer::onPlaytest();

		log::info("stop music (started playtest)");
		AudioManager::get().pause();
	}

	void onResumePlaytest() {
		LevelEditorLayer::onResumePlaytest();

		log::info("stop music (resumed playtest)");
		AudioManager::get().pause();
	}

	void onPausePlaytest() {
		LevelEditorLayer::onPausePlaytest();

		log::info("start music quieter (paused playtest)");
		AudioManager::get().play();
		AudioManager::get().turnDownMusic();
	}

	void onStopPlaytest() {
		LevelEditorLayer::onStopPlaytest();

		log::info("start music (stopped playtest)");
		AudioManager::get().play();
		AudioManager::get().turnUpMusic();
	}

	bool init(GJGameLevel * p0, bool p1) {
		if (!LevelEditorLayer::init(p0, p1)) return false;

		log::info("start music (entered editor)");
		AudioManager::get().playNewSong();

		return true;
	}

	void stopPlayback() {
		LevelEditorLayer::stopPlayback();

		log::info("start music (stopped playback)");
		AudioManager::get().play();
	}
};

class $modify(EditorUI) {
	void onPlayback(CCObject * sender) {
		log::info("Stop music (toggle playback)");
		// EditorUI::onPlayback could run LevelEditorLayer::stopPlayback which will play the audio
		// and if we stop it now it could already be stopped
		// bit hard to think of but should work
		AudioManager::get().pause();

		EditorUI::onPlayback(sender);
	}
};

class $modify(FunkyEditorPauseLayer, EditorPauseLayer) {
	void onExitEditor(CCObject* sender) {
		log::info("stop music (exited editor generic)");
		AudioManager::get().pause();

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
		log::info("stop music (save&exit)");
		AudioManager::get().pause();

		EditorPauseLayer::onSaveAndExit(sender);
	}

	void onSaveAndPlay(CCObject* sender) {
		log::info("stop music (save&play)");
		AudioManager::get().pause();

		EditorPauseLayer::onSaveAndPlay(sender);
	}

	// turn up and down music when pausing
	static EditorPauseLayer* create(LevelEditorLayer* editor) {
		log::info("turn down music");
		AudioManager::get().turnDownMusic();
		return EditorPauseLayer::create(editor);
	}

	void onResume(CCObject* sender) {
		EditorPauseLayer::onResume(sender);
		log::info("turn up music");
		AudioManager::get().turnUpMusic();
	}

	bool init(LevelEditorLayer* p0) {
		if (!EditorPauseLayer::init(p0)) return false;

		// skip song
		if (Mod::get()->getSettingValue<bool>("skip-song")) {
			auto smallActionsMenu = this->getChildByID("small-actions-menu");

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
			auto smallActionsMenu = this->getChildByID("small-actions-menu");

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

			auto layer = CCLayer::create();
			layer->setID("current-song"_spr);

			auto song = CCLabelBMFont::create(AudioManager::get().currentSongName.c_str(), "bigFont.fnt");
			layer->addChild(song);
			song->setScale(.35f);
			song->setID("current-song-title"_spr);
			song->setPositionX(topMenu->getContentWidth() / 2);
			song->setPositionY(35.f);

			auto songHeader = CCLabelBMFont::create("Current song:", "bigFont.fnt");
			layer->addChild(songHeader);
			songHeader->setScale(.27f);
			songHeader->setID("current-song-header"_spr);
			songHeader->setPositionX(topMenu->getContentWidth() / 2);
			songHeader->setPositionY(46.f);

			auto bg = CCScale9Sprite::create("GJ_square01.png");
			layer->addChild(bg);
			bg->setContentSize(CCSize{ 185.f, 60.f });
			bg->setID("current-song-bg"_spr);
			bg->setPositionX(topMenu->getContentWidth() / 2);
			bg->setPositionY(45.f);
			bg->setZOrder(-1);

			auto settingsMenu = CCMenu::create();
			settingsMenu->setPosition(CCPoint{0.f, 0.f});
			layer->addChild(settingsMenu);

			auto settingsSprite = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
			settingsSprite->setScale(.275f);

			auto settingsButton = CCMenuItemSpriteExtra::create(settingsSprite, this, menu_selector(FunkyEditorPauseLayer::onSettings));
			settingsMenu->addChild(settingsButton);
			settingsButton->setID("editormusic-settings"_spr);
			settingsButton->setPositionX(topMenu->getContentWidth() / 2 + 80.f);
			settingsButton->setPositionY(46.f);

			topMenu->addChild(layer);
		}

		return true;
	}

	void onNextSong(CCObject* sender) {
		AudioManager::get().playNewSong();
		AudioManager::get().turnDownMusic(); // pause menu so has to turn down the music!
	}

	void onPrevSong(CCObject* sender) {
		AudioManager::get().prevSong();
		AudioManager::get().turnDownMusic(); // pause menu so has to turn down the music!
	}

	
	void onSettings(CCObject* sender) {
		openSettingsPopup(Mod::get());
	}
};

class $modify(CCScheduler) {
	void update(float dt) {
		CCScheduler::update(dt);

		if (AudioManager::get().hasNoSongs) return;
		AudioManager::get().tick(dt);

		// damn this is a lot of indentation
		if (auto levelEditor = LevelEditorLayer::get()) {
			if (auto pauseLayer = static_cast<EditorPauseLayer*>(levelEditor->getChildByID("EditorPauseLayer"))) {
				if (auto topMenu = static_cast<CCMenu*>(pauseLayer->getChildByID("top-menu"))) {
					if (auto songTitleWrapper = static_cast<CCLayer*>(topMenu->getChildByID("current-song"_spr))) {
						if (auto songTitle = static_cast<CCLabelBMFont*>(songTitleWrapper->getChildByID("current-song-title"_spr))) {
							if (songTitle->getString() == AudioManager::get().currentSongName.c_str()) return;

							songTitle->setString(AudioManager::get().currentSongName.c_str());
						}
					}
				}
			}
		}
	}
};

class $modify(MenuLayer) {
	bool init() {
		if (!MenuLayer::init()) return false;

		if (AudioManager::get().hasNoSongs) {
			auto alert = FLAlertLayer::create(
				"Message from EditorMusic",
				"You have no songs in your config folder! Press the pencil icon next to the mod config to access the folder, where you can place music. See the mod description for more details.",
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
}
