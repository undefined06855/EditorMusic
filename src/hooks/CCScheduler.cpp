#include "CCScheduler.hpp"
#include "../AudioManager.hpp"

void HookedCCScheduler::update(float dt) {
    CCScheduler::update(dt);
    AudioManager::get().update(dt);
}
