#include <Windows.h>
#include "Touhou12_8.h"

Touhou12_8::Touhou12_8(PROCESSENTRY32W const& pe32) : TouhouBase(pe32) {}

Touhou12_8::~Touhou12_8() {}

void Touhou12_8::readDataFromGameProcess() {
    char mainMenuState = -1;
    state.gameState = GameState::Playing;
    state.stageState = StageState::Stage;

    // The BGM playing will be used to determine some things
    char bgm_playing[20];
    ReadProcessMemory(processHandle, (LPCVOID) BGM_STR_1, bgm_playing, 20, NULL);

    // Convert the part after the _ and before the . to int
    // That way it is possible to switch case the BGM playing
    bool prefixBGM = bgm_playing[0] == 'b';
    char bgm_id_str[3];
    bgm_id_str[0] = bgm_playing[prefixBGM ? 10 : 6];
    bgm_id_str[1] = bgm_playing[prefixBGM ? 11 : 7];
    bgm_id_str[2] = '\x00';
    int bgm_id = atoi(bgm_id_str);

    bgm = bgm_id;

    // Character
    state.character = Character::Cirno;

    // Difficulty
    ReadProcessMemory(processHandle, (LPCVOID) DIFFICULTY, (LPVOID) &difficulty, 4, NULL);
    switch (difficulty) {
        default:
        case 0: state.difficulty = Difficulty::Easy; break;
        case 1: state.difficulty = Difficulty::Normal; break;
        case 2: state.difficulty = Difficulty::Hard; break;
        case 3: state.difficulty = Difficulty::Lunatic; break;
        case 4: state.difficulty = Difficulty::Extra; break;
    }

    // Stage
    ReadProcessMemory(processHandle, (LPCVOID) STAGE, (LPVOID) &stage, 4, NULL);

    // Stage (number of frames in this stage)
    ReadProcessMemory(processHandle, (LPCVOID) STAGE_FRAMES, (LPVOID) &stageFrames, 4, NULL);

    // Game state
    ReadProcessMemory(processHandle, (LPCVOID) GAME_STATE, (LPVOID) &gameState, 4, NULL);

    // Game state (number of frames in this current state)
    ReadProcessMemory(processHandle, (LPCVOID) GAME_STATE_FRAMES, (LPVOID) &gameStateFrames, 4, NULL);

    // Menu state
    DWORD menuPtr = 0;
    ReadProcessMemory(processHandle, (LPCVOID) MENU_POINTER, &menuPtr, 4, NULL); // Get menu class address
    if (state.gameState == GameState::Playing && menuPtr) {
        unsigned int displayState = 0;
        ReadProcessMemory(processHandle, (LPCVOID) (menuPtr + 0x1C), (LPVOID) &displayState, 4, NULL);

        switch (displayState) {
            default:
            case 1: state.mainMenuState = MainMenuState::TitleScreen; break;
            case 5: state.mainMenuState = MainMenuState::GameStart; break;
            case 11: state.mainMenuState = MainMenuState::Replays; break;
            case 10: state.mainMenuState = MainMenuState::PlayerData; break;
            case 13: state.mainMenuState = MainMenuState::MusicRoom; break;
            case 3: state.mainMenuState = MainMenuState::Options; break;
            case 16: state.mainMenuState = MainMenuState::Manual; break;
        }

        mainMenuState = 0;
        state.gameState = GameState::MainMenu;
    }

    if (state.gameState == GameState::Playing) {
        // Note that ZUN's naming for the BGM file names is not very consistent
        switch (bgm_id) {
            case 7:
                mainMenuState = 0;
                state.mainMenuState = MainMenuState::TitleScreen;
                state.gameState = GameState::MainMenu;
                break;
            case 8:
                state.gameState = GameState::GameOver;
                break;
            case 9: // ending
                state.gameState = GameState::Ending;
                break;
            default:
                break;
        }
    }

    if (state.gameState == GameState::Playing) {
        switch (stage) {
            case A1_1:
            case B1_1:
            case C1_1:
            {
                if (stageFrames > gameStateFrames) {
                    state.stageState = StageState::Boss;
                }
                else if (stageFrames > 3200 && stageFrames < 5200) {
                    state.stageState = StageState::Midboss;
                }
                break;
            }

            case A1_2:
            case A2_2:
            case B1_2:
            case B2_2:
            case C1_2:
            case C2_2:
            {
                if (stageFrames > gameStateFrames) {
                    state.stageState = StageState::Boss;
                }
                else if (stageFrames > 2900 && stageFrames < 5000) {
                    state.stageState = StageState::Midboss;
                }
                break;
            }

            case A1_3:
            case A2_3:
            case B1_3:
            case B2_3:
            case C1_3:
            case C2_3:
            {
                if (stageFrames > 3800 && stageFrames < 7000) {
                    state.stageState = StageState::Midboss;
                }
                else if (stageFrames > 10500) {
                    state.stageState = StageState::Boss;
                }
                break;
            }

            case EX:
            {
                if (stageFrames > gameStateFrames) {
                    state.stageState = StageState::Boss;
                }
                else if (stageFrames > 6200 && stageFrames < 9500) {
                    state.stageState = StageState::Midboss;
                }
                break;
            }
        }
    }

    // Read current game progress
    ReadProcessMemory(processHandle, (LPCVOID) MOTIVATION, (LPVOID) &motivation, 4, NULL);
    state.lives = motivation / (100 * 100); // to get auto update from death, trigger every 100% in difference.
    ReadProcessMemory(processHandle, (LPCVOID) PERFECT_FREEZE, (LPVOID) &perfectFreeze, 4, NULL);
    ReadProcessMemory(processHandle, (LPCVOID) SCORE, (LPVOID) &state.score, 4, NULL);
    ReadProcessMemory(processHandle, (LPCVOID) GAMEOVERS, (LPVOID) &state.gameOvers, 4, NULL);

    if (state.gameState == GameState::Playing) {
        unsigned int replayFlag = 0;
        ReadProcessMemory(processHandle, (LPCVOID) REPLAY_FLAG, (LPVOID) &replayFlag, 4, NULL);

        if (replayFlag == 2) {
            state.gameState = GameState::WatchingReplay;
        }
        else {
            state.gameState = GameState::Playing_CustomResources;
        }
    }
}

