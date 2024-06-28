#pragma once
#include <functional>

class Ease {
public:
	Ease(float start, float end, double duration, float* target);
	Ease(float start, float end, double duration, float* target, const std::function<void()>& callback);

	float start;
	float end;
	double duration;
	float* target;
	double step = 0;
	std::function<void()> callback;

	void tick(float dt);
};
