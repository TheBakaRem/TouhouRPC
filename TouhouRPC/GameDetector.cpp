#include "GameDetector.h"
#include <iostream>
#include <string>

bool findRunningTouhouProcess(const wchar_t* processList[], PROCESSENTRY32W* processEntry)
{

        WCHAR* compare;
        bool procRunning = false;

        HANDLE hProcessSnap;
        PROCESSENTRY32 pe32;
        hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

        if (hProcessSnap == INVALID_HANDLE_VALUE) {
            procRunning = false;
        }
        else {
            pe32.dwSize = sizeof(PROCESSENTRY32);
            if (Process32First(hProcessSnap, &pe32)) {
                // Check every process
                do {
                    compare = pe32.szExeFile;
                    for (size_t i = 0; i < PROCESS_NAME_LIST_SIZE; i++)
                    {
                        const wchar_t* process = processList[i];
                        if (!wcscmp(compare, process)) {
                            // If found process is from the touhou exe name list, set to true and break from loop
                            *processEntry = pe32;
                            procRunning = true;
                            break;
                        }
                    }
                } while (Process32Next(hProcessSnap, &pe32) && !procRunning);
                // clean the snapshot object
                CloseHandle(hProcessSnap);
            }
        }

        return procRunning;
}

TouhouBase* initializeTouhouGame()
{
	PROCESSENTRY32W* pe32 = new PROCESSENTRY32W;

    if (!pe32) {
        std::cerr << "FATAL ERROR: No memory space left for memory allocation. Terminating program immediately." << std::endl;
        std::exit(-1);
    }
    else {
        if (findRunningTouhouProcess(processNameList, pe32)) {
            TouhouBase* thGame = nullptr;

            // Game check
            if (!wcscmp(pe32->szExeFile, L"eosd.exe") || !wcscmp(pe32->szExeFile, L"th06e.exe")) {
                // Touhou 06 detected
                std::cout << "Found running game: Touhou 06 - Embodiment of Scarlet Devil." << std::endl;
                thGame = new Touhou06(pe32);
            }
            else if (!wcscmp(pe32->szExeFile, L"th08.exe") || !wcscmp(pe32->szExeFile, L"th08e.exe")) {
                // Touhou 08 detected
                std::cout << "Found running game: Touhou 08 - Imperishable Night." << std::endl;
                thGame = new Touhou08(pe32);
            }
            else if (!wcscmp(pe32->szExeFile, L"th11.exe")) {
                // Touhou 11 detected
                std::cout << "Found running game: Touhou 11 - Subterranean Animism." << std::endl;
                thGame = new Touhou11(pe32);
            }
            else if (!wcscmp(pe32->szExeFile, L"th15.exe")) {
                // Touhou 15 detected
                std::cout << "Found running game: Touhou 15 - Legacy of Lunatic Kingdom." << std::endl;
                thGame = new Touhou15(pe32);   
            }
			else if (!wcscmp(pe32->szExeFile, L"th17.exe")) {
				std::cout << "Found running game: Touhou 17 - Wily Beast and Weakest Creature." << std::endl;
				thGame = new Touhou17(pe32);
			}
            else {
                std::cout << "An unexpected error occured. If you see this message, then the game has been detected but isn't properly linked by the program." << std::endl;
                std::cout << "Exiting now..." << std::endl;
                std::exit(-1);
            }

            std::wcout << "Found process: \"" << pe32->szExeFile << "\", PID: " << pe32->th32ProcessID << std::endl; // For debug
    
            if (thGame->isLinkedToProcess()) {
				std::cout << "The program is now linked to the game. Starting Rich Presence display..." << std::endl;
				return thGame;
            }
            else {
                std::cout << "Game linking has failed." << std::endl;
				return nullptr;
            }
        }
        else {
            std::wcout << "Error: No supported game currently running." << std::endl;
			return nullptr;
        }

    }

}
