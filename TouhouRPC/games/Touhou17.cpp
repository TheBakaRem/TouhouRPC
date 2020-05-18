#include "Touhou17.h"

namespace Touhou17
{

Touhou17::Touhou17(PROCESSENTRY32W const& pe32) : TouhouMainGameBase(pe32)
{
}

Touhou17::~Touhou17()
{
}

void Touhou17::readDataFromGameProcess() {
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
			DWORD extraFlag = 0;
			ReadProcessMemory(processHandle, (LPCVOID)EXTRA_FLAG, (LPVOID)&extraFlag, 4, NULL);

			// could be normal game, extra, or stage practice, we can check some extra stuff in order to find out.
			if (extraFlag == 4)
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
		case 14: state.mainMenuState = MainMenuState::MusicRoom; break;
		case 3: state.mainMenuState = MainMenuState::Options; break;
		case 17: state.mainMenuState = MainMenuState::Manual; break;
		}

		menuState = 0;
		state.gameState = GameState::MainMenu;
	}


	if (state.gameState == GameState::Playing)
	{
		// Intentionak fallthroughs
		// Note that ZUN's naming for the BGM file names is not very consistent
		switch (bgm_id)
		{
		default:
		case 1:
			menuState = 0;
			state.mainMenuState = MainMenuState::TitleScreen;
			state.gameState = GameState::MainMenu;
			break;

		case 4: // stage 1 boss
			state.stageState = StageState::Boss;
		case 3: // stage 1
			stage = 1;
			break;

		case 6: // stage 2 boss
			state.stageState = StageState::Boss;
		case 5: // stage 2
			stage = 2;
			break;

		case 8: // stage 3 boss
			state.stageState = StageState::Boss;
		case 7:// stage 3
			stage = 3;
			break;

		case 10: // stage 4 boss
			state.stageState = StageState::Boss;
		case 9: // stage 4
			stage = 4;
			break;

		case 12: // stage 5 boss
			state.stageState = StageState::Boss;
		case 11: // stage 5
			stage = 5;
			break;

		case 13: // stage 6 boss
			state.stageState = StageState::Boss;
		case 14: // stage 6
			stage = 6;
			break;

		case 18: // extra stage boss
			state.stageState = StageState::Boss;
		case 17: // extra stage
			stage = 7;
			break;

		case 15: // ending
			state.gameState = GameState::Ending;
			break;
		case 16: // staff roll
			state.gameState = GameState::StaffRoll;
			break;
		}
	}

	if (state.stageState == StageState::Stage) {
		// Enemy state object
		// This object holds various information about general ecl state.
		// Offset 44h in this structure is used to indicate the main boss enemy
		// If it's 0 there is no main boss, 1 then there is, except stage 6 where it's 1 in the section leading up to Mayumi too, and cuts to 0 before she's even been beaten!
		// Offset 48h is some kind of enemy ID, which isn't consistent between runs except is always 0 when there's no boss, and non-zero when there is.
		// Unlike 44h this holds true for stage 6 too.
		// Neither of these differentiate between midbosses and bosses.
		DWORD enemy_state_ptr = 0;

		ReadProcessMemory(processHandle, (LPCVOID)ENEMY_STATE_POINTER, (LPVOID)&enemy_state_ptr, 4, NULL);
		if (enemy_state_ptr != 0)
		{
			DWORD enemyID = 0;
			ReadProcessMemory(processHandle, (LPCVOID)(enemy_state_ptr + 0x48), (LPVOID)&enemyID, 4, NULL);

			if (enemyID > 0)
			{
				// Since music only kicks in a little bit after the boss appears, until that happens we'll be showing the midboss by accident.
				// ... not that it's an issue except for stage 6 and EX, since otherwise all the bosses are also the midbosses.
				// We can check a stage state number that will confirm if the boss we're looking at is midboss or not.

				// Stage state.
				// 0 -> pre-midboss + midboss
				// 2 -> post-midboss (except stage 6 where it stays 0)
				// 41 -> pre-keiki appearance conversation
				// 43 -> boss
				// 81 -> post-boss
				DWORD stageState = 0;

				ReadProcessMemory(processHandle, (LPCVOID)STAGE_STATE, (LPVOID)&stageState, 4, NULL);
				if (stageState == 0)
				{
					state.stageState = StageState::Midboss;
				}
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
	case 2: state.character = Character::Youmu; break;
	}

	ReadProcessMemory(processHandle, (LPCVOID)SUB_CHARACTER, (LPVOID)&characterSub, 4, NULL);
	switch (characterSub)
	{
	default:
	case 0: state.subCharacter = SubCharacter::Wolf; break;
	case 1: state.subCharacter = SubCharacter::Otter; break;
	case 2: state.subCharacter = SubCharacter::Eagle; break;
	}

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

std::string Touhou17::getStageName() const
{
	if (stage <= 6)
	{
		std::string name = "Stage ";
		name.append(std::to_string(stage));
		return name;
	}
	else
	{
		return "Extra Stage";
	}
}

std::string Touhou17::getMidbossName() const
{
	switch (stage)
	{
	case 1: return "Eika Ebisu";
	case 2: return "Urumi Ushizaki";
	case 7:
	case 3: return "Kutaka Niwatari";
	case 4: return "Yachie Kicchou";
	case 5:
	case 6: return "Mayumi Joutouguu";
	}
}

std::string Touhou17::getBossName() const
{
	switch (stage)
	{
	case 1: return "Eika Ebisu";
	case 2: return "Urumi Ushizaki";
	case 3: return "Kutaka Niwatari";
	case 4: return "Yachie Kicchou";
	case 5: return "Mayumi Joutouguu";
	case 6: return "Keiki Haniyasushin";
	case 7: return "Saki Kurokoma";

	}
}

std::string const& Touhou17::getSpellCardName() const
{
	return th17_spellCardName[spellCardID];
}

std::string const& Touhou17::getBGMName() const
{
	return th17_musicNames[bgm];
}

}