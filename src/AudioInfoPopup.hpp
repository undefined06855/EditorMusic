#pragma once
#include <Geode/Geode.hpp>
using namespace geode::prelude;

class AudioInfoPopup : public geode::Popup<> {
protected:
	bool setup() override;
public:
	static AudioInfoPopup* create();
	void tick();
};
