#pragma once
#include "TouhouMainGameBase.h"

namespace Touhou16_5 
{

class Touhou16_5 :
	public TouhouMainGameBase
{
	// Memory data stored for stages. (24 bytes / stage + 540 useless bytes)
	// First stage (Sunday 1) data located at the address given by 0x004b5660, offset 0x14.
	struct StageData {
		int stage_number;
		int clear_count; // Number of clears
		int unused;
		int tries_count; // Number of total tries
		int photo_count; // Number of photos taken
		int best_score; // Current best score
	};

public:
	Touhou16_5(PROCESSENTRY32W const& pe32);
	~Touhou16_5();

	// Inherited from TouhouBase
	int64_t getClientId() const override { return 904710573558747166; };
	const char* getGameName() const override { return "Touhou 16.5 - Violet Detector"; }

	void readDataFromGameProcess() override;


	// Inherited from TouhouMainGameBase
	std::string getMidbossName() const override { return notSupported; }
	std::string getBossName() const override { return notSupported; }
	std::string getStageName() const override;
	std::string const& getBGMName() const override;

	std::string getCustomMenuResources() const override; // Menu custom display

	void setGameName(std::string& name) const override;
	void setGameInfo(std::string& info) const override;

protected:

	// Game mode
	int gameMode{ GameMode::GAME_MODE_STANDARD };

	// For score displays
	int completedScenes{ 0 }; // Number of completed scenes
	int totalScore{ 0 }; // Total score of photos


	char bgm_playing[20] = "\0"; // Current music file played (used in the music room)

private:
	// Note: Nicknames are not supported yet.
	enum address {
		// Menus
		MENU_DATA_PTR = 0x004B5660L, // Pointer on the in-game data. Stores info about all levels completed
		MENU_DATA_FIRST_LEVEL_OFFSET = 0x14,				// Data on Sunday 1
		MENU_DATA_NEXT_LEVEL_OFFSET_ADD = 0x234,			// Offset to add to point to the next level in memory (done in a loop.)
															// There are 10X (Unknown) scenes in the game, all following each other in memory. 
		// MENU_DATA_NB_SCENES_COMPLETED_OFFSET = 0xEF2C,		// Offset on MENU_DATA to get the number of completed scenes. Unknown if really in memory or not.
		
		IN_MENU_PTR = 0x004B5674L, // Points to the menu info. nullptr while in game.
		IN_MENU_STATUS_OFFSET = 0x18, // Offset to the current menu opened.

		MUSIC_FILE_PLAYED = 0x0050C3F0L, // Holds a string containing the current music played. 16 bytes long max.

		// In-game

		PAUSE = 0x00509C20L,		// Pause value. 0 = Paused, 1 = Win/Lose/Menu, 2 = In-game.
		LIFE_COUNT = 0x004B3A98L,	// Life counter. Should always be at 0. If it's -1, then it's game over
									// Use PAUSE and LIFE_COUNT together in-game to know when you passed or failed to clear the scene.
		CURRENT_STAGE = 0x004B3AD8L, // Current stage played (Stage ID). Goes from 1 to 10X (Unknown), not reset when going back to menu.
		CURRENT_TIMER = 0x004B3AEC, // Time elapsed in the stage.

		PHOTO_COUNT_PTR = 0x004B5654L, // Pointer to the photo count values. The player needs at least one photo to clear the stage on boss death.
		PHOTO_COUNT_OFFSET = 0x16890,

		GAME_MODE = 0x004B5670L,	// game mode (normal mode, menu or replay mode)
	};


	enum PauseType {
		PAUSED = 0,
		WIN_LOSE,
		IN_GAME,
	};

	enum GameMode {
		GAME_MODE_MENU = 1,		// main menu
		GAME_MODE_REPLAY = 2,	// replay
		GAME_MODE_STANDARD = 6, // normal gameplay
	};

};

};