#pragma once
import Games;


class Touhou12_5 : public TouhouBase {
public:
    Touhou12_5(PROCESSENTRY32W const& pe32);
    ~Touhou12_5();

    // Inherited from TouhouBase
    int64_t getClientId() const override { return 896809286456467456; };
    const char* getGameName() const override { return "Touhou 12.5 - Double Spoiler"; }

    void readDataFromGameProcess() override;

    // Inherited from TouhouBase
    std::string getMidbossName() const override { return notSupported; }
    std::string getBossName() const override { return notSupported; }
    std::string getStageName() const override;
    std::string getBGMName() const override;

    void setGameInfo(std::string& info) const override; // Override for the Playing_CustomResources state
    void setLargeImageInfo(std::string& icon, std::string& text) const override; // Override for the Menu_CustomResources state

    std::string getCustomMenuResources() const override; // Menu custom display
    std::string getCustomResources() const override; // In-game custom display

protected:
    std::string createFormattedScore() const override;

    char bgm_playing[20] = "\0"; // Background music for music room
    int player{ 0 }; // Player selected (Aya or Hatate)

    // For score displays
    int combinedPhotoScoreAya{ 0 }; // Total score from all scenes (Aya)
    int completedScenesAya{ 0 }; // Number of completed scenes (Aya)
    int combinedPhotoScoreHatate{ 0 }; // Same values, but for Hatate
    int completedScenesHatate{ 0 };

    // In-stage photo counts are listed in the game state.

    int lastSelectionValue{ 0 }; // Used to store the last menu value. We use it to know if we're in-game or in a replay when the game starts
private:
    // Adresses correct for v1.00a
    enum address {
        // Player select
        PLAYER_SELECT = 0x004B308CL, // 0 is Aya, 1 is Hatate (available in the menu and in-game)

        // Menus
        MENU_DATA_PTR = 0x004B68D0L, // Pointer on the in-game data. Stores info about all levels completed
        MENU_DATA_FIRST_SCORE_OFFSET_AYA = 0x48C,		// Highest score on Level 1 - Scene 1 for Aya
        MENU_DATA_FIRST_SCORE_OFFSET_HATATE = 0x2BEC,	// Highest score on Level 1 - Scene 1 for Hatate
        MENU_DATA_NEXT_SCORE_OFFSET_ADD = 0x48,			// Offset to add to point to the next highest score in memory (done in a loop.)
        // 139 levels are available for Aya this way, and 129 for Hatate (unused levels [Level 1 - Scene 9 for example is not used] are zeroed out.)
        // Score > 0 means the scene has been completed.
        // Addition of all scores gives the combined photo score.

        MENU_STATE_PTR = 0x004B68E8L, // Pointer that we use to get to the menu state (menu select, music room, replays...). nullptr when in-game.
        MENU_STATE_OFFSET = 0x65CC,

        BGM_STR = 0x004D7B38L,	// Filename of the currently played music.

        // In-game
        GAME_DATA_TIMER = 0x004B30C0L, // Incrementing timer giving the time pased on each scene
        GAME_DATA_PLAYER_STATE = 0x004D3850L, // 0 when paused, 1 when player wins/loses, 2 when in-game (Note: Stage 1-1 tutorial boxes are considered win/lose states)
        GAME_DATA_STAGE = 0x004B30ACL, // Level / Scene value (decimals are levels, units are scenes)
        GAME_DATA_SCORE = 0x004B305CL, // Score

        GAME_PHOTO_STATS_PTR = 0x004B68C8L, // Pointer that stores other data (namely, number of photos current and required for the level)
        GAME_PHOTO_STATS_CURR_PHOTOS_OFFSET = 0x3834, // Photo number
        GAME_PHOTO_STATS_REQUIRED_PHOTOS_OFFSET = 0x383C, // Number of photos needed to complete the scene
    };

    enum mainMenuStateValues {
        MAIN_MENU = 1,
        MISSION_SELECT = 2,
        REPLAY_SELECT = 3,
        OPTIONS = 7,
        MUSIC_ROOM = 8,
        MANUAL = 9,
    };

    enum PauseType {
        PAUSED = 0,
        WIN_LOSE_TUTORIAL = 1,
        IN_GAME = 2,
    };
};
