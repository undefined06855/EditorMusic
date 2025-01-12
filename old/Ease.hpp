#pragma once
#include <Geode/Geode.hpp>

class Ease : public cocos2d::CCObject {
public:
	static Ease* create(float start, float end, double duration, float* target);
	static Ease* create(float start, float end, double duration, float* target, const std::function<void()>& callback);

	bool init(float start, float end, double duration, float* target, const std::function<void()>& callback);

	float start;
	float end;
	double duration;
	float* target;
	double step = 0;
	std::function<void()> callback;

	virtual void update(float dt);
};
