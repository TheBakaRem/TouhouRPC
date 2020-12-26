#include "Touhou07.h"
#include <iostream>

namespace Touhou07
{

Touhou07::Touhou07(PROCESSENTRY32W const& pe32)
	: TouhouMainGameBase(pe32)
{
}

Touhou07::~Touhou07()
{}

void Touhou07::readDataFromGameProcess()
{
	// TODO
	// need a way to distinguish options menu from title screen
	// need a way to distinguish replay select and extra start submenu from start submenu
	// need a way to know if we've game over'd, are watching the credits, or watching the staff roll
	// since these are minor and PCB's data is laid out in a pretty hellish way, tempted to just leave it

	// Reset menuState, bgm will tell us if we're in the menu
	menuState = -1;
	state.gameState = GameState::Playing;
	state.stageState = StageState::Stage;

	// this is 0 only if we're not playing currently
	DWORD inGameFlagB = 0;
	ReadProcessMemory(processHandle, (LPCVOID)IN_GAME_FLAG_B, (LPVOID)&inGameFlagB, 4, NULL);

	char stageMode = 0;
	ReadProcessMemory(processHandle, (LPCVOID)STAGE_MODE, (LPVOID)&stageMode, 1, NULL);

	if (inGameFlagB == 0 || (stageMode & STAGE_MODE_DEMO_FLAG) != 0)
	{
		state.gameState = GameState::MainMenu;

		DWORD menu_pointer = 0;
		ReadProcessMemory(processHandle, (LPCVOID)MENU_POINTER, (LPVOID)&menu_pointer, 4, NULL);
		if (menu_pointer != 0)
		{
			// this menu state seems to be all over the place. might be tied to background image being displayed
			ReadProcessMemory(processHandle, (LPCVOID)(menu_pointer + 0x0C), (LPVOID)&menuState, 4, NULL);

			switch (menuState)
			{
			default:
			case 130: state.mainMenuState = MainMenuState::TitleScreen; break;
			case 47: state.mainMenuState = MainMenuState::PlayerData; break;
			case 35: state.mainMenuState = MainMenuState::MusicRoom; break;

			case 129:
			{
				// 129 is anything other than the above special cases. we can distinguish them a little. todo: figure out how to distinguish the rest
				if ((stageMode & STAGE_MODE_PRACTICE_FLAG) != 0)
				{
					state.mainMenuState = MainMenuState::StagePractice;
				}
				else
				{
					state.mainMenuState = MainMenuState::GameStart;
				}
				break;
			}
			}
		}

		if (state.mainMenuState == MainMenuState::MusicRoom)
		{
			ReadProcessMemory(processHandle, (LPCVOID)MUSIC_ROOM_TRACK, (LPVOID)&bgm, 4, NULL);
		}
	}

	ReadProcessMemory(processHandle, (LPCVOID)STAGE, (LPVOID)&stage, 4, NULL);

	if (state.gameState == GameState::Playing)
	{
		if ((stageMode & STAGE_MODE_PRACTICE_FLAG) != 0)
		{
			state.gameState = GameState::StagePractice;
		}
		else if ((stageMode & STAGE_MODE_REPLAY_FLAG) != 0)
		{
			state.gameState = GameState::WatchingReplay;
		}

		DWORD bossFlag = 0;
		ReadProcessMemory(processHandle, (LPCVOID)BOSS_FLAG, (LPVOID)&bossFlag, 4, NULL);
		if (bossFlag == 1)
		{
			// fighting either a boss or a midboss, check which it is
			char midBossFlag = 0;
			ReadProcessMemory(processHandle, (LPCVOID)IS_MAIN_BOSS, (LPVOID)&midBossFlag, 1, NULL);
			state.stageState = (midBossFlag == 3) ? StageState::Boss : StageState::Midboss;
			if (stage == 6)
			{
				// For some reason stage 6 youmu also has flag == 3 so let's take a guess based on frame counter
				// from testing, 5200 seems a reasonable cutoff
				unsigned int frameCounter = 0;
				ReadProcessMemory(processHandle, (LPCVOID)FRAME_COUNTER, (LPVOID)&frameCounter, 4, NULL);
				state.stageState = (frameCounter < 5200) ? StageState::Midboss : StageState::Boss;
			}
		}
	}

	// Read character and difficulty info
	char chara = 0;
	ReadProcessMemory(processHandle, (LPCVOID)CHARACTER, (LPVOID)&chara, 1, NULL);
	switch (chara)
	{
	default:
	case 0: state.character = Character::Reimu; break;
	case 1: state.character = Character::Marisa; break;
	case 2: state.character = Character::Sakuya; break;
	}
	character = chara;

	char charaSub = 0;
	ReadProcessMemory(processHandle, (LPCVOID)SUB_CHARACTER, (LPVOID)&charaSub, 1, NULL);
	switch (charaSub)
	{
	default:
	case 0: state.subCharacter = SubCharacter::A; break;
	case 1: state.subCharacter = SubCharacter::B; break;
	}
	characterSub = charaSub;

	ReadProcessMemory(processHandle, (LPCVOID)DIFFICULTY, (LPVOID)&difficulty, 4, NULL);
	switch (difficulty)
	{
	default:
	case 0: state.difficulty = Difficulty::Easy; break;
	case 1: state.difficulty = Difficulty::Normal; break;
	case 2: state.difficulty = Difficulty::Hard; break;
	case 3: state.difficulty = Difficulty::Lunatic; break;
	case 4: state.difficulty = Difficulty::Extra; break;
	case 5: state.difficulty = Difficulty::Phantasm; break;
	}

	// Read current game progress
	DWORD player_pointer = 0;
	ReadProcessMemory(processHandle, (LPCVOID)PLAYER_POINTER, (LPVOID)&player_pointer, 4, NULL);

	if (player_pointer)
	{
		float lives = 0;
		ReadProcessMemory(processHandle, (LPCVOID)(player_pointer + 0x5C), (LPVOID)&lives, 4, NULL);
		state.lives = (int)lives;

		float bombs = 0;
		ReadProcessMemory(processHandle, (LPCVOID)(player_pointer + 0x68), (LPVOID)&bombs, 4, NULL);
		state.bombs = (int)bombs;

		ReadProcessMemory(processHandle, (LPCVOID)(player_pointer + 0x00), (LPVOID)&state.score, 4, NULL);

		char gameOvers = 0;
		ReadProcessMemory(processHandle, (LPCVOID)(player_pointer + 0x20), (LPVOID)&gameOvers, 1, NULL);
		state.gameOvers = gameOvers;
	}
}

std::string Touhou07::getStageName() const
{
	if (stage <= 7)
	{
		return TouhouMainGameBase::getStageName();
	}
	else
	{
		return "Phantasm Stage";
	}
}

std::string Touhou07::getMidbossName() const
{
	switch (stage)
	{
	case 1: return "Cirno";
	case 2: return "Chen";
	case 3: return "Alice Margatroid";
	case 4: return "Lily White";
	case 5:
	case 6: return "Youmu Konpaku";
	case 7: return "Chen";
	case 8: return "Ran Yakumo";
	default: return "";
	}
}

std::string Touhou07::getBossName() const
{
	switch (stage)
	{
	case 1: return "Letty Whiterock";
	case 2: return "Chen";
	case 3: return "Alice Margatroid";
	case 4: return "Prismriver Sisters";
	case 5: return "Youmu Konpaku";
	case 6: return "Yuyuko Saigyouji";
	case 7: return "Ran Yakumo";
	case 8: return "Yukari Yakumo";
	default: return "";
	}
}

std::string const& Touhou07::getBGMName() const
{
	return th07_musicNames[bgm];
}

}