#include "Touhou11.h"

namespace Touhou11
{

Touhou11::Touhou11(PROCESSENTRY32W const& pe32) : TouhouMainGameBase(pe32)
{
}

Touhou11::~Touhou11()
{
}

void Touhou11::readDataFromGameProcess()
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
	case 0: state.subCharacter = (state.character == Character::Reimu) ? SubCharacter::AndYukari : SubCharacter::AndAlice; break;
	case 1: state.subCharacter = (state.character == Character::Reimu) ? SubCharacter::AndSuika : SubCharacter::AndPatchouli; break;
	case 2: state.subCharacter = (state.character == Character::Reimu) ? SubCharacter::AndAya : SubCharacter::AndNitori; break;
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
		case 0:
			menuState = 0;
			state.mainMenuState = MainMenuState::TitleScreen;
			state.gameState = GameState::MainMenu;
			break;
		case 18: // ending
			state.gameState = GameState::Ending;
			break;
		case 19: // staff roll
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
		ReadProcessMemory(processHandle, (LPCVOID)(enemyStatePtr + 0x131C), &fightingBoss, 4, NULL);

		if (fightingBoss == 3)
		{
			// Stage 1: Kisume
			if (stage == 1 && (gameState >= 6 && gameState <= 8) || gameState == 15)
			{
				state.stageState = StageState::Midboss;
			}
			// Stage 1: Yamame
			else if (stage == 1 && (gameState == 2 || gameState >= 24))
			{
				state.stageState = StageState::Boss;
			}

			// Stage 2: Parsee (mid-boss)
			else if (stage == 2 && (gameState == 6 || (gameState == 43 && gameStateFrames <= 2040)))
			{
				state.stageState = StageState::Midboss;
			}
			// Stage 2: Parsee (boss)
			else if (stage == 2 && (gameState == 2 || gameState >= 24))
			{
				state.stageState = StageState::Boss;
			}

			// Stage 3: Yuugi (mid-boss)
			else if (stage == 3 && (gameState == 6 || gameState == 43 || gameState == 4))
			{
				state.stageState = StageState::Midboss;
			}
			// Stage 3: Yuugi (boss)
			else if (stage == 3 && (gameState == 2 || gameState >= 24))
			{
				state.stageState = StageState::Boss;
			}

			// Stage 4: Orin (start)
			else if (stage == 4 && (gameState == 6 && gameStateFrames <= 900))
			{
				state.stageState = StageState::Midboss;
			}
			// Stage 4: Orin (mid-boss)
			else if (stage == 4 && (gameState == 7 && gameStateFrames <= 1590))
			{
				state.stageState = StageState::Midboss;
			}
			// Stage 4: Satori (boss)
			else if (stage == 4 && (gameState == 2 || gameState >= 24))
			{
				state.stageState = StageState::Boss;
			}

			// Stage 5: Orin (mid-boss)
			else if (stage == 5 && (gameState == 6 || (gameState == 7 && gameStateFrames <= 1200) || gameState == 15))
			{
				state.stageState = StageState::Midboss;
			}
			// Stage 5: Orin (boss)
			else if (stage == 5 && (gameState == 2 || gameState >= 24))
			{
				state.stageState = StageState::Boss;
			}

			// Stage 6: Orin (mid-boss)
			else if (stage == 6 && (gameState == 6 || (gameState == 15 && gameStateFrames <= 2220)))
			{
				state.stageState = StageState::Midboss;
			}
			// Stage 6: Utsuho (boss)
			else if (stage == 6 && gameState >= 24)
			{
				state.stageState = StageState::Boss;
			}

			// Stage 7 (Extra): Sanae
			else if (stage == 7 && (gameState == 6 || (gameState == 0 && (stageFrames > gameStateFrames) && gameStateFrames <= 11200)))
			{
				state.stageState = StageState::Midboss;
			}
			// Stage 7 (Extra): Koishi
			else if (stage == 7 && (gameState == 2 || gameState >= 24))
			{
				state.stageState = StageState::Boss;
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

std::string Touhou11::getMidbossName() const
{
	switch (stage)
	{
	case 1: return "Kisume";
	case 2: return "Parsee Mizuhashi";
	case 3: return "Yuugi Hoshiguma";
	case 4:
	case 5:
	case 6: return "Rin Kaenbyou";
	case 7: return "Sanae Kochiya";
	default: return "";
	}
}

std::string Touhou11::getBossName() const
{
	switch (stage)
	{
	case 1: return "Yamame Kurodani";
	case 2: return "Parsee Mizuhashi";
	case 3: return "Yuugi Hoshiguma";
	case 4: return "Satori Komeiji";
	case 5: return "Rin Kaenbyou";
	case 6: return "Utsuho Reiuji";
	case 7: return "Koishi Komeiji";
	default: return "";
	}
}

std::string const& Touhou11::getBGMName() const
{
	return th11_musicNames[bgm];
}

std::string Touhou11::getCustomResources() const
{
	std::string resources = std::to_string(state.lives);
	resources.append("/");
	std::string powerStr = std::to_string(power);
	powerStr.resize(4); // ez way to format to 2 d.p.
	resources.append(powerStr);

	return resources;
}

}