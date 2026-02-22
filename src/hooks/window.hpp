#pragma once
#ifdef GEODE_IS_WINDOWS

#include <Geode/modify/GameManager.hpp>

namespace em::window {

extern bool g_isReloading;
extern bool g_isSwitchingModes;
extern bool g_isClosing;
extern LONG_PTR g_origWindowProc;

LRESULT CALLBACK HookedWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
void modifyWindowProc();

class $modify(HookedGameManager, GameManager) {
    void reloadAll(bool switchingModes, bool toFullscreen, bool borderless, bool fix, bool unused);
    void reloadAllStep5();
};

}

#endif
