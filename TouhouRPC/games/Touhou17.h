#pragma once
import Games;
#include <cstring>

class Touhou17 : public TouhouBase {
public:
    Touhou17(PROCESSENTRY32W const& pe32);
    ~Touhou17();

    // Inherited from TouhouBase
    int64_t getClientId() const override { return 712071166143234109; };
    const char* getGameName() const override { return "Touhou 17 - Wily Beast and Weakest Creature"; }

    void readDataFromGameProcess() override;

    // Inherited from TouhouBase
    std::string getMidbossName() const override;
    std::string getBossName() const override;
    std::string getSpellCardName() const override;
    std::string getBGMName() const override;

protected:
    int spellCardID{ -1 };
    GameMode gameMode{ GAME_MODE_STANDARD };
    int bgm{ 1 };

private:
    // addresses correct for v1.00b
    enum address {
        CHARACTER = 0x004B59F4L,
        SUB_CHARACTER = 0x004B59F8L,
        DIFFICULTY = 0x004B5A00L,
        STAGE = 0x004B59DCL,
        MENU_POINTER = 0x004B77F0L,
        BGM_STR = 0x00526B08L,
        ENEMY_STATE_POINTER = 0x004B76A0L,
        STAGE_STATE = 0x004B59E4L,
        SPELL_CARD_ID = 0x004B5A14L,
        LIVES = 0x004B5A40L,
        BOMBS = 0x004B5A4CL,
        SCORE = 0x004B59FCL,
        GAMEOVERS = 0x004B5A04L,
        GAME_MODE = 0x004B77ECL,
        PRACTICE_SELECT_FLAG = 0x004B59C8L, // is set to 16 when practice selected, 32 when spell practice selected, and 0 otherwise, on main menu.
    };
};
