import "Touhou12.h";

Touhou12::Touhou12(PROCESSENTRY32W const& pe32) : TouhouBase(pe32) {}

Touhou12::~Touhou12() {}

void Touhou12::readDataFromGameProcess() {
    char mainMenuState = -1;
    state.gameState = GameState::Playing;
    state.stageState = StageState::Stage;

    // The BGM playing will be used to determine a lot of things
    std::string bgm_playing = ReadProcessMemoryString(processHandle, BGM_STR_1, 20);

    // Check if the game over music is playing.
    if (bgm_playing == "th10_17.wav") {
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
    }

    // Teammate
    int subCharacter = ReadProcessMemoryInt(processHandle, SUB_CHARACTER);
    switch (subCharacter) {
        default:
        case 0: state.subCharacter = SubCharacter::A; break;
        case 1: state.subCharacter = SubCharacter::B; break;
    }

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

    // Stage
    stage = ReadProcessMemoryInt(processHandle, STAGE);

    // Stage (number of frames in this stage)
    stageFrames = ReadProcessMemoryInt(processHandle, STAGE_FRAMES);

    // Game state
    gameState = ReadProcessMemoryInt(processHandle, GAME_STATE);

    // Game state (number of frames in this current state)
    gameStateFrames = ReadProcessMemoryInt(processHandle, GAME_STATE_FRAMES);

    // Menu state
    TouhouAddress menuPtr = ReadProcessMemoryInt(processHandle, MENU_POINTER); // Get menu class address
    if (state.gameState == GameState::Playing && menuPtr) {
        int inSubMenu = ReadProcessMemoryInt(processHandle, (menuPtr + 0xB4));

        if (inSubMenu != 0) {
            int subMenuSelection = ReadProcessMemoryInt(processHandle, (menuPtr + 0x34));

            switch (subMenuSelection) {
                default:
                case 0: state.mainMenuState = MainMenuState::GameStart; break;
                case 1: state.mainMenuState = MainMenuState::ExtraStart; break;
                case 2: state.mainMenuState = MainMenuState::StagePractice; break;
                case 3: state.mainMenuState = MainMenuState::Replays; break;
                case 4: state.mainMenuState = MainMenuState::PlayerData; break;
                case 5: state.mainMenuState = MainMenuState::MusicRoom; break;
                case 6: state.mainMenuState = MainMenuState::Options; break;
            }
        }
        else {
            state.mainMenuState = MainMenuState::TitleScreen;
        }

        mainMenuState = 0;
        state.gameState = GameState::MainMenu;
    }

    if (state.gameState == GameState::Playing) {
        // Note that ZUN's naming for the BGM file names is not very consistent
        switch (bgm) {
            case 1:
                mainMenuState = 0;
                state.mainMenuState = MainMenuState::TitleScreen;
                state.gameState = GameState::MainMenu;
                break;
            case 20: // ending
                state.gameState = GameState::Ending;
                break;
            case 21: // staff roll
                state.gameState = GameState::StaffRoll;
                break;
            default:
                break;
        }
    }

    TouhouAddress enemyStatePtr = ReadProcessMemoryInt(processHandle, ENEMY_STATE); // Get enemy state address
    if (state.gameState == GameState::Playing && enemyStatePtr) {
        int fightingBoss = ReadProcessMemoryInt(processHandle, (enemyStatePtr + 0x1594));

        if (fightingBoss == 3) {
            if (gameState == 6 || gameState == 7) {
                // These states are always a midboss
                state.stageState = StageState::Midboss;
            }
            else if (gameState == 24 || gameState == 25 || gameState == 44) {
                // These states are always a boss
                state.stageState = StageState::Boss;
            }
            else if (gameState == 43) {
                // Some stages have gameState == 43 for both midboss and boss, so split by stage frames
                switch (stage) {
                    default:
                    case 2: state.stageState = stageFrames < 7300 ? StageState::Midboss : StageState::Boss; break;
                    case 3: state.stageState = stageFrames < 8900 ? StageState::Midboss : StageState::Boss; break;
                    case 5: state.stageState = stageFrames < 9000 ? StageState::Midboss : StageState::Boss; break;
                }
            }
        }
    }

    // Read current game progress
    state.lives = ReadProcessMemoryInt(processHandle, LIVES);
    state.bombs = ReadProcessMemoryInt(processHandle, BOMBS);
    state.score = ReadProcessMemoryInt(processHandle, SCORE);
    state.gameOvers = ReadProcessMemoryInt(processHandle, GAMEOVERS);

    if (state.gameState == GameState::Playing) {
        int practiceFlag = ReadProcessMemoryInt(processHandle, PRACTICE_FLAG);
        int replayFlag = ReadProcessMemoryInt(processHandle, REPLAY_FLAG);

        if (practiceFlag == 16) {
            state.gameState = GameState::StagePractice;
        }
        else if (replayFlag == 2) {
            state.gameState = GameState::WatchingReplay;
        }
    }
}

std::string Touhou12::getMidbossName() const {
    switch (stage) {
        case 1:
        case 5: return "Nazrin";
        case 2: return "Kogasa Tatara";
        case 3: return "Ichirin Kumoi & Unzan";
        case 4:
        case 6: return "Nue Houjuu (Unknown Form)";
        case 7: return "Kogasa Tatara";
        default: return "";
    }
}

std::string Touhou12::getBossName() const {
    switch (stage) {
        case 1: return "Nazrin";
        case 2: return "Kogasa Tatara";
        case 3: return "Ichirin Kumoi & Unzan";
        case 4: return "Minamitsu Murasa";
        case 5: return "Shou Toramaru";
        case 6: return "Byakuren Hijiri";
        case 7: return "Nue Houjuu";
        default: return "";
    }
}

std::string Touhou12::getBGMName() const {
    return th12_musicNames[bgm];
}
