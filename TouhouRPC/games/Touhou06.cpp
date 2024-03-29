#include "Touhou06.h"

Touhou06::Touhou06(PROCESSENTRY32W const& pe32) : TouhouBase(pe32) {
    gameState2 = 0;
}

Touhou06::~Touhou06() {}

bool InvalidmainMenuStateForGame(int mainMenuState) {
    // Only certain menu states are valid for being in-game.
    // They are valid when the game is able to be started from them.
    // This is important because the 'check in menu' marker can be 0 on some inappropriate menus
    // 16 -> music room
    // 1, 2 -> main menu (i.e. on the demo)
    // 10 -> player data
    return mainMenuState == 16 || mainMenuState == 1 || mainMenuState == 2 || mainMenuState == 10;
}

void Touhou06::readDataFromGameProcess() {
    state.gameState = GameState::Playing;
    state.stageState = StageState::Stage;

    // Character
    character = ReadProcessMemoryInt(processHandle, CHARACTER, 1);
    switch (character) {
        default:
        case 0: state.character = Character::Reimu; break;
        case 1: state.character = Character::Marisa; break;
    }

    // Teammate
    int subCharacter = ReadProcessMemoryInt(processHandle, SUB_CHARACTER, 1);
    switch (subCharacter) {
        default:
        case 0: state.subCharacter = SubCharacter::A; break;
        case 1: state.subCharacter = SubCharacter::B; break;
    }

    // Difficulty
    difficulty = ReadProcessMemoryInt(processHandle, DIFFICULTY, 1);
    switch (difficulty) {
        default:
        case 0: state.difficulty = Difficulty::Easy; break;
        case 1: state.difficulty = Difficulty::Normal; break;
        case 2: state.difficulty = Difficulty::Hard; break;
        case 3: state.difficulty = Difficulty::Lunatic; break;
        case 4: state.difficulty = Difficulty::Extra; break;
    }

    // Stage
    stage = ReadProcessMemoryInt(processHandle, STAGE, 1);

    // Game state
    gameState = ReadProcessMemoryInt(processHandle, GAME_STATE, 1);

    // Game state 2 (boss life)
    gameState2 = ReadProcessMemoryInt(processHandle, GAME_STATE_2, 1);

    // Read menu state
    menuState = ReadProcessMemoryInt(processHandle, MENU_STATE);

    int practiceFlag = ReadProcessMemoryInt(processHandle, PRACTICE_FLAG, 1);

    // Are we in the menu?
    int aMenu = ReadProcessMemoryInt(processHandle, CHECK_IN_MENU_VALUE); // == 0 when we are in-game
    if (stage > 0 && aMenu == 0 && !InvalidmainMenuStateForGame(menuState)) {
        // We are in-game
        int replayFlag = ReadProcessMemoryInt(processHandle, REPLAY_FLAG, 1);

        if (practiceFlag == 1) {
            state.gameState = GameState::StagePractice;
        }
        else if (replayFlag == 1) {
            state.gameState = GameState::WatchingReplay;
        }

        // Stage 1: Rumia (mid-boss)
        if (stage == 1 && (gameState == 6 || gameState == 7)) {
            state.stageState = StageState::Midboss;
        }
        // Stage 1: Rumia (boss)
        else if (stage == 1 && (gameState >= 16)) {
            state.stageState = StageState::Boss;
        }

        // Stage 2: Daiyousei
        else if (stage == 2 && gameState == 19) {
            state.stageState = StageState::Midboss;
        }
        // Stage 2: Cirno
        else if (stage == 2 && gameState >= 25) {
            state.stageState = StageState::Boss;
        }

        // Stage 3: Meiling (mid-boss and boss)
        else if (stage == 3 && gameState >= 16) {
            state.stageState = StageState::Boss;
        }

        // Stage 4: Koakuma
        else if (stage == 4 && (gameState == 0 && gameState2 > 0)) {
            state.stageState = StageState::Midboss;
        }
        // Stage 4: Patchouli
        else if (stage == 4 && gameState > 0) {
            state.stageState = StageState::Boss;
        }

        // Stage 5: Sakuya (mid-boss and boss)
        else if (stage == 5 && gameState >= 17) {
            state.stageState = StageState::Boss;
        }

        // Stage 6: Sakuya (mid-boss)
        else if (stage == 6 && gameState == 13) {
            state.stageState = StageState::Midboss;
        }
        // Stage 6: Remilia (boss)
        else if (stage == 6 && gameState >= 19) {
            state.stageState = StageState::Boss;
        }

        // Stage 7 Patchouli (mid-boss):
        else if (stage == 7 && (gameState >= 18 && gameState <= 20)) {
            state.stageState = StageState::Midboss;
        }
        // Stage 7 Flandre (boss):
        else if (stage == 7 && gameState != 0) {
            state.stageState = StageState::Boss;
        }
    }
    else {
        // We're in the menu
        state.gameState = GameState::MainMenu;

        switch (menuState) {
            default:
            case 0:
            case 1:
            case 2:
            case 8:
            case 14: state.mainMenuState = MainMenuState::TitleScreen; break;

            case 6:
            case 7:
            case 9:
            case 11: state.mainMenuState = (practiceFlag == 1) ? MainMenuState::StagePractice : MainMenuState::GameStart; break;

            case 17: state.mainMenuState = MainMenuState::StagePractice; break;

            case 12:
            case 13:
            case 15: state.mainMenuState = MainMenuState::Replays; break;

            case 10: state.mainMenuState = MainMenuState::PlayerData; break;

            case 16:
            {
                state.mainMenuState = MainMenuState::MusicRoom;

                bgm = ReadProcessMemoryInt(processHandle, MUSIC_ROOM_TRACK);
                break;
            }

            case 3: state.mainMenuState = MainMenuState::Options; break;
        }
    }

    // Read current game progress
    int lives = ReadProcessMemoryInt(processHandle, LIVES, 1);
    state.lives = lives;

    int bombs = ReadProcessMemoryInt(processHandle, BOMBS, 1);
    state.bombs = bombs;

    int gameOvers = ReadProcessMemoryInt(processHandle, GAMEOVERS, 1);
    state.gameOvers = gameOvers;

    int score = ReadProcessMemoryInt(processHandle, SCORE);
    state.score = (score - gameOvers) / 10;
}

std::string Touhou06::getMidbossName() const {
    switch (stage) {
        case 1: return "Rumia";
        case 2: return "Daiyousei";
        case 3: return "Hong Meiling";
        case 4: return "Koakuma";
        case 5:
        case 6: return "Sakuya Izayoi";
        case 7: return "Patchouli Knowledge";
        default: return "";
    }
}

std::string Touhou06::getBossName() const {
    switch (stage) {
        case 1: return "Rumia";
        case 2: return "Cirno";
        case 3: return "Hong Meiling";
        case 4: return "Patchouli Knowledge";
        case 5: return "Sakuya Izayoi";
        case 6: return "Remilia Scarlet";
        case 7: return "Flandre Scarlet";
        default: return "";
    }
}

std::string Touhou06::getBGMName() const {
    return th06_musicNames[bgm];
}
