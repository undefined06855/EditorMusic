#pragma once
#include <Geode/Geode.hpp>
using namespace geode::prelude;

class Utils {
public:
	static std::string formatTime(int unformattedSeconds) {
		int minutes = static_cast<int>(unformattedSeconds / 60) % 60;
		int hours = static_cast<int>(minutes / 60);
		int seconds = unformattedSeconds % 60;

		if (minutes == 0) {
			return fmt::format("00:{:02}", seconds);
		} else if (hours == 0) {
			return fmt::format("{:02}:{:02}", minutes, seconds);
		} else {
			return fmt::format("{:02}:{:02}:{:02}", hours, minutes, seconds);
		}
	}
};
