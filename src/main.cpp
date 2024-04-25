#include <Geode/Geode.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/CCScheduler.hpp>
#include "AudioManager.hpp"

using namespace geode::prelude;

AudioManager* audioManager = new AudioManager();

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

class $modify(MacosCompliantEditorPauseLayer, EditorPauseLayer) {
	void onExitEditor(CCObject* sender) {
#ifndef GEODE_IS_MACOS
		EditorPauseLayer::onExitEditor(sender);
#endif
		log::info("stop music (exit)");
		audioManager->stopAudio();
		audioManager->onExitEditor();
	}

#ifdef GEODE_IS_MACOS
	void onExitNoSave(CCObject* sender) {
		MacosCompliantEditorPauseLayer::onExitEditor(sender);
		EditorPauseLayer::onExitNoSave(sender);
	}

	void onSaveAndExit(CCObject* sender) {
		MacosCompliantEditorPauseLayer::onExitEditor(sender);
		EditorPauseLayer::onSaveAndExit(sender);
	}

	void onSaveAndPlay(CCObject* sender) {
		MacosCompliantEditorPauseLayer::onExitEditor(sender);
		EditorPauseLayer::onSaveAndPlay(sender);
	}
#endif

	// turn up and down music when pausing
	static EditorPauseLayer* create(LevelEditorLayer* editor) {
		log::info("turn down music");
		audioManager->turnDownMusic();
		return EditorPauseLayer::create(editor);
	}

	void onResume(CCObject* sender) {
		log::info("turn up music");
		audioManager->turnUpMusic();
		EditorPauseLayer::onResume(sender);
	}
};

class $modify(CCScheduler) {
	void update(float dt) {
		CCScheduler::update(dt);
		audioManager->tick(dt);
	}
};

$execute {
	// load audio
	audioManager->setup();
}
