#include <iostream>
#include "Touhou09_5.h"

namespace Touhou09_5
{

Touhou09_5::Touhou09_5(PROCESSENTRY32W const& pe32) : TouhouMainGameBase(pe32)
{
}

Touhou09_5::~Touhou09_5()
{
}

void Touhou09_5::readDataFromGameProcess()
{
    menuState = 0;
    state.gameState = GameState::MainMenu;
    state.stageState = StageState::Stage;
    state.mainMenuState = MainMenuState::TitleScreen;

    int gameDataPtr;
    int menuDataPtr;

    int otherMenuStatePtr = 0;
    
    // MENUS
    ReadProcessMemory(processHandle, (LPCVOID)IN_MENU, (LPVOID)&menuState, 1, NULL);
    ReadProcessMemory(processHandle, (LPCVOID)GAME_DATA_PTR, (LPVOID)&gameDataPtr, 4, NULL);
    ReadProcessMemory(processHandle, (LPCVOID)MENU_DATA_PTR, (LPVOID)&menuDataPtr, 4, NULL);
    ReadProcessMemory(processHandle, (LPCVOID)OTHER_MENU_STATE_PTR, (LPVOID)&otherMenuStatePtr, 4, NULL);

    switch (menuState) {
    default:
    case 1:
        if (gameDataPtr == 0) {
            // Game data pointer == 0, means we are in the main menu
            int otherMenuStateValue = 0;

            // When in the main menu, otherMenuStatePtr isn't nullptr, so we can use it here.
            ReadProcessMemory(processHandle, (LPCVOID)(otherMenuStatePtr + OTHER_MENU_STATE_OFFSET), (LPVOID)&otherMenuStateValue, 4, NULL);

            // Check which type of main menu we're in (we don't)
            switch (otherMenuStateValue)
            {
            default:
            case OtherMenuStateValues::MAIN_MENU:
                state.gameState = GameState::MainMenu;
                state.mainMenuState = MainMenuState::TitleScreen;
                break;
            case OtherMenuStateValues::OPTIONS:
                state.gameState = GameState::MainMenu;
                state.mainMenuState = MainMenuState::Options;
                break;
            case OtherMenuStateValues::MUSIC_ROOM:
                state.gameState = GameState::MainMenu;
                state.mainMenuState = MainMenuState::MusicRoom;
                ReadProcessMemory(processHandle, (LPCVOID)BGM_STR, (LPVOID)&bgm_playing, 20, NULL);
                break;
            case OtherMenuStateValues::MANUAL:
                state.gameState = GameState::MainMenu;
                state.mainMenuState = MainMenuState::Manual;
                break;
            }
            // Mission select and replay select are not checked here, as they are already detected by menuState
        }
        else {
            // Game data pointer != 0, means we are in game
            state.gameState = GameState::Playing_CustomResources;
            state.character = Character::Aya;
            state.difficulty = Difficulty::NoDifficultySettings;
        }
        break;
    case 2:
        // We are in a replay
        state.gameState = GameState::WatchingReplay;
        state.character = Character::Aya;
        state.difficulty = Difficulty::NoDifficultySettings;
        break;
    case 5:
        // We are selecting a scene
        state.gameState = GameState::MainMenu;
        state.mainMenuState = MainMenuState::GameStart_Custom;
        break;
    case 21:
        // We are selecting a replay
        state.gameState = GameState::MainMenu;
        state.mainMenuState = MainMenuState::Replays;
        break;
    }

    // Custom menu display
    if (state.gameState == GameState::MainMenu && state.mainMenuState == MainMenuState::GameStart_Custom)
    {
        // Setting total score and completed scenes
        if (menuDataPtr != 0)
        {
            combinedPhotoScore = 0;
            completedScenes = 0;

            for (size_t i = 0; i < 108; i++) // Iterating over all levels scores
            {
                int levelScore = 0;
                ReadProcessMemory(processHandle, (LPCVOID)(menuDataPtr + MENU_DATA_FIRST_SCORE_OFFSET + (MENU_DATA_NEXT_SCORE_OFFSET_ADD*i)), (LPVOID)&levelScore, 4, NULL);
                if (levelScore > 0)
                {
                    completedScenes++;
                    combinedPhotoScore += levelScore;
                }
            }

        }
    }

    // IN-GAME
    if (state.gameState == GameState::Playing_CustomResources) {

        // Read current game progress
        int playerState = 0;
        ReadProcessMemory(processHandle, (LPCVOID)(gameDataPtr + GAME_DATA_SCORE_OFFSET), (LPVOID)&state.score, 4, NULL);
        ReadProcessMemory(processHandle, (LPCVOID)(gameDataPtr + GAME_DATA_STAGE_OFFSET), (LPVOID)&stage, 4, NULL);
        ReadProcessMemory(processHandle, (LPCVOID)(gameDataPtr + GAME_DATA_TIMER_OFFSET), (LPVOID)&stageFrames, 4, NULL);
        ReadProcessMemory(processHandle, (LPCVOID)(gameDataPtr + GAME_DATA_PLAYER_STATE_OFFSET), (LPVOID)&playerState, 1, NULL);

        int photoDataPtr = 0;
        ReadProcessMemory(processHandle, (LPCVOID)(gameDataPtr + GAME_PHOTO_STATS_PTR_OFFSET), (LPVOID)&photoDataPtr, 4, NULL);

        if (photoDataPtr != 0)
        {
            ReadProcessMemory(processHandle, (LPCVOID)(photoDataPtr + GAME_PHOTO_STATS_CURR_PHOTOS_OFFSET), (LPVOID)&state.currentPhotoCount, 4, NULL);
            ReadProcessMemory(processHandle, (LPCVOID)(photoDataPtr + GAME_PHOTO_STATS_REQUIRED_PHOTOS_OFFSET), (LPVOID)&state.requiredPhotoCount, 4, NULL);
        }

        // Check player death
        if (playerState & 0x20)
        {
            state.gameState = GameState::Fail;
        }

        // Check player completion
        if (playerState & 0x40)
        {
            state.score /= 10; // getFormattedScore multiplies the given score by 10.
            state.gameState = GameState::Completed;
        }
    }
}

// Custom mission select resources
std::string Touhou09_5::getCustomMenuResources() const
{

    // Formatted score 
    std::string scoreString = std::to_string(combinedPhotoScore);
    int insertPosition = scoreString.length() - 3;
    while (insertPosition > 0)
    {
        scoreString.insert(insertPosition, ",");
        insertPosition -= 3;
    }

    // Resource string
    std::string resources = std::to_string(completedScenes);
    resources.append(" completed scenes / ");

    resources.append(scoreString);
    resources.append(" combined photo score");
    return resources;
}

// Custom in-game resources (photo n° display)
std::string Touhou09_5::getCustomResources() const
{

    std::string resources = "Photo No ";
    resources.append(std::to_string(state.currentPhotoCount));
    resources.append("/");
    resources.append(std::to_string(state.requiredPhotoCount));
    return resources;
}

// Change how the Playing_CustomResources is handled for this game.
void Touhou09_5::setGameInfo(std::string& info) const
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
    info.append(th095_bossAndSpells[stage]);
}

