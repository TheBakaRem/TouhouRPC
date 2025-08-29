#pragma once
#include <string>
import Games;
import WindowsUtils;


class Touhou20 : public TouhouBase {
public:
    Touhou20(PROCESSENTRY32W const& pe32);

    // Inherited from TouhouBase
    int64_t getClientId() const override { return 1410229815122071724; };
    const char* getGameName() const override { return "Touhou 20 - Fossilized Wonders"; }

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
    bool seenMidboss{ false };

private:
    // addresses correct for v1.00a (According to title screen, not window title)
    //These are relative offsets. Add them to the module base to get absolute addresses
    enum address : TouhouAddress {
        CHARACTER = 0x001BA5F8L,
        STORYSTONE = 0x001BA5FCL,
        NARROWSTONE = 0x001BA600L,
        WIDESTONE = 0x001BA604L,
        SUBSTONE = 0X001BA608L,
        DIFFICULTY = 0x001BA7D0L,
        STAGE = 0x001BA7E4L,
        MENU_POINTER = 0x001C6124L, //Find address in memory matching ds, subtract -0x18. MENU_POINTER's value contains the result
        BGM_STR = 0x001BCC7CL,
        //ENEMY_STATE_POINTER = 0x00D5C568L,
        STAGE_STATE = 0x001BA7ECL,
        SPELL_CARD_ID = 0x001BA7F4L,
        LIVES = 0x001BA6A8L,
        BOMBS = 0x001BA6BCL,
        SCORE = 0x001BA5F0L,
        GAMEOVERS = 0x001BA7D8L,
        GAME_MODE = 0x001C6128L,
        PRACTICE_SELECT_FLAG = 0x001BA5D8L, // On main menu, is set to 1 if Practice mode is selected, 2 if Spell practice mode is selected, 0 otherwise.
    };
};
