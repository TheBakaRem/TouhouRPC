#include <iostream>
#include "Touhou09.h"

namespace Touhou09
{

Touhou09::Touhou09(PROCESSENTRY32W const& pe32) : TouhouMainGameBase(pe32)
{
}

Touhou09::~Touhou09()
{
}

void Touhou09::readDataFromGameProcess()
{
    menuState = 0;
    gameMode = GameMode::Normal;
    state.gameState = GameState::Playing;
    state.stageState = StageState::Stage;
    state.mainMenuState = MainMenuState::TitleScreen;

    // MENUS
    ReadProcessMemory(processHandle, (LPCVOID)MENU_MODE, (LPVOID)&menuState, 1, NULL);
    char inMenu = 0;
    ReadProcessMemory(processHandle, (LPCVOID)IN_MENU, (LPVOID)&inMenu, 1, NULL);
    if (inMenu == 0 || inMenu == 14)
    {
        state.gameState = GameState::MainMenu;
        switch (menuState)
        {
        default:
        case 176: state.mainMenuState = MainMenuState::TitleScreen; break;
        case 196:
        case 180: state.mainMenuState = MainMenuState::GameStart; break;
        case 40: state.mainMenuState = MainMenuState::Replays; break;
        case 164:
        {
            state.mainMenuState = MainMenuState::MusicRoom;

            DWORD musicRoomPtr = 0;
            ReadProcessMemory(processHandle, (LPCVOID)MUSIC_ROOM_POINTER, (LPVOID)&musicRoomPtr, 4, NULL);
            if (musicRoomPtr)
            {
                ReadProcessMemory(processHandle, (LPCVOID)(musicRoomPtr + MUSIC_ROOM_SELECTION_OFFSET), (LPVOID)&bgm, 4, NULL);
            }
            break;
        }
        }
    }
    else
    {
        switch (menuState)
        {
        case 180: gameMode = GameMode::Match; // fallthrough
        default:
        case 196: state.gameState = GameState::Playing; break;
        case 40: state.gameState = GameState::WatchingReplay; break;
        }
    }

    // GENERAL
    ReadProcessMemory(processHandle, (LPCVOID)DIFFICULTY, (LPVOID)&difficulty, 4, NULL);
    ReadProcessMemory(processHandle, (LPCVOID)STAGE, (LPVOID)&stage, 4, NULL);
    ReadProcessMemory(processHandle, (LPCVOID)LOCATION, (LPVOID)&location, 4, NULL);
    ReadProcessMemory(processHandle, (LPCVOID)ROUND_NUMBER, (LPVOID)&roundNum, 4, NULL);
    int matchModeMode = 0;
    ReadProcessMemory(processHandle, (LPCVOID)MATCH_MODE_MODE, (LPVOID)&matchModeMode, 4, NULL);
    if (gameMode == GameMode::Match && matchModeMode == 2)
    {
        gameMode = GameMode::MatchAsP2;
    }

    // PLAYER 1
    ReadProcessMemory(processHandle, (LPCVOID)P1_CHARACTER, (LPVOID)&p1.chara, 4, NULL);
    ReadProcessMemory(processHandle, (LPCVOID)P1_ALT_COLOUR, (LPVOID)&p1.altCol, 4, NULL);
    ReadProcessMemory(processHandle, (LPCVOID)P1_WIN_COUNT, (LPVOID)&p1.wins, 4, NULL);
    DWORD p1StatePtr = 0;
    ReadProcessMemory(processHandle, (LPCVOID)P1_STATE_POINTER, (LPVOID)&p1StatePtr, 4, NULL);
    if (p1StatePtr)
    {
        short attempts = 0;
        ReadProcessMemory(processHandle, (LPCVOID)(p1StatePtr + P1_ATTEMPTS_REMAINING), (LPVOID)&attempts, 2, NULL);
        switch (attempts)
        {
        default:
        case 0: p1.attempts = 0; break;
        case 16256: p1.attempts = 1; break;
        case 16384: p1.attempts = 2; break;
        case 16448: p1.attempts = 3; break;
        case 16512: p1.attempts = 4; break;
        }

        ReadProcessMemory(processHandle, (LPCVOID)(p1StatePtr + P1_SCORE_OFFSET), (LPVOID)&p1.score, 4, NULL);
        ReadProcessMemory(processHandle, (LPCVOID)(p1StatePtr + P1_GAMEOVERS_OFFSET), (LPVOID)&p1.gameOvers, 4, NULL);
    }
    DWORD p1LivesPtr = 0;
    ReadProcessMemory(processHandle, (LPCVOID)P1_LIVES_POINTER, (LPVOID)&p1LivesPtr, 4, NULL);
    if (p1LivesPtr)
    {
        ReadProcessMemory(processHandle, (LPCVOID)(p1LivesPtr + P1_LIVES_OFFSET), (LPVOID)&p1.lives, 4, NULL);
    }

    // PLAYER 2
    ReadProcessMemory(processHandle, (LPCVOID)P2_CHARACTER, (LPVOID)&p2.chara, 4, NULL);
    ReadProcessMemory(processHandle, (LPCVOID)P2_ALT_COLOUR, (LPVOID)&p2.altCol, 4, NULL);
    ReadProcessMemory(processHandle, (LPCVOID)P2_WIN_COUNT, (LPVOID)&p2.wins, 4, NULL);
    DWORD p2StatePtr = 0;
    ReadProcessMemory(processHandle, (LPCVOID)P2_STATE_POINTER, (LPVOID)&p2StatePtr, 4, NULL);
    if (p2StatePtr)
    {
        ReadProcessMemory(processHandle, (LPCVOID)(p2StatePtr + P2_SCORE_OFFSET), (LPVOID)&p2.score, 4, NULL);
        ReadProcessMemory(processHandle, (LPCVOID)(p2StatePtr + P2_GAMEOVERS_OFFSET), (LPVOID)&p2.gameOvers, 4, NULL);
    }
    DWORD p2LivesPtr = 0;
    ReadProcessMemory(processHandle, (LPCVOID)P2_LIVES_POINTER, (LPVOID)&p2LivesPtr, 4, NULL);
    if (p2LivesPtr)
    {
        ReadProcessMemory(processHandle, (LPCVOID)(p2LivesPtr + P2_LIVES_OFFSET), (LPVOID)&p2.lives, 4, NULL);
    }

    bool const useP1 = (gameMode != GameMode::MatchAsP2);

    switch (useP1 ? p1.chara : p2.chara)
    {
    default:
    case 0: state.character = Character::Reimu; break;
    case 1: state.character = Character::Marisa; break;
    case 2: state.character = Character::Sakuya; break;
    case 3: state.character = Character::Youmu; break;
    case 4: state.character = Character::Reisen; break;
    case 5: state.character = Character::Cirno; break;
    case 6: state.character = Character::Lyrica; break;
    case 7: state.character = Character::Mystia; break;
    case 8: state.character = Character::Tewi; break;
    case 9: state.character = Character::Yuuka; break;
    case 10: state.character = Character::Aya; break;
    case 11: state.character = Character::Medicine; break;
    case 12: state.character = Character::Komachi; break;
    case 13: state.character = Character::Eiki; break;
    case 14: state.character = Character::Merlin; break;
    case 15: state.character = Character::Lunasa; break;
    }

    state.subCharacter = (useP1 ? p1.altCol == 1 : p2.altCol == 1) ? SubCharacter::AltColour : SubCharacter::None;
    state.score = (useP1 ? p1.score : p2.score);
    state.gameOvers = (useP1 ? p1.gameOvers : p2.gameOvers);

    if (gameMode == GameMode::Normal)
    {
        state.lives = p1.attempts;
    }

    switch (difficulty)
    {
    default:
    case 0: state.difficulty = Difficulty::Easy; break;
    case 1: state.difficulty = Difficulty::Normal; break;
    case 2: state.difficulty = Difficulty::Hard; break;
    case 3: state.difficulty = Difficulty::Lunatic; break;
    case 4: state.difficulty = Difficulty::Extra; break;
    }
}

void Touhou09::setGameName(std::string& name) const
{
    switch (state.gameState)
    {
    default: TouhouMainGameBase::setGameName(name); break;

    case GameState::Playing:
    {
        switch (gameMode)
        {
        case GameMode::Normal:
        {
            name.append(getStageName());

            if (state.difficulty != Difficulty::Extra)
            {
                // normal play shows resources or score
                name.reserve(name.size() + 13 /* text characters */ + 12 /* max score size */);
                name.append(" - (");
                if (showScoreInsteadOfResources)
                {
                    name.append(createFormattedScore());
                }
                else
                {
                    name.append(std::to_string(p1.attempts));
                    name.append(" chances");
                }
                name.append(")");
            }
            break;
        }
        case GameMode::Match:
        case GameMode::MatchAsP2:
        {
            name.append(getStageName());
            break;
        }
        }
        break;
    }
    }
}

void Touhou09::setGameInfo(std::string& info) const
{
    switch (state.gameState)
    {
    default: TouhouMainGameBase::setGameInfo(info); break;

    case GameState::Playing:
    {
        switch (gameMode)
        {
        case GameMode::Normal:
        {
            info.append(getCharacterName(p1.chara));
            if (state.difficulty != Difficulty::Extra)
            {
                info.append(" (");
                std::string p1Lives = std::to_string(static_cast<double>(p1.lives) / 2.0);
                p1Lives.resize(3); // lazy way to get 1 d.p.
                info.append(p1Lives);
                info.append(")");
            }

            info.append(" vs ");
            info.append(getCharacterName(p2.chara));

            if (state.difficulty != Difficulty::Extra)
            {
                info.append(" (");
                std::string p2Lives = std::to_string(static_cast<double>(p2.lives) / 2.0);
                p2Lives.resize(3);
                info.append(p2Lives);
                info.append(")");
            }
            break;
        }
        case GameMode::Match:
        case GameMode::MatchAsP2:
        {
            info.append(getCharacterName(p1.chara));
            info.append(" (");
            info.append(std::to_string(p1.wins));
            info.append(") vs ");
            info.append(getCharacterName(p2.chara));
            info.append(" (");
            info.append(std::to_string(p2.wins));
            info.append(")");
            break;
        }
        }
        break;
    }


    }
}

std::string Touhou09::getStageName() const
{
    if (gameMode == GameMode::Normal)
    {
        std::string ret = (difficulty == 4) ? "Extra Stage " : "Stage ";
        ret.append(std::to_string(stage + 1));
        return ret;
    }
    else
    {
        std::string ret = "Round ";
        ret.append(std::to_string(roundNum + 1));
        if (location < 14) // location can be set to 255 before setting itself correctly
        {
            ret.append(" - ");
            ret.append(th09_stageNames[location]);
        }
        return ret;
    }
}

std::string const& Touhou09::getBGMName() const
{
    return th09_musicNames[bgm];
}

std::string const& Touhou09::getCharacterName(int character) const
{
    return th09_characterNames[character];
}

}