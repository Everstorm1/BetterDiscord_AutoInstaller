/**
 * 
 *          IT WORKS BUT...

 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        This file contains a version of the autoinjector script that also starts discord. This has however
        been extremely unstable thus far which is why I left it out if the "normal" injector .exe and main
        cpp file. This file will be moved into a prototype folder so be aware that when using this all the
        paths to the finder_TXT folder etc. will not be correct anymore. To compile this file would need 
        to be in the same directory as all folders / or the autoinstaller.cpp 
  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  Use at own risk lol
*/

#include <windows.h>
#include <tlhelp32.h>
#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>

std::string getLocalPath(){
    std::ifstream finderFile("finder_TXT/finder.txt");
    std::string path_to_localDiscord;

    //twice for line 2
    std::getline(finderFile, path_to_localDiscord);
    std::getline(finderFile, path_to_localDiscord);

    finderFile.close();

    return path_to_localDiscord;
}

std::vector<std::string> getAllAppDirs(std::string& path){
    std::string dirKeyword = "app-1.0.";
    std::vector<std::string> allAppDirs;

    for (const auto & entry : std::filesystem::directory_iterator(path)){
        //convert to std::string
        std::string pathToSubdirs = entry.path().u8string();
        if(pathToSubdirs.find(dirKeyword) != std::string::npos){
            allAppDirs.push_back(entry.path().u8string());
        }
    }

    return allAppDirs;
}

std::string getPathToNewestVersion(std::vector<std::string>& allDirs, std::string& localDPath){
    std::vector<int> DcVersions; //dim-0: index of dir in allAppDirs (in-case multiple folders exist); dim-1: version number
    int pathLength = allDirs[0].length();
    int versionLength = 4;
    for(int i = 0; i < allDirs.size(); i++){
        std::string rawVersion = allDirs[i].substr(pathLength - versionLength, pathLength);
        DcVersions.push_back(std::stoi(rawVersion));
    }
    //get newest dc version (sort through all found dirs)
    int currentNewestVersion = 0;
    for(int v = 0; v < DcVersions.size(); v++){
        if(DcVersions[v] > currentNewestVersion){
            currentNewestVersion = DcVersions[v];
        }
    }

    //make sure the app-1.0. is still up to date. Will cause issues when discord decided to start versions like this app-1.1.xxxx
    std::string pathToCurrentVersion = localDPath + "/" + "app-1.0." + std::to_string(currentNewestVersion); //---------------------------

    return pathToCurrentVersion;
}

std::string getPathToCore(std::string& currentPath){
    std::string corePathExtension = "/modules/discord_desktop_core-1/discord_desktop_core";
    std::string fullPath = currentPath + corePathExtension;

    return fullPath;
}

void injectBetterDiscord(std::string pathToCore){
    //for creating the .asar
    std::string pathToBetterDiscordASAR = "betterDiscord_ASAR/betterdiscord.asar";
    std::string pathToASARDestination = pathToCore + "/betterdiscord.asar";
    // Remove the destination file if it exists
    if (std::filesystem::exists(pathToASARDestination)) {
        std::filesystem::remove(pathToASARDestination);
    }
    std::filesystem::copy_file(pathToBetterDiscordASAR, pathToASARDestination, std::filesystem::copy_options::overwrite_existing);

    //for editing the .js
    std::string pathToIndexReplayerJS = "index_JSON/index_replacer.js";
    std::string pathToJSDestination = pathToCore + "/index.js";
    // Remove the destination file if it exists
    if (std::filesystem::exists(pathToJSDestination)) {
        std::filesystem::remove(pathToJSDestination);
    }
    std::filesystem::copy_file(pathToIndexReplayerJS, pathToJSDestination, std::filesystem::copy_options::overwrite_existing);
}

// starting here the code was written by Claude 3.5 Sonnet and I do not possess the knowledge about what it does :) -----
void terminateProcess(const std::wstring& processName) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        std::wcerr << L"Failed to create snapshot" << std::endl;
        return;
    }

    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);

    if (Process32FirstW(hSnapshot, &pe32)) {
        do {
            if (processName == pe32.szExeFile) {
                HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
                if (hProcess != NULL) {
                    if (TerminateProcess(hProcess, 0)) {
                        std::wcout << L"Process terminated successfully" << std::endl;
                    } else {
                        std::wcerr << L"Failed to terminate process" << std::endl;
                    }
                    CloseHandle(hProcess);
                } else {
                    std::wcerr << L"Failed to open process" << std::endl;
                }
                break;
            }
        } while (Process32NextW(hSnapshot, &pe32));
    } else {
        std::wcerr << L"Failed to get first process" << std::endl;
    }

    CloseHandle(hSnapshot);
}

bool startProcess(const std::wstring& applicationPath) {
    STARTUPINFOW si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    std::wstring commandLine = L"\"" + applicationPath + L"\"";

    if (!CreateProcessW(
        NULL,
        &commandLine[0],
        NULL,
        NULL,
        FALSE,
        CREATE_NO_WINDOW,  // This flag prevents a console window from opening
        NULL,
        std::filesystem::path(applicationPath).parent_path().c_str(),
        &si,
        &pi
    )) {
        DWORD error = GetLastError();
        std::wcerr << L"Failed to start Discord. Error code: " << error << std::endl;
        return false;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return true;
}

// but it works.. - rest of the (simple) code written by me (Everstorm1)  -----------------------------------------

int main(){
    //kill Discord
    terminateProcess(L"Discord.exe"); //Don't ask me about how this function works..
    //set within finder.txt file
    std::string localDataPath = getLocalPath();
    //condense Discord Local AppData to dirs containing the app files (each dc update creates a new app-1.x.xxxx folder)
    std::vector<std::string> allAppDirs = getAllAppDirs(localDataPath);
    //find the path to the newest dc dir
    std::string newestAppDir = getPathToNewestVersion(allAppDirs, localDataPath);
    //find path to discord_desktop_core
    std::string pathToCore = getPathToCore(newestAppDir);
    
    // ---- until here the code only retrieves the needed directory, jesus christ ----

    //check if betterdiscord.asar already exists (if it does, then better Discord is already injected)
    if(std::filesystem::exists(pathToCore + "/betterdiscord.asar")){
        //exists - start Discord
        std::cout << "BetterDiscord is already injected! You can already start Discord :P" << std::endl;
        return 0;
    }else{
        //does not exist, create and inject, wait
        injectBetterDiscord(pathToCore);
        Sleep(500);

        std::cout << "Injected BetterDiscord successfully! You may now start Discord :)" << std::endl;


        //--------------------------- add user name here !! -----------------------------------------------------------------------------------------------------------------------------------
        std::wstring discordPath = L"C:\\Users\\{user name}\\AppData\\Local\\Discord\\app-1.0.9157\\Discord.exe";
    
        if (startProcess(discordPath)) {
            std::wcout << L"Waiting for Discord to start..." << std::endl;
            Sleep(5000);  // Wait for 5 seconds
            std::wcout << L"Check if Discord is now running." << std::endl;

            return 0;
        } else {
            std::wcerr << L"Failed to initiate Discord launch" << std::endl;
        }
    }

    return 0;
}