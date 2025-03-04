#pragma once
#include <Geode/modify/LoadingLayer.hpp>

class $modify(HookedLoadingLayer, LoadingLayer) {
    bool init(bool isReloading);
};
