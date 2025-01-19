#ifdef GEODE_IS_WINDOWS
#include <Geode/modify/GameManager.hpp>
#include "../AudioManager.hpp"

// thanks alpha for hooking windowproc code
// https://github.com/Alphalaneous/Responsive-Closing/blob/main/src/main.cpp

bool g_isReloading = false;
bool g_isSwitchingModes = false;
LONG_PTR g_origWindowProc;

LRESULT CALLBACK HookedWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    switch (msg) {
        case WM_DESTROY:
        case WM_QUIT:
        case WM_CLOSE:
            if (!g_isReloading) AudioManager::get().onCloseGDWindow();
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

class $modify(GameManager) {
    void reloadAll(bool switchingModes, bool toFullscreen, bool borderless, bool fix, bool unused) {
		g_isReloading = true;
        g_isSwitchingModes = switchingModes;
    	GameManager::reloadAll(switchingModes, toFullscreen, borderless, fix, unused);
	}

	void reloadAllStep5() {
		GameManager::reloadAllStep5();
        if (g_isSwitchingModes) modifyWindowProc();
        g_isReloading = false;
	}
};

$on_mod(Loaded) { modifyWindowProc(); }

#endif
