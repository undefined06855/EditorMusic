#pragma once
#include <Geode/Geode.hpp>

class PreloadUI : public cocos2d::CCNodeRGBA {
public:
    static PreloadUI* create();
    bool init();
    void addToSceneAndAnimate();

    int m_totalSongs;
    int m_songsLoaded;
    cocos2d::CCLabelBMFont* m_progressLabel;
    cocos2d::CCLabelBMFont* m_subtitleLabel;
    Slider* m_progressBar;
    
    cocos2d::CCPoint m_animUpperPos;
    cocos2d::CCPoint m_animLowerPos;

    void increment(std::string lastSongLoadedName);
    void runCompleteAnimationAndRemove();
};
