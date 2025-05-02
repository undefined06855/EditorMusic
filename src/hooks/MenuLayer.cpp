#include "MenuLayer.hpp"
#include "../AudioManager.hpp"
#include "../log.hpp"

cocos2d::CCScene* HookedMenuLayer::scene(bool isVideoOptionsOpen) {
    auto ret = MenuLayer::scene(isVideoOptionsOpen);

    em::log::debug("menulayer balls");
    AudioManager::get().showLoadErrors(ret);

    return ret;
}
