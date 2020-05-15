#include "Touhou14.h"
#include <iostream>

namespace Touhou14
{
Touhou14::Touhou14(PROCESSENTRY32W* pe32)
	: TouhouBase(pe32)
{
}

Touhou14::~Touhou14()
{}

void Touhou14::readDataFromGameProcess()
{
	// Reset menuState, bgm will tell us if we're in the menu
	menuState = -1;
	gameState = STATE_STAGE;
	gameMode = GAME_MODE_STANDARD;

	// Update game mode
	ReadProcessMemory(processHandle, (LPCVOID)GAME_MODE, (LPVOID)&gameMode, 4, NULL);

	// The BGM playing will be used to determine a lot of things
	char bgm_playing[20];
	ReadProcessMemory(processHandle, (LPCVOID)BGM_STR, bgm_playing, 20, NULL);

	// Check if the game over music is playing.
	if (std::strncmp(bgm_playing, "th128_08.wav", std::strlen("th128_08.wav")) == 0)
	{
		gameState = STATE_GAMEOVER;
		return;
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

	// Update menu state. The object at menu_pointer contains many things to do with the main menu,
	// and they kind of behave awkwardly in DDC. For now we mainly care about whether we're in the music room or not.

	unsigned int menu_pointer = 0;
	ReadProcessMemory(processHandle, (LPCVOID)MENU_POINTER, (LPVOID)&menu_pointer, 4, NULL);
	if (menu_pointer != 0)
	{
		// Are you ready for the most bizarre bug in DDC?
		// The menu object stores a stack of the selected sub menus. The base is meant to be at offset 0x34 inside the menu object.
		// i.e. any sub menu selected from the top level of the title screen will fill in to this base number.
		// So, you might naively think, simply reading from this base should be sufficient to know which sub menu we're in?
		// Haha.
		// At offset 0xB4 is stored the number of sub menus recorded in the stack, and this is also used to determine where to insert
		// so when the game increases this when you enter the Manual section BUT DOESN'T DECREASE IT WHEN YOU EXIT
		// WE KEEP ON MOVING THE SUB MENU LOCATION 4 BYTES FORWARD
		// Therefore, we have to go and search for it, and we can't just trust that 0 offset == title
		DWORD subMenuStackOffset = 0;
		ReadProcessMemory(processHandle, (LPCVOID)(menu_pointer + 0xB4), (LPVOID)&subMenuStackOffset, 4, NULL);

		const unsigned int sub_menu_pointer = menu_pointer + 0x34;

		if (subMenuStackOffset == 0)
		{
			menuState = 0;
		}
		else
		{
			// first, read the highest level.
			int offset = subMenuStackOffset - 1;
			ReadProcessMemory(processHandle, (LPCVOID)(sub_menu_pointer + (offset * 4)), (LPVOID)&mainMenuSub, 4, NULL);

			// If it is 8 (manual) then we must be on the manual or the title screen.
			// If not, we're on a sub menu, but not yet sure which. We need to check firstly for the lowest non-8 number
			if (mainMenuSub != 8)
			{
				for (; offset >= 0; offset--)
				{
					int subMenu = 0;
					ReadProcessMemory(processHandle, (LPCVOID)(sub_menu_pointer + (offset * 4)), (LPVOID)&subMenu, 4, NULL);
					if (subMenu == 8)
					{
						break;
					}
					else
					{
						mainMenuSub = subMenu;
					}
				}

				// Now, mainMenuSub should be lowest non-8 value, and we know for sure we're on this sub menu.
				menuState = mainMenuSub + 1;
			}
			else
			{
				// How do we tell if we're on the main menu if we got 8 back?
				// We can check offset 0x1C, which stores some state to do with the main menu, alongside offset 0x30.
				// These values are pretty arbitrary and are dependent on how deep into the sub menu you are, but
				// on the main menu, at 0x1C we have 1, and in the manual we have 16.
				ReadProcessMemory(processHandle, (LPCVOID)(menu_pointer + 0x1C), (LPVOID)&mainMenuDisplayStateA, 4, NULL);
				if (mainMenuDisplayStateA == 1)
				{
					menuState = 0;
				}
				else
				{
					menuState = 9;
				}
			}
		}


		// Changes 2 places, tracks where cursor is. Not sure why there's 2.
		// ReadProcessMemory(processHandle, (LPCVOID)(menu_pointer + 0x28), (LPVOID)&mainMenuSelectionA, 4, NULL);
		// ReadProcessMemory(processHandle, (LPCVOID)(menu_pointer + 0x2C), (LPVOID)&mainMenuSelectionB, 4, NULL);

		/*
			10 -> main menu /1
			4 -> game start /5
			1 -> extra start /5
			4 -> practice start (has 0x10 flag set at 0x004F58B8) /5
			7 -> spell card practice (has 0x20 flag set at 0x004F58B8) /17
			25 -> replay /11
			6 -> player data /10
			17 -> music room /13
			5 -> options /3
			10 -> manual (presumably flags set?) /16
			3 -> char select /6
			2 -> type select /7
			6 -> practice stage select /8
			3 -> spell card select /18
			5 -> spell card difficulty select /19
			7 -> replay stage select /11

			right = display state A
			left = display state B
		*/

		// Since the sub state isn't reliable? There's reliable display states but the numbers are arbitrary.
		ReadProcessMemory(processHandle, (LPCVOID)(menu_pointer + 0x1C), (LPVOID)&mainMenuDisplayStateA, 4, NULL);
		ReadProcessMemory(processHandle, (LPCVOID)(menu_pointer + 0x30), (LPVOID)&mainMenuDisplayStateB, 4, NULL);

		gameState = STATE_MENU;
		return;
	}

	// Intentionak fallthroughs
	// Note that ZUN's naming for the BGM file names is not very consistent
	switch (bgm_id)
	{
	default:
	case 1:
		menuState = 0;
		gameState = STATE_MENU;
		return;

	case 3: // stage 1 boss
		gameState = STATE_BOSS;
	case 2: // stage 1
		stage = 1;
		break;

	case 5: // stage 2 boss
		gameState = STATE_BOSS;
	case 4: // stage 2
		stage = 2;
		break;

	case 7: // stage 3 boss
		gameState = STATE_BOSS;
	case 6:// stage 3
		stage = 3;
		break;

	case 10: // stage 4 boss
		gameState = STATE_BOSS;
	case 9: // stage 4
		stage = 4;
		break;

	case 12: // stage 5 boss
		gameState = STATE_BOSS;
	case 11: // stage 5
		stage = 5;
		break;

	case 15: // stage 6 boss
		gameState = STATE_BOSS;
	case 14: // stage 6
		stage = 6;
		break;

	case 19: // extra stage boss
		gameState = STATE_BOSS;
	case 18: // extra stage
		stage = 7;
		break;

	case 16: // ending
		gameState = STATE_ENDING;
		break;
	case 17: // staff roll
		gameState = STATE_CREDITS;
		break;
	}

	// We know the main stage boss is triggered when music changes
	// So if the state isn't already defined, we check if the mid-boss is present
	if (gameState == STATE_STAGE)
	{
		DWORD boss_mode = 0;
		ReadProcessMemory(processHandle, (LPCVOID)(ENEMY_STATE), (LPVOID)&boss_mode, 4, NULL);
		// If it's 0 there is no main boss, if it's 4 it's a mid-boss, 6 is boss, 7 is post-boss
		if (boss_mode == 4)
		{
			gameState = STATE_MIDBOSS;
		}
	}

	// Read character and difficulty info
	ReadProcessMemory(processHandle, (LPCVOID)CHARACTER, (LPVOID)&character, 4, NULL);
	ReadProcessMemory(processHandle, (LPCVOID)SUB_CHARACTER, (LPVOID)&characterSub, 4, NULL);
	ReadProcessMemory(processHandle, (LPCVOID)DIFFICULTY, (LPVOID)&difficulty, 4, NULL);

	// Read Spell Card ID (for Spell Practice)
	ReadProcessMemory(processHandle, (LPCVOID)SPELL_CARD_ID, (LPVOID)&spellCardID, 4, NULL);

	// Read current game progress
	ReadProcessMemory(processHandle, (LPCVOID)LIVES, (LPVOID)&lives, 4, NULL);
	ReadProcessMemory(processHandle, (LPCVOID)BOMBS, (LPVOID)&bombs, 4, NULL);
	ReadProcessMemory(processHandle, (LPCVOID)SCORE, (LPVOID)&score, 4, NULL);
	ReadProcessMemory(processHandle, (LPCVOID)GAMEOVERS, (LPVOID)&gameOvers, 4, NULL);
}

std::string formatScore(int score, int gameOvers)
{
	std::string scoreString = std::to_string((score * 10) + gameOvers);
	size_t insertPosition = scoreString.length() - 3;
	while (insertPosition > 0)
	{
		scoreString.insert(insertPosition, ",");
		insertPosition -= 3;
	}
	return scoreString;
}

void Touhou14::setGameName(std::string& name)
{
	name = "Touhou 14 - Double Dealing Character";
}

void Touhou14::setGameInfo(std::string& info)
{
	// Menu
	if (menuState != -1 || stage == 0)
	{
		switch (menuState)
		{
		case 0: info = "On the title screen"; break;
		case 1: info = "Preparing to play"; break;
		case 2: info = "Preparing to play Extra"; break;
		case 3: info = "Selecting a stage to practice"; break;
		case 4: info = "Selecting a spell to practice"; break;
		case 5: info = "Selecting a replay"; break;
		case 6: info = "Viewing player data"; break;
		case 7: info = "In the music room:\n"; info.append(th14_musicNames[bgm]); break;
		case 8: info = "Changing options"; break;
		case 9: info = "Viewing the manual"; break;
		}
		return;
	}

	// Game over
	if (gameState == STATE_GAMEOVER)
	{
		info = ("Game over");
		return;
	}

	// Ending
	if (gameState == STATE_ENDING || gameState == STATE_CREDITS)
	{
		info.assign("Cleared with ");
		info.append(formatScore(score, gameOvers));
		return;
	}

	// Spell practice
	if (spellCardID != -1)
	{
		// spellcard practice
		info = ("Practicing a spell:\n");
	}

	// Spell practice
	if (spellCardID != -1)
	{
		// spellcard practice
		info.append(th14_spellCardName[spellCardID]);
		return;
	}

	switch (gameMode)
	{
	case GAME_MODE_REPLAY:
	{
		info = ("Watching a replay");
		return;
	}
	case GAME_MODE_PRACTICE:
	{
		info = ("Practicing ");
		break;
	}
	default:
	{
		break;
	}
	}

	// Normal play
	if (stage <= 6)
	{
		info.append("Stage ");
		info.append(std::to_string(stage));
	}
	else
	{
		info.append("Extra Stage");
	}

	if (gameMode == GAME_MODE_STANDARD)
	{
		info.append(" - (");
		if (showScoreInsteadOfRes)
		{
			info.append(formatScore(score, gameOvers));
		}
		else
		{
			info.append(std::to_string(lives));
			info.append("/");
			info.append(std::to_string(bombs));
		}
		info.append(")");
	}

	// Boss name display
	switch (gameState)
	{
	case STATE_MIDBOSS: // Mid-bosses
		info.append(" | Fighting ");
		switch (stage)
		{
		case 1:
			info.append("Cirno");
			break;
		case 2:
			info.append("Sekibanki");
			break;
		case 3:
			info.append("Kagerou Imaizumi");
			break;
		case 4:
			if (characterSub == SUBCHAR_A)
			{
				info.append("Yatsuhashi Tsukumo");
			}
			else // if (characterSub == SUBCHAR_B)
			{
				info.append("Benben Tsukumo");
			}
			break;
		case 5:
		case 6:
			info.append("Seija Kijin");
			break;
		case 7:
			info.append("Benben & Yatsuhashi Tsukumo");
			break;
		}
		break;
	case STATE_BOSS: // Bosses
		info.append(" | Fighting ");
		switch (stage)
		{
		case 1:
			info.append("Wakasagihime");
			break;
		case 2:
			info.append("Sekibanki");
			break;
		case 3:
			info.append("Kagerou Imaizumi");
			break;
		case 4:
			if (characterSub == SUBCHAR_A)
			{
				info.append("Benben Tsukumo");
			}
			else // if (characterSub == SUBCHAR_B)
			{
				info.append("Yatsuhashi Tsukumo");
			}
			break;
		case 5:
			info.append("Seija Kijin");
			break;
		case 6:
			info.append("Shinmyoumaru Sukuna");
			break;
		case 7:
			info.append("Raiko Horikawa");
			break;

		}
		break;
	}

}

void Touhou14::setLargeImageInfo(std::string& icon, std::string& text)
{
	icon = "", text = "";
	if (menuState != -1 || stage == 0)
	{
		return;
	}

	switch (character)
	{
	case CHAR_REIMU:
		icon = "th14reimu", text = "Shot: Reimu ";
		break;
	case CHAR_MARISA:
		icon = "th14marisa", text = "Marisa ";
		break;
	case CHAR_SAKUYA:
		icon = "th14sakuya", text = "Sakuya ";
		break;
	}
	switch (characterSub)
	{
	case SUBCHAR_A:
		icon.append("a");
		text.append("A");
		break;
	case SUBCHAR_B:
		icon.append("b");
		text.append("B");
		break;
	}
}

void Touhou14::setSmallImageInfo(std::string& icon, std::string& text)
{
	icon = "", text = "";
	if (menuState != -1 || stage == 0 || spellCardID != -1) return;

	text.append("Difficulty: ");
	switch (difficulty)
	{
	case DIFFICULTY_EASY:
		text.append("Easy");
		icon.assign("easy");
		break;
	case DIFFICULTY_NORMAL:
		text.append("Normal");
		icon.assign("normal");
		break;
	case DIFFICULTY_HARD:
		text.append("Hard");
		icon.assign("hard");
		break;
	case DIFFICULTY_LUNATIC:
		text.append("Lunatic");
		icon.assign("lunatic");
		break;
	case DIFFICULTY_EXTRA:
		text.append("Extra");
		icon.assign("extra");
		break;
	}
}
}