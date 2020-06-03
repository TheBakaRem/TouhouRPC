#include <iostream>
#include "Touhou08.h"

namespace Touhou08
{

Touhou08::Touhou08(PROCESSENTRY32W const& pe32) : TouhouMainGameBase(pe32)
{
	isBoss = 0;
    bossStateChange = 0;
    oldStageFrames = 0;
	spellCardID = 0;
}

Touhou08::~Touhou08()
{
}

void Touhou08::readDataFromGameProcess()
{
    bool oldIsBoss;
    int oldStage; // to reset bossStateChange
    state.gameState = GameState::Playing;
    state.stageState = StageState::Stage;
    state.mainMenuState = MainMenuState::TitleScreen;

    // PLAYER
    ReadProcessMemory(processHandle, (LPCVOID)CHARACTER, (LPVOID)&character, 2, NULL);
    state.subCharacter = (character <= 3) ? SubCharacter::Team : SubCharacter::Solo;
    switch (character)
    {
    case 0: state.character = Character::Border; break;
    case 1: state.character = Character::Magic; break;
    case 2: state.character = Character::Scarlet; break;
    case 3: state.character = Character::Nether; break;
    case 4: state.character = Character::Reimu; break;
    case 5: state.character = Character::Yukari; break;
    case 6: state.character = Character::Marisa; break;
    case 7: state.character = Character::Alice; break;
    case 8: state.character = Character::Sakuya; break;
    case 9: state.character = Character::Remilia; break;
    case 10: state.character = Character::Youmu; break;
    case 11: state.character = Character::Yuyuko; break;
    }

    // DIFFICULTY
    ReadProcessMemory(processHandle, (LPCVOID)DIFFICULTY, (LPVOID)&difficulty, 1, NULL);
    switch (difficulty)
    {
    default:
    case 0: state.difficulty = Difficulty::Easy; break;
    case 1: state.difficulty = Difficulty::Normal; break;
    case 2: state.difficulty = Difficulty::Hard; break;
    case 3: state.difficulty = Difficulty::Lunatic; break;
    case 4: state.difficulty = Difficulty::Extra; break;
    }

    // FRAMES
    oldStageFrames = stageFrames;
    ReadProcessMemory(processHandle, (LPCVOID)STAGE_FRAMES, (LPVOID)&stageFrames, 4, NULL);

    // STAGE
    oldStage = stage;
    if (oldStageFrames - stageFrames > 0)
    {
        oldStage = -1;
    }
    ReadProcessMemory(processHandle, (LPCVOID)STAGE, (LPVOID)&stage, 1, NULL);
    if (stage != oldStage)
    {
        bossStateChange = 0;
    }

    // BOSS
    oldIsBoss = isBoss;
    ReadProcessMemory(processHandle, (LPCVOID)BOSS_APPEARANCE, (LPVOID)&isBoss, 1, NULL);
    if (oldIsBoss != isBoss)
    {
        bossStateChange++;
    }

    // SPELL_CARD_ID
    ReadProcessMemory(processHandle, (LPCVOID)SPELL_CARD_ID, (LPVOID)&spellCardID, 1, NULL);

    unsigned int menuMode = 0;
    ReadProcessMemory(processHandle, (LPCVOID)MENU_MODE, (LPVOID)&menuMode, 4, NULL);
    // menu mode being 2 implies we're in-game

    char stageMode = 0;
    ReadProcessMemory(processHandle, (LPCVOID)STAGE_MODE, (LPVOID)&stageMode, 1, NULL);

    if (menuMode != 2 || (stageMode & STAGE_MODE_DEMO_FLAG) != 0)
    {
        state.gameState = GameState::MainMenu;

        switch (menuMode)
        {
        default:
        case 1: state.mainMenuState = MainMenuState::TitleScreen; break;
        case 5: state.mainMenuState = MainMenuState::PlayerData; break;
        case 8: state.mainMenuState = MainMenuState::MusicRoom; break;
        }

        if (state.mainMenuState == MainMenuState::MusicRoom)
        {
            ReadProcessMemory(processHandle, (LPCVOID)MUSIC_ROOM_TRACK, (LPVOID)&bgm, 4, NULL);
        }
    }


    if (state.gameState == GameState::Playing)
    {
        char bgm_playing[1];
        ReadProcessMemory(processHandle, (LPCVOID)BGM_STR_1, bgm_playing, 1, NULL);

        if ((stageMode & STAGE_MODE_SPELL_PRACTICE_FLAG) != 0 || bgm_playing[0] != 'b')
        {
            state.gameState = GameState::SpellPractice;
        }
        else if ((stageMode & STAGE_MODE_PRACTICE_FLAG) != 0)
        {
            state.gameState = GameState::StagePractice;
        }
        else if ((stageMode & STAGE_MODE_REPLAY_FLAG) != 0)
        {
            state.gameState = GameState::WatchingReplay;
        }

        // boss
        if (isBoss)
        {
            // screw stage 5, 6B, ex
            if (stage == 5 || stage == 7 || stage == 8)
            {
                switch (bossStateChange)
                {
                case 0: // for people who open in middle of a replay
                case 1: // but really launch this before launching your game
                    state.stageState = StageState::Midboss;
                    break;
                default:
                    state.stageState = StageState::Boss;
                    break;
                }
            }
            else
            {
                state.stageState = StageState::Boss;
            }
        }
    }

    // Read current game progress
    DWORD player_pointer = 0;
    ReadProcessMemory(processHandle, (LPCVOID)PLAYER_POINTER, (LPVOID)&player_pointer, 4, NULL);

    if (player_pointer)
    {
        float lives = 0;
        ReadProcessMemory(processHandle, (LPCVOID)(player_pointer + 0x74), (LPVOID)&lives, 4, NULL);
        state.lives = lives;

        float bombs = 0;
        ReadProcessMemory(processHandle, (LPCVOID)(player_pointer + 0x80), (LPVOID)&bombs, 4, NULL);
        state.bombs = bombs;

        ReadProcessMemory(processHandle, (LPCVOID)(player_pointer + 0x00), (LPVOID)&state.score, 4, NULL);

        char gameOvers = 0;
        ReadProcessMemory(processHandle, (LPCVOID)(player_pointer + 0x28), (LPVOID)&gameOvers, 1, NULL);
        state.gameOvers = gameOvers;
    }
}

std::string Touhou08::getStageName() const
{
    return stageName[stage];
}

std::string Touhou08::getMidbossName() const
{
    return midBossName[stage];
}

std::string Touhou08::getBossName() const
{
    return bossName[stage];
}

std::string const& Touhou08::getSpellCardName() const
{
    return th08_spellCardName[spellCardID];
}

std::string const& Touhou08::getBGMName() const
{
    return th08_musicNames[bgm];
}

}