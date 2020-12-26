#include "Touhou16.h"

namespace Touhou16
{

Touhou16::Touhou16(PROCESSENTRY32W const& pe32) : TouhouMainGameBase(pe32)
{
}

Touhou16::~Touhou16()
{
}

void Touhou16::readDataFromGameProcess() {
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

	unsigned int menu_pointer = 0;
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
			17 -> spell card practice
			11 -> replay
			10 -> player data
			13 -> music room
			 3 -> options
			16 -> all manual screens

			---- sub sub menus ----
			 6 -> char select
			 7 -> subchar select
			 8 -> practice stage select
			18 -> spell card select, N == num spells for stage
			19 -> spell card difficulty select
			20 -> spell card subtype select
			11 -> replay stage select
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
		case 17:
		case 18:
		case 19:
		case 20: state.mainMenuState = MainMenuState::SpellPractice; break;
		case 11: state.mainMenuState = MainMenuState::Replays; break;
		case 10: state.mainMenuState = MainMenuState::PlayerData; break;
		case 13: state.mainMenuState = MainMenuState::MusicRoom; break;
		case 3: state.mainMenuState = MainMenuState::Options; break;
		case 16: state.mainMenuState = MainMenuState::Manual; break;
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

		case 3: // stage 1 boss
			state.stageState = StageState::Boss;
		case 2: // stage 1
			stage = 1;
			break;

		case 5: // stage 2 boss
			state.stageState = StageState::Boss;
		case 4: // stage 2
			stage = 2;
			break;

		case 7: // stage 3 boss
			state.stageState = StageState::Boss;
		case 6:// stage 3
			stage = 3;
			break;

		case 9: // stage 4 boss
			state.stageState = StageState::Boss;
		case 8: // stage 4
			stage = 4;
			break;

		case 11: // stage 5 boss
			state.stageState = StageState::Boss;
		case 10: // stage 5
			stage = 5;
			break;

		case 12: // stage 6 boss
			state.stageState = StageState::Boss;
		case 13: // stage 6
			stage = 6;
			break;

		case 17: // extra stage boss
			state.stageState = StageState::Boss;
		case 16: // extra stage
			stage = 7;
			break;

		case 14: // ending
			state.gameState = GameState::Ending;
			break;
		case 15: // staff roll
			state.gameState = GameState::StaffRoll;
			break;
		}
	}
	
	// Enemy state object
	// This object holds various information about general ecl state.
	// Offset 1BC8h in this structure is used to indicate the main boss enemy
	// If it's 0 there is no main boss, if it's 4 it's a mid-boss or a boss

	// We know the main stage boss is triggered when music changes
	// So if the state isn't already defined, we check if the mid-boss is present
	if (state.stageState == StageState::Stage) {
		DWORD enemy_state_ptr = 0;
		ReadProcessMemory(processHandle, (LPCVOID)ENEMY_STATE_POINTER, (LPVOID)&enemy_state_ptr, 4, NULL);
		if (enemy_state_ptr != 0) {
			DWORD boss_mode = 0;
			ReadProcessMemory(processHandle, (LPCVOID)(enemy_state_ptr + 0x1BC8), (LPVOID)&boss_mode, 4, NULL);
			if (boss_mode == 4) {
				state.stageState = StageState::Midboss;
			}
		}
	}
	
	// Read character and difficulty info
	ReadProcessMemory(processHandle, (LPCVOID)CHARACTER, (LPVOID)&character, 4, NULL);
	switch (character)
	{
	default:
	case 0: state.character = Character::Reimu; break;
	case 1: state.character = Character::Cirno; break;
	case 2: state.character = Character::Aya; break;
	case 3: state.character = Character::Marisa; break;
	}

	ReadProcessMemory(processHandle, (LPCVOID)SUB_CHARACTER, (LPVOID)&characterSub, 4, NULL);
	switch (characterSub)
	{
	default:
	case 0: state.subCharacter = SubCharacter::Spring; break;
	case 1: state.subCharacter = SubCharacter::Summer; break;
	case 2: state.subCharacter = SubCharacter::Fall; break;
	case 3: state.subCharacter = SubCharacter::Winter; break;
	}

	// Read current game progress
	ReadProcessMemory(processHandle, (LPCVOID)LIVES, (LPVOID)&state.lives, 4, NULL);
	ReadProcessMemory(processHandle, (LPCVOID)BOMBS, (LPVOID)&state.bombs, 4, NULL);
	ReadProcessMemory(processHandle, (LPCVOID)SCORE, (LPVOID)&state.score, 4, NULL);
	ReadProcessMemory(processHandle, (LPCVOID)GAMEOVERS, (LPVOID)&state.gameOvers, 4, NULL);

	// Read Spell Card ID (for Spell Practice)
	ReadProcessMemory(processHandle, (LPCVOID)SPELL_CARD_ID, (LPVOID)&spellCardID, 4, NULL);

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

std::string Touhou16::getMidbossName() const
{
	switch (stage)
	{
	case 1: return "Eternity Larva";
	case 2: return "Nemuno Sakata";
	case 3: return "Lily White";
	case 4: return "Mai Teireida";
	case 5: return "Satono Nishida";
	// case 6: none
	case 7: return "Mai Teireida & Satono Nishida";
	default: return "";
	}
}

std::string Touhou16::getBossName() const
{
	switch (stage)
	{
	case 1: return "Eternity Larva";
	case 2: return "Nemuno Sakata";
	case 3: return "Aunn Komano";
	case 4: return "Narumi Yatadera";
	case 5: return "Mai Teireida & Satono Nishida";
	case 6:
	case 7: return "Okina Matara";
	default: return "";
	}
}

std::string const& Touhou16::getSpellCardName() const
{
	return th16_spellCardName[spellCardID];
}

std::string const& Touhou16::getBGMName() const
{
	return th16_musicNames[bgm];
}

}