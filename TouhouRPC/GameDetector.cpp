#include "GameDetector.h"
#include <iostream>
#include <string>

#include "Log.h"

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

std::unique_ptr<TouhouBase> initializeTouhouGame(bool initLogSilence)
{
    PROCESSENTRY32W pe32{};
    SupportedGame game{ SupportedGame::Invalid };

    if (findRunningTouhouProcess(processNameList, pe32, game))
    {
        std::unique_ptr<TouhouBase> thGame{};

        // Game check
        switch (game)
        {
        case SupportedGame::EoSD_6:     thGame = std::make_unique<Touhou06::Touhou06>(pe32); break;
        case SupportedGame::PCB_7:      thGame = std::make_unique<Touhou07::Touhou07>(pe32); break;
        case SupportedGame::IN_8:       thGame = std::make_unique<Touhou08::Touhou08>(pe32); break;
        case SupportedGame::PoFV_9:     thGame = std::make_unique<Touhou09::Touhou09>(pe32); break;
        case SupportedGame::StB_9_5:    thGame = std::make_unique<Touhou09_5::Touhou09_5>(pe32); break;
        case SupportedGame::MoF_10:     thGame = std::make_unique<Touhou10::Touhou10>(pe32); break;
        case SupportedGame::SA_11:      thGame = std::make_unique<Touhou11::Touhou11>(pe32); break;
        case SupportedGame::UFO_12:     thGame = std::make_unique<Touhou12::Touhou12>(pe32); break;
        case SupportedGame::DS_12_5:     thGame = std::make_unique<Touhou12_5::Touhou12_5>(pe32); break;
        case SupportedGame::GFW_12_8:   thGame = std::make_unique<Touhou12_8::Touhou12_8>(pe32); break;
        case SupportedGame::TD_13:      thGame = std::make_unique<Touhou13::Touhou13>(pe32); break;
        case SupportedGame::DDC_14:     thGame = std::make_unique<Touhou14::Touhou14>(pe32); break;
        case SupportedGame::ISC_14_3:   thGame = std::make_unique<Touhou14_3::Touhou14_3>(pe32); break;
        case SupportedGame::LoLK_15:    thGame = std::make_unique<Touhou15::Touhou15>(pe32); break;
        case SupportedGame::HSiFS_16:   thGame = std::make_unique<Touhou16::Touhou16>(pe32); break;
        case SupportedGame::WBaWC_17:   thGame = std::make_unique<Touhou17::Touhou17>(pe32); break;
        case SupportedGame::UM_18_Trial:thGame = std::make_unique<Touhou18::Touhou18Trial>(pe32); break;
        case SupportedGame::UM_18:      thGame = std::make_unique<Touhou18::Touhou18>(pe32); break;

        case SupportedGame::Invalid:
        {
            logSystem->print(Log::LOG_ERROR, "The game has been detected but isn't properly linked by the program. Exiting now...");
            logSystem->closeLogFile();
            std::exit(-1);
            return {};
        }
        // no default, forces compile error when supported game added to enum but this switch isn't updated.
        }

        logSystem->print(Log::LOG_INFO, "Supported game found: %s", thGame->getGameName());
        logSystem->print(Log::LOG_DEBUG, "Supported game process name: \"%S\", PID: %d", pe32.szExeFile, pe32.th32ProcessID);

        if (thGame->isLinkedToProcess())
        {
            logSystem->print(Log::LOG_INFO, "TouhouRPC is now linked to the game.");
            return thGame;
        }
        else
        {
            logSystem->print(Log::LOG_WARNING, "Failed linking TouhouRPC to the game.");
            return {};
        }
    }
    else
    {
        if (!initLogSilence) logSystem->print(Log::LOG_WARNING, "No supported game currently running. Waiting for a game to be launched...");
        return {};
    }
}