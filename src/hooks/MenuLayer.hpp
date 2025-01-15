#pragma once
#include "../AudioManager.hpp"
#include <Geode/modify/MenuLayer.hpp>

class $modify(HookedMenuLayer, MenuLayer) {
    bool init();
};
