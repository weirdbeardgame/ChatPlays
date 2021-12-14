# ChatPlays
A Twitch plays chat bot API written and intended for in C++ that is compatible with emulator IPC Protocols such as PINE.

The bot currently features support for the PINE protocol in PCSX2 and soon to be RPCS3 that allows for live on the fly edits of game memory allowing for tailored custom commands for each and every game experience developed for chat with it. More information can be found at: https://code.govanify.com/govanify/pine

The bot features ViGEm, https://github.com/ViGEm/ViGEmClient for setup and installation before attempted use of the bot in windows. Which is used to generate a virtual xbox controller intended for general chat input in Windows. Though can be used for TAS like input handling with PINE when nessacary for custom commands.

Linux support with an evdev based controller is currently in WIP.

# How To Use:
When you're finally ready to connect your bot to chat there's a setup process that can be run through manually or one can edit the generated settings file with the nessacary info namely

`channelName`: Your channel you want the bot to connect to.

`userName`: This can be your username or a generated username for the bot.

`oauthToken`: An oauth token used as a password to enable the bot to connect to Twitch. The token can be obtained from, https://twitchapps.com/tmi/

In the controller section for windows all that's needed is a set of commands for chat to enter in the room. 
Any additional settings and commands for PINE will depend on your bot's use and must be added by the implmentation you use.


# ToDo's:
Fix path of settings folder to revolve around main executeable's path.

Fix settings and general interface to be a bit clearer

Fix Linux EvDev handling and configuration which I plan to revolve around user's controller being captured in the settings and played back as a virtual device

Potentially add a GUI for controller interface in Linux and other general bot settings.

Potentially add support for Discord.

Add a demo of PINE functionality that can be fully played.