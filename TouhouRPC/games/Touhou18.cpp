#include "Touhou18.h"

namespace Touhou18
{

Touhou18Trial::Touhou18Trial(PROCESSENTRY32W const& pe32) : TouhouMainGameBase(pe32)
{
}

void Touhou18Trial::readDataFromGameProcess() {
	menuState = -1;
	state.gameState = GameState::Playing;
	state.stageState = StageState::Stage;
	gameMode = GAME_MODE_STANDARD;

	// The BGM playing will be used to determine a lot of things
	char bgm_playing[20];
	ReadProcessMemory(processHandle, (LPCVOID)BGM_STR, bgm_playing, 20, NULL);

	// Check if the game over music is playing.
	if (std::strncmp(bgm_playing, "th128_08.wav", std::strlen("th128_08.wav")) == 0)
	{
		state.gameState = GameState::GameOver;
	}

	// Read stage value
	ReadProcessMemory(processHandle, (LPCVOID)STAGE, (LPVOID)&stage, 4, NULL);

	// Convert the part after the _ and before the . to int
	// That way it is possible to switch case the BGM playing
	bool prefixBGM = bgm_playing[0] == 'b';
	char bgm_id_str[3];
	bgm_id_str[0] = bgm_playing[prefixBGM ? 9 : 5];
	bgm_id_str[1] = bgm_playing[prefixBGM ? 10 : 6];
	bgm_id_str[2] = '\x00';
	int bgm_id = atoi(bgm_id_str);

	bgm = bgm_id;

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

	DWORD menu_pointer = 0;
	ReadProcessMemory(processHandle, (LPCVOID)MENU_POINTER, (LPVOID)&menu_pointer, 4, NULL);
	if (state.gameState == GameState::Playing && menu_pointer != 0)
	{
		// The most reliable way of determining our current menu state is through the combination of
		// menu display state and extra flags that get set.
		// This is because of a bug detailed in Touhou14's source file

		/*
			display state (0x18) -> menu screen
			-----------------------------
			 0 -> loading
			 1 -> main menu
			 5 -> game start
			 5 -> extra start
			 5 -> practice start
			18 -> spell card practice
			12 -> replay
			10 -> player data
			14 -> music room
			 3 -> options
			17 -> all manual screens

			---- sub sub menus ----
			 6 -> char select
			 7 -> subchar select
			 8 -> practice stage select
			19 -> spell card select, N == num spells for stage
			20 -> spell card difficulty select
			12 -> replay stage select
			11 -> player records, N == 9 on shot type, 3 on spell cards
			23 -> achievements
			24 -> ability cards
		*/

		DWORD ds = 0;
		ReadProcessMemory(processHandle, (LPCVOID)(menu_pointer + 0x18), (LPVOID)&ds, 4, NULL);

		switch (ds)
		{
		default: state.mainMenuState = MainMenuState::TitleScreen; break;
		case 5:
		case 6:
		case 7: state.mainMenuState = MainMenuState::GameStart; break;
		case 8: state.mainMenuState = MainMenuState::StagePractice; break;
		case 18:
		case 19:
		case 20: state.mainMenuState = MainMenuState::SpellPractice; break;
		case 12: state.mainMenuState = MainMenuState::Replays; break;
		case 10:
		case 11: state.mainMenuState = MainMenuState::PlayerData; break;
		case 23: state.mainMenuState = MainMenuState::Achievements; break;
		case 24: state.mainMenuState = MainMenuState::AbilityCards; break;
		case 14: state.mainMenuState = MainMenuState::MusicRoom; break;
		case 3: state.mainMenuState = MainMenuState::Options; break;
		case 17: state.mainMenuState = MainMenuState::Manual; break;
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
		case 1:
			menuState = 0;
			state.mainMenuState = MainMenuState::TitleScreen;
			state.gameState = GameState::MainMenu;
			break;
		default:
			break;
		}
	}
	
	if (state.gameState != GameState::Playing)
	{
		// if we're not playing, reset seenMidboss.
		seenMidboss = false;
	}

	if (state.stageState == StageState::Stage)
	{
		// We can check a stage state number that will confirm if we're in the stage or in the boss

		// Stage state.
		// 0 -> pre-midboss + midboss
		// 2 -> post-midboss
		// 41 -> pre-fight appearance conversations
		// 43 -> boss
		// 81 -> post-boss
		DWORD stageState = 0;

		ReadProcessMemory(processHandle, (LPCVOID)STAGE_STATE, (LPVOID)&stageState, 4, NULL);
		if (stageState == 0)
		{
			if (seenMidboss)
			{
				state.stageState = StageState::Midboss;
			}
			else
			{
				// If we're in stage state 0, we might be facing a midboss. We can check this to find out:

				// Enemy state object
				// This object holds various information about general ecl state.
				// Offset 210 is some kind of 'boss attack active' flag, so it briefly flicks to 0 between attacks

				// Since it flickers, we can remember that we saw it and just assume we're in a midboss until the stage state or game state changes.
				DWORD enemy_state_ptr = 0;

				ReadProcessMemory(processHandle, (LPCVOID)ENEMY_STATE_POINTER, (LPVOID)&enemy_state_ptr, 4, NULL);
				if (enemy_state_ptr != 0)
				{
					DWORD enemyID = 0;
					ReadProcessMemory(processHandle, (LPCVOID)(enemy_state_ptr + 0x210), (LPVOID)&enemyID, 4, NULL);

					if (enemyID > 0)
					{
						state.stageState = StageState::Midboss;
						seenMidboss = true;
					}
				}
			}
		}
		else
		{
			// reset once we've finished fighting the midboss.
			seenMidboss = false;
			if (stageState == 43)
			{
				state.stageState = StageState::Boss;
			}
		}

	}

	ReadProcessMemory(processHandle, (LPCVOID)CHARACTER, (LPVOID)&character, 4, NULL);
	switch (character)
	{
	default:
	case 0: state.character = Character::Reimu; break;
	case 1: state.character = Character::Marisa; break;
	case 2: state.character = Character::Sakuya; break;
	case 3: state.character = Character::Sanae; break;
	}

	// Read current game progress
	ReadProcessMemory(processHandle, (LPCVOID)LIVES, (LPVOID)&state.lives, 4, NULL);
	ReadProcessMemory(processHandle, (LPCVOID)BOMBS, (LPVOID)&state.bombs, 4, NULL);
	ReadProcessMemory(processHandle, (LPCVOID)SCORE, (LPVOID)&state.score, 4, NULL);
	ReadProcessMemory(processHandle, (LPCVOID)GAMEOVERS, (LPVOID)&state.gameOvers, 4, NULL);

	// Read game mode
	ReadProcessMemory(processHandle, (LPCVOID)GAME_MODE, (LPVOID)&gameMode, 4, NULL);
	switch (gameMode)
	{
	case GAME_MODE_STANDARD: /* could be main menu or playing, no need to overwrite anything */ break;
	case GAME_MODE_REPLAY: state.gameState = GameState::WatchingReplay; break;
	case GAME_MODE_CLEAR: state.gameState = GameState::StaffRoll; break;
	case GAME_MODE_PRACTICE: state.gameState = GameState::StagePractice; break;
	case GAME_MODE_SPELLPRACTICE: state.gameState = GameState::SpellPractice; break;
	}
}

std::string Touhou18Trial::getMidbossName() const
{
	switch (stage)
	{
	case 1: return "Mike Goutokuji";
	case 2: return "Takane Yamashiro";
	case 3: return "Sannyo Komakusa";
	default: return "";
	}
}

std::string Touhou18Trial::getBossName() const
{
	switch (stage)
	{
	case 1: return "Mike Goutokuji";
	case 2: return "Takane Yamashiro";
	case 3: return "Sannyo Komakusa";
	default: return "";
	}
}

std::string const& Touhou18Trial::getBGMName() const
{
	return th18_musicNames[bgm];
}


Touhou18::Touhou18(PROCESSENTRY32W const& pe32) : TouhouMainGameBase(pe32)
{}

void Touhou18::readDataFromGameProcess()
{
	menuState = -1;
	state.gameState = GameState::Playing;
	state.stageState = StageState::Stage;
	gameMode = GAME_MODE_STANDARD;

	// The BGM playing will be used to determine a lot of things
	char bgm_playing[20];
	ReadProcessMemory(processHandle, (LPCVOID)BGM_STR, bgm_playing, 20, NULL);

	// Check if the game over music is playing.
	if (std::strncmp(bgm_playing, "th128_08.wav", std::strlen("th128_08.wav")) == 0)
	{
		state.gameState = GameState::GameOver;
	}

	// Read stage value
	ReadProcessMemory(processHandle, (LPCVOID)STAGE, (LPVOID)&stage, 4, NULL);

	// Convert the part after the _ and before the . to int
	// That way it is possible to switch case the BGM playing
	bool prefixBGM = bgm_playing[0] == 'b';
	char bgm_id_str[3];
	bgm_id_str[0] = bgm_playing[prefixBGM ? 9 : 5];
	bgm_id_str[1] = bgm_playing[prefixBGM ? 10 : 6];
	bgm_id_str[2] = '\x00';
	int bgm_id = atoi(bgm_id_str);

	bgm = bgm_id;

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

	DWORD menu_pointer = 0;
	ReadProcessMemory(processHandle, (LPCVOID)MENU_POINTER, (LPVOID)&menu_pointer, 4, NULL);
	if (state.gameState == GameState::Playing && menu_pointer != 0)
	{
		// The most reliable way of determining our current menu state is through the combination of
		// menu display state and extra flags that get set.
		// This is because of a bug detailed in Touhou14's source file

		/*
			display state (0x18) -> menu screen
			-----------------------------
			 0 -> loading
			 1 -> main menu
			 5 -> game start
			 5 -> extra start
			 5 -> practice start
			18 -> spell card practice
			12 -> replay
			10 -> player data
			14 -> music room
			 3 -> options
			17 -> all manual screens

			---- sub sub menus ----
			 6 -> char select
			 7 -> subchar select
			 8 -> practice stage select
			19 -> spell card select, N == num spells for stage
			20 -> spell card difficulty select
			12 -> replay stage select
			11 -> player records, N == 9 on shot type, 3 on spell cards
			23 -> achievements
			24 -> ability cards
		*/

		DWORD ds = 0;
		ReadProcessMemory(processHandle, (LPCVOID)(menu_pointer + 0x18), (LPVOID)&ds, 4, NULL);

		switch (ds)
		{
		default: state.mainMenuState = MainMenuState::TitleScreen; break;
		case 5:
		case 6:
		case 7:
		{
			// could be normal game, extra, or stage practice, we can check some extra stuff in order to find out.
			if (difficulty == 4)
			{
				state.mainMenuState = MainMenuState::ExtraStart;
			}
			else
			{
				DWORD practiceFlag = 0;
				ReadProcessMemory(processHandle, (LPCVOID)PRACTICE_SELECT_FLAG, (LPVOID)&practiceFlag, 4, NULL);
				state.mainMenuState = (practiceFlag != 0) ? MainMenuState::StagePractice : MainMenuState::GameStart;
			}
			break;
		}
		case 8: state.mainMenuState = MainMenuState::StagePractice; break;
		case 18:
		case 19:
		case 20: state.mainMenuState = MainMenuState::SpellPractice; break;
		case 12: state.mainMenuState = MainMenuState::Replays; break;
		case 10:
		case 11: state.mainMenuState = MainMenuState::PlayerData; break;
		case 23: state.mainMenuState = MainMenuState::Achievements; break;
		case 24: state.mainMenuState = MainMenuState::AbilityCards; break;
		case 14: state.mainMenuState = MainMenuState::MusicRoom; break;
		case 3: state.mainMenuState = MainMenuState::Options; break;
		case 17: state.mainMenuState = MainMenuState::Manual; break;
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
		case 1:
			menuState = 0;
			state.mainMenuState = MainMenuState::TitleScreen;
			state.gameState = GameState::MainMenu;
			break;
		case 15: // ending
			state.gameState = GameState::Ending;
			break;
		case 16: // staff roll
			state.gameState = GameState::StaffRoll;
			break;
		default:
			break;
		}
	}

	if (state.gameState != GameState::Playing)
	{
		// if we're not playing, reset seenMidboss.
		seenMidboss = false;
	}

	if (state.stageState == StageState::Stage)
	{
		// We can check a stage state number that will confirm if we're in the stage or in the boss

		// Stage state.
		// 0 -> pre-midboss + midboss
		// 2 -> post-midboss
		// 41 -> pre-fight appearance conversations
		// 43 -> boss
		// 81 -> post-boss
		DWORD stageState = 0;

		ReadProcessMemory(processHandle, (LPCVOID)STAGE_STATE, (LPVOID)&stageState, 4, NULL);
		if (stageState == 0)
		{
			if (seenMidboss)
			{
				state.stageState = StageState::Midboss;
			}
			else
			{
				// If we're in stage state 0, we might be facing a midboss. We can check this to find out:

				// Enemy state object
				// This object holds various information about general ecl state.
				// Offset 210 is some kind of 'boss attack active' flag, so it briefly flicks to 0 between attacks

				// Since it flickers, we can remember that we saw it and just assume we're in a midboss until the stage state or game state changes.
				DWORD enemy_state_ptr = 0;

				ReadProcessMemory(processHandle, (LPCVOID)ENEMY_STATE_POINTER, (LPVOID)&enemy_state_ptr, 4, NULL);
				if (enemy_state_ptr != 0)
				{
					DWORD enemyID = 0;
					ReadProcessMemory(processHandle, (LPCVOID)(enemy_state_ptr + 0x210), (LPVOID)&enemyID, 4, NULL);

					if (enemyID > 0)
					{
						state.stageState = StageState::Midboss;
						seenMidboss = true;
					}
				}
			}
		}
		else
		{
			// reset once we've finished fighting the midboss.
			seenMidboss = false;
			if (stageState == 43)
			{
				state.stageState = StageState::Boss;
			}
		}

	}

	// Read Spell Card ID (for Spell Practice)
	ReadProcessMemory(processHandle, (LPCVOID)SPELL_CARD_ID, (LPVOID)&spellCardID, 4, NULL);

	ReadProcessMemory(processHandle, (LPCVOID)CHARACTER, (LPVOID)&character, 4, NULL);
	switch (character)
	{
	default:
	case 0: state.character = Character::Reimu; break;
	case 1: state.character = Character::Marisa; break;
	case 2: state.character = Character::Sakuya; break;
	case 3: state.character = Character::Sanae; break;
	}

	// Read current game progress
	ReadProcessMemory(processHandle, (LPCVOID)LIVES, (LPVOID)&state.lives, 4, NULL);
	ReadProcessMemory(processHandle, (LPCVOID)BOMBS, (LPVOID)&state.bombs, 4, NULL);
	ReadProcessMemory(processHandle, (LPCVOID)SCORE, (LPVOID)&state.score, 4, NULL);
	ReadProcessMemory(processHandle, (LPCVOID)MONEY, (LPVOID)&money, 4, NULL);
	ReadProcessMemory(processHandle, (LPCVOID)GAMEOVERS, (LPVOID)&state.gameOvers, 4, NULL);

	// Read game mode
	ReadProcessMemory(processHandle, (LPCVOID)GAME_MODE, (LPVOID)&gameMode, 4, NULL);
	switch (gameMode)
	{
	case GAME_MODE_STANDARD: /* could be main menu or playing, no need to overwrite anything */ break;
	case GAME_MODE_REPLAY: state.gameState = GameState::WatchingReplay; break;
	case GAME_MODE_CLEAR: state.gameState = GameState::StaffRoll; break;
	case GAME_MODE_PRACTICE: state.gameState = GameState::StagePractice; break;
	case GAME_MODE_SPELLPRACTICE: state.gameState = GameState::SpellPractice; break;
	}

	if (state.gameState == GameState::Playing)
	{
		state.gameState = GameState::Playing_CustomResources;
	}
}

std::string Touhou18::getMidbossName() const
{
	switch (stage)
	{
	case 1: return "Mike Goutokuji";
	case 2: return "Takane Yamashiro";
	case 3: return "Sannyo Komakusa";
	case 4: return "Giant Yin-Yang";
	case 5:
	case 6:
	case 7: return "Tsukasa Kudamaki";
	default: return "";
	}
}

std::string Touhou18::getBossName() const
{
	switch (stage)
	{
	case 1: return "Mike Goutokuji";
	case 2: return "Takane Yamashiro";
	case 3: return "Sannyo Komakusa";
	case 4: return "Misumaru Tamatsukuri";
	case 5: return "Megumu Iizunamaru";
	case 6: return "Chimata Tenkyuu";
	case 7: return "Momoyo Himemushi";
	default: return "";
	}
}

std::string const& Touhou18::getSpellCardName() const
{
	return th18_spellCardName[spellCardID];
}

std::string const& Touhou18::getBGMName() const
{
	return th18_musicNames[bgm];
}

std::string Touhou18::getCustomResources() const
{
	std::string resources = std::to_string(state.lives);
	resources.append("/");
	resources.append(std::to_string(state.bombs));
	resources.append("/¥");
	resources.append(std::to_string(money));

	return resources;
}

}