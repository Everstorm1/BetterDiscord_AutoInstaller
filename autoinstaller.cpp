#include <windows.h>
#include <tlhelp32.h>
#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>

bool autoASAR = false;
std::string ASAR_v_file = "";

bool to_bool(std::string str){
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    std::istringstream is(str);
    bool b;
    is >> std::boolalpha >> b;
    return b;
}

std::string getLocalPath(){
    std::ifstream finderFile("finder_TXT/finder.txt");
    std::string path_to_localDiscord;

    //twice for line 2
    std::cout << "get local path.." << std::endl;
    std::getline(finderFile, path_to_localDiscord);
    std::getline(finderFile, path_to_localDiscord);

    //thrice for line 5
    std::string val;
    std::getline(finderFile, val);
    std::getline(finderFile, val);
    std::getline(finderFile, val);
    autoASAR = to_bool(val);
    std::cout << "auto asar?: " << autoASAR << std::endl;
    
    finderFile.close();

    std::ifstream versionFile("finder_TXT/BD_version.txt");
    //first line
    std::getline(versionFile, ASAR_v_file);
    versionFile.close();

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
    std::cout << "copy .asar to core.." << std::endl;
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
// but it works.. - rest of the (simple) code written by me (Everstorm1)  -----------------------------------------

std::string getAsarVersion(){

    std::cout << "Check for new BetterDiscord versions.." << std::endl;

    #ifdef _WIN32
    std::system("powershell -Command \"Invoke-WebRequest -Uri 'https://api.github.com/repos/BetterDiscord/BetterDiscord/tags' -OutFile tags.json\"");
    #else
    std::system("wget -qO tags.json https://api.github.com/repos/BetterDiscord/BetterDiscord/tags");
    #endif

    std::ifstream file("tags.json");

    std::string line, latestTag;
    while (std::getline(file, line)) {
        size_t pos = line.find("\"name\":\"v");
        if (pos != std::string::npos) {
            latestTag = line.substr(pos + 8);
            latestTag = latestTag.substr(0, latestTag.find_first_of("\""));
            break;
        }
    }

    file.close();
    std::system("del tags.json"); // Delete temp file

    std::cout << "Latest BetterDiscord version: " << latestTag << std::endl;
    return latestTag;
}

bool ASAR_version_match(){
    std::string currentVersion = getAsarVersion();

    if(currentVersion == ASAR_v_file){
        std::cout << "Versions match!" << std::endl;
        return true;
    }
    ASAR_v_file = currentVersion;
    std::cout << "Versions do NOT match. Proceeding to download newest betterDiscord.asar" << std::endl;
    return false;
}

void replace_asar(std::string version){

    std::system("del \"betterDiscord_ASAR\\betterdiscord.asar\""); // Delete old .asar file

    std::string pathToVERSION = "finder_TXT/BD_version.txt";
    std::string url = "https://github.com/BetterDiscord/BetterDiscord/releases/download/" + version + "/betterdiscord.asar";
    std::string outputFile = "betterDiscord_ASAR/betterdiscord.asar";

    #ifdef _WIN32
    std::string command = "powershell -Command \"Invoke-WebRequest -Uri '" + url + "' -OutFile '" + outputFile + "'\"";
    #else
    std::string command = "wget -O " + outputFile + " " + url;
    #endif
    int result = std::system(command.c_str());
    
    if (result == 0) {
        std::cout << "Download successfull: " << outputFile << std::endl;

        //Update version in BD_version.txt
        //del old file
        if (std::filesystem::exists(pathToVERSION)) 
        {
            std::filesystem::remove(pathToVERSION);
        }
        //create new
        std::ofstream file{"finder_TXT/BD_version.txt"};
        //add new content
        file << version;
        file.close();

    } else {
        std::cerr << "Download failed! Insert .asar manually!" << std::endl;
    }
}



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


    //check if ASAR_autoupdate is enabled
    if(autoASAR){
        //check if versions match
        bool versionIsMatching = ASAR_version_match();
        if(versionIsMatching){
            //versions match, check if BD ist installed
            if(std::filesystem::exists(pathToCore + "/betterdiscord.asar")){
                //exists - start Discord
                std::cout << "\n" << "BetterDiscord is already injected! You can already start Discord :P" << std::endl;

            }else{
                //does not exist, create and inject, wait
                injectBetterDiscord(pathToCore);
                Sleep(500);

                std::cout << "\n" << "Injected BetterDiscord successfully! You may now start Discord :)" << std::endl;
            }
        }
        else{
            //versions don't match, renew .asar
            replace_asar(ASAR_v_file);
            //then inject as normal
            injectBetterDiscord(pathToCore);
            Sleep(500);

            std::cout << "\n" << "Injected BetterDiscord successfully! You may now start Discord :)" << std::endl;
        }
    }else{
        //normal procedure
        //check if betterdiscord.asar already exists (if it does, then better Discord is already injected)
        if(std::filesystem::exists(pathToCore + "/betterdiscord.asar")){
            //exists - start Discord
            std::cout << "\n" << "BetterDiscord is already injected! You can already start Discord :P" << std::endl;

        }else{
            //does not exist, create and inject, wait
            injectBetterDiscord(pathToCore);
            Sleep(500);

            std::cout << "Injected BetterDiscord successfully! You may now start Discord :)" << std::endl;
        }
    }

    std::cout << "\n" << "\n" << "Press any key to exit.." << std::endl;
    std::getchar();
    return 0;
}