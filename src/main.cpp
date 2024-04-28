#include <Geode/Geode.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/CCScheduler.hpp>
#include <Geode/modify/LoadingLayer.hpp>
#include "AudioManager.hpp"
#include <fmt/core.h>
#include <fmt/color.h>

using namespace geode::prelude;

AudioManager* audioManager = new AudioManager();

class $modify(LevelEditorLayer) {
	void onPlaytest() {
		LevelEditorLayer::onPlaytest();

		log::info("stop music (started playtest)");
		audioManager->stopAudio();
	}

	void onResumePlaytest() {
		LevelEditorLayer::onResumePlaytest();

		log::info("stop music (resumed playtest)");
		audioManager->stopAudio();
	}

	void onPausePlaytest() {
		LevelEditorLayer::onPausePlaytest();

		log::info("start music quieter (paused playtest)");
		audioManager->playAudio(false);
		audioManager->turnDownMusic();
	}

	void onStopPlaytest() {
		LevelEditorLayer::onStopPlaytest();

		log::info("start music (stopped playtest)");
		audioManager->playAudio(false);
		audioManager->turnUpMusic();
	}

	bool init(GJGameLevel * p0, bool p1) {
		if (!LevelEditorLayer::init(p0, p1)) return false;

		log::info("start music (entered editor)");
		audioManager->playAudio(true);

		return true;
	}

	void stopPlayback() {
		LevelEditorLayer::stopPlayback();

		log::info("start music (stopped playback)");
		audioManager->playAudio(false);
	}
};

class $modify(EditorUI) {
	void onPlayback(CCObject * sender) {
		log::info("Stop music (toggle playback)");
		// EditorUI::onPlayback could run LevelEditorLayer::stopPlayback which will play the audio
		// and if we stop it now it could already be stopped
		// bit hard to think of but should work
		audioManager->stopAudio();

		EditorUI::onPlayback(sender);
	}
};

class $modify(FunkyEditorPauseLayer, EditorPauseLayer) {
	void onExitEditor(CCObject* sender) {
		log::info("stop music (exited editor generic)");
		audioManager->stopAudio();

		EditorPauseLayer::onExitEditor(sender);
	}

	/*
	// this (obviously) gets called before the flalertlayer so it's probably not
	// of much use
	void onExitNoSave(CCObject* sender) {
		log::info("stop music (exit)");
		audioManager->stopAudio();
		audioManager->onExitEditor();

		EditorPauseLayer::onExitNoSave(sender);
	}
	*/

	void onSaveAndExit(CCObject* sender) {
		log::info("stop music (save&exit)");
		audioManager->stopAudio();
		audioManager->onExitEditor();

		EditorPauseLayer::onSaveAndExit(sender);
	}

	void onSaveAndPlay(CCObject* sender) {
		log::info("stop music (save&play)");
		audioManager->stopAudio();
		audioManager->onExitEditor();

		EditorPauseLayer::onSaveAndPlay(sender);
	}

	// turn up and down music when pausing
	static EditorPauseLayer* create(LevelEditorLayer* editor) {
		log::info("turn down music");
		audioManager->turnDownMusic();
		return EditorPauseLayer::create(editor);
	}

	void onResume(CCObject* sender) {
		EditorPauseLayer::onResume(sender);
		log::info("turn up music");
		audioManager->turnUpMusic();
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

		// current song
		if (Mod::get()->getSettingValue<bool>("current-song")) {
			auto topMenu = this->getChildByIDRecursive("top-menu");
			if (!topMenu) return true;

			auto song = CCLabelBMFont::create(audioManager->currentSongName.c_str(), "bigFont.fnt");
			topMenu->addChild(song);
			song->setScale(.35f);
			song->setID("current-song-title"_spr);
			song->setPositionX(topMenu->getContentWidth() / 2);
			song->setPositionY(35.f);

			auto songHeader = CCLabelBMFont::create("Current song:", "bigFont.fnt");
			topMenu->addChild(songHeader);
			songHeader->setScale(.27f);
			songHeader->setID("current-song-header"_spr);
			songHeader->setPositionX(topMenu->getContentWidth() / 2);
			songHeader->setPositionY(46.f);

			auto bg = CCScale9Sprite::create("GJ_square01.png");
			topMenu->addChild(bg);
			bg->setContentSize(CCSize{ 185.f, 60.f });
			bg->setID("current-song-bg"_spr);
			bg->setPositionX(topMenu->getContentWidth() / 2);
			bg->setPositionY(45.f);
			bg->setZOrder(-1);
		}

		return true;
	}

	void onNextSong(CCObject* sender) {
		audioManager->nextSong();
		audioManager->turnDownMusic(); // pause menu so has to turn down the music!
	}
};

class $modify(CCScheduler) {
	void update(float dt) {
		CCScheduler::update(dt);

		if (audioManager->hasNoSongs) return;
		audioManager->tick(dt);

		// damn this is a lot of indentation
		if (auto levelEditor = LevelEditorLayer::get()) {
			if (auto pauseLayer = static_cast<EditorPauseLayer*>(levelEditor->getChildByID("EditorPauseLayer"))) {
				if (auto topMenu = static_cast<CCMenu*>(pauseLayer->getChildByID("top-menu"))) {
					if (auto songTitle = static_cast<CCLabelBMFont*>(topMenu->getChildByID("current-song-title"_spr))) {
						if (songTitle->getString() == audioManager->currentSongName.c_str()) return;

						songTitle->setString(audioManager->currentSongName.c_str());
					}
				}
			}
		}
	}
};

class $modify(MenuLayer) {
	bool init() {
		if (!MenuLayer::init()) return false;

		if (audioManager->hasNoSongs) {
			auto alert = FLAlertLayer::create(
				"Message from EditorMusic",
				"You have no songs in your config folder! Press the pencil icon next to the mod config to access the folder, where you can place music. See the mod description for more details.",
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
	audioManager->setup();
}
