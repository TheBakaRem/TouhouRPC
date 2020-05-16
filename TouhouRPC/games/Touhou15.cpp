#include "Touhou15.h"

Touhou15::Touhou15(PROCESSENTRY32W const& pe32) : TouhouBase(pe32)
{
	gameType = 0;
	retryCount = 0;
	chapterRetryCount = 0;
}

Touhou15::~Touhou15()
{
}

void Touhou15::readDataFromGameProcess()
{
	// Character
	ReadProcessMemory(processHandle, (LPCVOID)CHARACTER, (LPVOID)&character, 4, NULL);

	// Game type
	ReadProcessMemory(processHandle, (LPCVOID)GAME_TYPE, (LPVOID)&gameType, 4, NULL);

	// Difficulty
	ReadProcessMemory(processHandle, (LPCVOID)DIFFICULTY, (LPVOID)&difficulty, 4, NULL);

	// Stage
	ReadProcessMemory(processHandle, (LPCVOID)STAGE, (LPVOID)&stage, 4, NULL);

	// Stage chapter
	ReadProcessMemory(processHandle, (LPCVOID)STAGE_CHAPTER, (LPVOID)&gameState, 4, NULL);

	// Stage chapter frames (number of frames in this current state)
	ReadProcessMemory(processHandle, (LPCVOID)STAGE_CHAPTER_FRAMES, (LPVOID)&gameStateFrames, 4, NULL);

	// Global retry count
	ReadProcessMemory(processHandle, (LPCVOID)GLOBAL_RETRY_COUNT, (LPVOID)&retryCount, 4, NULL);

	// Chapter retry count
	ReadProcessMemory(processHandle, (LPCVOID)CHAPTER_RETRY_COUNT, (LPVOID)&chapterRetryCount, 4, NULL);

	// Menu state
	int aMenu;
	ReadProcessMemory(processHandle, (LPCVOID)MENU_POINTER, &aMenu, 4, NULL); // Get menu class address
	if (aMenu == 0)
	{
		// We are in-game
		menuState = -1;
	}
	else {
		// We are in menu
		ReadProcessMemory(processHandle, (LPCVOID)(aMenu + 0x18), (LPVOID)&menuState, 4, NULL); // Read menu state
	}

}

void Touhou15::setGameName(std::string& name) const
{
	name.assign(getGameName());
}

void Touhou15::setGameInfo(std::string& info) const
{
	if (menuState >= 0 || stage == 0) {
		info.assign("In the main menu");
	}
	else {
		std::string details = "";

		// Stage display
		if (stage <= 6) {
			details = "Stage ";
			details.append(std::to_string(stage));
		}
		else {
			details = "Extra Stage ";
		}

		// Boss name display

		// Stage 1: Seiran (mid-boss 1)
		if (stage == 1 && (gameState == 2 && gameStateFrames <= 1600)) {
			details.append(" | Fighting Seiran");
		}
		// Stage 1: Seiran (mid-boss 2 and boss)
		else if (stage == 1 && (gameState == 4 || gameState >= 31) && gameState != 81) {
			details.append(" | Fighting Seiran");
		}

		// Stage 2: Ringo
		else if (stage == 2 && (gameState == 3 || gameState >= 41) && gameState != 81) {
			details.append(" | Fighting Ringo");
		}

		// Stage 3: Doremy
		else if (stage == 3 && (gameState == 3 || gameState >= 31) && gameState != 81) {
			details.append(" | Fighting Doremy");
		}

		// Stage 4: Sagume
		else if (stage == 4 && (gameState == 3 || gameState >= 41) && gameState != 81) {
			details.append(" | Fighting Sagume");
		}

		// Stage 5: Clownpiece
		else if (stage == 5 && gameState >= 41 && gameState != 81) {
			details.append(" | Fighting Clownpiece");
		}

		// Stage 6: Junko
		else if (stage == 6 && gameState >= 41 && gameState != 81) {
			details.append(" | Fighting Junko");
		}

		// Stage 7 (Extra): Doremy
		else if (stage == 7 && (gameState == 5 || (gameState >= 23 && gameState <= 33))) {
			details.append(" | Fighting Doremy");
		}
		// Stage 7 (Extra): Junko + Hecatia (dialogue, final spell)
		else if (stage == 7 && (gameState == 41 || gameState == 71 || gameState == 81)) {
			details.append(" | Fighting Junko & Hecatia");
		}
		// Stage 7 (Extra): Junko only
		else if (stage == 7 && (gameState == 46 || gameState == 65 || gameState == 50 || gameState == 69)) {
			details.append(" | Fighting Junko");
		}
		// Stage 7 (Extra): Hecatia only
		else if (stage == 7 && (gameState >= 43 && gameState <= 70)) {
			details.append(" | Fighting Hecatia");
		}


		info.assign(details);
	}
}

void Touhou15::setLargeImageInfo(std::string& icon, std::string& text) const
{
	icon.assign("");
	text.assign("");
	if (menuState >= 0 || stage == 0) return; // So that the rest isn't executed while in the menus

	switch (character) {
	case 0: // Reimu
		icon.assign("th15reimu");
		text.assign("Reimu");
		break;
	case 1: // Marisa
		icon.assign("th15marisa");
		text.assign("Marisa");
		break;
	case 2: // Sanae
		icon.assign("th15sanae");
		text.assign("Sanae");
		break;
	case 3: // Reisen
		icon.assign("th15reisen");
		text.assign("Reisen");
		break;
	}
}

void Touhou15::setSmallImageInfo(std::string& icon, std::string& text) const
{
	icon.assign("");
	text.assign("");
	if (menuState >= 0 || stage == 0) return; // So that the rest isn't executed while in the menus

	if (gameType == 0 && stage != 7)
	{
		text.append("Pointdevice ");
	}
	else {
		text.append("Legacy ");
	}

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

	if (gameType == 0 && stage != 7) // Retry count for Pointdevice mode
	{
		text.append(" (Retry count: ");
		text.append(std::to_string(retryCount));
		text.append(")");
	}
}
