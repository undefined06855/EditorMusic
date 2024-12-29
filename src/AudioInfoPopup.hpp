#pragma once
#include <Geode/Geode.hpp>
using namespace geode::prelude;

class AudioInfoPopup : public geode::Popup<> {
protected:
	bool setup() override;
public:
	static AudioInfoPopup* create();

	CCMenu* btnMenu;

	void update(float dt);

	// button moment
	void onPrevBtn(CCObject* sender);
	void onNextBtn(CCObject* sender);
	void onSettingsButton(CCObject* sender);
	void onPlayPause(CCObject* sender);
	void onSongRestart(CCObject* sender);
	void onRw10Sec(CCObject* sender); // it's rewind time
	void onFF10Sec(CCObject* sender);
};
