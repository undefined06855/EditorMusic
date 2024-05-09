#include "AudioInfoPopup.hpp"

bool AudioInfoPopup::setup() {
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    this->setTitle("Hi mom!");

    auto label = CCLabelBMFont::create("skibidi toilet will be mine ohio time", "bigFont.fnt");
    label->setPosition(winSize / 2);
    this->addChild(label);

    this->setID("audio-info-popup"_spr);

    return true;
}

AudioInfoPopup* AudioInfoPopup::create() {
    auto ret = new AudioInfoPopup();
    if (ret && ret->init(240.f, 160.f)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

void AudioInfoPopup::tick() {

}
