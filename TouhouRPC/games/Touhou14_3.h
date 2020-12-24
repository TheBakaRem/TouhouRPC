#pragma once
#include "TouhouMainGameBase.h"

namespace Touhou14_3 
{

class Touhou14_3 :
	public TouhouMainGameBase
{
	// Memory data stored for stages. (788 bytes / stage)
	// First stage (1-1) data located at the address given by 0x004e6b9c, offset 0x31c.
	struct StageData {
		int unused1[3];
		int stagenb;
		int clear_items[9]; // Number of clears with item.
		int clear_noitem; // Number of clears without item usage.
		int clear_total; // Number of total clears (Clear).
		int tries_items[9]; // Number of tries with item.
		int unused2; // Unused value in-game. Due to it's position in memory, it's supposed to track the number of tries without items.
		int tries_total; // Number of total tries (Challenge)
		int best_score; // Current best score (Score, divided by 10)
		int unused3[170];
	};

public:
	Touhou14_3(PROCESSENTRY32W const& pe32);
	~Touhou14_3();

	// Inherited from TouhouBase
	int64_t getClientId() const override { return 791038671322480681; };
	const char* getGameName() const override { return "Touhou 14.3 - Impossible Spell Card"; }

	void readDataFromGameProcess() override;


	// Inherited from TouhouMainGameBase
	std::string getMidbossName() const override { return notSupported; }
	std::string getBossName() const override { return notSupported; }
	std::string getStageName() const override;
	std::string const& getBGMName() const override;

	std::string getCustomMenuResources() const override; // Menu custom display

	void setLargeImageInfo(std::string& icon, std::string& text) const override; // Overrides the large image info to show main item
	void setSmallImageInfo(std::string& icon, std::string& text) const override; // Overrides the small image info to show sub item
	void setGameName(std::string& name) const override;
	void setGameInfo(std::string& info) const override;

protected:
	// For score displays
	int completedScenes{ 0 }; // Number of completed scenes

	char bgm_playing[20]; // Current music file played (used in the music room)

	// For in-stage displays
	int subItemLock{ 0 };				// Check if the sub items are locked. if value == 0 then sub items are locked. if value > 0 then sub items are unlocked.

	int currMainItem{ Items::FABRIC };	// Current main item
	int currMainItemUses{ 0 };			// Current number of main item uses left

	int currSubItem{ Items::FABRIC };	// Current sub item
	int currSubItemUses{ 0 };			// Current number of sub item uses left

private:
	// Note: Replay, music room and nicknames are not supported yet.
	enum address {
		// Menus
		MENU_DATA_PTR = 0x004E6B9CL, // Pointer on the in-game data. Stores info about all levels completed
		MENU_DATA_FIRST_LEVEL_OFFSET = 0x31C,				// Data on Day 1 - Scene 1
		MENU_DATA_NEXT_LEVEL_OFFSET_ADD = 0x314,			// Offset to add to point to the next level in memory (done in a loop.)
															// There are 75 scenes in the game, all following each other in memory. 
		MENU_DATA_NB_SCENES_COMPLETED_OFFSET = 0xEF2C,		// Offset on MENU_DATA to get the number of completed scenes.
		MENU_DATA_SUB_ITEM_LOCK_OFFSET = 0x6C28,			// Points to Scene 6-1 number of completions (Stage ID 35). 
															// Since sub-items are unlocker after completing this stage (or while playing it) we can use this as the sub-item lock value
															// Sub-items are unlocked if this value is > 0 (or if playing in this stage).

		IN_MENU_PTR = 0x004E6BB4L, // Points to the menu info. nullptr while in game.
		IN_MENU_STATUS_OFFSET = 0x1C, // Offset to the current menu opened.

		MUSIC_FILE_PLAYED = 0x0053D5D8L, // Holds a string containing the current music played. 16 bytes long max.

		// In-game
		MAIN_ITEM = 0x004E4710L, // Main item held by Seija.
		SUB_ITEM = 0x004E4714L,  // Sub-item held by Seija (before unlocking on day 6, the value is 0 = fabric)
		// Item list: 0 = fabric, 1 = camera, 2 = umbrella, 3 = lantern, 4 = yin-yang orb, 5 = bomb, 6 = jizo, 7 = doll, 8 = mallet

		ITEM_DATA_PTR = 0x004E6B68L,			// Item value pointer (not nullptr while in the menu!). Can point to useful data on the items (number of uses).
		ITEM_DATA_MAIN_NB_USES_OFFSET = 0x2BC,	// Main item number of uses left.
		ITEM_DATA_SUB_NB_USES_OFFSET = 0x2C0,	// Sub item number of uses left. (Passive effects count as 0 uses)

		PAUSE = 0x0053B150L,		// Pause value. 0 = Paused, 1 = Win/Lose/Menu, 2 = In-game.
		LIFE_COUNT = 0x004E475CL,	// Life counter. Should always be at 0. If it's -1, then it's game over
									// Use PAUSE and LIFE_COUNT together in-game to know when you passed or failed to clear the scene.
		CURRENT_GRAZE = 0x004E4708, // Graze value.
		CURRENT_STAGE = 0x004E479CL, // Current stage played (Stage ID). Goes from 1 to 75, not reset when going back to menu.
		CURRENT_TIMER = 0x004E47A8, // Time elapsed in the stage.
		CURRENT_HIGHSCORE = 0x004E4790L, // High score on the current stage (divided by 10).
		CURRENT_SCORE = 0x004E46F8L, // Current score on the current stage (divided by 10).
	};

	enum Items {
		FABRIC = 0,
		CAMERA,
		UMBRELLA,
		LANTERN,
		YIN_YANG_ORB,
		BOMB,
		JIZO,
		DOLL,
		MALLET,
	};

	enum PauseType {
		PAUSED = 0,
		WIN_LOSE,
		IN_GAME,
	};
};

};