#include "Touhou11.h"

Touhou11::Touhou11(PROCESSENTRY32W const& pe32) : TouhouBase(pe32)
{
}

Touhou11::~Touhou11()
{
}

void Touhou11::readDataFromGameProcess()
{

	// Character
	ReadProcessMemory(processHandle, (LPCVOID)CHARACTER, (LPVOID)&character, 4, NULL);

	// Teammate
	ReadProcessMemory(processHandle, (LPCVOID)SUB_CHARACTER, (LPVOID)&characterSub, 4, NULL);
	
	// Difficulty
	ReadProcessMemory(processHandle, (LPCVOID)DIFFICULTY, (LPVOID)&difficulty, 4, NULL);
	
	// Stage
	ReadProcessMemory(processHandle, (LPCVOID)STAGE, (LPVOID)&stage, 4, NULL);

	// Stage (number of frames in this stage)
	ReadProcessMemory(processHandle, (LPCVOID)STAGE_FRAMES, (LPVOID)&stageFrames, 4, NULL);

	// Game state
	ReadProcessMemory(processHandle, (LPCVOID)GAME_STATE, (LPVOID)&gameState, 4, NULL);

	// Game state (number of frames in this current state)
	ReadProcessMemory(processHandle, (LPCVOID)GAME_STATE_FRAMES, (LPVOID)&gameStateFrames, 4, NULL);


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
		ReadProcessMemory(processHandle, (LPCVOID)(aMenu + 0x1C), (LPVOID)&menuState, 4, NULL); // Read menu state
	}
}

void Touhou11::setGameName(std::string & name) const
{
	name.assign(getGameName());
}

void Touhou11::setGameInfo(std::string & info) const
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

		// Stage 1: Kisume
		if (stage == 1 && (gameState >= 6 && gameState <= 8) || gameState == 15) {
			details.append(" | Fighting Kisume");
		} 
		// Stage 1: Yamame
		else if (stage == 1 && (gameState == 2 || gameState >= 24)) {
			details.append(" | Fighting Yamame");
		}

		// Stage 2: Parsee (mid-boss)
		else if (stage == 2 && (gameState == 6 || (gameState == 43 && gameStateFrames <= 2040))) {
			details.append(" | Fighting Parsee");
		}
		// Stage 2: Parsee (boss)
		else if (stage == 2 && (gameState == 2 || gameState >= 24)) {
			details.append(" | Fighting Parsee");
		}

		// Stage 3: Yuugi (mid-boss)
		else if (stage == 3 && (gameState == 6 || gameState == 43 || gameState == 4)) {
			details.append(" | Fighting Yuugi");
		}
		// Stage 3: Yuugi (boss)
		else if (stage == 3 && (gameState == 2 || gameState >= 24)) {
			details.append(" | Fighting Yuugi");
		}

		// Stage 4: Orin (start)
		else if (stage == 4 && (gameState == 6 && gameStateFrames <= 900)) {
			details.append(" | Fighting Orin");
		}
		// Stage 4: Orin (mid-boss)
		else if (stage == 4 && (gameState == 7 && gameStateFrames <= 1590)) {
			details.append(" | Fighting Orin");
		}
		// Stage 4: Satori (boss)
		else if (stage == 4 && (gameState == 2 || gameState >= 24)) {
			details.append(" | Fighting Satori");
		}

		// Stage 5: Orin (mid-boss)
		else if (stage == 5 && (gameState == 6 || (gameState == 7 && gameStateFrames <= 1200) || gameState == 15)) {
			details.append(" | Fighting Orin");
		}
		// Stage 5: Orin (boss)
		else if (stage == 5 && (gameState == 2 || gameState >= 24)) {
			details.append(" | Fighting Orin");
		}

		// Stage 6: Orin (mid-boss)
		else if (stage == 6 && (gameState == 6 || (gameState == 15 && gameStateFrames <= 2220))) {
			details.append(" | Fighting Orin");
		}
		// Stage 6: Utsuho (boss)
		else if (stage == 6 && gameState >= 24) {
			details.append(" | Fighting Utsuho");
		}

		// Stage 7 (Extra): Sanae
		else if (stage == 7 && (gameState == 6 || (gameState == 0 && (stageFrames > gameStateFrames) && gameStateFrames <= 11200))) {
			details.append(" | Fighting Sanae");
		}
		// Stage 7 (Extra): Koishi
		else if (stage == 7 && (gameState == 2 ||gameState >= 24)) {
			details.append(" | Fighting Koishi");
		}

		info.assign(details);
	}
}

void Touhou11::setLargeImageInfo(std::string & icon, std::string & text) const
{
	icon.assign("");
	text.assign("");
	if (menuState >= 0 || stage == 0) return; // So that the rest isn't executed while in the menus

	std::string charTeamName;
	if (character == 0) {
		icon.assign("th11reimu");
		charTeamName = "Reimu + ";
		charTeamName.append((characterSub == 0 ? "Yukari" : (characterSub == 1 ? "Suika" : "Aya")));
	}
	else {
		icon.assign("th11marisa");
		charTeamName = "Marisa + ";
		charTeamName.append((characterSub == 0 ? "Alice" : (characterSub == 1 ? "Patchouli" : "Nitori")));

	}
	text.assign(charTeamName);
}

void Touhou11::setSmallImageInfo(std::string & icon, std::string & text) const
{
	icon.assign("");
	text.assign("");
	if (menuState >= 0 || stage == 0) return; // So that the rest isn't executed while in the menus

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
