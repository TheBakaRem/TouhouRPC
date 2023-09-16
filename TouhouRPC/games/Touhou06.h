#pragma once
import Games;
import <string>;

class Touhou06 : public TouhouBase {
public:
    Touhou06(PROCESSENTRY32W const& pe32);
    ~Touhou06();

    // Inherited from TouhouBase
    int64_t getClientId() const override { return 712067805398171658; };
    const char* getGameName() const override { return "Touhou 06 - Embodiment of Scarlet Devil"; }

    void readDataFromGameProcess() override;

    std::string getMidbossName() const /*override*/;
    std::string getBossName() const /*override*/;
    std::string getBGMName() const /*override*/;

    int gameState2{ 0 };
    int bgm{ 0 };

private:
    enum address {
        CHARACTER = 0x0069D4BDL, // 1 byte
        SUB_CHARACTER = 0x0069D4BEL, // 1 byte
        DIFFICULTY = 0x0069BCB0L, // 1 byte
        STAGE = 0x00487B48L, // 1 byte
        GAME_STATE = 0x004B974CL, // 1 byte
        GAME_STATE_2 = 0x0069BC57L, // 1 byte
        CHECK_IN_MENU_VALUE = 0x006DC8F8L, // If != 0, we are in... certain menus
        MENU_STATE = 0x006DC8B0L, // display state, is not cleared when game begins

        LIVES = 0x0069D4BAL, // 1 byte
        BOMBS = 0x0069D4BBL, // 1 byte
        SCORE = 0x0069BCA0L, // includes game overs
        GAMEOVERS = 0x0069D4B8L, // 1 byte
        PRACTICE_FLAG = 0x0069D4C3L, // 1 byte, set to 1 when in practice (or selecting practice), 0 otherwise
        REPLAY_FLAG = 0x0069BCBCL, // 1 byte, set to 1 when entering a replay, set to 0 when normal play starts, isn't cleared otherwise
        MUSIC_ROOM_TRACK = 0x006C7204L,
    };
};