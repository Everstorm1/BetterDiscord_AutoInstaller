#include <windows.h>
#include <tlhelp32.h>
#include <string>
#include <iostream>
#include <filesystem>
#include <shellapi.h>

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

int main() {
    terminateProcess(L"Discord.exe");
    Sleep(3000);

    std::wstring discordPath = L"C:\\Users\\Ayen Wohlberg\\AppData\\Local\\Discord\\app-1.0.9157\\Discord.exe";
    
    if (startProcess(discordPath)) {
        std::wcout << L"Waiting for Discord to start..." << std::endl;
        Sleep(5000);  // Wait for 5 seconds
        std::wcout << L"Check if Discord is now running." << std::endl;

        return 0;
    } else {
        std::wcerr << L"Failed to initiate Discord launch" << std::endl;
    }

    return 0;
}