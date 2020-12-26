#include "Touhou12.h"

namespace Touhou12
{

Touhou12::Touhou12(PROCESSENTRY32W const& pe32) : TouhouMainGameBase(pe32)
{
}

Touhou12::~Touhou12()
{
}

void Touhou12::readDataFromGameProcess()
{
	menuState = -1;
	state.gameState = GameState::Playing;
	state.stageState = StageState::Stage;

	// The BGM playing will be used to determine some things
	char bgm_playing[20];
	ReadProcessMemory(processHandle, (LPCVOID)BGM_STR_1, bgm_playing, 20, NULL);

	// Check if the game over music is playing.
	if (std::strncmp(bgm_playing, "th10_17.wav", std::strlen("th10_17.wav")) == 0)
	{
		state.gameState = GameState::GameOver;
	}

	// Convert the part after the _ and before the . to int
	// That way it is possible to switch case the BGM playing
	bool prefixBGM = bgm_playing[0] == 'b';
	char bgm_id_str[3];
	bgm_id_str[0] = bgm_playing[prefixBGM ? 9 : 5];
	bgm_id_str[1] = bgm_playing[prefixBGM ? 10 : 6];
	bgm_id_str[2] = '\x00';
	int bgm_id = atoi(bgm_id_str);

	bgm = bgm_id;

	// Character
	ReadProcessMemory(processHandle, (LPCVOID)CHARACTER, (LPVOID)&character, 4, NULL);
	switch (character)
	{
	default:
	case 0: state.character = Character::Reimu; break;
	case 1: state.character = Character::Marisa; break;
	case 2: state.character = Character::Sanae; break;
	}

	// Teammate
	ReadProcessMemory(processHandle, (LPCVOID)SUB_CHARACTER, (LPVOID)&characterSub, 4, NULL);
	switch (characterSub)
	{
	default:
	case 0: state.subCharacter = SubCharacter::A; break;
	case 1: state.subCharacter = SubCharacter::B; break;
	}
	
	// Difficulty
	ReadProcessMemory(processHandle, (LPCVOID)DIFFICULTY, (LPVOID)&difficulty, 4, NULL);
	switch (difficulty)
	{
	default:
	case 0: state.difficulty = Difficulty::Easy; break;
	case 1: state.difficulty = Difficulty::Normal; break;
	case 2: state.difficulty = Difficulty::Hard; break;
	case 3: state.difficulty = Difficulty::Lunatic; break;
	case 4: state.difficulty = Difficulty::Extra; break;
	}
	
	// Stage
	ReadProcessMemory(processHandle, (LPCVOID)STAGE, (LPVOID)&stage, 4, NULL);

	// Stage (number of frames in this stage)
	ReadProcessMemory(processHandle, (LPCVOID)STAGE_FRAMES, (LPVOID)&stageFrames, 4, NULL);

	// Game state
	ReadProcessMemory(processHandle, (LPCVOID)GAME_STATE, (LPVOID)&gameState, 4, NULL);

	// Game state (number of frames in this current state)
	ReadProcessMemory(processHandle, (LPCVOID)GAME_STATE_FRAMES, (LPVOID)&gameStateFrames, 4, NULL);

	// Menu state
	DWORD menuPtr = 0;
	ReadProcessMemory(processHandle, (LPCVOID)MENU_POINTER, &menuPtr, 4, NULL); // Get menu class address
	if (state.gameState == GameState::Playing && menuPtr)
	{
		unsigned int inSubMenu = 0;
		ReadProcessMemory(processHandle, (LPCVOID)(menuPtr + 0xB4), (LPVOID)&inSubMenu, 4, NULL);

		if (inSubMenu != 0)
		{
			unsigned int subMenuSelection = 0;
			ReadProcessMemory(processHandle, (LPCVOID)(menuPtr + 0x34), (LPVOID)&subMenuSelection, 4, NULL);

			switch (subMenuSelection)
			{
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
		else
		{
			state.mainMenuState = MainMenuState::TitleScreen;
		}

		menuState = 0;
		state.gameState = GameState::MainMenu;
	}

	if (state.gameState == GameState::Playing)
	{
		// Note that ZUN's naming for the BGM file names is not very consistent
		switch (bgm_id)
		{
		case 1:
			menuState = 0;
			state.mainMenuState = MainMenuState::TitleScreen;
			state.gameState = GameState::MainMenu;
			break;
		case 20: // ending
			state.gameState = GameState::Ending;
			break;
		case 21: // staff roll
			state.gameState = GameState::StaffRoll;
			break;
		default:
			break;
		}
	}

	DWORD enemyStatePtr = 0;
	ReadProcessMemory(processHandle, (LPCVOID)ENEMY_STATE, &enemyStatePtr, 4, NULL); // Get enemy state address
	if (state.gameState == GameState::Playing && enemyStatePtr)
	{
		unsigned int fightingBoss = 0;
		ReadProcessMemory(processHandle, (LPCVOID)(enemyStatePtr + 0x1594), &fightingBoss, 4, NULL);

		if (fightingBoss == 3)
		{
			if (gameState == 6 || gameState == 7)
			{
				// These states are always a midboss
				state.stageState = StageState::Midboss;
			}
			else if (gameState == 24 || gameState == 25 || gameState == 44)
			{
				// These states are always a boss
				state.stageState = StageState::Boss;
			}
			else if (gameState == 43)
			{
				// Some stages have gameState == 43 for both midboss and boss, so split by stage frames
				switch (stage)
				{
				default:
				case 2: state.stageState = stageFrames < 7300 ? StageState::Midboss : StageState::Boss; break;
				case 3: state.stageState = stageFrames < 8900 ? StageState::Midboss : StageState::Boss; break;
				case 5: state.stageState = stageFrames < 9000 ? StageState::Midboss : StageState::Boss; break;
				}
			}
		}
	}

	// Read current game progress
	ReadProcessMemory(processHandle, (LPCVOID)LIVES, (LPVOID)&state.lives, 4, NULL);
	ReadProcessMemory(processHandle, (LPCVOID)BOMBS, (LPVOID)&state.bombs, 4, NULL);
	ReadProcessMemory(processHandle, (LPCVOID)SCORE, (LPVOID)&state.score, 4, NULL);
	ReadProcessMemory(processHandle, (LPCVOID)GAMEOVERS, (LPVOID)&state.gameOvers, 4, NULL);

	if (state.gameState == GameState::Playing)
	{
		unsigned int practiceFlag = 0;
		unsigned int replayFlag = 0;
		ReadProcessMemory(processHandle, (LPCVOID)PRACTICE_FLAG, (LPVOID)&practiceFlag, 4, NULL);
		ReadProcessMemory(processHandle, (LPCVOID)REPLAY_FLAG, (LPVOID)&replayFlag, 4, NULL);

		if (practiceFlag == 16)
		{
			state.gameState = GameState::StagePractice;
		}
		else if (replayFlag == 2)
		{
			state.gameState = GameState::WatchingReplay;
		}
	}
}

std::string Touhou12::getMidbossName() const
{
	switch (stage)
	{
	case 1:
	case 5: return "Nazrin";
	case 2: return "Kogasa Tatara";
	case 3: return "Ichirin Kumoi & Unzan";
	case 4:
	case 6: return "Nue Houjuu (Unknown Form)";
	case 7: return "Kogasa Tatara";
	default: return "";
	}
}

std::string Touhou12::getBossName() const
{
	switch (stage)
	{
	case 1: return "Nazrin";
	case 2: return "Kogasa Tatara";
	case 3: return "Ichirin Kumoi & Unzan";
	case 4: return "Minamitsu Murasa";
	case 5: return "Shou Toramaru";
	case 6: return "Byakuren Hijiri";
	case 7: return "Nue Houjuu";
	default: return "";
	}
}

std::string const& Touhou12::getBGMName() const
{
	return th12_musicNames[bgm];
}

}