# EditorMusic
## v2.2.6
- Added Geode v5 and GD 2.2081 support!
- Replaced all tabs with spaces
- Updated simdutf from 7.0.0 to 7.7.1
## v2.2.5
- Fixed a crash that occured if you entered the editor before all your songs were loaded
## v2.2.4
- Improved compatibility with Song Stopped At
## v2.2.3
- Fixed url album covers to ensure they don't crash
## v2.2.2
- Fixed the next song label being visible in the editor always if you had zero songs
## v2.2.1
- Fixed the mod instantly crashing on startup
## v2.2.0
- Fixed many, many issues with song paths that contain non-ascii characters - they should no longer be an issue!
- Added caching to album covers to reduce memory usage
- Fixed issues with the loading UI not showing above all layers
- Fixed issues where the song would not end after it should have
- Fixed issues relating to incorrect song lengths
## v2.1.15
- iOS support!
- Fixed issues with the pause keybind not updating the button in the current song popup
- Fixed issues with the current song popup not closing if you closed the editor pause menu
- Fixed issues with songs in history not being preloaded when they should
- Add popup to show any songs that had errors while loading
## v2.1.14
- Fixed songs longer than one hour or take up more space than memory available on the system
- Made reading metadata more consistent and less crashable when filtering invalid characters
- Included more information in crashlogs and add more error checking
## v2.1.13
- Formatted multiple artists correctly
- Fixed hardware buttons not working and music not closing when game is closed
- Refixed other crash issues when closing the game with the song popup open
## v2.1.12
- Fixed issues with changing between fullscreen and windowed
- Fixed other crash issues when closing the game with the song popup open
- Added mod preview images!
## v2.1.11
- Fixed issues that caused songs to start playing if it's meant to be paused on levels that take a long time to load
## v2.1.10
- New logo!
## v2.1.9
- Fixed a memory leak when reloading songs
## v2.1.8
- Fixed issues with Creation Rotation and other mods
## v2.1.7
- Added Custom Keybinds support for MacOS
## v2.1.6
- Fixed issues with zero songs
## v2.1.5
- Add physical media button support
- Fixed issues with Quick Volume Controls
- Made music stop instantly when the game is closed
## v2.1.4
- Added currently playing song notification
- Fixed issues with non-square album covers and rounding
## v2.1.3
- Fixed keybinds in editor not working on android
- Fixed issues with positioning in the song popup when you have zero songs
- Fixed issues with songs not preloading if you've just entered the editor
- Improved compatibility with Object Workshop
## v2.1.2
- Rift label support to allow you to use EditorMusic info in Eclipse Discord Rich Presence!
## v2.1.1
- MacOS support! (Erymanthus)
## v2.1.0
- Rearranged order of some settings
- Fixed crash with one song coming from a level (#27)
- Added ability to round corners of the album cover
- Added ability to refresh song list from mod settings
- Saved paused state between sessions
## v2.0.0
- Completed rewrite of the entire mod from the ground up!
- Redid and improved UI
- Added loading album art
- Improved reliability to reduce crashes
- Fixed many bugs
- Improved logging to allow for easier debugging and easier fixing
- Preloads the next 3 and last 3 songs
- Fixed loading issues (#23)
- Now runs on a queue and history system, so songs should not be repeated
- Now supports folders inside the music list
---
## v1.3.11
- Fixed UI in pause layer
## v1.3.10
- idk i think i fixed something relating to v1.3.9??? maybe??
## v1.3.9
- General bugfixes for crashes relating to the "Show current song in editor" setting
## v1.3.8
- Geode 4.0.0 / GD 2.2074 support!
- Other bugfixes with text sizes and custom keybinds
## v1.3.7
- Small fixes on how metadata is read (see #16)
- I should've really made v1.3.6 and v1.3.4 minor changes and not patches but whatever
## v1.3.6
- Change the text for how to get to mod config folder
- Add fading for volume as well as low-pass filter to prevent popping (and make it sound nice)
- Songs now only load when needed and not at startup
## v1.3.5
- Remove a log that logged info every single frame
## v1.3.4
- Geode 3.1.0 support! (this required a rewrite of like everything :sob:)
- Added togglable low-pass filter
## v1.3.3
- Fix -10s button going to previous song regardless of how long was left
## v1.3.2
- Filter out file names that aren't supported
- Add option to customise filtered letter
- Bump to beta 27
## v1.3.1
- Fix folder input
- Time formatting changes
## v1.3.0
- Slightly wider menu (raydeeux)
- Dynamic song label scaling, after much pestering from Capeling (raydeeux)
- Song info dialog (Suggested and mockup created by NonCloud9)
## v1.2.1
- Fix android not being able to update
## v1.2.0
- Keybinds!
- Quick settings in the pause menu
- Rework of parts of AudioManager
## v1.1.2
- Bugfixes (this is probably the most bugfixes I've had to do in a mod)
- Custom song path
## v1.1.1
- Bugfixes
- Support for utf-16 strings (probably could be put under bugfixes)
## v1.1.0
- Bugfixes
- Added the current playing song in the editor
- "Added" .flac support
## v1.0.2
- Fix random bugs and add editor pause menu button
## v1.0.1
- Android support
## v1.0.0
- Initial release
