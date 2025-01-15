#include <Geode/Geode.hpp>

class $modify(HookedCCScheduler, cocos2d::CCScheduler) {
    void update(float dt);
};