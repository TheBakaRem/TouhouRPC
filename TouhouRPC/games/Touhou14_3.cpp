#include <Windows.h>
#include "Touhou14_3.h"

Touhou14_3::Touhou14_3(PROCESSENTRY32W const& pe32) : TouhouBase(pe32) {}

Touhou14_3::~Touhou14_3() {}

void Touhou14_3::readDataFromGameProcess() {
    menuState = 0;
    state.gameState = GameState::MainMenu;
    state.stageState = StageState::Stage;
    state.mainMenuState = MainMenuState::TitleScreen;

    int inMenuPtr;      // Pointer to menu data, most notably which menu is currently open. If nullptr, we're in game, else we're in the menus.
    int menuDataPtr;    // Pointer to the data available in the menu (stage completion, scenes completed, etc.)

    // MENUS
    ReadProcessMemory(processHandle, (LPCVOID) IN_MENU_PTR, (LPVOID) &inMenuPtr, 4, NULL);
    ReadProcessMemory(processHandle, (LPCVOID) (inMenuPtr + IN_MENU_STATUS_OFFSET), (LPVOID) &menuState, 4, NULL);

    ReadProcessMemory(processHandle, (LPCVOID) MENU_DATA_PTR, (LPVOID) &menuDataPtr, 4, NULL);
    ReadProcessMemory(processHandle, (LPCVOID) (menuDataPtr + MENU_DATA_SUB_ITEM_LOCK_OFFSET), (LPVOID) &subItemLock, 4, NULL); // If == 0, we don't have sub-items. Else if > 0, we have access to sub-items. (Counts the number of completions in Stage 6-1)

    if (inMenuPtr == 0) {
        state.gameState = GameState::Playing_CustomResources;
        state.character = Character::Seija;
        state.difficulty = Difficulty::NoDifficultySettings;
    }
    else {
        switch (menuState) {
            default:
            case 1:
                // We are in the main menu
                state.gameState = GameState::MainMenu;
                state.mainMenuState = MainMenuState::TitleScreen;
                break;
            case 3:
            case 4:
                // We are in the options
                state.gameState = GameState::MainMenu;
                state.mainMenuState = MainMenuState::Options;
                break;
            case 5:
                // We are in the manual pages
                state.gameState = GameState::MainMenu;
                state.mainMenuState = MainMenuState::Manual;
                break;
            case 6:
                // We are in the music room
                state.gameState = GameState::MainMenu;
                state.mainMenuState = MainMenuState::MusicRoom;
                ReadProcessMemory(processHandle, (LPCVOID) MUSIC_FILE_PLAYED, (LPVOID) &bgm_playing, 20, NULL);
                break;
            case 7:
                // We are selecting a replay
                state.gameState = GameState::MainMenu;
                state.mainMenuState = MainMenuState::Replays;
                break;
            case 11:
                // We are selecting a scene
                state.gameState = GameState::MainMenu;
                state.mainMenuState = MainMenuState::GameStart_Custom;
                break;
            case 12:
                // We are in the nicknames list (achievements page)
                state.gameState = GameState::MainMenu;
                state.mainMenuState = MainMenuState::Achievements;
                break;
        }
    }

    // Custom menu display
    if (state.gameState == GameState::MainMenu && state.mainMenuState == MainMenuState::GameStart_Custom) {
        // Setting total score and completed scenes
        completedScenes = 0;

        if (menuDataPtr != 0) {
            ReadProcessMemory(processHandle, (LPCVOID) (menuDataPtr + MENU_DATA_NB_SCENES_COMPLETED_OFFSET), (LPVOID) &completedScenes, 4, NULL);
        }
    }

    // IN-GAME
    if (state.gameState == GameState::Playing_CustomResources) {

        // Read current game progress
        int playerState = 0;
        int pauseType = PauseType::IN_GAME;

        ReadProcessMemory(processHandle, (LPCVOID) PAUSE, (LPVOID) &pauseType, 4, NULL);

        ReadProcessMemory(processHandle, (LPCVOID) CURRENT_SCORE, (LPVOID) &state.score, 4, NULL);
        ReadProcessMemory(processHandle, (LPCVOID) CURRENT_STAGE, (LPVOID) &stage, 4, NULL);
        ReadProcessMemory(processHandle, (LPCVOID) CURRENT_TIMER, (LPVOID) &stageFrames, 4, NULL);
        ReadProcessMemory(processHandle, (LPCVOID) LIFE_COUNT, (LPVOID) &playerState, 4, NULL);

        int itemDataPtr = 0;    // Item data pointer (number of uses on items).
        ReadProcessMemory(processHandle, (LPCVOID) ITEM_DATA_PTR, (LPVOID) &itemDataPtr, 4, NULL);

        ReadProcessMemory(processHandle, (LPCVOID) MAIN_ITEM, (LPVOID) &currMainItem, 4, NULL);
        ReadProcessMemory(processHandle, (LPCVOID) (itemDataPtr + ITEM_DATA_MAIN_NB_USES_OFFSET), (LPVOID) &state.mainItemUses, 4, NULL);

        ReadProcessMemory(processHandle, (LPCVOID) SUB_ITEM, (LPVOID) &currSubItem, 4, NULL);
        ReadProcessMemory(processHandle, (LPCVOID) (itemDataPtr + ITEM_DATA_SUB_NB_USES_OFFSET), (LPVOID) &state.subItemUses, 4, NULL);

        // Check player death
        if (playerState == -1 && pauseType == PauseType::WIN_LOSE) {
            state.gameState = GameState::Fail;
        }

        // Check player completion (item and no-item clear)
        if (playerState == 0 && pauseType == PauseType::WIN_LOSE) {
            state.gameState = GameState::Completed;
        }

        // Read game mode (done in last, so we can overwrite other game states in case we're in a replay)
        ReadProcessMemory(processHandle, (LPCVOID) GAME_MODE, (LPVOID) &gameMode, 4, NULL);
        switch (gameMode) {
            case GAME_MODE_STANDARD: break; // could be main menu or playing, no need to overwrite anything
            case GAME_MODE_REPLAY: state.gameState = GameState::WatchingReplay; break;
        }
    }

}

