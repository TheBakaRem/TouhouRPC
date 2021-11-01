#include <iostream>
#include "Touhou16_5.h"

namespace Touhou16_5
{

Touhou16_5::Touhou16_5(PROCESSENTRY32W const& pe32) : TouhouMainGameBase(pe32)
{
}

Touhou16_5::~Touhou16_5()
{
}

void Touhou16_5::readDataFromGameProcess()
{
    menuState = 0;
    state.gameState = GameState::MainMenu;
    state.stageState = StageState::Stage;
    state.mainMenuState = MainMenuState::TitleScreen;

    int inMenuPtr;      // Pointer to menu data, most notably which menu is currently open. If nullptr, we're in game, else we're in the menus.
    int menuDataPtr;    // Pointer to the data available in the menu (stage completion, scenes completed, etc.)

    // MENUS
    ReadProcessMemory(processHandle, (LPCVOID)IN_MENU_PTR, (LPVOID)&inMenuPtr, 4, NULL);
    ReadProcessMemory(processHandle, (LPCVOID)(inMenuPtr + IN_MENU_STATUS_OFFSET), (LPVOID)&menuState, 4, NULL);

    ReadProcessMemory(processHandle, (LPCVOID)MENU_DATA_PTR, (LPVOID)&menuDataPtr, 4, NULL);
    
    if (inMenuPtr == 0)
    {
        state.gameState = GameState::Playing_CustomResources;
        state.character = Character::Sumireko;
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
            ReadProcessMemory(processHandle, (LPCVOID)MUSIC_FILE_PLAYED, (LPVOID)&bgm_playing, 20, NULL);
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
    if (state.gameState == GameState::MainMenu && state.mainMenuState == MainMenuState::GameStart_Custom)
    {
        // Setting total score and completed scenes
        completedScenes = 0;
        totalScore = 0;

        if (menuDataPtr != 0) {
            // Check if the value exists in memory first.
            for (size_t i = 0; i <= 102; i++) // Iterating over all scores
            {
                StageData stage;
                ReadProcessMemory(processHandle, (LPCVOID)(menuDataPtr + MENU_DATA_FIRST_LEVEL_OFFSET + (MENU_DATA_NEXT_LEVEL_OFFSET_ADD * i)), (LPVOID)&stage, 24, NULL);
                if (stage.clear_count > 0)
                {
                    completedScenes++;
                    totalScore += stage.best_score;
                }
            }
        }
    }

    
    // IN-GAME
    if (state.gameState == GameState::Playing_CustomResources) {

        // Read current game progress
        int playerState = 0;
        int photoCountPointer = 0;
        int photoCount = 0;
        int pauseType = PauseType::IN_GAME;

        ReadProcessMemory(processHandle, (LPCVOID)PAUSE, (LPVOID)&pauseType, 4, NULL);

        ReadProcessMemory(processHandle, (LPCVOID)CURRENT_STAGE, (LPVOID)&stage, 4, NULL);
        ReadProcessMemory(processHandle, (LPCVOID)CURRENT_TIMER, (LPVOID)&stageFrames, 4, NULL);
        ReadProcessMemory(processHandle, (LPCVOID)LIFE_COUNT, (LPVOID)&playerState, 4, NULL);

        ReadProcessMemory(processHandle, (LPCVOID)PHOTO_COUNT_PTR, (LPVOID)&photoCountPointer, 4, NULL);
        ReadProcessMemory(processHandle, (LPCVOID)(photoCountPointer + PHOTO_COUNT_OFFSET), (LPVOID)&photoCount, 4, NULL);

        // Check player death or not enough photos taken
        if ((playerState == -1 || photoCount == 0) && pauseType == PauseType::WIN_LOSE)
        {
            state.gameState = GameState::Fail;
        }

        // Check player completion
        if (playerState == 0 && photoCount > 0 && pauseType == PauseType::WIN_LOSE)
        {
            state.gameState = GameState::Completed;
        }

        // Read game mode (done in last, so we can overwrite other game states in case we're in a replay)
        ReadProcessMemory(processHandle, (LPCVOID)GAME_MODE, (LPVOID)&gameMode, 4, NULL);
        switch (gameMode)
        {
        case GAME_MODE_STANDARD:
        case GAME_MODE_MENU: /* could be main menu or playing, no need to overwrite anything */ break;
        case GAME_MODE_REPLAY: state.gameState = GameState::WatchingReplay; break;
        }
    }
    
}

// Custom mission select resources
std::string Touhou16_5::getCustomMenuResources() const
{
    // Resource string
    std::string resources = std::to_string(completedScenes);
    resources.append(" completed scenes / ");

    resources.append(std::to_string(totalScore));
    resources.append(" combined photo score");
    return resources;
}

// Change how the Playing_CustomResources is handled for this game.
void Touhou16_5::setGameName(std::string& name) const
{
    if (state.gameState != GameState::Playing_CustomResources)
    {
        // We just want to change how Playing_CustomResources is handled.
        // The rest is unchanged.
        TouhouMainGameBase::setGameName(name);
        return;
    }

    name.clear();
    // We show the stage number (day-scene)
    if (stage >= 0 && stage <= 102)
    {
        name.append(getStageName());
    }
}

// Change how the Playing_CustomResources is handled for this game.
void Touhou16_5::setGameInfo(std::string& info) const
{
    // We just want to change how Playing_CustomResources and Completed are handled.
    // The rest is unchanged.
    if (state.gameState == GameState::Playing_CustomResources)
    {
        info.clear();
        info.append("Fighting ");
        info.append(th165_bossAndSpells[stage]);
        return;
    }
    else if (state.gameState == GameState::Completed)
    {
        // We override the MainGameBase to not show the score of the stage on completion.
        info.clear();
        return;
    }

    // Use normal info handling
    TouhouMainGameBase::setGameInfo(info);
}


// Custom stage name, because the game operates with a day-scene style.
std::string Touhou16_5::getStageName() const
{
    if (stage >= 0 && stage <= 102)
    {
        std::string day = "";
        int scene = 0;

        // Bruteforce to get day-scene values
        if      (stage >= 0  && stage <= 1)     { day = "Sunday";  scene = stage+1; }
        else if (stage >= 2  && stage <= 5)     { day = "Monday";  scene = stage-1; }
        else if (stage >= 6  && stage <= 8)     { day = "Tuedsay";  scene = stage-5; }
        else if (stage >= 9  && stage <= 12)    { day = "Wednesday";  scene = stage-8; }
        else if (stage >= 13 && stage <= 15)    { day = "Thursday";  scene = stage-12; }
        else if (stage >= 16 && stage <= 18)    { day = "Friday";  scene = stage-15; }
        else if (stage == 19)                   { day = "Saturday";  scene = 1; }

        else if (stage >= 20 && stage <= 26)    { day = "Wrong Sunday";  scene = stage-19; }
        else if (stage >= 27 && stage <= 30)    { day = "Wrong Monday";  scene = stage-26; }
        else if (stage >= 31 && stage <= 34)    { day = "Wrong Tuedsay";  scene = stage-30; }
        else if (stage >= 35 && stage <= 40)    { day = "Wrong Wednesday";  scene = stage-34; }
        else if (stage >= 41 && stage <= 45)    { day = "Wrong Thursday";  scene = stage-40; }
        else if (stage >= 46 && stage <= 50)    { day = "Wrong Friday";  scene = stage-45; }
        else if (stage >= 51 && stage <= 56)    { day = "Wrong Saturday";  scene = stage-50; }

        else if (stage >= 57 && stage <= 62)    { day = "Nightmare Sunday";  scene = stage-56; }
        else if (stage >= 63 && stage <= 68)    { day = "Nightmare Monday";  scene = stage-62; }
        else if (stage >= 69 && stage <= 74)    { day = "Nightmare Tuedsay";  scene = stage-68; }
        else if (stage >= 75 && stage <= 80)    { day = "Nightmare Wednesday";  scene = stage-74; }
        else if (stage >= 81 && stage <= 86)    { day = "Nightmare Thursday";  scene = stage-80; }
        else if (stage >= 87 && stage <= 92)    { day = "Nightmare Friday";  scene = stage-86; }
        else if (stage >= 93 && stage <= 98)    { day = "Nightmare Saturday";  scene = stage-92; }
        else if (stage >= 99 && stage <= 102)   { day = "Nightmare Diary";  scene = stage-98; }

        std::string name = day;
        name.append(" - "); name.append(std::to_string(scene));
        return name;
    }
    else { return ""; }
}

// Music name
std::string const& Touhou16_5::getBGMName() const
{    
    // Todo : Use VD music lists and music count
    std::string fileName(bgm_playing);
    if (bgm_playing[0] == 'b') fileName = fileName.substr(4); // Remove the "bgm/" part if it exists

    if      (fileName.rfind("th165_01", 0) == 0) { return th165_musicNames[0]; }
    else if (fileName.rfind("th165_04", 0) == 0) { return th165_musicNames[1]; }
    else if (fileName.rfind("th165_06", 0) == 0) { return th165_musicNames[2]; }
    else if (fileName.rfind("th165_07", 0) == 0) { return th165_musicNames[3]; }
    else if (fileName.rfind("th165_02", 0) == 0) { return th165_musicNames[4]; }
    else if (fileName.rfind("th165_05", 0) == 0) { return th165_musicNames[5]; }
    else if (fileName.rfind("th15_07", 0) == 0)  { return th165_musicNames[6]; }
    else if (fileName.rfind("th16_12", 0) == 0)  { return th165_musicNames[7]; }
    else { return notSupported; } // In case an error occurs
}

}