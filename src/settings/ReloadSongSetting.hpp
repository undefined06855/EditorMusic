#pragma once
#include <Geode/Geode.hpp>
#include "ReloadSongNode.hpp"

class ReloadSongSetting : public geode::SettingV3 {
public:
    static geode::Result<std::shared_ptr<geode::SettingV3>> parse(const std::string& key, const std::string& modID, const matjson::Value& json);

    bool load(const matjson::Value& json);
    bool save(matjson::Value& json) const;
    bool isDefaultValue() const;
    void reset();
    ReloadSongNode* createNode(float width);
};
