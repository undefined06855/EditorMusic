#include "ReloadSongSetting.hpp"
#include "ReloadSongNode.hpp"

geode::Result<std::shared_ptr<geode::SettingV3>> ReloadSongSetting::parse(std::string const &key, std::string const &modID, matjson::Value const &json) {
    auto ret = std::make_shared<ReloadSongSetting>();
    auto root = geode::checkJson(json, "ReloadSongSetting");

    ret->init(key, modID, root);
    ret->parseNameAndDescription(root);
    ret->parseEnableIf(root);

    root.checkUnknownKeys();
    return root.ok(std::static_pointer_cast<geode::SettingV3>(ret));
}

bool ReloadSongSetting::load(const matjson::Value& json) { return true; }
bool ReloadSongSetting::save(matjson::Value& json) const { return true; }
bool ReloadSongSetting::isDefaultValue() const { return true; }
void ReloadSongSetting::reset() {}

ReloadSongNode* ReloadSongSetting::createNode(float width) {
    return ReloadSongNode::create(std::static_pointer_cast<ReloadSongSetting>(shared_from_this()), width);
}