// Custom mission select resources
std::string Touhou14_3::getCustomMenuResources() const {
    // Resource string
    std::string resources = std::to_string(completedScenes);
    resources.append(" completed scenes");
    // Can add the number of nicknames earned here.
    return resources;

}

// Change how the Playing_CustomResources is handled for this game.
void Touhou14_3::setGameName(std::string& name) const {
    if (state.gameState != GameState::Playing_CustomResources) {
        // We just want to change how Playing_CustomResources is handled.
        // The rest is unchanged.
        TouhouBase::setGameName(name);
        return;
    }

    name.clear();
    // We show the stage number (day-scene) and it's name
    if (stage >= 1 && stage <= 75) {
        name.append(getStageName());

        name.append(": ");
        name.append(th143_stageNames[stage]);
    }
}

// Change how the Playing_CustomResources is handled for this game.
void Touhou14_3::setGameInfo(std::string& info) const {
    if (state.gameState != GameState::Playing_CustomResources) {
        // We just want to change how Playing_CustomResources is handled.
        // The rest is unchanged.
        TouhouBase::setGameInfo(info);
        return;
    }

    info.clear();
    info.append("Fighting ");
    info.append(th143_bossAndSpells[stage]);
}

void Touhou14_3::setLargeImageInfo(std::string& icon, std::string& text) const {
    icon.clear(), text.clear();

    // In the scene selection menu (we show Seija's image here since she's not shown anywhere else)
    if (state.mainMenuState == MainMenuState::GameStart_Custom) {
        icon.append("seija");
        text.append("Seija");
        return;
    }

    // In-menu check
    if (shouldShowCoverIcon()) {
        icon.append("cover");
        return;
    }

    // In-game
    text.append("Main: ");
    switch (currMainItem) {
        default:
        case Items::FABRIC:
            icon.append("fabric");
            text.append("Nimble Fabric");
            break;
        case Items::CAMERA:
            icon.append("camera");
            text.append("Tengu's Toy Camera");
            break;
        case Items::UMBRELLA:
            icon.append("umbrella");
            text.append("Gap-Folding Umbrella");
            break;
        case Items::LANTERN:
            icon.append("lantern");
            text.append("Ghastly Send-Off Lantern");
            break;
        case Items::YIN_YANG_ORB:
            icon.append("yinyang");
            text.append("Bloodthirsty Yin-Yang Orb");
            break;
        case Items::BOMB:
            icon.append("bomb");
            text.append("Four-Foot Magic Bomb");
            break;
        case Items::JIZO:
            icon.append("jizo");
            text.append("Substitute Jizo");
            break;
        case Items::DOLL:
            icon.append("doll");
            text.append("Cursed Decoy Doll");
            break;
        case Items::MALLET:
            icon.append("mallet");
            text.append("Miracle Mallet (Replica)");
            break;
    }

    text.append(" - "); text.append(std::to_string(state.mainItemUses)); text.append(" uses left.");
}

