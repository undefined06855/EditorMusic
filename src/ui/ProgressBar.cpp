#include "ProgressBar.hpp"
// basic implementation + sprites taken from globed
// thanks dank

ProgressBar* ProgressBar::create(float width) {
    auto ret = new ProgressBar;
    if (ret->init(width)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

bool ProgressBar::init(float width) {
    if (!CCNode::init()) return false;

    m_value = 0.f;
    m_height = 11.f;
    m_fillHeight = 8.f;
    m_fillPad = 2.f;

    m_outlineLeft = cocos2d::CCSprite::create("slider-start.png"_spr);
    m_outlineMiddle = cocos2d::CCSprite::create("slider-middle.png"_spr);
    m_outlineRight = cocos2d::CCSprite::create("slider-end.png"_spr);
    m_fill = cocos2d::CCSprite::create("slider-fill.png"_spr);

    m_outlineLeft->setID("outline-left");
    m_outlineMiddle->setID("outline-middle");
    m_outlineRight->setID("outline-right");
    m_fill->setID("fill");

    m_outlineLeft->setAnchorPoint({ 0.f, .5f });
    m_outlineRight->setAnchorPoint({ 1.f, .5f });
    m_fill->setAnchorPoint({ 0.f, .5f });

    m_fill->setZOrder(-1);

    cocos2d::ccTexParams texParams = { GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_CLAMP_TO_EDGE };
    m_fill->getTexture()->setTexParameters(&texParams);

    addChildAtPosition(m_outlineLeft, geode::Anchor::Left);
    addChildAtPosition(m_outlineMiddle, geode::Anchor::Center);
    addChildAtPosition(m_outlineRight, geode::Anchor::Right);
    addChildAtPosition(m_fill, geode::Anchor::Left, { m_fillPad, 0.f });
    
    m_edgeWidth = m_outlineLeft->getContentWidth();

    setAnchorPoint({ .5f, .5f });
    setContentSize({ width, 8.f });

    return true;
}

void ProgressBar::setContentSize(cocos2d::CCSize size) {
    CCNode::setContentSize({ size.width, m_height });
    updateSprites();
}

void ProgressBar::updateSprites() {
    auto middleWidth = getContentWidth() - m_edgeWidth * 2;
    auto middleScale = middleWidth / m_outlineMiddle->getContentWidth();
    m_outlineMiddle->setScaleX(middleScale);
    m_fill->setTextureRect({ 0.f, 0.f, (getContentWidth() - m_fillPad * 2) * m_value, m_fillHeight });
    updateLayout();
}

void ProgressBar::setValue(float value) {
    m_value = value;
    updateSprites();
}