// Custom stage name, because the game operates with a level-scene style.
std::string Touhou09_5::getStageName() const
{
    if (stage >= 0 && stage < 110)
    {
        int level = (stage / 10)+1;
        int scene = (stage % 10)+1;

        if (level < 11)
        {
            std::string name = "Level ";
            name.append(std::to_string(level));
            name.append("-");
            name.append(std::to_string(scene));
            return name;
        }
        else
        {
            std::string name = "Level EX-";
            name.append(std::to_string(scene));
            return name;
        }
    }
    else { return ""; }
}

std::string const& Touhou09_5::getBGMName() const
{
    std::string fileName(bgm_playing);

    if (fileName.rfind("th095_00", 0) == 0
        || fileName.rfind("th09_00", 0) == 0)     { return th095_musicNames[0]; } // When started by the main menu and not the music room, the filename is "th09_00.wav".
    else if (fileName.rfind("th095_01", 0) == 0)  { return th095_musicNames[1]; }
    else if (fileName.rfind("th095_02", 0) == 0)  { return th095_musicNames[2]; }
    else if (fileName.rfind("th095_03", 0) == 0)  { return th095_musicNames[3]; }
    else if (fileName.rfind("th095_04", 0) == 0)  { return th095_musicNames[4]; }
    else if (fileName.rfind("th09_08_2", 0) == 0) { return th095_musicNames[5]; }
    else { return notSupported; } // In case an error occurs
}

}