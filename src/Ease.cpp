#include "Ease.hpp"
#include <Geode/Geode.hpp>
#include <Geode/modify/CCScheduler.hpp>
#include <functional>

using namespace geode::prelude;

std::vector<Ease*> eases;

Ease::Ease(float start, float end, double duration, float* target) {
	this->start = start;
	this->end = end;
	this->duration = duration;
	this->target = target;
	log::info("Created easing\nstart: {}, end: {}, dur: {}", start, end, duration);
	eases.push_back(this);
}

Ease::Ease(float start, float end, double duration, float* target, const std::function<void()>& callback) {
	this->start = start;
	this->end = end;
	this->duration = duration;
	this->target = target;
	this->callback = callback;
	log::info("Created easing\nstart: {}, end: {}, dur: {}", start, end, duration);
	eases.push_back(this);
}

void Ease::tick(float dt) {
	log::debug("Tick from {} to {}", this->step, this->step + dt);

	this->step += dt;

	bool okINeedToDieNow = false;

	if (this->step > this->duration) {
		this->step = this->duration;
		okINeedToDieNow = true;
	}

	*this->target = std::lerp(
		this->start,
		this->end,
		this->step / this->duration
	);

	if (okINeedToDieNow) {
		log::info("deleting easing");
		if (this->callback) {
			this->callback();
		}
		eases.erase(std::remove(eases.begin(), eases.end(), this), eases.end());
	}
}

class $modify(CCScheduler) {
	void update(float dt) {
		CCScheduler::update(dt);
		for (auto ease : eases) {
			ease->tick(dt);
		}
	}
};
