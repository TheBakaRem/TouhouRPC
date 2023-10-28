import "Touhou12_5.h";

Touhou12_5::Touhou12_5(PROCESSENTRY32W const& pe32) : TouhouBase(pe32) {}

Touhou12_5::~Touhou12_5() {}

void Touhou12_5::readDataFromGameProcess() {
    menuState = 0;
    state.gameState = GameState::MainMenu;
    state.stageState = StageState::Stage;
    state.mainMenuState = MainMenuState::TitleScreen;

    // MENUS
    TouhouAddress menuDataPtr = ReadProcessMemoryInt(processHandle, MENU_DATA_PTR);
    TouhouAddress mainMenuStatePtr = ReadProcessMemoryInt(processHandle, MENU_STATE_PTR);

    // CHARACTER
    player = ReadProcessMemoryInt(processHandle, PLAYER_SELECT);

    // MUSIC PLAYING
    bgm_playing = ReadProcessMemoryString(processHandle, BGM_STR, 20);

    // If we have mainMenuStatePtr, then we are in the main menu
    if (mainMenuStatePtr) {
        menuState = ReadProcessMemoryInt(processHandle, (mainMenuStatePtr + MENU_STATE_OFFSET));
        switch (menuState) {
            default:
            case mainMenuStateValues::MAIN_MENU:
                state.gameState = GameState::MainMenu;
                state.mainMenuState = MainMenuState::TitleScreen;
                break;
            case mainMenuStateValues::MISSION_SELECT:
                state.gameState = GameState::MainMenu;
                state.mainMenuState = MainMenuState::GameStart_Custom;
                state.character = (player == 0 ? Character::Aya : Character::Hatate);
                lastSelectionValue = mainMenuStateValues::MISSION_SELECT;
                break;
            case mainMenuStateValues::REPLAY_SELECT:
                state.gameState = GameState::MainMenu;
                state.mainMenuState = MainMenuState::Replays;
                lastSelectionValue = mainMenuStateValues::REPLAY_SELECT;
                break;
            case mainMenuStateValues::OPTIONS:
                state.gameState = GameState::MainMenu;
                state.mainMenuState = MainMenuState::Options;
                break;
            case mainMenuStateValues::MUSIC_ROOM:
                state.gameState = GameState::MainMenu;
                state.mainMenuState = MainMenuState::MusicRoom;
                break;
            case mainMenuStateValues::MANUAL:
                state.gameState = GameState::MainMenu;
                state.mainMenuState = MainMenuState::Manual;
                break;
        }
    }
    else {
        // Menu pointer is nullptr, this means we are in game

        if (bgm_playing[0] == 'b') {
            // If menu pointer is nullptr and the music starts with "b", we know we are on the demo replays, we stay on the main menu
            state.gameState = GameState::MainMenu;
            state.mainMenuState = MainMenuState::TitleScreen;
        }
        else if (lastSelectionValue == mainMenuStateValues::REPLAY_SELECT) {
            // We are in a replay if the last valid selection value was replay select
            state.gameState = GameState::WatchingReplay;
        }
        else {
            // We are in-game otherwise
            state.gameState = GameState::Playing_CustomResources;
        }
        state.character = (player == 0 ? Character::Aya : Character::Hatate);
        state.difficulty = Difficulty::NoDifficultySettings;
    }

    // Custom menu display
    if (state.gameState == GameState::MainMenu && state.mainMenuState == MainMenuState::GameStart_Custom) {
        // Setting total score and completed scenes
        if (menuDataPtr) {
            combinedPhotoScoreAya = 0;
            completedScenesAya = 0;

            combinedPhotoScoreHatate = 0;
            completedScenesHatate = 0;

            for (size_t i = 0; i < 139; i++) // Iterating over all scores for Aya
            {
                int levelScore = ReadProcessMemoryInt(processHandle, (menuDataPtr + MENU_DATA_FIRST_SCORE_OFFSET_AYA + (MENU_DATA_NEXT_SCORE_OFFSET_ADD * i)));
                if (levelScore > 0) {
                    completedScenesAya++;
                    combinedPhotoScoreAya += levelScore;
                }
            }

            for (size_t i = 0; i < 129; i++) // Iterating over all scores for Hatate
            {
                int levelScore = ReadProcessMemoryInt(processHandle, (menuDataPtr + MENU_DATA_FIRST_SCORE_OFFSET_HATATE + (MENU_DATA_NEXT_SCORE_OFFSET_ADD * i)));
                if (levelScore > 0) {
                    completedScenesHatate++;
                    combinedPhotoScoreHatate += levelScore;
                }
            }
        }
    }

    // IN-GAME
    if (state.gameState == GameState::Playing_CustomResources) {

        // Read current game progress
        int playerState = ReadProcessMemoryInt(processHandle, GAME_DATA_PLAYER_STATE, 1);
        state.score = ReadProcessMemoryInt(processHandle, GAME_DATA_SCORE);
        stage = ReadProcessMemoryInt(processHandle, GAME_DATA_STAGE);
        stageFrames = ReadProcessMemoryInt(processHandle, GAME_DATA_TIMER);

        TouhouAddress photoDataPtr = ReadProcessMemoryInt(processHandle, GAME_PHOTO_STATS_PTR);

        if (photoDataPtr) {
            state.currentPhotoCount = ReadProcessMemoryInt(processHandle, (photoDataPtr + GAME_PHOTO_STATS_CURR_PHOTOS_OFFSET));
            state.requiredPhotoCount = ReadProcessMemoryInt(processHandle, (photoDataPtr + GAME_PHOTO_STATS_REQUIRED_PHOTOS_OFFSET));
        }

        // Check player death
        if (playerState == PauseType::WIN_LOSE_TUTORIAL && state.currentPhotoCount < state.requiredPhotoCount) {
            state.gameState = GameState::Fail;
        }

        // Check player completion
        if (playerState == PauseType::WIN_LOSE_TUTORIAL && state.currentPhotoCount == state.requiredPhotoCount) {
            state.gameState = GameState::Completed;
        }
    }
}

