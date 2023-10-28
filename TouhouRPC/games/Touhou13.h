#pragma once
import Games;
import WindowsUtils;

class Touhou13 : public TouhouBase {
public:
    Touhou13(PROCESSENTRY32W const& pe32);
    ~Touhou13();

    // Inherited from TouhouBase
    int64_t getClientId() const override { return 712836601407078410; };
    const char* getGameName() const override { return "Touhou 13 - Ten Desires"; }

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
    // addresses correct for v1.00c
    enum address : TouhouAddress {
        MENU_POINTER = 0x004C22E0L, // offsets and data detailed in code
        CHARACTER = 0x004BE7B8L,
        DIFFICULTY = 0x004BE7C4L,
        BGM_STR = 0x004E13C0L,
        ENEMY_STATE = 0x004BE824L,
        SPELL_CARD_ID = 0x004BE7D4L,
        LIVES = 0x004BE7F4L,
        BOMBS = 0x004BE800L,
        STAGE = 0x004BE81CL,
        GAME_MODE = 0x004DC670L,
        SCORE = 0x004BE7C0L,
        GAMEOVERS = 0x004BE7C8L,
        PRACTICE_SELECT_FLAG = 0x004BE830L, // is set to 16 when practice selected, 32 when spell practice selected, and 0 otherwise, on main menu.
    };
};

