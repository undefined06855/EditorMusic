#pragma once
#include <Geode/modify/MenuLayer.hpp>

class $modify(HookedMenuLayer, MenuLayer) {
    static cocos2d::CCScene* scene(bool isVideoOptionsOpen);
};
