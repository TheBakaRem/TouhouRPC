#pragma once
import Games;


enum GFW_STAGE {
    A1_1 = 1,
    A1_2,
    A1_3,
    A2_2,
    A2_3,
    B1_1,
    B1_2,
    B1_3,
    B2_2,
    B2_3,
    C1_1,
    C1_2,
    C1_3,
    C2_2,
    C2_3,
    EX,
};

class Touhou12_8 : public TouhouBase {
public:
    Touhou12_8(PROCESSENTRY32W const& pe32);
    ~Touhou12_8();

    // Inherited from TouhouBase
    int64_t getClientId() const override { return 717045124076405239; };
    const char* getGameName() const override { return "Touhou 12.8 - Fairy Wars"; }

    void readDataFromGameProcess() override;

    // Inherited from TouhouBase
    std::string getStageName() const override;
    std::string getMidbossName() const override;
    std::string getBossName() const override;
    std::string getBGMName() const override;
    std::string getCustomResources() const override;

protected:
    int bgm{ 1 };
    unsigned int motivation{ 0 };
    unsigned int perfectFreeze{ 0 };

private:
    // addresses correct for v1.00a
    enum address {
        DIFFICULTY = 0x004B4D0CL,
        STAGE = 0x004B4D14L,
        STAGE_FRAMES = 0x004B4D20L,
        GAME_STATE = 0x004B4D1CL,
        GAME_STATE_FRAMES = 0x004B4D24L,
        MENU_POINTER = 0x004B8A9CL, // display state at 0x1C
        ENEMY_STATE = 0x004B8920L,
        BGM_STR_1 = 0x004D9D98L, // currently playing outside of stage, or stage theme in-stage
        BGM_STR_2 = 0x004D9E98L, // boss theme
        REPLAY_FLAG = 0x004D2E30L, // 2 when watching replay, 1 otherwise
        MOTIVATION = 0x004B4D64L, // the % * 100 as integer
        PERFECT_FREEZE = 0x004B4D70L, // the % * 100 as integer
        SCORE = 0x004B4CC4L,
        GAMEOVERS = 0x004B4D28L,
    };
};

