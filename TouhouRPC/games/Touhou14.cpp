#include "Touhou14.h"

Touhou14::Touhou14(PROCESSENTRY32W const& pe32) : TouhouBase(pe32) {}

Touhou14::~Touhou14() {}

void Touhou14::readDataFromGameProcess() {
    // Reset mainMenuState, bgm will tell us if we're in the menu
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

    difficulty = ReadProcessMemoryInt(processHandle, DIFFICULTY);
    switch (difficulty) {
        default:
        case 0: state.difficulty = Difficulty::Easy; break;
        case 1: state.difficulty = Difficulty::Normal; break;
        case 2: state.difficulty = Difficulty::Hard; break;
        case 3: state.difficulty = Difficulty::Lunatic; break;
        case 4: state.difficulty = Difficulty::Extra; break;
    }

    // Read stage value
    stage = ReadProcessMemoryInt(processHandle, STAGE);

    // Update menu state. The object at menu_pointer contains many things to do with the main menu,
    // and they kind of behave awkwardly in DDC. For now we mainly care about whether we're in the music room or not.

    TouhouAddress menu_pointer = ReadProcessMemoryInt(processHandle, MENU_POINTER);
    if (state.gameState == GameState::Playing && menu_pointer) {
        // The most reliable way of determining our current menu state is through the combination of
        // menu display state, option count, and extra flags that get set.
        // This is because of a bug detailed in the mini-documentation below the code

        /*
            display state (0x1C) / option count (0x30) -> menu screen
            -----------------------------
             0 /  0 -> loading
             1 / 10 -> main menu
             5 /  4 -> game start
             5 /  1 -> extra start (has 0x004F5834 set to 4, garbage otherwise?)
             5 /  4 -> practice start (has 0x004F58B8 set to 16, 0 otherwise)
            17 /  7 -> spell card practice (has 0x004F58B8 set to 32, 0 otherwise)
            11 / 25 -> replay
            10 /  6 -> all player data screens
            13 / 17 -> music room
             3 /  5 -> options
            16 / 10 -> all manual screens

            ---- sub sub menus ----
             6 /  3 -> char select
             7 /  2 -> subchar select
             8 /  6 -> practice stage select
            18 /  N -> spell card select, N == num spells for stage
            19 /  5 -> spell card difficulty select
            11 /  7 -> replay stage select
        */

        int ds = ReadProcessMemoryInt(processHandle, (menu_pointer + 0x1C));

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
                    state.mainMenuState = (practiceFlag != 0) ? MainMenuState::StagePractice : MainMenuState::GameStart;
                }
                break;
            }
            case 8: state.mainMenuState = MainMenuState::StagePractice; break;
            case 17:
            case 18:
            case 19: state.mainMenuState = MainMenuState::SpellPractice; break;
            case 11: state.mainMenuState = MainMenuState::Replays; break;
            case 10: state.mainMenuState = MainMenuState::PlayerData; break;
            case 13: state.mainMenuState = MainMenuState::MusicRoom; break;
            case 3: state.mainMenuState = MainMenuState::Options; break;
            case 16: state.mainMenuState = MainMenuState::Manual; break;
        }

        mainMenuState = 0;
        state.gameState = GameState::MainMenu;

        ///////////////
        // Unused menu stuff, documented here in case it's ever necessary.

        // These track where the cursor is, changed in 2 places for some reason.
        // ReadProcessMemory(processHandle, (LPCVOID)(menu_pointer + 0x28), (LPVOID)&mainMenuSelectionA, 4, NULL);
        // ReadProcessMemory(processHandle, (LPCVOID)(menu_pointer + 0x2C), (LPVOID)&mainMenuSelectionB, 4, NULL);

        // The menu object stores a stack of the selected sub menus. The bottom of the stack is meant to be at offset 0x34 inside the menu object.
        // i.e. any sub menu selected from the top level of the title screen will fill in to this base number.
        // The size of the stack is stored at offset 0xB4, and this is also used to determine where to insert next.
        // This should've been much more useful than fiddling with display states and option counts and flags etc.
        // but there is a bug when exiting the Manual sub menu where the stack size doesn't decrease,
        // making reading from the stack annoying as its base will keep on shifting 4 bytes forward every time the player exits the Manual.

        ///////////////
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
            case 16: // ending
                state.gameState = GameState::Ending;
                break;
            case 17: // staff roll
                state.gameState = GameState::StaffRoll;
                break;
            default:
                break;
        }
    }

    // We know the main stage boss is triggered when music changes
    // So if the state isn't already defined, we check if the mid-boss is present
    if (state.stageState == StageState::Stage) {
        int boss_mode = ReadProcessMemoryInt(processHandle, ENEMY_STATE);
        // If it's 0 there is no main boss, if it's 4 it's a mid-boss, 6 is boss, 7 is post-boss
        if (boss_mode == 4) {
            state.stageState = StageState::Midboss;
        }
        else if (boss_mode == 6) {
            state.stageState = StageState::Boss;
        }
    }

    // Read Spell Card ID (for Spell Practice)
    spellCardID = ReadProcessMemoryInt(processHandle, SPELL_CARD_ID);

    // Read character and difficulty info
    character = ReadProcessMemoryInt(processHandle, CHARACTER);
    switch (character) {
        default:
        case 0: state.character = Character::Reimu; break;
        case 1: state.character = Character::Marisa; break;
        case 2: state.character = Character::Sakuya; break;
    }

    int subCharacter = ReadProcessMemoryInt(processHandle, SUB_CHARACTER);
    switch (subCharacter) {
        default:
        case 0: state.subCharacter = SubCharacter::A_DistinctIcons; break;
        case 1: state.subCharacter = SubCharacter::B_DistinctIcons; break;
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
}

std::string Touhou14::getMidbossName() const {
    switch (stage) {
        case 1: return "Cirno";
        case 2: return "Sekibanki";
        case 3: return "Kagerou Imaizumi";
        case 4: return (state.subCharacter == SubCharacter::A_DistinctIcons) ? "Yatsuhashi Tsukumo" : "Benben Tsukumo";
        case 5:
        case 6: return "Seija Kijin";
        case 7: return "Benben & Yatsuhashi Tsukumo";
        default: return "";
    }
}

std::string Touhou14::getBossName() const {
    switch (stage) {
        case 1: return "Wakasagihime";
        case 2: return "Sekibanki";
        case 3: return "Kagerou Imaizumi";
        case 4: return (state.subCharacter == SubCharacter::A_DistinctIcons) ? "Benben Tsukumo" : "Yatsuhashi Tsukumo";
        case 5: return "Seija Kijin";
        case 6: return "Shinmyoumaru Sukuna";
        case 7: return "Raiko Horikawa";
        default: return "";
    }
}

std::string Touhou14::getSpellCardName() const {
    return th14_spellCardName[spellCardID];
}

std::string Touhou14::getBGMName() const {
    return th14_musicNames[bgm];
}
