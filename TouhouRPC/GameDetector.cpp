#include "GameDetector.h"
#include <iostream>
#include <string>

bool findRunningTouhouProcess(const ProcessNameGamePair processList[], PROCESSENTRY32W& processEntry, SupportedGame& processGame)
{

        WCHAR* compare;
        bool procRunning = false;

        HANDLE hProcessSnap;
        PROCESSENTRY32W pe32;
        hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

        if (hProcessSnap == INVALID_HANDLE_VALUE) {
            procRunning = false;
        }
        else {
            pe32.dwSize = sizeof(PROCESSENTRY32W);
            if (Process32FirstW(hProcessSnap, &pe32)) {
                // Check every process
                do {
                    compare = pe32.szExeFile;
                    for (size_t i = 0; i < PROCESS_NAME_LIST_SIZE; i++)
                    {
                        if (!wcscmp(compare, processList[i].processName)) {
                            // If found process is from the touhou exe name list, set to true and break from loop
                            processEntry = pe32;
                            processGame = processList[i].game;
                            procRunning = true;
                            break;
                        }
                    }
                } while (Process32NextW(hProcessSnap, &pe32) && !procRunning);
                // clean the snapshot object
                CloseHandle(hProcessSnap);
            }
        }

        return procRunning;
}

std::unique_ptr<TouhouBase> initializeTouhouGame()
{
    PROCESSENTRY32W pe32{};
    SupportedGame game{ SupportedGame::Invalid };

    if (findRunningTouhouProcess(processNameList, pe32, game))
    {
        std::unique_ptr<TouhouBase> thGame{};

        // Game check
        switch (game)
        {
        case SupportedGame::EoSD_6:     thGame = std::make_unique<Touhou06>(pe32); break;
        case SupportedGame::PCB_7:     thGame = std::make_unique<Touhou07::Touhou07>(pe32); break;
        case SupportedGame::IN_8:       thGame = std::make_unique<Touhou08>(pe32); break;
        case SupportedGame::SA_11:      thGame = std::make_unique<Touhou11>(pe32); break;
        case SupportedGame::DDC_14:     thGame = std::make_unique<Touhou14::Touhou14>(pe32); break;
        case SupportedGame::LoLK_15:    thGame = std::make_unique<Touhou15>(pe32); break;
        case SupportedGame::HSiFS_16:   thGame = std::make_unique<Touhou16>(pe32); break;
        case SupportedGame::WBaWC_17:   thGame = std::make_unique<Touhou17>(pe32); break;

        case SupportedGame::Invalid:
        {
            std::cout << "An unexpected error occured. If you see this message, then the game has been detected but isn't properly linked by the program." << std::endl;
            std::cout << "Exiting now..." << std::endl;
            std::exit(-1);
            return {};
        }
        // no default, forces compile error when supported game added to enum but this switch isn't updated.
        }

        std::cout << "Found running game: " << thGame->getGameName() << "." << std::endl;
        std::wcout << "Found process: \"" << pe32.szExeFile << "\", PID: " << pe32.th32ProcessID << std::endl; // For debug

        if (thGame->isLinkedToProcess())
        {
            std::cout << "The program is now linked to the game. Starting Rich Presence display..." << std::endl;
            return thGame;
        }
        else
        {
            std::cout << "Game linking has failed." << std::endl;
            return {};
        }
    }
    else
    {
        std::wcout << "Error: No supported game currently running." << std::endl;
        return {};
    }
}