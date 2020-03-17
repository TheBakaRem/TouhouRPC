#include "Touhou16.h"
#include <iostream>
Touhou16::Touhou16(PROCESSENTRY32W* pe32) : TouhouBase(pe32)
{
	spellCardID = 0;
}

Touhou16::~Touhou16()
{
}

void Touhou16::readDataFromGameProcess() {
	// The gameState member will be used like following: 0 = stage, 1 = midboss, 2 = boss
	gameState = 0;

	unsigned int menu_pointer = 0;
	ReadProcessMemory(processHandle, (LPCVOID)MENU_POINTER, (LPVOID)&menu_pointer, 4, NULL);
	if (menu_pointer != 0) {
		ReadProcessMemory(processHandle, (LPCVOID)(menu_pointer + 0x18), (LPVOID)&menuState, 4, NULL);
		return;
	}

	// Here, we are not in the menu
	menuState = -1;

	// The BGM playing will be used to determine a lot of things
	char bgm_playing[20];
	ReadProcessMemory(processHandle, (LPCVOID)BGM_STR, bgm_playing, 20, NULL);

	// Check if the game over music is playing.
	if (std::strncmp(bgm_playing, "th128_08.wav", std::strlen("th128_08.wav")) == 0) {
		gameState = 0;
	}
	
	// Read stage value
	ReadProcessMemory(processHandle, (LPCVOID)STAGE, (LPVOID)&stage, 4, NULL);

	// Convert the part after the _ and before the . to int
	// That way it is possible to switch case the BGM playing
	char bgm_id_str[3];
	bgm_id_str[0] = bgm_playing[5];
	bgm_id_str[1] = bgm_playing[6];
	bgm_id_str[2] = '\x00';
	int bgm_id = atoi(bgm_id_str);

	// Intentionak fallthroughs
	// Note that ZUN's naming for the BGM file names is not very consistent
	switch (bgm_id) {
	case 1:
		menuState = 1;
		break;
	case 4: // stage 1 boss
		gameState = 2;
	case 3: // stage 1
		stage = 1;
		break;

	case 6: // stage 2 boss
		gameState = 2;
	case 5: // stage 2
		stage = 2;
		break;

	case 8: // stage 3 boss
		gameState = 2;
	case 7:// stage 3
		stage = 3;
		break;

	case 10: // stage 4 boss
		gameState = 2;
	case 9: // stage 4
		stage = 4;
		break;

	case 12: // stage 5 boss
		gameState = 2;
	case 11: // stage 5
		stage = 5;
		break;

	case 13: // stage 6 boss
		gameState = 2;
	case 14: // stage 6
		stage = 6;
		break;
	case 18:
		gameState = 3;
	case 17:
		stage = 7;
	}

	
	// Enemy state object
	// This object holds various information about general ecl state.
	// Offset 1BC8h in this structure is used to indicate the main boss enemy
	// If it's 0 there is no main boss, if it's 4 it's a mid-boss or a boss

	// We know the main stage boss is triggered when music changes
	// So if the state isn't already defined, we check if the mid-boss is present
	if (gameState != 2) {
		DWORD enemy_state_ptr = 0;
		ReadProcessMemory(processHandle, (LPCVOID)ENEMY_STATE_POINTER, (LPVOID)&enemy_state_ptr, 4, NULL);
		if (enemy_state_ptr != 0) {
			DWORD boss_mode = 0;
			ReadProcessMemory(processHandle, (LPCVOID)(enemy_state_ptr + 0x1BC8), (LPVOID)&boss_mode, 4, NULL);
			if (boss_mode == 4) {
				gameState = 1;
			}
		}
	}
	
	// Read character and difficulty info
	ReadProcessMemory(processHandle, (LPCVOID)CHARACTER, (LPVOID)&character, 4, NULL);
	ReadProcessMemory(processHandle, (LPCVOID)SUB_CHARACTER, (LPVOID)&characterSub, 4, NULL);
	ReadProcessMemory(processHandle, (LPCVOID)DIFFICULTY, (LPVOID)&difficulty, 4, NULL);

	// Read Spell Card ID (for Spell Practice)
	ReadProcessMemory(processHandle, (LPCVOID)SPELL_CARD_ID, (LPVOID)&spellCardID, 4, NULL);
}

void Touhou16::setGameName(std::string& name) {
	name = "Touhou 16 - Hidden Stars in Four Seasons";
}

void Touhou16::setGameInfo(std::string& info) {
	// Menu
	if (menuState != -1 || stage == 0) {
		info = ("In the main menu");
		return;
	}

	// Spell practice
	if (spellCardID != -1)
	{
		// spellcard practice
		info.assign("Practicing a spell:\n");
		//info.append(th16_spellCardName[spellCardID]); // Uncomment this when Spell Card names are supported
		return;
	}

	// Normal play
	if (stage <= 6) {
		info = "Stage ";
		info.append(std::to_string(stage));
	}
	else {
		info = "Extra Stage";
	}

	// Boss name display
	switch (gameState) {
	case 1: // Mid-bosses
		info.append(" | Fighting ");
		switch (stage) {
		case 1:
			info.append("Eternity Larva");
			break;
		case 2:
			info.append("Nemuno Sakata");
			break;
		case 3:
			info.append("Lily White");
			break;
		case 4:
			info.append("Mai Teireida");
			break;
		case 5:
			info.append("Satono Nishida");
			break;
		case 7:
			info.append("Mai Teireida & Satono Nishida");
			break;
		}
		break;
	case 2: // Bosses
		info.append(" | Fighting ");
		switch (stage) {
		case 1:
			info.append("Eternity Larva");
			break;
		case 2:
			info.append("Nemuno Sakata");
			break;
		case 3:
			info.append("Aunn Komano");
			break;
		case 4:
			info.append("Narumi Yatadera");
			break;
		case 5:
			info.append("Mai Teireida & Satono Nishida");
			break;
		case 6:
		case 7:
			info.append("Okina Matara");
			break;

		}
		break;
	}

}

void Touhou16::setLargeImageInfo(std::string& icon, std::string& text) {
	icon = "", text = "";
	if (menuState != -1 || stage == 0) return;

	switch (character) {
	case 0:
		icon = "th16reimu", text = "Reimu ";
		break;
	case 1:
		icon = "th16cirno", text = "Cirno ";
		break;
	case 2:
		icon = "th16aya", text = "Aya ";
		break;
	case 3:
		icon = "th16marisa", text = "Marisa ";
		break;
	}
	switch (characterSub) {
	case 0:
		text.append("(Spring)");
		break;
	case 1:
		text.append("(Summer)");
		break;
	case 2:
		text.append("(Fall)");
		break; 
	case 3:
		text.append("(Winter)");
		break;
	}
}

void Touhou16::setSmallImageInfo(std::string& icon, std::string& text) {
	icon = "", text = "";
	if (menuState != -1 || stage == 0) return;

	text.append("Difficulty: ");
	switch (difficulty) {
	case 0: // Easy
		text.append("Easy");
		icon.assign("easy");
		break;
	case 1: // Normal
		text.append("Normal");
		icon.assign("normal");
		break;
	case 2: // Hard
		text.append("Hard");
		icon.assign("hard");
		break;
	case 3: // Lunatic
		text.append("Lunatic");
		icon.assign("lunatic");
		break;
	case 4: // Extra (difficulty == 4)
		text.append("Extra");
		icon.assign("extra");
		break;
	}
}