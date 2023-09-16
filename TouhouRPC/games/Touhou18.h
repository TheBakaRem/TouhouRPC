#pragma once
import Games;
#include <cstring>

class Touhou18 : public TouhouBase {
public:
    Touhou18(PROCESSENTRY32W const& pe32);

    // Inherited from TouhouBase
    int64_t getClientId() const override { return 823266075659599892; };
    const char* getGameName() const override { return "Touhou 18 - Unconnected Marketeers"; }

    void readDataFromGameProcess() override;

    // Inherited from TouhouBase
    std::string getMidbossName() const override;
    std::string getBossName() const override;
    std::string getSpellCardName() const override;
    std::string getBGMName() const override;
    std::string getCustomResources() const override;

protected:
    int spellCardID{ -1 };
    GameMode gameMode{ GAME_MODE_STANDARD };
    int bgm{ 1 };
    int money{ 0 };
    bool seenMidboss{ false };

private:
    // addresses correct for v1.00a
    enum address {
        CHARACTER = 0x004CCCF4L,
        DIFFICULTY = 0x004CCD00L,
        STAGE = 0x004CCCDCL,
        MENU_POINTER = 0x004CF43CL,
        BGM_STR = 0x0056D104L,
        ENEMY_STATE_POINTER = 0x004CF2E0L,
        STAGE_STATE = 0x004CCCE4L,
        SPELL_CARD_ID = 0x004CCD14L,
        LIVES = 0x004CCD48L,
        BOMBS = 0x004CCD58L,
        SCORE = 0x004CCCFCL,
        MONEY = 0x004CCD34L,
        GAMEOVERS = 0x004CCD04L,
        GAME_MODE = 0x004CF438L,
        PRACTICE_SELECT_FLAG = 0x004CCCC8L, // is set to 16 when practice selected, 32 when spell practice selected, and 0 otherwise, on main menu.
    };
};
