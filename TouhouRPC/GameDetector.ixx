module;

#include "games/TouhouBase.h"
#include "games/Touhou06.h"
#include "games/Touhou07.h"
#include "games/Touhou08.h"
#include "games/Touhou09.h"
#include "games/Touhou09_5.h"
#include "games/Touhou10.h"
#include "games/Touhou11.h"
#include "games/Touhou12.h"
#include "games/Touhou12_5.h"
#include "games/Touhou12_8.h"
#include "games/Touhou13.h"
#include "games/Touhou14.h"
#include "games/Touhou14_3.h"
#include "games/Touhou15.h"
#include "games/Touhou16.h"
#include "games/Touhou17.h"
#include "games/Touhou18.h"

export module GameDetector;

import <vector>;
import <string>;
import <memory>;
import Log;

using namespace std;

enum class SupportedGame {
    EoSD_6,
    PCB_7,
    IN_8,
    PoFV_9,
    StB_9_5,
    MoF_10,
    SA_11,
    UFO_12,
    DS_12_5,
    GFW_12_8,
    TD_13,
    DDC_14,
    ISC_14_3,
    LoLK_15,
    HSiFS_16,
    WBaWC_17,
    UM_18_Trial,
    UM_18,

    Invalid,
};

struct ProcessNameGamePair {
    SupportedGame game;
    const wstring processName;
    //const wchar_t* processName;
};

// Executables name list and associated game
const vector<ProcessNameGamePair> processNameList =
{
    { SupportedGame::EoSD_6,      L"eosd.exe" },
    { SupportedGame::EoSD_6,      L"th06e.exe" },
    { SupportedGame::EoSD_6,      L"“Œ•ûg–‚‹½.exe" },
    { SupportedGame::EoSD_6,      L"東方紅魔郷.exe" },
    { SupportedGame::PCB_7,       L"th07.exe" },
    { SupportedGame::PCB_7,       L"th07e.exe" },
    { SupportedGame::IN_8,        L"th08.exe" },
    { SupportedGame::IN_8,        L"th08e.exe" },
    { SupportedGame::PoFV_9,      L"th09.exe" },
    { SupportedGame::StB_9_5,     L"th095.exe" },
    { SupportedGame::MoF_10,      L"th10.exe" },
    { SupportedGame::SA_11,       L"th11.exe" },
    { SupportedGame::UFO_12,      L"th12.exe" },
    { SupportedGame::DS_12_5,     L"th125.exe" },
    { SupportedGame::GFW_12_8,    L"th128.exe" },
    { SupportedGame::TD_13,       L"th13.exe" },
    { SupportedGame::DDC_14,      L"th14.exe" },
    { SupportedGame::ISC_14_3,    L"Touhou 14,3 Impossible Spell Card.exe" },
    { SupportedGame::ISC_14_3,    L"th143.exe" },
    { SupportedGame::LoLK_15,     L"th15.exe" },
    { SupportedGame::HSiFS_16,    L"th16.exe" },
    { SupportedGame::WBaWC_17,    L"th17.exe" },
    { SupportedGame::UM_18_Trial, L"th18tr.exe"},
    { SupportedGame::UM_18,       L"th18.exe"},
};

bool findRunningTouhouProcess(PROCESSENTRY32W& processEntry, SupportedGame& processGame) {
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
                for (const auto& e : processNameList) {
                    if (!wcscmp(compare, e.processName.c_str())) {
                        // If found process is from the touhou exe name list, set to true and break from loop
                        processEntry = pe32;
                        processGame = e.game;
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

export unique_ptr<TouhouBase> initializeTouhouGame(bool initLogSilence) {
    PROCESSENTRY32W pe32{};
    SupportedGame game{ SupportedGame::Invalid };

    if (findRunningTouhouProcess(pe32, game)) {
        unique_ptr<TouhouBase> thGame{};

        // Game check
        switch (game) {
            case SupportedGame::EoSD_6:      thGame = make_unique<Touhou06::Touhou06>(pe32);      break;
            case SupportedGame::PCB_7:       thGame = make_unique<Touhou07::Touhou07>(pe32);      break;
            case SupportedGame::IN_8:        thGame = make_unique<Touhou08::Touhou08>(pe32);      break;
            case SupportedGame::PoFV_9:      thGame = make_unique<Touhou09::Touhou09>(pe32);      break;
            case SupportedGame::StB_9_5:     thGame = make_unique<Touhou09_5::Touhou09_5>(pe32);  break;
            case SupportedGame::MoF_10:      thGame = make_unique<Touhou10::Touhou10>(pe32);      break;
            case SupportedGame::SA_11:       thGame = make_unique<Touhou11::Touhou11>(pe32);      break;
            case SupportedGame::UFO_12:      thGame = make_unique<Touhou12::Touhou12>(pe32);      break;
            case SupportedGame::DS_12_5:     thGame = make_unique<Touhou12_5::Touhou12_5>(pe32);  break;
            case SupportedGame::GFW_12_8:    thGame = make_unique<Touhou12_8::Touhou12_8>(pe32);  break;
            case SupportedGame::TD_13:       thGame = make_unique<Touhou13::Touhou13>(pe32);      break;
            case SupportedGame::DDC_14:      thGame = make_unique<Touhou14::Touhou14>(pe32);      break;
            case SupportedGame::ISC_14_3:    thGame = make_unique<Touhou14_3::Touhou14_3>(pe32);  break;
            case SupportedGame::LoLK_15:     thGame = make_unique<Touhou15::Touhou15>(pe32);      break;
            case SupportedGame::HSiFS_16:    thGame = make_unique<Touhou16::Touhou16>(pe32);      break;
            case SupportedGame::WBaWC_17:    thGame = make_unique<Touhou17::Touhou17>(pe32);      break;
            case SupportedGame::UM_18_Trial: thGame = make_unique<Touhou18::Touhou18Trial>(pe32); break;
            case SupportedGame::UM_18:       thGame = make_unique<Touhou18::Touhou18>(pe32);      break;

            case SupportedGame::Invalid:
            {
                Log::error("The game has been detected but isn't properly linked by the program. Exiting now...");
                exit(-1);
            }
            // no default, forces compile error when supported game added to enum but this switch isn't updated.
        }

        Log::info("Supported game found: {}", thGame->getGameName());
        // FIXME/TODO: cannot format pe32 fields because szExeFile is a wchar_t*
        //Log::debug("Supported game process name: \"{}\", PID: {}", wstring( pe32.szExeFile ), static_cast<unsigned long>(pe32.th32ProcessID));

        if (thGame->isLinkedToProcess()) {
            Log::info("TouhouRPC is now linked to the game.");
            return thGame;
        }
        else {
            Log::warning("Failed linking TouhouRPC to the game.");
            return {};
        }
    }
    else {
        if (!initLogSilence)  Log::warning("No supported game currently running. Waiting for a game to be launched...");
        return {};
    }
}