// Custom mission select resources
std::string Touhou12_5::getCustomMenuResources() const {

    // Formatted score
    std::string scoreString = formatScore(state.character == Character::Aya ? combinedPhotoScoreAya : combinedPhotoScoreHatate);

    // Resource string
    std::string resources = std::to_string(state.character == Character::Aya ? completedScenesAya : completedScenesHatate);
    resources.append(" completed scenes / ");

    resources.append(scoreString);
    resources.append(" combined photo score");
    return resources;
}

// Custom in-game resources (photo n° display)
std::string Touhou12_5::getCustomResources() const {

    std::string resources = "Photo No ";
    resources.append(std::to_string(state.currentPhotoCount));
    resources.append("/");
    resources.append(std::to_string(state.requiredPhotoCount));
    return resources;
}

// Change how the Playing_CustomResources is handled for this game.
void Touhou12_5::setGameInfo(std::string& info) const {
    if (state.gameState != GameState::Playing_CustomResources) {
        // We just want to change how Playing_CustomResources is handled.
        // The rest is unchanged.
        TouhouBase::setGameInfo(info);
        return;
    }

    info.clear();
    info.append("Fighting ");
    info.append(th125_bossAndSpells[stage]);
}

void Touhou12_5::setLargeImageInfo(std::string& icon, std::string& text) const {
    icon.clear(), text.clear();

    // In the scene selection menu (we show the selected character)
    if (state.mainMenuState == MainMenuState::GameStart_Custom) {

        icon.append(state.character == Character::Aya ? "aya" : "hatate");
        text.append(state.character == Character::Aya ? "Aya" : "Hatate");
        return;
    }

    // In other cases we use the default selector
    TouhouBase::setLargeImageInfo(icon, text);
    return;
}

// Custom stage name, because the game operates with a level-scene style.
std::string Touhou12_5::getStageName() const {
    if (stage >= 0 && stage < 140) {
        int level = (stage / 10) + 1;
        int scene = (stage % 10) + 1;

        if (level < 13) {
            std::string name = "Level ";
            name.append(std::to_string(level));
            name.append("-");
            name.append(std::to_string(scene));
            return name;
        }
        else if (level == 13) {
            std::string name = "Level EX-";
            name.append(std::to_string(scene));
            return name;
        }
        else {
            std::string name = "Level ??-";
            name.append(std::to_string(scene));
            return name;
        }
    }
    else { return ""; }
}

std::string Touhou12_5::getBGMName() const {
    std::string fileName = bgm_playing;

    if (fileName.rfind("bgm/th125_00", 0) == 0) { return th125_musicNames[0]; }
    else if (fileName.rfind("bgm/th125_01", 0) == 0) { return th125_musicNames[1]; }
    else if (fileName.rfind("bgm/th125_03", 0) == 0) { return th125_musicNames[2]; }
    else if (fileName.rfind("bgm/th125_04", 0) == 0) { return th125_musicNames[3]; }
    else if (fileName.rfind("bgm/th125_05", 0) == 0) { return th125_musicNames[4]; }
    else if (fileName.rfind("bgm/th125_06", 0) == 0) { return th125_musicNames[5]; }
    else { return notSupported; } // In case an error occurs
}

std::string Touhou12_5::createFormattedScore() const {
    return formatScore(state.score);
}
