#pragma once
#include <Geode/loader/SettingV3.hpp>

class ReloadSongSetting;

class ReloadSongNode : public geode::SettingNodeV3 {
public:
    static ReloadSongNode* create(std::shared_ptr<ReloadSongSetting> setting, float width);
    bool init(std::shared_ptr<ReloadSongSetting> setting, float width);

    void onButton(CCObject* sender);
    void onCommit();
    void onResetToDefault();
    bool hasUncommittedChanges() const;
    bool hasNonDefaultValue() const;
};
