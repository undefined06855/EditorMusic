#ifndef GEODE_IS_IOS
#include <geode.custom-keybinds/include/Keybinds.hpp>

$on_mod(Loaded) {
	keybinds::BindManager::get()->registerBindable({
		"pause-song"_spr,
		"Play/Pause",
		"A keybind to toggle pause when not in the current song popup. This will not override space to pause the song when inside the current song popup!",
		{ keybinds::Keybind::create(cocos2d::KEY_P, keybinds::Modifier::Control) },
		keybinds::Category::EDITOR,
		false
	});

	keybinds::BindManager::get()->registerBindable({
		"next-song"_spr,
		"Next song",
		"",
		{ keybinds::Keybind::create(cocos2d::KEY_Right, keybinds::Modifier::Control) },
		keybinds::Category::EDITOR,
		false
	});

	keybinds::BindManager::get()->registerBindable({
		"prev-song"_spr,
		"Previous song",
		"",
		{ keybinds::Keybind::create(cocos2d::KEY_Left, keybinds::Modifier::Control) },
		keybinds::Category::EDITOR,
		false
	});
}

#endif
