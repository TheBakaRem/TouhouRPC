#include "Touhou10.h"

namespace Touhou10
{

Touhou10::Touhou10(PROCESSENTRY32W const& pe32) : TouhouMainGameBase(pe32)
{
}

Touhou10::~Touhou10()
{
}

void Touhou10::readDataFromGameProcess()
{
	menuState = -1;
	state.gameState = GameState::Playing;
	state.stageState = StageState::Stage;

	// The BGM playing will be used to determine a lot of things
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
	}

	// Teammate
	ReadProcessMemory(processHandle, (LPCVOID)SUB_CHARACTER, (LPVOID)&characterSub, 4, NULL);
	switch (characterSub)
	{
	default:
	case 0: state.subCharacter = SubCharacter::A; break;
	case 1: state.subCharacter = SubCharacter::B; break;
	case 2: state.subCharacter = SubCharacter::C; break;
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
		ReadProcessMemory(processHandle, (LPCVOID)(menuPtr + 0xB0), (LPVOID)&inSubMenu, 4, NULL);

		if (inSubMenu != 0)
		{
			unsigned int subMenuSelection = 0;
			ReadProcessMemory(processHandle, (LPCVOID)(menuPtr + 0x30), (LPVOID)&subMenuSelection, 4, NULL);

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
		case 2:
			menuState = 0;
			state.mainMenuState = MainMenuState::TitleScreen;
			state.gameState = GameState::MainMenu;
			break;
		case 13: // ending
			state.gameState = GameState::Ending;
			break;
		case 14: // staff roll
			state.gameState = GameState::StaffRoll;
			break;
		default:
			break;
		}
	}

	if (state.gameState == GameState::Playing)
	{
		// All non-(mid)boss states are below 6, e.g. 1 or 3 for conversations, 2 for post-boss
		if (gameState >= 24) // 24-31, 43-52 are boss only
		{
			state.stageState = StageState::Boss;
		}
		else if (gameState >= 6) // 6 and 15 are midboss only
		{
			// On all stages except 2 and 4, the gameState reverts to 0 after mid-boss disappears
			if (stage == 2 || stage == 4)
			{
				if (gameStateFrames < 900) // same for both
				{
					state.stageState = StageState::Midboss;
				}
			}
			else
			{
				state.stageState = StageState::Midboss;
			}
		}
	}

	// Read current game progress
	ReadProcessMemory(processHandle, (LPCVOID)LIVES, (LPVOID)&state.lives, 4, NULL);
	unsigned int integerPower = 0;
	ReadProcessMemory(processHandle, (LPCVOID)POWER, (LPVOID)&integerPower, 4, NULL);
	power = static_cast<float>(integerPower) / 20.0f;
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
		else
		{
			state.gameState = GameState::Playing_CustomResources; // display power instead of bombs
		}
	}
}

std::string Touhou10::getMidbossName() const
{
	switch (stage)
	{
	case 1: return "Shizuha Aki";
	case 2: return "Hina Kagiyama";
	case 3: return "Nitori Kawashiro";
	case 4: return "Momiji Inubashiri";
	case 5: return "Sanae Kochiya";
	// case 6: none
	case 7: return "Kanako Yasaka";
	default: return "";
	}
}

std::string Touhou10::getBossName() const
{
	switch (stage)
	{
	case 1: return "Minoriko Aki";
	case 2: return "Hina Kagiyama";
	case 3: return "Nitori Kawashiro";
	case 4: return "Aya Shameimaru";
	case 5: return "Sanae Kochiya";
	case 6: return "Kanako Yasaka";
	case 7: return "Suwako Moriya";
	default: return "";
	}
}

std::string const& Touhou10::getBGMName() const
{
	return th10_musicNames[bgm];
}

std::string Touhou10::getCustomResources() const
{
	std::string resources = std::to_string(state.lives);
	resources.append("/");
	std::string powerStr = std::to_string(power);
	powerStr.resize(4); // ez way to format to 2 d.p.
	resources.append(powerStr);

	return resources;
}

}