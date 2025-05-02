# EditorMusic
by [undefined0](user:13351341)

---

Ever thought your editor experience was too quiet?
Ever needed some funky music to boost your energy, or some calm music to help you focus on level building?
Hate editing in music in post, or want to liven your livestream?

Well do I have the mod for you!

This just adds music to the editor, which pauses when playtesting and shuffles through your music randomly. Place your .mp3, .wav or .ogg files in your config directory (press ![the settings button](frame:geode.loader/settings.png?scale=0.375) then ![the mod save folder button](undefined0.editormusic/geode-folder.png?scale=0.6)), or use a custom directory!


This mod has Rift label support! Use `{currentEditorSong}`, `{currentEditorSongArtist}`, `{editorSongDuration}` and `{editorSongTimestamp}` in your Eclipse Discord RPC or labels today!

If your game crashes on startup as a result of installing this mod, **please** ensure that it is not because of an installed song before sending a bug report! Common pitfalls (*especially* so on Windows, less so on android):
- Songs with non latin characters (A-z), numbers, spaces or - and _ in the song title
- Songs with non ascii characters in the metadata
- Songs with metadata not formatted as UTF-8
- Renamed mp4 files (though if your music is of an obscure format and it is certainly an audio file, try renaming to an mp3 to force the mod to attempt to load it!)
