import "Touhou10.h";

Touhou10::Touhou10(PROCESSENTRY32W const& pe32) : TouhouBase(pe32) {}

Touhou10::~Touhou10() {}

void Touhou10::readDataFromGameProcess() {
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
    }

    // Teammate
    int subCharacter = ReadProcessMemoryInt(processHandle, SUB_CHARACTER);
    switch (subCharacter) {
        default:
        case 0: state.subCharacter = SubCharacter::A; break;
        case 1: state.subCharacter = SubCharacter::B; break;
        case 2: state.subCharacter = SubCharacter::C; break;
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
        int inSubMenu = ReadProcessMemoryInt(processHandle, (menuPtr + 0xB0));

        if (inSubMenu != 0) {
            int subMenuSelection = ReadProcessMemoryInt(processHandle, (menuPtr + 0x30));

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
            case 2:
                mainMenuState = 0;
                state.mainMenuState = MainMenuState::TitleScreen;
                state.gameState = GameState::MainMenu;
                break;
            case 13: // ending
                state.gameState = GameState::Ending;
                break;
            case 14: // staff roll
                state.gameState = GameState::StaffRoll;
                break;
            default:
                break;
        }
    }

    if (state.gameState == GameState::Playing) {
        // All non-(mid)boss states are below 6, e.g. 1 or 3 for conversations, 2 for post-boss
        if (gameState >= 24) // 24-31, 43-52 are boss only
        {
            state.stageState = StageState::Boss;
        }
        else if (gameState >= 6) // 6 and 15 are midboss only
        {
            // On all stages except 2 and 4, the gameState reverts to 0 after mid-boss disappears
            if (stage == 2 || stage == 4) {
                if (gameStateFrames < 900) // same for both
                {
                    state.stageState = StageState::Midboss;
                }
            }
            else {
                state.stageState = StageState::Midboss;
            }
        }
    }

    // Read current game progress
    state.lives = ReadProcessMemoryInt(processHandle, LIVES);

    int integerPower = ReadProcessMemoryInt(processHandle, POWER);
    power = static_cast<float>(integerPower) / 20.0f;

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
        else {
            state.gameState = GameState::Playing_CustomResources; // display power instead of bombs
        }
    }
}

std::string Touhou10::getMidbossName() const {
    switch (stage) {
        case 1: return "Shizuha Aki";
        case 2: return "Hina Kagiyama";
        case 3: return "Nitori Kawashiro";
        case 4: return "Momiji Inubashiri";
        case 5: return "Sanae Kochiya";
            // case 6: none
        case 7: return "Kanako Yasaka";
        default: return "";
    }
}

std::string Touhou10::getBossName() const {
    switch (stage) {
        case 1: return "Minoriko Aki";
        case 2: return "Hina Kagiyama";
        case 3: return "Nitori Kawashiro";
        case 4: return "Aya Shameimaru";
        case 5: return "Sanae Kochiya";
        case 6: return "Kanako Yasaka";
        case 7: return "Suwako Moriya";
        default: return "";
    }
}

std::string Touhou10::getBGMName() const {
    return th10_musicNames[bgm];
}

std::string Touhou10::getCustomResources() const {
    std::string resources = std::to_string(state.lives);
    resources.append("/");
    std::string powerStr = std::to_string(power);
    powerStr.resize(4); // ez way to format to 2 d.p.
    resources.append(powerStr);

    return resources;
}