void Touhou14_3::setSmallImageInfo(std::string& icon, std::string& text) const {
    icon.clear(), text.clear();

    // In-menu check
    if (shouldShowCoverIcon()) return;

    // In-game (only if we're in stage 6-1 or if said stage has been completed at least once)
    if (stage == 35 || subItemLock > 0) {
        bool hasUses = false;

        text.append("Sub: ");
        switch (currSubItem) {
            default:
            case Items::FABRIC:
                icon.append("fabric");
                text.append("Nimble Fabric");
                hasUses = true;
                break;
            case Items::CAMERA:
                icon.append("camera");
                text.append("Tengu's Toy Camera");
                break;
            case Items::UMBRELLA:
                icon.append("umbrella");
                text.append("Gap-Folding Umbrella");
                hasUses = true;
                break;
            case Items::LANTERN:
                icon.append("lantern");
                text.append("Ghastly Send-Off Lantern");
                break;
            case Items::YIN_YANG_ORB:
                icon.append("yinyang");
                text.append("Bloodthirsty Yin-Yang Orb");
                break;
            case Items::BOMB:
                icon.append("bomb");
                text.append("Four-Foot Magic Bomb");
                break;
            case Items::JIZO:
                icon.append("jizo");
                text.append("Substitute Jizo");
                hasUses = true;
                break;
            case Items::DOLL:
                icon.append("doll");
                text.append("Cursed Decoy Doll");
                break;
            case Items::MALLET:
                icon.append("mallet");
                text.append("Miracle Mallet (Replica)");
                break;
        }

        if (hasUses) {
            text.append(" - "); text.append(std::to_string(state.subItemUses)); text.append(" uses left.");
        }
    }
}

// Custom stage name, because the game operates with a day-scene style.
std::string Touhou14_3::getStageName() const {
    if (stage > 0 && stage <= 75) {
        int day = 0;
        int scene = 0;

        // Bruteforce to get day-scene values
        if (stage >= 1 && stage <= 6) { day = 1;  scene = stage; }
        else if (stage >= 7 && stage <= 12) { day = 2;  scene = stage - 6; }
        else if (stage >= 13 && stage <= 19) { day = 3;  scene = stage - 12; }
        else if (stage >= 20 && stage <= 26) { day = 4;  scene = stage - 19; }
        else if (stage >= 27 && stage <= 34) { day = 5;  scene = stage - 26; }
        else if (stage >= 35 && stage <= 42) { day = 6;  scene = stage - 34; }
        else if (stage >= 43 && stage <= 50) { day = 7;  scene = stage - 42; }
        else if (stage >= 51 && stage <= 57) { day = 8;  scene = stage - 50; }
        else if (stage >= 58 && stage <= 65) { day = 9;  scene = stage - 57; }
        else if (stage >= 66 && stage <= 75) { day = 10; scene = stage - 65; }

        std::string name = "Stage ";
        name.append(std::to_string(day)); name.append("-"); name.append(std::to_string(scene));
        return name;
    }
    else { return ""; }
}

// Music name
std::string Touhou14_3::getBGMName() const {
    std::string fileName(bgm_playing);
    if (bgm_playing[0] == 'b') fileName = fileName.substr(4); // Remove the "bgm/" part if it exists

    if (fileName.rfind("th143_01", 0) == 0) { return th143_musicNames[0]; }
    else if (fileName.rfind("th143_02", 0) == 0) { return th143_musicNames[1]; }
    else if (fileName.rfind("th143_05", 0) == 0) { return th143_musicNames[2]; }
    else if (fileName.rfind("th143_06", 0) == 0) { return th143_musicNames[3]; }
    else if (fileName.rfind("th143_07", 0) == 0) { return th143_musicNames[4]; }
    else if (fileName.rfind("th14_03", 0) == 0) { return th143_musicNames[5]; }
    else if (fileName.rfind("th14_12", 0) == 0) { return th143_musicNames[6]; }
    else if (fileName.rfind("th14_10", 0) == 0) { return th143_musicNames[7]; }
    else if (fileName.rfind("th125_06", 0) == 0) { return th143_musicNames[8]; }
    else { return notSupported; } // In case an error occurs
}
