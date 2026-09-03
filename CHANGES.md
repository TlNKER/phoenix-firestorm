# Tinkerstorm changes
Maintained by Tinker (hello@ -- .sl)
Local build 81669. Confirmed stable against all commits up to 2026/09/02 11:00. 

## On c++ level
- **indra/newview/llappviewer.cpp** line #3564. Changed Windowtitle format to Tinkerstorm - [Username].

## On xml level
- **app_settings/grids.xml** Changed url to my own splash screen.
- **app_settings/cloud.xml** Changed colours of clouds from red to pink.
- **skins/default/xui/en/panel_chiclet_bar.xml** Moved chiclets outside view. 
- **skins/default/xui/en/anel_fs_nui_login.xml** Updated background colour for log-in screen.
- **skins/default/xui/en/panel_status_bar.xml** Hiding BUY button, clock, and all icons except stream and volume.
- **skins/default/xui/en/menu_viewer.xml** Replaced entire drop-down menu structure with my own.
- **skins/default/xui/en/menu_viewer.xml** Adding Alt-U shortcut for Unavailable.
- **skins/default/xui/en/menu_viewer.xml** Adding Alt-N shortcut for About Land.
- **skins/skins.xml** Adding Tinkered (Classic) as skin option.
- **skins/default/xui/en/panel_main_inventory.xml** Removed just about everything from inventory floater.
- **skins/default/xui/en/floater_profile.xml** Removed Feed and Classified from view.
- **skins/default/xui/en/floater_fs_im_session.xml** Removed tab toolbar for IM and group chats.
- **skins/default/xui/en/floater_fs_nearby_chat.xml** Removed tab toolbar for nearby chat.

## Other
- **skins/tinkered** My own skin based on LL's FlatUI and Ansastorm Blue elements.
- **skins/default/textures/legacy/active_voice_tab.tga** Replaced with transparent to remove voice icon on Nearby Chat tab.
- **skins/default/textures/windows/login_fs_logo.png** Changed the round logo at the bottom of the login screen.
- **skins/default/textures/icons/profile_badge_team.png** Replaced org FS logo with Blue Tinkerstorm logo for team profiles.
- **fsicon.ico** Placed in root, replaces the shortcut's icon PROBABLY OBSOLETE NOW THAT THE VIEWER HAS DIFFERENT ICON.
- **firestorm_icon.ico** replaced in indra/newview/icons/ various folders.

- Build with --chan Tinkerstorm. 
