#include <Geode/Geode.hpp>
#include <Geode/modify/CCScheduler.hpp>

class $modify(HookedCCScheduler, cocos2d::CCScheduler) {
    void update(float dt);
};
