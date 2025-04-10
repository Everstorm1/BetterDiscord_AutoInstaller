### Info:
Updated the code to work with the latest Discord update (April 10, 2025). Just replace the .exe with the new one.

# How-to-use:
Locate the *finder_TXT* folder and open the *finder.txt* file. Here you **MUST** add your username to the path in the second line! Otherwise the installer will NOT work! \
Here you can also select whether the program should automatically update BetterDiscord aswell or if it should only inject the betterdiscord.asar you manually chose (There is already a *betterdiscord.asar* file in the *betterDiscord_ASAR* folder that you can replace if you want a specific version). \
So choose either *true* or *false* in line 5 of the *finder.txt* file. 

(If you want to manually select a .asar file you can get them here: https://github.com/BetterDiscord/BetterDiscord/releases)

After doing this you can start the autoinstaller.exe which will inject BetterDiscord automatically into the selected path and, depending on what you selected, update BetterDiscord beforehand. The .exe needs to be in the same directory as the finder_TXT, betterDiscord_ASAR and index_JSON folders, otherwise it will **NOT** work.
For easier use I recommend making a shortcut to the autoinstaller.exe to your desktop for example.

# What it does:
With the autoinstaller.exe the .asar file will be automatically injected in the chosen path into the newest version of Discord.\
If BetterDiscord is already injected and up to date: nothing will happen and the program terminates.\
If BetterDiscord is not already injected: Discord will be closed and BetterDiscord injected, you can then reopen Discord.

> [!IMPORTANT]
> Discord will NOT be reopened after injection by the autoinstaller.exe, you will need to do that manually. \
> The autoInstallerPLUS.cpp in the prototype folder however WILL reopen Discord after injection. But since the code still contains bugs I left it out of the current version of the installer. Read the prototype section on how to make it work if you wish to try.


## Prototype
> [!CAUTION]
> The autoInstallerPLUS prototype may contain bugs, I got it to work on my system but have no clue how. So you'd need more than my very basic understanding of C++ to get better/stable code xD.

If you wish to use the autoInstallerPLUS (which will reopen Discord after injection) you will have to compile the autoInstallerPLUS.cpp. For this the .cpp file needs to be in the same directory as the finder_TXT, index_JSON and betterDiscord_ASAR folders.
You will also need to change the path variable to the Discord.exe in the main() function of the .cpp to contain your username, otherwise it will NOT work. \
(currently set to /User/{User name}/... 
```cpp
std::wstring discordPath = L"C:\\Users\\{user name}\\AppData\\Local\\Discord\\app-1.0.9157\\Discord.exe";
```
)  \
After you added your username and put the autoInstallerPLUS.cpp into the correct directory you can compile with
```console
g++ -m64 -static -o BetterDiscordAutoInjectorPLUS .\autoInstallerPLUS.cpp
```
\
for example.
Good luck :)
