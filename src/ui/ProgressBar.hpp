#pragma once
// basic implementation + sprites taken from globed
// thanks dank

class ProgressBar : public cocos2d::CCNodeRGBA {
public:
    static ProgressBar* create(float width);
    bool init(float width);

    cocos2d::CCSprite* m_outlineLeft;
    cocos2d::CCSprite* m_outlineMiddle;
    cocos2d::CCSprite* m_outlineRight;
    cocos2d::CCSprite* m_fill;
    float m_value;

    float m_edgeWidth;
    float m_height;
    float m_fillHeight;
    float m_fillPad;

    void setContentSize(cocos2d::CCSize size);
    void updateSprites();
    void setValue(float value);
};