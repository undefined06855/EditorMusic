#include "SongViewPopup.hpp"
#include "../AudioManager.hpp"
#include "../log.hpp"

SongViewPopup* SongViewPopup::create() {
    auto ret = new SongViewPopup;
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

bool SongViewPopup::init() {
    if (!Popup::init({ 380.f, 270.f })) return false;

    em::log::debug("opening songviewpopup...");

    setID("SongViewPopup"_spr);
    setTitle("Song Listing");

    m_playlistName = geode::TextInput::create(460.f, "Type a playlist name or \"all\" to view all songs...");
    m_playlistName->setScale(.65f);
    m_playlistName->getBGSprite()->setContentHeight(73.f);
    m_mainLayer->addChildAtPosition(m_playlistName, geode::Anchor::TopLeft, { 163.f, -49.5f });

    auto loadSpr = ButtonSprite::create("Load");
    loadSpr->setScale(.5f);
    auto loadBtn = CCMenuItemSpriteExtra::create(
        loadSpr,
        this, menu_selector(SongViewPopup::onLoadPlaylist)
    );
    m_buttonMenu->addChildAtPosition(loadBtn, geode::Anchor::TopRight, { -38.f, -41.f });

    auto createSpr = ButtonSprite::create("Create");
    createSpr->setScale(.3625f);
    auto createBtn = CCMenuItemSpriteExtra::create(
        createSpr,
        this, menu_selector(SongViewPopup::onCreatePlaylist)
    );
    m_buttonMenu->addChildAtPosition(createBtn, geode::Anchor::TopRight, { -38.f, -59.f });

    auto addSpr = ButtonSprite::create("Add to playlist");
    addSpr->setScale(.5f);
    auto addBtn = CCMenuItemSpriteExtra::create(
        addSpr,
        this, menu_selector(SongViewPopup::onAddToPlaylist)
    );
    m_buttonMenu->addChildAtPosition(addBtn, geode::Anchor::BottomLeft, { 65.f, 17.f });

    auto removeSpr = ButtonSprite::create("Remove from playlist");
    removeSpr->setScale(.5f);
    auto removeBtn = CCMenuItemSpriteExtra::create(
        removeSpr,
        this, menu_selector(SongViewPopup::onRemoveFromPlaylist)
    );
    m_buttonMenu->addChildAtPosition(removeBtn, geode::Anchor::BottomLeft, { 199.f, 17.f });

    auto folderSpr = geode::BasedButtonSprite::create(
        cocos2d::CCSprite::createWithSpriteFrameName("gj_folderBtn_001.png"),
        geode::BaseType::Circle,
        (int)geode::CircleBaseSize::Small,
        (int)geode::CircleBaseColor::Green
    );
    folderSpr->setScale(.6f);
    auto folderBtn = CCMenuItemSpriteExtra::create(
        folderSpr,
        this, menu_selector(SongViewPopup::onFolder)
    );
    m_buttonMenu->addChildAtPosition(folderBtn, geode::Anchor::TopLeft, { 107.f, -22.f });

    auto infoSpr = cocos2d::CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
    infoSpr->setScale(.8f);
    auto infoBtn = CCMenuItemSpriteExtra::create(
        infoSpr,
        this, menu_selector(SongViewPopup::onInfo)
    );
    m_buttonMenu->addChildAtPosition(infoBtn, geode::Anchor::TopRight, { -17.f, -17.f });

    m_selected.clear();

    auto size = cocos2d::CCSize{ 517.f, 256.f };
    m_scrollLayer = geode::ScrollLayer::create(size);
    m_scrollLayer->setScale(.65f);
    m_scrollLayer->ignoreAnchorPointForPosition(false);
    m_scrollLayer->m_contentLayer->setLayout(
        geode::ColumnLayout::create()
            ->setAutoGrowAxis(0.f)
            ->setAxisReverse(true)
            ->ignoreInvisibleChildren(false)
            ->setGap(0.f)
    );

    { auto pad = cocos2d::CCNode::create();
    pad->setContentSize({ 0.f, 5.f });
    m_scrollLayer->m_contentLayer->addChild(pad); }

    auto copy = AudioManager::get().m_songs;
    std::ranges::sort(copy, [](auto& a, const auto& b) { return a->m_name < b->m_name; });
    for (auto song : copy) {
        auto node = cocos2d::CCNode::create();
        node->setContentSize({ size.width, 40.f });

        if (song->m_albumCover) {
            auto stencil = cocos2d::CCLayerColor::create({ 255, 255, 255, 255 }, 100.f, 40.f);
            stencil->setSkewX(18.f);
            stencil->setPositionX(-6.f);
            auto clip = cocos2d::CCClippingNode::create(stencil);
            clip->setContentSize(stencil->getContentSize());
            clip->setAnchorPoint({ 0.f, .5f });
            auto sprite = cocos2d::CCSprite::createWithTexture(song->m_albumCover);
            sprite->setScale(clip->getContentWidth() / sprite->getContentWidth());
            clip->addChildAtPosition(sprite, geode::Anchor::Center);
            node->addChildAtPosition(clip, geode::Anchor::Left);
        }

        auto name = cocos2d::CCLabelBMFont::create(song->m_name.c_str(), "bigFont.fnt");
        name->setAnchorPoint({ 0.f, .5f });
        name->limitLabelWidth(290.f, 1.f, .02f);
        node->addChildAtPosition(name, geode::Anchor::Left, { 15.f, 0.f });

        auto artist = cocos2d::CCLabelBMFont::create(song->m_artist.c_str(), "bigFont.fnt");
        artist->setAnchorPoint({ 0.f, .5f });
        artist->limitLabelWidth(100.f, 1.f, .02f);
        node->addChildAtPosition(artist, geode::Anchor::Left, { 350.f, 0.f });

        auto check = CCMenuItemToggler::createWithStandardSprites(this, menu_selector(SongViewPopup::onToggleSong), .875f);
        auto menu = cocos2d::CCMenu::create();
        menu->setContentSize(check->getContentSize());
        menu->addChildAtPosition(check, geode::Anchor::Center);
        node->addChildAtPosition(menu, geode::Anchor::Right, { -20.f, 0.f });

        auto object = new SongUserObject;
        object->m_song = song;
        object->autorelease();
        check->setUserObject(object);

        m_scrollLayer->m_contentLayer->addChild(node);
    }

    { auto pad = cocos2d::CCNode::create();
        pad->setContentSize({ 0.f, 5.f });
        m_scrollLayer->m_contentLayer->addChild(pad); }

    if (copy.size() > 20) {
        auto wrap = cocos2d::CCNode::create();
        wrap->setContentSize({ size.width, 30.f });
        auto message = cocos2d::CCLabelBMFont::create("hi", "bigFont.fnt");
        message->setScale(.4f);
        auto icon = cocos2d::CCSprite::create("dialogIcon_054.png");
        icon->setScale(.3f);
        wrap->setLayout(
            geode::RowLayout::create()
                ->setGap(20.f)
                ->setAutoGrowAxis(0.f)
                ->setAutoScale(false)
        );
        wrap->addChild(icon);
        wrap->addChild(message);
        wrap->updateLayout();
        m_scrollLayer->m_contentLayer->addChild(wrap);

        { auto pad = cocos2d::CCNode::create();
            pad->setContentSize({ 0.f, 5.f });
            m_scrollLayer->m_contentLayer->addChild(pad); }
    }

    m_scrollLayer->m_contentLayer->updateLayout();
    m_scrollLayer->scrollToTop();

    m_mainLayer->addChildAtPosition(m_scrollLayer, geode::Anchor::Center, { 0.f, -20.f });

    // geode::Border is too silly
    auto border = geode::NineSlice::create("geode.loader/inverseborder.png");
    border->setContentSize(m_scrollLayer->getScaledContentSize() / 2.f);
    border->setScale(1.01f * 2.f); // remove tiny gaps
    m_mainLayer->addChildAtPosition(border, geode::Anchor::Center, { 0.f, -20.f });

    m_songCountLabel = cocos2d::CCLabelBMFont::create("...", "bigFont.fnt");
    m_songCountLabel->setAnchorPoint({ 0.f, .5f });
    m_songCountLabel->setScale(.3f);
    m_mainLayer->addChildAtPosition(m_songCountLabel, geode::Anchor::Bottom, { 88.f, 18.f });
    updateSongCount();

    return true;
}

void SongViewPopup::updateSongCount() {
    auto& am = AudioManager::get();
    auto plural = m_selected.size() == 1 ? "" : "s";
    auto plural2 = am.m_songs.size() == 1 ? "" : "s";
    auto str = fmt::format("{} song{} selected\n{} song{} total", m_selected.size(), plural, am.m_songs.size(), plural2);
    m_songCountLabel->setString(str.c_str());
}

void SongViewPopup::onInfo(cocos2d::CCObject* sender) {
    static std::vector<const char*> okays = {
        "cool cool",
        "got it",
        "gotcha",
        "nice",
        "i see",
        "this is a great mod",
        "oh yeah",
        "right okay",
        "uhh okay",
        "right",
        "i love editormusic"
    };

    const char* okay = okays[std::rand() % okays.size()];

    FLAlertLayer::create(
        nullptr,
        "Help",
        "<cg>Select</c> a playlist by typing its name at the top.\n"
        "All actions will then be applied to <co>that playlist</c>.\n\n"
        "Select songs from the current playlist, then press <cy>Add to playlist</c> "
        "or <cj>Remove from playlist</c> to do those actions to the playlist "
        "selected at the top.\n\n"
        "Press the <ca>folder icon</c> to open the folder containing the <cr>.m3u</c> files (you can <cl>import</c> "
        "your own playlist files, created externally!)",
        okay, nullptr,
        370.f
    )->show();
}

void SongViewPopup::onFolder(cocos2d::CCObject* sender) {
    geode::utils::file::openFolder(geode::Mod::get()->getConfigDir());
}

void SongViewPopup::onToggleSong(cocos2d::CCObject* sender) {
    auto check = static_cast<CCMenuItemToggler*>(sender);
    auto song = static_cast<SongUserObject*>(check->getUserObject())->m_song;

    if (!check->isOn()) { // callback gets called first, then it gets toggled?
        m_selected.push_back(song);
    } else {
        m_selected.erase(std::remove(m_selected.begin(), m_selected.end(), song), m_selected.end());
    }

    updateSongCount();
}

void SongViewPopup::onLoadPlaylist(cocos2d::CCObject* sender) {
    auto& am = AudioManager::get();
    auto str = geode::utils::string::toLower(geode::utils::string::trim(m_playlistName->getString()));
    m_playlistName->setString(str);
    if (str.empty()) {
        geode::Notification::create("Playlist name cannot be empty!", geode::NotificationIcon::Error)->show();
        return;
    }

    if (!am.playlistFileExists(str)) {
        geode::Notification::create(fmt::format("Playlist {} does not exist!", str), geode::NotificationIcon::Error)->show();
        return;
    }

    am.m_playlist = m_playlistName->getString();
    am.populateSongs();

    geode::Notification::create("Loaded playlist!", geode::NotificationIcon::Success)->show();
    onClose(nullptr);
}

void SongViewPopup::onCreatePlaylist(cocos2d::CCObject* sender) {
    auto& am = AudioManager::get();
    auto str = geode::utils::string::toLower(geode::utils::string::trim(m_playlistName->getString()));
    m_playlistName->setString(str);
    if (str.empty()) {
        geode::Notification::create("Playlist name cannot be empty!", geode::NotificationIcon::Error)->show();
        return;
    }

    if (m_selected.size() == 0) {
        geode::Notification::create("Cannot create empty playlist!\nSelect some songs first.", geode::NotificationIcon::Error)->show();
        return;
    }

    auto res = am.writePlaylistFile(str, m_selected);
    if (res.isErr()) {
        FLAlertLayer::create(
            fmt::format("Error creating {}!", str).c_str(),
            res.unwrapErr(),
            "ok"
        )->show();

        return;
    }

    auto plural = m_selected.size() == 1 ? "" : "s";
    geode::Notification::create(fmt::format("Created {} with {} song{}!", str, m_selected.size(), plural), geode::NotificationIcon::Success)->show();
}

void SongViewPopup::onAddToPlaylist(cocos2d::CCObject* sender) {
    auto& am = AudioManager::get();
    auto str = geode::utils::string::toLower(geode::utils::string::trim(m_playlistName->getString()));
    m_playlistName->setString(str);
    if (str.empty()) {
        geode::Notification::create("Playlist name cannot be empty!", geode::NotificationIcon::Error)->show();
        return;
    }

    if (m_selected.size() == 0) {
        geode::Notification::create("Cannot add zero songs!\nSelect some songs first.", geode::NotificationIcon::Error)->show();
        return;
    }

    if (!am.playlistFileExists(str)) {
        geode::Notification::create(fmt::format("Playlist {} does not exist!\n(did you press create first?)", str), geode::NotificationIcon::Error)->show();
        return;
    }

    auto res = am.addToPlaylistFile(str, m_selected);
    if (res.isErr()) {
        FLAlertLayer::create(
            fmt::format("Error adding to playlist {}!", str).c_str(),
            res.unwrapErr(),
            "ok"
        )->show();

        return;
    }

    auto plural = m_selected.size() == 1 ? "" : "s";
    geode::Notification::create(fmt::format("{} song{} added to {}!\n(Duplicates removed)", m_selected.size(), plural, str), geode::NotificationIcon::Success)->show();
}

void SongViewPopup::onRemoveFromPlaylist(cocos2d::CCObject* sender) {
    auto& am = AudioManager::get();
    auto str = geode::utils::string::toLower(geode::utils::string::trim(m_playlistName->getString()));
    m_playlistName->setString(str);
    if (str.empty()) {
        geode::Notification::create("Playlist name cannot be empty!", geode::NotificationIcon::Error)->show();
        return;
    }

    auto res = am.removeFromPlaylistFile(str, m_selected);
    if (res.isErr()) {
        FLAlertLayer::create(
            fmt::format("Error creating playlist {}!", str).c_str(),
            res.unwrapErr(),
            "ok"
        )->show();
    }

    auto plural = m_selected.size() == 1 ? "" : "s";
    geode::Notification::create(fmt::format("{} song{} removed from {}", m_selected.size(), plural, str), geode::NotificationIcon::Success)->show();
}
