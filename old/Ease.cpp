#include "Ease.hpp"
#include <Geode/Geode.hpp>
#include <Geode/modify/CCScheduler.hpp>
#include <functional>

using namespace geode::prelude;

CCArray* eases;
$on_mod(Loaded) { eases = CCArray::createWithCapacity(10); }

Ease* Ease::create(float start, float end, double duration, float* target, const std::function<void()>& callback) {
	auto ret = new Ease;
	if (ret->init(start, end, duration, target, callback)) {
		ret->autorelease();
		return ret;
	}

	delete ret;
	return nullptr;
}

Ease* Ease::create(float start, float end, double duration, float* target) {
	return Ease::create(start, end, duration, target, nullptr);
}

bool Ease::init(float start, float end, double duration, float* target, const std::function<void()>& callback) {
	this->start = start;
	this->end = end;
	this->duration = duration;
	this->target = target;
	this->callback = callback;
	log::info("Created easing\nstart: {}, end: {}, dur: {}", start, end, duration);
	eases->addObject(this);
	return true;
}

void Ease::update(float dt) {
	geode::log::info("{}", dt);
	this->step += dt / 1000.f;

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
		if (this->callback) {
			this->callback();
		}
		eases->removeObject(this, true);
	}
}


class $modify(CCScheduler) {
	void update(float dt) {
		CCScheduler::update(dt);
		if (!eases) return;
		for (int i = 0; i < eases->count(); i++) {
			eases->objectAtIndex(i)->update(dt);
		}
	}
};
