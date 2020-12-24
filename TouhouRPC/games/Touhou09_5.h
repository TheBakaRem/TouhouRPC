#pragma once
#include "TouhouMainGameBase.h"

namespace Touhou09_5
{

class Touhou09_5 :
	public TouhouMainGameBase
{
public:
	Touhou09_5(PROCESSENTRY32W const& pe32);
	~Touhou09_5();

	// Inherited from TouhouBase
	int64_t getClientId() const override { return 725634084050829352; };
	const char* getGameName() const override { return "Touhou 09.5 - Shoot the Bullet"; }

	void readDataFromGameProcess() override;


	// Inherited from TouhouMainGameBase
	std::string getMidbossName() const override { return notSupported; }
	std::string getBossName() const override { return notSupported; }
	std::string getStageName() const override;
	std::string const& getBGMName() const override;
	
	std::string getCustomMenuResources() const override; // Menu custom display
	std::string getCustomResources() const override; // In-game custom display


protected:

	char bgm_playing[20]; // Background music for music room

	// For score displays
	int combinedPhotoScore{ 0 }; // Total score from all scenes
	int completedScenes{ 0 }; // Number of completed scenes

	// For in-stage displays
	int currPhotoNb{ 0 }; // Current number of photos
	int reqPhotoNb{ 0 }; // Number of photos required to complete the scene
private:
	// Adresses correct for v1.02a
	enum address {
		// Menus
		MENU_DATA_PTR = 0x004C4E78L, // Pointer on the in-game data. Stores info about all levels completed
		MENU_DATA_FIRST_SCORE_OFFSET = 0x470, // Highest score on Level 1 - Scene 1
		MENU_DATA_NEXT_SCORE_OFFSET_ADD = 0x60, // Offset to add to point to the next highest score in memory (done in a loop.)
												// 108 levels are available this way (unused levels [Level 1 - Scene 9 for example is not used] are zeroed out.)
												// Score > 0 means the scene has been completed.
												// Addition of all scores gives the combined photo score.
		
		IN_MENU = 0x004B1FE4, // 1 byte, value is: 1 on main menu / in-game (GAME_DATA_PTR = nullptr if on main menu), 2 on in-game replay, 5 on mission select, 21 on replay select

		BGM_STR = 0x004C8FF4L,	// Filename of the currently played music. (the string starts at FF0, but I start it at FF4 to remove the "bgm/" part automatically)

		OTHER_MENU_STATE_PTR = 0x004C4DF0L, // Pointer that gets to another menu state (music room, manual, options). nullptr when in game.
		OTHER_MENU_STATE_OFFSET = 0x6110,

		// In-game
		GAME_DATA_PTR = 0x004C4DF4L, // Pointer on the in-game player data (score, timer, player state, current stage...)
		GAME_DATA_TIMER_OFFSET = 0x28, // Incrementing timer giving the time pased on each scene
		GAME_DATA_PLAYER_STATE_OFFSET = 0xFC, // 1 byte (0x40 is true when a scene is completed, 0x20 when player died, 0x10 when paused)
		GAME_DATA_STAGE_OFFSET = 0x100, // Level / Scene value (decimals are levels, units are scenes)
		GAME_DATA_SCORE_OFFSET = 0x114, // Score (real score, no need to multiply it by 10)

		GAME_PHOTO_STATS_PTR_OFFSET = 0xC, // Offset to the pointer that stores other data (namely, number of photos current and required for the level
		GAME_PHOTO_STATS_CURR_PHOTOS_OFFSET = 0x29E4, // Photo number
		GAME_PHOTO_STATS_REQUIRED_PHOTOS_OFFSET = 0x29EC, // Number of photos needed to complete the scene
	};

	enum OtherMenuStateValues {
		MAIN_MENU = 1,
		MISSION_SELECT = 2,
		REPLAY_SELECT = 3,
		OPTIONS = 7,
		MUSIC_ROOM = 8,
		MANUAL = 9,
	};

};

};

