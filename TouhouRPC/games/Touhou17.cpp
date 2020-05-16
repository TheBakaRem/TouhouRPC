#include "Touhou17.h"

Touhou17::Touhou17(PROCESSENTRY32W const& pe32) : TouhouBase(pe32)
{
}

Touhou17::~Touhou17()
{
}

void Touhou17::readDataFromGameProcess() {
	// The gameState member will be used like following: 0 = stage, 1 = midboss, 2 = boss
	gameState = 0;

	DWORD menu_pointer = 0;
	ReadProcessMemory(processHandle, (LPCVOID)MENU_POINTER, (LPVOID)&menu_pointer, 4, NULL);
	if (menu_pointer) {
		ReadProcessMemory(processHandle, (LPCVOID)(menu_pointer + 0x24), (LPVOID)&menuState, 4, NULL);
		return;
	}
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

	if (gameState != 2) {
		// Enemy state object
		// This object holds various information about general ecl state.
		// Offset 44h in this structure is used to indicate the main boss enemy
		// If it's 0 there is no main boss
		// It doesn't differenciate between midbosses and bosses but we already did that anyways
		DWORD enemy_state_ptr = 0;

		ReadProcessMemory(processHandle, (LPCVOID)ENEMY_STATE_POINTER, (LPVOID)&enemy_state_ptr, 4, NULL);
		if (enemy_state_ptr != 0) {
			DWORD boss_mode = 0;
			ReadProcessMemory(processHandle, (LPCVOID)(enemy_state_ptr + 0x44), (LPVOID)&boss_mode, 4, NULL);
			if (boss_mode > 0) {
				gameState = 1;
			}
		}
	}


	ReadProcessMemory(processHandle, (LPCVOID)CHARACTER, (LPVOID)&character, 4, NULL);
	ReadProcessMemory(processHandle, (LPCVOID)SUB_CHARACTER, (LPVOID)&characterSub, 4, NULL);
	ReadProcessMemory(processHandle, (LPCVOID)DIFFICULTY, (LPVOID)&difficulty, 4, NULL);
}

void Touhou17::setGameName(std::string& name) const
{
	name = getGameName();
}

void Touhou17::setGameInfo(std::string& info) const
{
	if (menuState >= 0 || stage == 0) {
		info = ("In the main menu");
		return;
	}

	
	if (stage <= 6) {
		info = "Stage ";
		info.append(std::to_string(stage));
	}
	else {
		info = "Extra Stage";
	}

	// Boss name display
	switch (gameState) {
	case 1:
		info.append(" | Fighting ");
		switch (stage) {
		case 1:
			info.append("Eika Ebisu");
			break;
		case 2:
			info.append("Urumi Ushizaki");
			break;
		case 3:
		case 7:
			info.append("Kutaka Niwatari");
			break;
		case 4:
			info.append("Yachie Kicchou");
			break;
		case 5:
		case 6:
			info.append("Mayumi Joutouguu");
			break;
		}
		break;
	case 2:
		info.append(" | Fighting ");
		switch (stage) {
		case 1:
			info.append("Eika Ebisu");
			break;
		case 2:
			info.append("Urumi Ushizaki");
			break;
		case 3:
			info.append("Kutaka Niwatari");
			break;
		case 4:
			info.append("Yachie Kicchou");
			break;
		case 5:
			info.append("Mayumi Joutouguu");
			break;
		case 6:
			info.append("Keiki Haniyasushin");
			break;
		case 7:
			info.append("Saki Kurokoma");
			break;
		}
		break;
	}

}

void Touhou17::setLargeImageInfo(std::string& icon, std::string& text) const
{
	icon = "", text = "";
	if (menuState >= 0 || stage == 0) return;

	switch (character) {
	case 0:
		icon = "th17reimu", text = "Reimu ";
		break;
	case 1:
		icon = "th17marisa", text = "Marisa ";
		break;
	case 2:
		icon = "th17youmu", text = "Youmu ";
		break;
	}
	switch (characterSub) {
	case 0:
		text.append("(Wolf)");
		break;
	case 1:
		text.append("(Otter)");
		break;
	case 2:
		text.append("(Eagle)");
		break;
	}
}

void Touhou17::setSmallImageInfo(std::string& icon, std::string& text) const
{
	icon = "", text = "";
	if (menuState >= 0 || stage == 0) return;

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