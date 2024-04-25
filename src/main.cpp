#include <Geode/Geode.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/modify/EditorUI.hpp>
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
		audioManager->playAudio();
		audioManager->turnDownMusic();
	}

	void onStopPlaytest() {
		LevelEditorLayer::onStopPlaytest();

		log::info("start music (stopped playtest)");
		audioManager->playAudio();
		audioManager->turnUpMusic();
	}

	bool init(GJGameLevel* p0, bool p1) {
		if (!LevelEditorLayer::init(p0, p1)) return false;

		log::info("start music (entered editor)");
		audioManager->playAudio();

		return true;
	}
};

class $modify(EditorUI) {
	void onPlayback(CCObject* sender) {
		this->
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
		log::info("turn up music");
		audioManager->turnUpMusic();
		EditorPauseLayer::onResume(sender);
	}
};

$execute {
	// load audio
	audioManager->setup();
}
