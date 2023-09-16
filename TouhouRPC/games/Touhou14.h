#pragma once
import Games;

class Touhou14 : public TouhouBase {
public:
    Touhou14(PROCESSENTRY32W const& pe32);
    ~Touhou14();

    // Inherited from TouhouBase
    int64_t getClientId() const override { return 709074475789844602; };
    const char* getGameName() const override { return "Touhou 14 - Double Dealing Character"; }

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
        MENU_POINTER = 0x004DB6A4L, // offsets and data detailed in code
        CHARACTER = 0x004F5828L, // the menu also remembers last chosen at 0x004DB6ACL
        SUB_CHARACTER = 0x004F582CL, // the menu also remembers last chosen at 0x004DB6A8L
        DIFFICULTY = 0x004F5834L, // the menu also remembers last chosen at 0x004D5984L
        BGM_STR = 0x004F9E00L,
        ENEMY_STATE = 0x004F58ACL,
        SPELL_CARD_ID = 0x004F5844L,
        LIVES = 0x004F5864L,
        BOMBS = 0x004F5870L,
        STAGE = 0x004F58A4L, // also found at 0x004F58A8L
        GAME_MODE = 0x004DB6A0L,
        SCORE = 0x004F5830L,
        GAMEOVERS = 0x004F5838L,

        PRACTICE_SELECT_FLAG = 0x004F58B8L, // is set to 16 when practice selected, 32 when spell practice selected, and 0 otherwise, on main menu.
    };
};
