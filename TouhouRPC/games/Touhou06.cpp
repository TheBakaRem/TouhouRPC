#include "Touhou06.h"

Touhou06::Touhou06(PROCESSENTRY32W const& pe32) : TouhouBase(pe32)
{
	gameState2 = 0;
}

Touhou06::~Touhou06()
{
}

void Touhou06::readDataFromGameProcess()
{
	// Character
	ReadProcessMemory(processHandle, (LPCVOID)CHARACTER, (LPVOID)&character, 1, NULL);

	// Teammate
	ReadProcessMemory(processHandle, (LPCVOID)SUBWEAPON, (LPVOID)&characterSub, 1, NULL);

	// Difficulty
	ReadProcessMemory(processHandle, (LPCVOID)DIFFICULTY, (LPVOID)&difficulty, 1, NULL);

	// Stage
	ReadProcessMemory(processHandle, (LPCVOID)STAGE, (LPVOID)&stage, 1, NULL);

	// Game state
	ReadProcessMemory(processHandle, (LPCVOID)GAME_STATE, (LPVOID)&gameState, 1, NULL);

	// Game state 2 (boss life)
	ReadProcessMemory(processHandle, (LPCVOID)GAME_STATE_2, (LPVOID)&gameState2, 1, NULL);

	// Menu state
	int aMenu;
	ReadProcessMemory(processHandle, (LPCVOID)CHECK_IN_MENU_VALUE, &aMenu, 4, NULL); // == 0 when we are in-game
	if (aMenu == 0)
	{
		// We are in-game
		menuState = -1;
	}
	else {
		// We are in menu
		ReadProcessMemory(processHandle, (LPCVOID)MENU_STATE, (LPVOID)&menuState, 4, NULL); // Read menu state
	}
}

void Touhou06::setGameName(std::string& name) const
{
	name.assign(getGameName());
}

void Touhou06::setGameInfo(std::string& info) const
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

		// Stage 1: Rumia (mid-boss)
		if (stage == 1 && (gameState == 6 || gameState == 7)) {
			details.append(" | Fighting Rumia");
		}
		// Stage 1: Rumia (boss)
		else if (stage == 1 && (gameState >= 16)) {
			details.append(" | Fighting Rumia");
		}

		// Stage 2: Daiyousei
		else if (stage == 2 && gameState == 19) {
			details.append(" | Fighting Daiyousei");
		}
		// Stage 2: Cirno
		else if (stage == 2 && gameState >= 25) {
			details.append(" | Fighting Cirno");
		}

		// Stage 3: Meiling (mid-boss and boss)
		else if (stage == 3 && gameState >= 16) {
			details.append(" | Fighting Meiling");
		}

		// Stage 4: Koakuma
		else if (stage == 4 && (gameState == 0 && gameState2 > 0)) {
			details.append(" | Fighting Koakuma");
		}
		// Stage 4: Patchouli
		else if (stage == 4 && gameState > 0) {
			details.append(" | Fighting Patchouli");
		}

		// Stage 5: Sakuya (mid-boss and boss)
		else if (stage == 5 && gameState >= 17) {
			details.append(" | Fighting Sakuya");
		}

		// Stage 6: Sakuya (mid-boss)
		else if (stage == 6 && gameState == 13) {
			details.append(" | Fighting Sakuya");
		}
		// Stage 6: Remilia (boss)
		else if (stage == 6 && gameState >= 19) {
			details.append(" | Fighting Remilia");
		}

		// Stage 7 Patchouli (mid-boss):
		else if (stage == 7 && (gameState >= 18 && gameState <= 20)) {
			details.append(" | Fighting Patchouli");
		}
		// Stage 7 Flandre (boss):
		else if (stage == 7 && gameState != 0) {
			details.append(" | Fighting Flandre");
		}

		info.assign(details);
	}
}

void Touhou06::setLargeImageInfo(std::string& icon, std::string& text) const
{
	icon.assign("");
	text.assign("");
	if (menuState >= 0 || stage == 0) return; // So that the rest isn't executed while in the menus

	std::string charTeamName;
	if (character == 0) {
		icon.assign("th06reimu");
		charTeamName = "Reimu ";
		charTeamName.append((characterSub == 0 ? "A" : "B"));
	}
	else {
		icon.assign("th06marisa");
		charTeamName = "Marisa ";
		charTeamName.append((characterSub == 0 ? "A" : "B"));

	}
	text.assign(charTeamName);
}

void Touhou06::setSmallImageInfo(std::string& icon, std::string& text) const
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
