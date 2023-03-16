# Unreal-Achievement-OnlineToolBox
An exemple of a online toolbox plugin made for a network course to get player stats and achievements from Epic Online Game Services

All the source code for the plugin is in 
Plugins/OnlineToolbox/Source/OnlineToolbox 

The plugin is composed of : 
- Identity Subsystem : subsystem to login/logout the user to the game services with the information of the user.
- Session Subsystem : manage the client session from login to logout
- Stats Subsystem : handle the request to get and update player statistics create on the game product (kill count, walk distances,...).
- Achievment Subsystem : handle the request to get player achievements. 
