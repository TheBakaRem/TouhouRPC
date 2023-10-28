#pragma once
import Games;
import WindowsUtils;


class Touhou07 : public TouhouBase {
public:
    Touhou07(PROCESSENTRY32W const& pe32);
    ~Touhou07();

    // Inherited from TouhouBase
    int64_t getClientId() const override { return 711300438867312692; };
    const char* getGameName() const override { return "Touhou 07 - Perfect Cherry Blossom"; }

    void readDataFromGameProcess() override;

    // Inherited from TouhouBase
    std::string getStageName() const override;
    std::string getMidbossName() const override;
    std::string getBossName() const override;
    std::string getBGMName() const override;

protected:
    int bgm{ 1 }; // correct in music room only

private:
    // addresses correct for v1.00b
    enum address : TouhouAddress {
        MENU_POINTER = 0x004B9E44L, // offset C is 130 on title, 129 in a submenu (except options), 47 in player data, 35 in music room, 0 in gameplay
        CHARACTER = 0x0062F645L, // 1 byte
        SUB_CHARACTER = 0x0062F646L, // 1 byte
        DIFFICULTY = 0x00626280L,
        // ENEMY_ID              = 0x009B46C0L,
        IS_MAIN_BOSS = 0x009B655AL, // 1 byte, set to 3 when fighting the boss, 0 otherwise
        PLAYER_POINTER = 0x00626278L, // score at offset 00 (int); lives at offset 5C (float); bombs at offset 68 (float); game overs offset 20 (byte)
        STAGE = 0x0062F85CL, // also found at 0x01347FC8L
        BOSS_FLAG = 0x0049FC14L,
        IN_GAME_FLAG_A = 0x0134D9C4L, // -1 on menu, 59 on stage section, 29 on boss
        IN_GAME_FLAG_B = 0x0134D9CCL, // 0 on menu, 60 on stage section, 30 on boss
        MUSIC_ROOM_TRACK = 0x0135E2BCL,
        PAUSE_FLAG = 0x0062F64CL, // 1 byte, 2 when paused, 0 when not.
        FRAME_COUNTER = 0x004BDD54L,
        // PRACTICE_FLAG         = 0x0134E396L, // unused as stage mode supersedes // locked as 808464432 when in practice mode, otherwise changes per stage/difficulty/(character?)

        // 1 byte bitset
        // when in practice mode (including on main menu), bit 0 is set
        // when on title screen demo, bits 1 and 3 are set (3 for being a replay)
        // when unpaused, bit 2 is set
        // when in replay, bit 3 is set
        // otherwise, none set
        STAGE_MODE = 0x0062F648L,
        STAGE_MODE_PRACTICE_FLAG = 1,
        STAGE_MODE_DEMO_FLAG = 2,
        STAGE_MODE_PAUSE_FLAG = 4,
        STAGE_MODE_REPLAY_FLAG = 8,
    };
};

