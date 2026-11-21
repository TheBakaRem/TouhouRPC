#include "GameDetector.hpp"

#include <cstdint>
#include <cstdlib>
#include <handleapi.h>
#include <memory>
#include <memoryapi.h>
#include <string>
#include <TlHelp32.h>
#include <vector>
#include <Windows.h>

#include "Log.hpp"
#include "Touhou06.hpp"
#include "Touhou07.hpp"
#include "Touhou08.hpp"
#include "Touhou09.hpp"
#include "Touhou09_5.hpp"
#include "Touhou10.hpp"
#include "Touhou11.hpp"
#include "Touhou12.hpp"
#include "Touhou12_5.hpp"
#include "Touhou12_8.hpp"
#include "Touhou13.hpp"
#include "Touhou14.hpp"
#include "Touhou14_3.hpp"
#include "Touhou15.hpp"
#include "Touhou16.hpp"
#include "Touhou17.hpp"
#include "Touhou18.hpp"
#include "TouhouBase.hpp"

namespace {
    enum class SupportedGame : std::uint8_t {
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
        UM_18,
    
        Invalid,
    };
    
    struct ProcessNameGamePair {
        SupportedGame game;
        std::wstring processName;
    };

    // Executables name list and associated game
    const std::vector<ProcessNameGamePair> processNameList
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
        { SupportedGame::UM_18,       L"th18.exe"},
    };
    
    bool findRunningTouhouProcess(PROCESSENTRY32W& processEntry, SupportedGame& processGame) {
        std::wstring compare{};
        bool procRunning{ false };
    
        PROCESSENTRY32W pe32;
        HANDLE hProcessSnap{ CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0) };
    
        if (hProcessSnap != INVALID_HANDLE_VALUE) {
            pe32.dwSize = sizeof(PROCESSENTRY32W);
            if (Process32FirstW(hProcessSnap, &pe32) != 0) {
                // Check every process
                do {
                    compare = pe32.szExeFile;
                    for (const auto& e : processNameList) {
                        if (compare == e.processName) {
                            // If found process is from the touhou exe name list, set to true and break from loop
                            processEntry = pe32;
                            processGame = e.game;
                            procRunning = true;
                            break;
                        }
                    }
                } while ((Process32NextW(hProcessSnap, &pe32) != 0) && !procRunning);
                // clean the snapshot object
                CloseHandle(hProcessSnap);
            }
        }
    
        return procRunning;
    }
} // namespace


std::unique_ptr<TouhouBase> initializeTouhouGame(bool initLogSilence) {
    PROCESSENTRY32W pe32;
    SupportedGame game{ SupportedGame::Invalid };

    if (findRunningTouhouProcess(pe32, game)) {
        std::unique_ptr<TouhouBase> thGame;

        // Game check
        switch (game) {
            case SupportedGame::EoSD_6:      thGame = std::make_unique<Touhou06>(pe32);      break;
            case SupportedGame::PCB_7:       thGame = std::make_unique<Touhou07>(pe32);      break;
            case SupportedGame::IN_8:        thGame = std::make_unique<Touhou08>(pe32);      break;
            case SupportedGame::PoFV_9:      thGame = std::make_unique<Touhou09>(pe32);      break;
            case SupportedGame::StB_9_5:     thGame = std::make_unique<Touhou09_5>(pe32);    break;
            case SupportedGame::MoF_10:      thGame = std::make_unique<Touhou10>(pe32);      break;
            case SupportedGame::SA_11:       thGame = std::make_unique<Touhou11>(pe32);      break;
            case SupportedGame::UFO_12:      thGame = std::make_unique<Touhou12>(pe32);      break;
            case SupportedGame::DS_12_5:     thGame = std::make_unique<Touhou12_5>(pe32);    break;
            case SupportedGame::GFW_12_8:    thGame = std::make_unique<Touhou12_8>(pe32);    break;
            case SupportedGame::TD_13:       thGame = std::make_unique<Touhou13>(pe32);      break;
            case SupportedGame::DDC_14:      thGame = std::make_unique<Touhou14>(pe32);      break;
            case SupportedGame::ISC_14_3:    thGame = std::make_unique<Touhou14_3>(pe32);    break;
            case SupportedGame::LoLK_15:     thGame = std::make_unique<Touhou15>(pe32);      break;
            case SupportedGame::HSiFS_16:    thGame = std::make_unique<Touhou16>(pe32);      break;
            case SupportedGame::WBaWC_17:    thGame = std::make_unique<Touhou17>(pe32);      break;
            case SupportedGame::UM_18:       thGame = std::make_unique<Touhou18>(pe32);      break;

            case SupportedGame::Invalid: {
                Log::error("The game has been detected but isn't properly linked by the program. Exiting now...");
                std::exit(-1);
            }
            // no default, forces compile error when supported game added to enum but this switch isn't updated.
        }

        // FIXME: use a temporary lvalue so the name can be properly consumed by the log system (vformat / make_format_args).
        const std::string tmp{ thGame->getGameName() };
        Log::info("Supported game found: {}", tmp);
        // FIXME/TODO: cannot format pe32 fields because szExeFile is a wchar_t*
        //Log::debug("Supported game process name: \"{}\", PID: {}", wstring( pe32.szExeFile ), static_cast<unsigned long>(pe32.th32ProcessID));

        if (thGame->isLinkedToProcess()) {
            Log::info("TouhouRPC is now linked to the game.");
            return thGame;
        }

        // Couldn't link to the game program.
        Log::warning("Failed linking TouhouRPC to the game.");
        return {};
        
    }
    
    // Couldn't find a valid running process
    if (!initLogSilence) { Log::warning("No supported game currently running. Waiting for a game to be launched..."); }
    return {};
    
}
