#include <Geode/Geode.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/CCScheduler.hpp>
#include "AudioManager.hpp"

using namespace geode::prelude;

AudioManager* audioManager = new AudioManager();

bool isPlaybackPlaying = false;

class $modify(LevelEditorLayer) {
	void onPlaytest() {
		LevelEditorLayer::onPlaytest();

		log::info("stop music (started playtest)");
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

	bool init(GJGameLevel* p0, bool p1) {
		if (!LevelEditorLayer::init(p0, p1)) return false;

		log::info("start music (entered editor)");
		audioManager->playAudio(true);

		return true;
	}
};

class $modify(EditorPauseLayer) {
	void onExitEditor(CCObject* sender) {
		EditorPauseLayer::onExitEditor(sender);
		log::info("stop music (exit)");
		audioManager->stopAudio();
		audioManager->onExitEditor();
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
};

class $modify(CCScheduler) {
	void update(float dt) {
		CCScheduler::update(dt);

		if (audioManager->hasNoSongs) return;
		audioManager->tick(dt);
	}
};

class $modify(MenuLayer) {
	bool init() {
		if (!MenuLayer::init()) return false;

		if (audioManager->hasNoSongs) {
			FLAlertLayer::create("Message from EditorMusic", "You have no songs in your config folder! Press the pencil icon next to the mod config to access the folder, where you can place music. See the mod description for more details.", "OK");
		}

		return true;
	}
};

$execute {
	// load audio
	audioManager->setup();
}
