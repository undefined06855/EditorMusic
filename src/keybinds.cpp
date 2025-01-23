#include <geode.custom-keybinds/include/Keybinds.hpp>
#include "AudioManager.hpp"
#include "log.hpp"

$on_mod(Loaded) {
	keybinds::BindManager::get()->registerBindable({
		"pause-song"_spr,
		"Play/Pause",
		"",
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