std::string Touhou12_8::getStageName() const {
    switch (stage) {
        case A1_1: return "Stage A1-1";
        case A1_2: return "Stage A1-2";
        case A1_3: return "Stage A1-3";
        case A2_2: return "Stage A2-2";
        case A2_3: return "Stage A2-3";

        case B1_1: return "Stage B1-1";
        case B1_2: return "Stage B1-2";
        case B1_3: return "Stage B1-3";
        case B2_2: return "Stage B2-2";
        case B2_3: return "Stage B2-3";

        case C1_1: return "Stage C1-1";
        case C1_2: return "Stage C1-2";
        case C1_3: return "Stage C1-3";
        case C2_2: return "Stage C2-2";
        case C2_3: return "Stage C2-3";

        default:
        case EX: return "Extra Stage";
    }
}

std::string Touhou12_8::getMidbossName() const {
    switch (stage) {
        case A1_1:
        case B1_2:
        case C1_2: return "Lily White";

        case A2_2:
        case B2_2:
        case C1_1: return "Lily Black";

        case A1_2:
        case B1_1:
        case C2_2: return "Daiyousei";

        case A2_3:
        case B2_3: return "Star Sapphire";

        case B1_3:
        case C1_3: return "Luna Child";

        case A1_3:
        case C2_3: return "Sunny Milk";

        default:
        case EX: return "Lily White and Daiyousei";
    }
}

std::string Touhou12_8::getBossName() const {
    switch (stage) {
        case A1_2:
        case B1_2:
        case C1_1: return "Star Sapphire";

        case A1_1:
        case B2_2:
        case C2_2: return "Luna Child";

        case A2_2:
        case B1_1:
        case C1_2: return "Sunny Milk";

        case A1_3:
        case A2_3:
        case B1_3:
        case B2_3:
        case C1_3:
        case C2_3: return "Three Faires of Light";

        default:
        case EX: return "Marisa Kirisame";
    }
}

std::string Touhou12_8::getBGMName() const {
    return th12_8_musicNames[bgm];
}

std::string Touhou12_8::getCustomResources() const {
    std::string resources = std::to_string(motivation / 100);
    resources.append("%/");
    resources.append(std::to_string(perfectFreeze / 100));
    resources.append("%");

    return resources;
}
