# How-to-use:
There are 2 folders that are important to you. In the betterDiscord_ASAR folder you need to put the betterdiscord.asar file that you want to inject into discord (you can get it from the official BetterDiscord GitHub Repo)
The second folder is **finder_TXT** where you **MUST** add your username to the path in the second line! Otherwise the installer will NOT work!

After doing this you can start the BetterDiscordAutoInjector.exe which will inject BetterDiscord automatically into the selected path. The .exe needs to be in the same directory as the finder_TXT, betterDiscord_ASAR and index_JSON folders, otherwise it will NOT work.
For easier use I recommend making a shortcut to the .exe

# What it does:
With the BetterDiscordAutoInjector.exe the .asar file will be automatically injected in the chosen path into the newest version of Discord.
If BetterDiscord is already injected: nothing will happen and the program terminates.
If BetterDiscord is not already injected: Discord will be closed and BetterDiscord injected, you can then reopen Discord.

> [!IMPORTANT]
> Discord will NOT be reopened after injection by the BetterDiscordAutoInjector.exe, you will need to do that manually
> The autoInstallerPLUS.cpp in the prototype folder however WILL reopen Discord after injection. But since the code still contains bugs I left it out of the current version of the installer. Read the prototype section on how to make it work if you wish to try.


## Prototype
> [!CAUTION]
> The autoInstallerPLUS prototype may contain bugs, I got it to work on my system but have no clue how. So you'd need more than my very basic understanding of C++ to get better/stable code xD.
If you wish to use the autoInstallerPLUS (which will reopen Discord after injection) you will have to compile the autoInstallerPLUS.cpp. For this the .cpp file needs to be in the same directory as the finder_TXT, index_JSON and betterDiscord_ASAR folders.
You will also need to change the path variable to the Discord.exe in the main() function of the .cpp to contain your username, otherwise it will NOT work (currently set to /User/{User name}/...).
After you added your username and put the autoInstallerPLUS.cpp into the correct directory you can compile with\
``
g++ -o BetterDiscordAutoInjectorPLUS .\autoInstallerPlus.cpp
``\
for example.
Good luck :)
