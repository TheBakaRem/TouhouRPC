#pragma once
#include "TouhouMainGameBase.h"
#include <cstring>

namespace Touhou17
{

enum GameMode
{
	GAME_MODE_STANDARD = 1, // main menu, endings, staff roll, and normal gameplay
	GAME_MODE_REPLAY = 2,
	GAME_MODE_CLEAR = 3, // seems to appear after staff roll ends, but goes back to 1 on the name registration screen.
	GAME_MODE_PRACTICE = 4,
	GAME_MODE_SPELLPRACTICE = 5,
};

class Touhou17 :
	public TouhouMainGameBase
{
public:
	Touhou17(PROCESSENTRY32W const& pe32);
	~Touhou17();



	// Inherited from TouhouBase
	int64_t getClientId() const override { return 712016062048370689; };
	const char* getGameName() const override { return "Touhou 17 - Wily Beast and Weakest Creature"; }

	void readDataFromGameProcess() override;

	// Inherited from TouhouMainGameBase
	std::string getStageName() const override;
	std::string getMidbossName() const override;
	std::string getBossName() const override;
	std::string const& getSpellCardName() const override;
	std::string const& getBGMName() const override;

protected:
	int spellCardID{ -1 };
	int gameMode{ 0 };
	int bgm{ 1 };

private:
	enum address {
		CHARACTER                   = 0x004B59F4L,
		SUB_CHARACTER               = 0x004B59F8L,
		DIFFICULTY                  = 0x004B5A00L,
		STAGE                       = 0x004B59DCL,
		MENU_POINTER                = 0x004B77F0L,
		BGM_STR                     = 0x00526B08L,
		ENEMY_STATE_POINTER         = 0x004B76A0L,
		SPELL_CARD_ID               = 0x004B5A14L,
		LIVES                       = 0x004B5A40L,
		BOMBS                       = 0x004B5A4CL,
		SCORE                       = 0x004B59FCL,
		GAMEOVERS                   = 0x004B5A04L,
		GAME_MODE                   = 0x004B77ECL,
		EXTRA_FLAG                  = 0x004B5A00L, // is set to 4 when extra was selected, 1 otherwise
		PRACTICE_SELECT_FLAG        = 0x004B59C8L, // is set to 16 when practice selected, 32 when spell practice selected, and 0 otherwise, on main menu.
	};
};

}