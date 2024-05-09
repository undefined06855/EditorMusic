#pragma once
#include <Geode/Geode.hpp>
using namespace geode::prelude;

class Utils {
public:
	static std::string formatSingleTime(int value) {
		auto string = std::to_string(value);
		if (string.size() == 1) {
			string = "0" + string;
		}
		return string;
	}

	static std::string formatTime(int unformattedSeconds) {
		int minutes = static_cast<int>(unformattedSeconds / 60) % 60;
		int hours = static_cast<int>(minutes / 60);
		int seconds = unformattedSeconds % 60;

		if (minutes == 0) {
			return fmt::format("00:{}", Utils::formatSingleTime(seconds));
		} else if (hours == 0) {
			return fmt::format("{}:{}", Utils::formatSingleTime(minutes), Utils::formatSingleTime(seconds));
		} else {
			return fmt::format("{}:{}:{}", Utils::formatSingleTime(hours), Utils::formatSingleTime(minutes), Utils::formatSingleTime(seconds));
		}
	}
};
