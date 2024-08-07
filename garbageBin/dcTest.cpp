#include <windows.h>
#include <tlhelp32.h>
#include <string>
#include <iostream>

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

int main() {
    terminateProcess(L"Discord.exe");
    return 0;
}