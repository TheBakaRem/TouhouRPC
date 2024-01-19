#pragma once
#include <string>
import Games;
import WindowsUtils;


class Touhou16 : public TouhouBase {
public:
    Touhou16(PROCESSENTRY32W const& pe32);
    ~Touhou16();

    // Inherited from TouhouBase
    int64_t getClientId() const override { return 712067956481458197; };
    const char* getGameName() const override { return "Touhou 16 - Hidden Stars in Four Seasons"; }

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
    // addresses correct for v1.00a
    enum address : TouhouAddress {
        CHARACTER = 0x004A57A4L,
        SUB_CHARACTER = 0x004A57ACL,
        DIFFICULTY = 0x004A57B4L,
        STAGE = 0x004A5790,
        MENU_POINTER = 0x004A6F20L,
        BGM_STR = 0x004DC0F0L,
        ENEMY_STATE_POINTER = 0x004A6D98L, // State pointer, add 0x1BC8 to get the boss state
        SPELL_CARD_ID = 0x004A57C8,
        LIVES = 0x004A57F4L,
        BOMBS = 0x004A5800L,
        SCORE = 0x004A57B0L,
        GAMEOVERS = 0x004A57B8L,
        GAME_MODE = 0x004A6F1CL,
        PRACTICE_SELECT_FLAG = 0x004A5BECL, // is set to 16 when practice selected, 32 when spell practice selected, and 0 otherwise, on main menu.
    };
};
