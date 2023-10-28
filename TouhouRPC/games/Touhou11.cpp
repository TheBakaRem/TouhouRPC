import "Touhou11.h";

Touhou11::Touhou11(PROCESSENTRY32W const& pe32) : TouhouBase(pe32) {}

Touhou11::~Touhou11() {}

void Touhou11::readDataFromGameProcess() {
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
        case 0: state.subCharacter = (state.character == Character::Reimu) ? SubCharacter::AndYukari : SubCharacter::AndAlice; break;
        case 1: state.subCharacter = (state.character == Character::Reimu) ? SubCharacter::AndSuika : SubCharacter::AndPatchouli; break;
        case 2: state.subCharacter = (state.character == Character::Reimu) ? SubCharacter::AndAya : SubCharacter::AndNitori; break;
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
            case 0:
                mainMenuState = 0;
                state.mainMenuState = MainMenuState::TitleScreen;
                state.gameState = GameState::MainMenu;
                break;
            case 18: // ending
                state.gameState = GameState::Ending;
                break;
            case 19: // staff roll
                state.gameState = GameState::StaffRoll;
                break;
            default:
                break;
        }
    }

    TouhouAddress enemyStatePtr = ReadProcessMemoryInt(processHandle, ENEMY_STATE); // Get enemy state address
    if (state.gameState == GameState::Playing && enemyStatePtr) {
        int fightingBoss = ReadProcessMemoryInt(processHandle, (enemyStatePtr + 0x131C));

        if (fightingBoss == 3) {
            // Stage 1: Kisume
            if (stage == 1 && (gameState >= 6 && gameState <= 8) || gameState == 15) {
                state.stageState = StageState::Midboss;
            }
            // Stage 1: Yamame
            else if (stage == 1 && (gameState == 2 || gameState >= 24)) {
                state.stageState = StageState::Boss;
            }

            // Stage 2: Parsee (mid-boss)
            else if (stage == 2 && (gameState == 6 || (gameState == 43 && gameStateFrames <= 2040))) {
                state.stageState = StageState::Midboss;
            }
            // Stage 2: Parsee (boss)
            else if (stage == 2 && (gameState == 2 || gameState >= 24)) {
                state.stageState = StageState::Boss;
            }

            // Stage 3: Yuugi (mid-boss)
            else if (stage == 3 && (gameState == 6 || gameState == 43 || gameState == 4)) {
                state.stageState = StageState::Midboss;
            }
            // Stage 3: Yuugi (boss)
            else if (stage == 3 && (gameState == 2 || gameState >= 24)) {
                state.stageState = StageState::Boss;
            }

            // Stage 4: Orin (start)
            else if (stage == 4 && (gameState == 6 && gameStateFrames <= 900)) {
                state.stageState = StageState::Midboss;
            }
            // Stage 4: Orin (mid-boss)
            else if (stage == 4 && (gameState == 7 && gameStateFrames <= 1590)) {
                state.stageState = StageState::Midboss;
            }
            // Stage 4: Satori (boss)
            else if (stage == 4 && (gameState == 2 || gameState >= 24)) {
                state.stageState = StageState::Boss;
            }

            // Stage 5: Orin (mid-boss)
            else if (stage == 5 && (gameState == 6 || (gameState == 7 && gameStateFrames <= 1200) || gameState == 15)) {
                state.stageState = StageState::Midboss;
            }
            // Stage 5: Orin (boss)
            else if (stage == 5 && (gameState == 2 || gameState >= 24)) {
                state.stageState = StageState::Boss;
            }

            // Stage 6: Orin (mid-boss)
            else if (stage == 6 && (gameState == 6 || (gameState == 15 && gameStateFrames <= 2220))) {
                state.stageState = StageState::Midboss;
            }
            // Stage 6: Utsuho (boss)
            else if (stage == 6 && gameState >= 24) {
                state.stageState = StageState::Boss;
            }

            // Stage 7 (Extra): Sanae
            else if (stage == 7 && (gameState == 6 || (gameState == 0 && (stageFrames > gameStateFrames) && gameStateFrames <= 11200))) {
                state.stageState = StageState::Midboss;
            }
            // Stage 7 (Extra): Koishi
            else if (stage == 7 && (gameState == 2 || gameState >= 24)) {
                state.stageState = StageState::Boss;
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

std::string Touhou11::getMidbossName() const {
    switch (stage) {
        case 1: return "Kisume";
        case 2: return "Parsee Mizuhashi";
        case 3: return "Yuugi Hoshiguma";
        case 4:
        case 5:
        case 6: return "Rin Kaenbyou";
        case 7: return "Sanae Kochiya";
        default: return "";
    }
}

std::string Touhou11::getBossName() const {
    switch (stage) {
        case 1: return "Yamame Kurodani";
        case 2: return "Parsee Mizuhashi";
        case 3: return "Yuugi Hoshiguma";
        case 4: return "Satori Komeiji";
        case 5: return "Rin Kaenbyou";
        case 6: return "Utsuho Reiuji";
        case 7: return "Koishi Komeiji";
        default: return "";
    }
}

std::string Touhou11::getBGMName() const {
    return th11_musicNames[bgm];
}

std::string Touhou11::getCustomResources() const {
    std::string resources = std::to_string(state.lives);
    resources.append("/");
    std::string powerStr = std::to_string(power);
    powerStr.resize(4); // ez way to format to 2 d.p.
    resources.append(powerStr);

    return resources;
}
