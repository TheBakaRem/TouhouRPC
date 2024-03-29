#include "Touhou15.h"

Touhou15::Touhou15(PROCESSENTRY32W const& pe32) : TouhouBase(pe32) {}

Touhou15::~Touhou15() {}

void Touhou15::readDataFromGameProcess() {
    char mainMenuState = -1;
    state.gameState = GameState::Playing;
    state.stageState = StageState::Stage;
    gameMode = GAME_MODE_STANDARD;

    // The BGM playing will be used to determine a lot of things
    std::string bgm_playing = ReadProcessMemoryString(processHandle, BGM_STR, 20);

    // Check if the game over music is playing.
    if (bgm_playing == "th128_08.wav") {
        state.gameState = GameState::GameOver;
    }

    // Convert the part after the _ and before the . to int
    // That way it is possible to switch case the BGM playing
    bool prefixBGM = bgm_playing[0] == 'b';
    char bgm_id_str[3]{ bgm_playing[prefixBGM ? 9 : 5], bgm_playing[prefixBGM ? 10 : 6], '\0' };
    bgm = atoi(bgm_id_str);

    // Character
    character = ReadProcessMemoryInt(processHandle, CHARACTER);
    switch (character) {
        default:
        case 0: state.character = Character::Reimu; break;
        case 1: state.character = Character::Marisa; break;
        case 2: state.character = Character::Sanae; break;
        case 3: state.character = Character::Reisen; break;
    }

    // Stage
    stage = ReadProcessMemoryInt(processHandle, STAGE);

    // Game type
    gameType = static_cast<GameType>(ReadProcessMemoryInt(processHandle, GAME_TYPE));

    // Difficulty
    difficulty = ReadProcessMemoryInt(processHandle, DIFFICULTY);
    switch (difficulty) {
        default:
        case 0: state.difficulty = Difficulty::Easy; break;
        case 1: state.difficulty = Difficulty::Normal; break;
        case 2: state.difficulty = Difficulty::Hard; break;
        case 3: state.difficulty = Difficulty::Lunatic; break;
        case 4: state.difficulty = Difficulty::Extra; break;
    }

    // Stage chapter
    stageChapter = ReadProcessMemoryInt(processHandle, STAGE_CHAPTER);

    // Stage chapter frames (number of frames in this current state)
    currentStageChapterFrames = ReadProcessMemoryInt(processHandle, STAGE_CHAPTER_FRAMES);

    // Global retry count
    retryCount = ReadProcessMemoryInt(processHandle, GLOBAL_RETRY_COUNT);

    // Chapter retry count
    chapterRetryCount = ReadProcessMemoryInt(processHandle, CHAPTER_RETRY_COUNT);

    // Menu state
    TouhouAddress menu_pointer = ReadProcessMemoryInt(processHandle, MENU_POINTER); // Get menu class address
    if (state.gameState == GameState::Playing && menu_pointer) {
        // The most reliable way of determining our current menu state is through the combination of
        // menu display state and extra flags that get set.
        // This is because of a bug detailed in Touhou14's source file

        /*
            display state (0x18) -> menu screen
            -----------------------------
             0 -> loading
             1 -> main menu
             5 -> game start
             6 -> extra start
             6 -> practice start
            12 -> replay
            11 -> player data
            14 -> music room
             3 -> options
            17 -> all manual screens

            ---- sub sub menus ----
             6 -> difficulty select
             7 -> char select
             9 -> practice stage select
            11 -> replay stage select
        */

        int ds = ReadProcessMemoryInt(processHandle, (menu_pointer + 0x18));

        switch (ds) {
            default: state.mainMenuState = MainMenuState::TitleScreen; break;
            case 5:
            case 6:
            case 7:
            {
                // could be normal game, extra, or stage practice, we can check some extra stuff in order to find out.
                if (difficulty == 4) {
                    state.mainMenuState = MainMenuState::ExtraStart;
                }
                else {
                    int practiceFlag = ReadProcessMemoryInt(processHandle, PRACTICE_SELECT_FLAG);
                    state.mainMenuState = (practiceFlag != 0 && practiceFlag != 256) ? MainMenuState::StagePractice : MainMenuState::GameStart;
                }
                break;
            }
            case 9: state.mainMenuState = MainMenuState::StagePractice; break;
            case 12: state.mainMenuState = MainMenuState::Replays; break;
            case 11: state.mainMenuState = MainMenuState::PlayerData; break;
            case 14: state.mainMenuState = MainMenuState::MusicRoom; break;
            case 3: state.mainMenuState = MainMenuState::Options; break;
            case 17: state.mainMenuState = MainMenuState::Manual; break;
        }

        mainMenuState = 0;
        state.gameState = GameState::MainMenu;
    }

    if (state.gameState == GameState::Playing) {
        // Note that ZUN's naming for the BGM file names is not very consistent
        switch (bgm) {
            case 0:
            case 1:
                mainMenuState = 0;
                state.mainMenuState = MainMenuState::TitleScreen;
                state.gameState = GameState::MainMenu;
                break;
            case 14: // ending
                state.gameState = GameState::Ending;
                break;
            case 15: // staff roll
                state.gameState = GameState::StaffRoll;
                break;
            default:
                break;
        }
    }

    // Boss name display

    // Stage 1: Seiran (mid-boss 1)
    if (stage == 1 && (stageChapter == 2 && currentStageChapterFrames <= 1600)) {
        state.stageState = StageState::Midboss;
    }
    // Stage 1: Seiran (mid-boss 2 and boss)
    else if (stage == 1 && (stageChapter == 4 || stageChapter >= 31) && stageChapter != 81) {
        state.stageState = StageState::Boss;
    }

    // Stage 2: Ringo
    else if (stage == 2 && (stageChapter == 3 || stageChapter >= 41) && stageChapter != 81) {
        state.stageState = StageState::Boss;
    }

    // Stage 3: Doremy
    else if (stage == 3 && (stageChapter == 3 || stageChapter >= 31) && stageChapter != 81) {
        state.stageState = StageState::Boss;
    }

    // Stage 4: Sagume
    else if (stage == 4 && (stageChapter == 3 || stageChapter >= 41) && stageChapter != 81) {
        state.stageState = StageState::Boss;
    }

    // Stage 5: Clownpiece
    else if (stage == 5 && stageChapter >= 41 && stageChapter != 81) {
        state.stageState = StageState::Boss;
    }

    // Stage 6: Junko
    else if (stage == 6 && stageChapter >= 41 && stageChapter != 81) {
        state.stageState = StageState::Boss;
    }

    // Stage 7 (Extra): Doremy
    else if (stage == 7 && (stageChapter == 5 || (stageChapter >= 23 && stageChapter <= 33))) {
        state.stageState = StageState::Midboss;
    }
    // Stage 7 (Extra): Junko + Hecatia (dialogue, final spell)
    else if (stage == 7 && (stageChapter == 41 || stageChapter == 71 || stageChapter == 81)) {
        state.stageState = StageState::Boss;
        extraBossState = ExtraBossState::JUNKO_AND_HECATIA;
    }
    // Stage 7 (Extra): Junko only
    else if (stage == 7 && (stageChapter == 46 || stageChapter == 65 || stageChapter == 50 || stageChapter == 69)) {
        state.stageState = StageState::Boss;
        extraBossState = ExtraBossState::JUNKO;
    }
    // Stage 7 (Extra): Hecatia only
    else if (stage == 7 && (stageChapter >= 43 && stageChapter <= 70)) {
        state.stageState = StageState::Boss;
        extraBossState = ExtraBossState::HECATIA;
    }

    // Read current game progress
    state.lives = ReadProcessMemoryInt(processHandle, LIVES);
    state.bombs = ReadProcessMemoryInt(processHandle, BOMBS);
    state.score = ReadProcessMemoryInt(processHandle, SCORE);
    state.gameOvers = ReadProcessMemoryInt(processHandle, GAMEOVERS);

    // Read game mode
    gameMode = static_cast<GameMode>(ReadProcessMemoryInt(processHandle, GAME_MODE));
    switch (gameMode) {
        case GAME_MODE_STANDARD: break; // could be main menu or playing, no need to overwrite anything
        case GAME_MODE_REPLAY: state.gameState = GameState::WatchingReplay; break;
        case GAME_MODE_CLEAR: state.gameState = GameState::StaffRoll; break;
        case GAME_MODE_PRACTICE: state.gameState = GameState::StagePractice; break;
        case GAME_MODE_SPELLPRACTICE: state.gameState = GameState::SpellPractice; break;
    }

    if ((state.gameState == GameState::Playing && stage == 7) || state.gameState != GameState::Playing) {
        gameType = LEGACY;
    }

    if (state.gameState == GameState::Playing && gameType == POINT_DEVICE) {
        state.gameState = GameState::Playing_CustomResources; // display retries instead of lives
    }
}

