#include <iostream>
#include "Touhou12_5.h"

namespace Touhou12_5
{

Touhou12_5::Touhou12_5(PROCESSENTRY32W const& pe32) : TouhouMainGameBase(pe32)
{
}

Touhou12_5::~Touhou12_5()
{
}

void Touhou12_5::readDataFromGameProcess()
{
    menuState = 0;
    state.gameState = GameState::MainMenu;
    state.stageState = StageState::Stage;
    state.mainMenuState = MainMenuState::TitleScreen;
    
    int menuDataPtr;
    int menuStatePtr;
    
    // MENUS
    ReadProcessMemory(processHandle, (LPCVOID)MENU_STATE_PTR, (LPVOID)&menuStatePtr, 4, NULL);
    ReadProcessMemory(processHandle, (LPCVOID)MENU_DATA_PTR, (LPVOID)&menuDataPtr, 4, NULL);

    // CHARACTER
    ReadProcessMemory(processHandle, (LPCVOID)PLAYER_SELECT, (LPVOID)&player, 4, NULL);

    // MUSIC PLAYING
    ReadProcessMemory(processHandle, (LPCVOID)BGM_STR, bgm_playing, 20, NULL);

    // If we have menuStatePtr, then we are in the main menu
    if (menuStatePtr != 0) {
        ReadProcessMemory(processHandle, (LPCVOID)(menuStatePtr + MENU_STATE_OFFSET), (LPVOID)&menuState, 4, NULL);
        switch (menuState) {
        default:
        case MenuStateValues::MAIN_MENU:
            state.gameState = GameState::MainMenu;
            state.mainMenuState = MainMenuState::TitleScreen;
            break;
        case MenuStateValues::MISSION_SELECT:
            state.gameState = GameState::MainMenu;
            state.mainMenuState = MainMenuState::GameStart_Custom;
            state.character = (player == 0 ? Character::Aya : Character::Hatate);
            lastSelectionValue = MenuStateValues::MISSION_SELECT;
            break;
        case MenuStateValues::REPLAY_SELECT:
            state.gameState = GameState::MainMenu;
            state.mainMenuState = MainMenuState::Replays;
            lastSelectionValue = MenuStateValues::REPLAY_SELECT;
            break;
        case MenuStateValues::OPTIONS:
            state.gameState = GameState::MainMenu;
            state.mainMenuState = MainMenuState::Options;
            break;
        case MenuStateValues::MUSIC_ROOM:
            state.gameState = GameState::MainMenu;
            state.mainMenuState = MainMenuState::MusicRoom;
            break;
        case MenuStateValues::MANUAL:
            state.gameState = GameState::MainMenu;
            state.mainMenuState = MainMenuState::Manual;
            break;
        }
    } else {
        // Menu pointer is nullptr, this means we are in game

        if (bgm_playing[0] == 'b') {
            // If menu pointer is nullptr and the music starts with "b", we know we are on the demo replays, we stay on the main menu
            state.gameState = GameState::MainMenu;
            state.mainMenuState = MainMenuState::TitleScreen;
        } else if (lastSelectionValue == MenuStateValues::REPLAY_SELECT) {
            // We are in a replay if the last valid selection value was replay select
            state.gameState = GameState::WatchingReplay;
        } else {
            // We are in-game otherwise
            state.gameState = GameState::Playing_CustomResources;
        }
        state.character = (player == 0 ? Character::Aya : Character::Hatate);
        state.difficulty = Difficulty::NoDifficultySettings;
    }

    // Custom menu display
    if (state.gameState == GameState::MainMenu && state.mainMenuState == MainMenuState::GameStart_Custom)
    {
        // Setting total score and completed scenes
        if (menuDataPtr != 0)
        {
            combinedPhotoScoreAya = 0;
            completedScenesAya = 0;

            combinedPhotoScoreHatate = 0;
            completedScenesHatate = 0;

            for (size_t i = 0; i < 139; i++) // Iterating over all scores for Aya
            {
                int levelScore = 0;
                ReadProcessMemory(processHandle, (LPCVOID)(menuDataPtr + MENU_DATA_FIRST_SCORE_OFFSET_AYA + (MENU_DATA_NEXT_SCORE_OFFSET_ADD*i)), (LPVOID)&levelScore, 4, NULL);
                if (levelScore > 0)
                {
                    completedScenesAya++;
                    combinedPhotoScoreAya += levelScore;
                }
            }

            for (size_t i = 0; i < 129; i++) // Iterating over all scores for Hatate
            {
                int levelScore = 0;
                ReadProcessMemory(processHandle, (LPCVOID)(menuDataPtr + MENU_DATA_FIRST_SCORE_OFFSET_HATATE + (MENU_DATA_NEXT_SCORE_OFFSET_ADD * i)), (LPVOID)&levelScore, 4, NULL);
                if (levelScore > 0)
                {
                    completedScenesHatate++;
                    combinedPhotoScoreHatate += levelScore;
                }
            }
        }
    }

    // IN-GAME
    if (state.gameState == GameState::Playing_CustomResources) {

        // Read current game progress
        int playerState = 0;
        ReadProcessMemory(processHandle, (LPCVOID)GAME_DATA_SCORE, (LPVOID)&state.score, 4, NULL);
        ReadProcessMemory(processHandle, (LPCVOID)GAME_DATA_STAGE, (LPVOID)&stage, 4, NULL);
        ReadProcessMemory(processHandle, (LPCVOID)GAME_DATA_TIMER, (LPVOID)&stageFrames, 4, NULL);
        ReadProcessMemory(processHandle, (LPCVOID)GAME_DATA_PLAYER_STATE, (LPVOID)&playerState, 1, NULL);

        int photoDataPtr = 0;
        ReadProcessMemory(processHandle, (LPCVOID)GAME_PHOTO_STATS_PTR, (LPVOID)&photoDataPtr, 4, NULL);

        if (photoDataPtr != 0)
        {
            ReadProcessMemory(processHandle, (LPCVOID)(photoDataPtr + GAME_PHOTO_STATS_CURR_PHOTOS_OFFSET), (LPVOID)&state.currentPhotoCount, 4, NULL);
            ReadProcessMemory(processHandle, (LPCVOID)(photoDataPtr + GAME_PHOTO_STATS_REQUIRED_PHOTOS_OFFSET), (LPVOID)&state.requiredPhotoCount, 4, NULL);
        }

        // Check player death
        if (playerState == PauseType::WIN_LOSE_TUTORIAL && state.currentPhotoCount < state.requiredPhotoCount)
        {
            state.gameState = GameState::Fail;
        }

        // Check player completion
        if (playerState == PauseType::WIN_LOSE_TUTORIAL && state.currentPhotoCount == state.requiredPhotoCount)
        {
            state.gameState = GameState::Completed;
        }
    }
}

// Custom mission select resources
std::string Touhou12_5::getCustomMenuResources() const
{

    // Formatted score
    std::string scoreString = std::to_string(state.character == Character::Aya ? combinedPhotoScoreAya : combinedPhotoScoreHatate);
    int insertPosition = scoreString.length() - 3;
    while (insertPosition > 0)
    {
        scoreString.insert(insertPosition, ",");
        insertPosition -= 3;
    }

    // Resource string
    std::string resources = std::to_string(state.character == Character::Aya ? completedScenesAya : completedScenesHatate);
    resources.append(" completed scenes / ");

    resources.append(scoreString);
    resources.append(" combined photo score");
    return resources;
}

// Custom in-game resources (photo n° display)
std::string Touhou12_5::getCustomResources() const
{

    std::string resources = "Photo No ";
    resources.append(std::to_string(state.currentPhotoCount));
    resources.append("/");
    resources.append(std::to_string(state.requiredPhotoCount));
    return resources;
}

// Change how the Playing_CustomResources is handled for this game.
void Touhou12_5::setGameInfo(std::string& info) const
{
    if (state.gameState != GameState::Playing_CustomResources)
    {
        // We just want to change how Playing_CustomResources is handled.
        // The rest is unchanged.
        TouhouMainGameBase::setGameInfo(info);
        return;
    }

    info.clear();
    info.append("Fighting ");
    info.append(th125_bossAndSpells[stage]);
}

void Touhou12_5::setLargeImageInfo(std::string& icon, std::string& text) const
{
    icon.clear(), text.clear();

    // In the scene selection menu (we show the selected character)
    if (state.mainMenuState == MainMenuState::GameStart_Custom)
    {

        icon.append(state.character == Character::Aya ? "aya" : "hatate");
        text.append(state.character == Character::Aya ? "Aya" : "Hatate");
        return;
    }

    // In other cases we use the default selector
    TouhouMainGameBase::setLargeImageInfo(icon, text);
    return;
}



// Custom stage name, because the game operates with a level-scene style.
std::string Touhou12_5::getStageName() const
{
    if (stage >= 0 && stage < 140)
    {
        int level = (stage / 10)+1;
        int scene = (stage % 10)+1;

        if (level < 13)
        {
            std::string name = "Level ";
            name.append(std::to_string(level));
            name.append("-");
            name.append(std::to_string(scene));
            return name;
        }
        else if (level == 13)
        {
            std::string name = "Level EX-";
            name.append(std::to_string(scene));
            return name;
        }
        else
        {
            std::string name = "Level ??-";
            name.append(std::to_string(scene));
            return name;
        }
    }
    else { return ""; }
}

std::string const& Touhou12_5::getBGMName() const
{
    std::string fileName(bgm_playing);

    if (fileName.rfind("bgm/th125_00", 0) == 0)   { return th125_musicNames[0]; }
    else if (fileName.rfind("bgm/th125_01", 0) == 0)  { return th125_musicNames[1]; }
    else if (fileName.rfind("bgm/th125_03", 0) == 0)  { return th125_musicNames[2]; }
    else if (fileName.rfind("bgm/th125_04", 0) == 0)  { return th125_musicNames[3]; }
    else if (fileName.rfind("bgm/th125_05", 0) == 0)  { return th125_musicNames[4]; }
    else if (fileName.rfind("bgm/th125_06", 0) == 0) { return th125_musicNames[5]; }
    else { return notSupported; } // In case an error occurs
}

std::string Touhou12_5::createFormattedScore() const
{
    std::string scoreString = std::to_string(state.score);
    int insertPosition = scoreString.length() - 3; // Do NOT use size_t as it is unsigned and can't be properly tested in the loop after, causing std::out_of_range exceptions.
    while (insertPosition > 0)
    {
        scoreString.insert(insertPosition, ",");
        insertPosition -= 3;
    }
    return scoreString;
}

}