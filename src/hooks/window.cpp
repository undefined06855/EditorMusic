#ifdef GEODE_IS_WINDOWS

#include "window.hpp"
#include "../AudioManager.hpp"

// thanks alpha for hooking windowproc code
// https://github.com/Alphalaneous/Responsive-Closing/blob/main/src/main.cpp

namespace em::window {

bool g_isReloading = false;
bool g_isSwitchingModes = false;
bool g_isClosing = false;
LONG_PTR g_origWindowProc;

LRESULT CALLBACK HookedWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    switch (msg) {
        case WM_DESTROY:
        case WM_QUIT:
        case WM_CLOSE:
            // g_isClosing to ensure this doesnt trigger like 3 times when you
            // close the game
            if (!g_isReloading && !g_isClosing) {
                AudioManager::get().onCloseGDWindow();
                g_isClosing = true;
            }
            
            break;
        
        case WM_KEYDOWN:
            if (!geode::Mod::get()->getSettingValue<bool>("media-controls")) break;
            switch (wparam) {
                case VK_MEDIA_PLAY_PAUSE:
                    AudioManager::get().togglePause();
                    break;
                case VK_MEDIA_NEXT_TRACK:
                    AudioManager::get().nextSong();
                    break;
                case VK_MEDIA_PREV_TRACK:
                    AudioManager::get().prevSong();
                    break;
                case VK_MEDIA_STOP:
                    AudioManager::get().setPaused(true);
                    break;
            }

            break;
    }

    return CallWindowProc((WNDPROC)g_origWindowProc, hwnd, msg, wparam, lparam);
}

void modifyWindowProc() {
    g_origWindowProc = SetWindowLongPtrA(WindowFromDC(wglGetCurrentDC()), -4, (LONG_PTR)HookedWindowProc);
}

void HookedGameManager::reloadAll(bool switchingModes, bool toFullscreen, bool borderless, bool fix, bool unused) {
    g_isReloading = true;
    g_isSwitchingModes = switchingModes;
    GameManager::reloadAll(switchingModes, toFullscreen, borderless, fix, unused);
}

void HookedGameManager::reloadAllStep5() {
    GameManager::reloadAllStep5();
    if (g_isSwitchingModes) modifyWindowProc();
    g_isReloading = false;
}


}

#endif
