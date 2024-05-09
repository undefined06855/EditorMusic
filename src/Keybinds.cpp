// there must be a better way to do this
#ifndef GEODE_IS_ANDROID:

#include <Geode/Geode.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include "AudioManager.hpp"
#include "Keybinds.hpp"

class $modify(LevelEditorLayer) {
	bool init(GJGameLevel* p0, bool p1) {
		if (!LevelEditorLayer::init(p0, p1)) return false;

		using namespace keybinds;

		// Mute
		this->template addEventListener<InvokeBindFilter>([=](InvokeBindEvent* event) {
			if (event->isDown()) {
				log::info("Mute");
				auto am = AudioManager::get();
				bool paused;
				am.channel->getPaused(&paused);
				if (paused) {
					log::info("keybind: mute (paused) ----v");
					am.play();
				} else {
					log::info("keybind: mute (not paused) ----v");
					am.pause();
				}
			}

			return ListenerResult::Propagate;
		}, "mute-song"_spr);

		// Next
		this->template addEventListener<InvokeBindFilter>([=](InvokeBindEvent* event) {
			if (event->isDown()) {
				log::info("keybind: next song ----v");
				AudioManager::get().nextSong();
			}

			return ListenerResult::Propagate;
		}, "next-song"_spr);

		// Prev
		this->template addEventListener<InvokeBindFilter>([=](InvokeBindEvent* event) {
			if (event->isDown()) {
				log::info("keybind: prev song ----v");
				AudioManager::get().prevSong();
			}

			return ListenerResult::Propagate;
		}, "prev-song"_spr);

		return true;
	}
};

$execute {
	using namespace keybinds;

	BindManager::get()->registerBindable({
		"mute-song"_spr,
		"Mute",
		"Mutes and pauses the current song",
		{ Keybind::create(KEY_P) },
		Category::EDITOR
	});

	BindManager::get()->registerBindable({
		"next-song"_spr,
		"Next song",
		"Switches to the next song",
		{ Keybind::create(KEY_Right, Modifier::Control) },
		Category::EDITOR
	});

	BindManager::get()->registerBindable({
		"prev-song"_spr,
		"Previous song",
		"Switches to the previous song",
		{ Keybind::create(KEY_Left, Modifier::Control) },
		Category::EDITOR
	});
}

#endif