std::string Touhou15::getMidbossName() const {
    switch (stage) {
        case 1: return "Seiran";
        case 2: return "Ringo";
        case 3: return "Doremy Sweet";
        case 4: return "Sagume Kishin";
        case 5: return "Clownpiece";
        case 6: return "Junko";
        case 7: return "Doremy Sweet";
        default: return "";
    }
}

std::string Touhou15::getBossName() const {
    switch (stage) {
        case 1: return "Seiran";
        case 2: return "Ringo";
        case 3: return "Doremy Sweet";
        case 4: return "Sagume Kishin";
        case 5: return "Clownpiece";
        case 6: return "Junko";
        case 7:
        {
            switch (extraBossState) {
                case JUNKO_AND_HECATIA: return "Junko & Hecatia Lapislazuli";
                case JUNKO: return "Junko";
                default:
                case HECATIA: return "Hecatia Lapislazuli";
            }
        }
        default: return "";
    }
}

std::string Touhou15::getBGMName() const {
    return th15_musicNames[bgm];
}

std::string Touhou15::getCustomResources() const {
    std::string resources = std::to_string(state.bombs);
    resources.append(" bombs, ");
    resources.append(std::to_string(chapterRetryCount));
    resources.append("/");
    resources.append(std::to_string(retryCount));
    resources.append(" retries");

    return resources;
}

void Touhou15::setSmallImageInfo(std::string& icon, std::string& text) const {
    TouhouBase::setSmallImageInfo(icon, text);

    if (shouldShowCoverIcon()) return; // So that the rest isn't executed while in the menus

    switch (gameType) {
        case POINT_DEVICE:
        {
            text.append(" Pointdevice (");
            text.append(std::to_string(chapterRetryCount));
            text.append("/");
            text.append(std::to_string(retryCount));
            text.append(" retries)");
            break;
        }

        default:
        case LEGACY:
        {
            text.append(" Legacy");
            break;
        }
    }
}

bool Touhou15::stateHasChangedSinceLastCheck() {
    bool const changed = TouhouBase::stateHasChangedSinceLastCheck() || (prevExtraBossState != extraBossState);
    prevExtraBossState = extraBossState;
    return changed;
}
