#pragma once
#include "TouhouMainGameBase.h"

namespace Touhou14
{

enum GameMode
{
	GAME_MODE_STANDARD = 1, // main menu, endings, staff roll, and normal gameplay
	GAME_MODE_REPLAY = 2,
	GAME_MODE_CLEAR = 3, // seems to appear after staff roll ends, but goes back to 1 on the name registration screen.
	GAME_MODE_PRACTICE = 4,
	GAME_MODE_SPELLPRACTICE = 5,
};

class Touhou14 :
	public TouhouMainGameBase
{
public:
	Touhou14(PROCESSENTRY32W const& pe32);
	~Touhou14();

	// Inherited from TouhouBase
	int64_t getClientId() const override { return 709074475789844602; };
	const char* getGameName() const override { return "Touhou 14 - Double Dealing Character"; }

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
	// addresses correct for v1.00b
	enum address
	{
		MENU_POINTER            = 0x004DB6A4L, // offsets and data detailed in code
		CHARACTER               = 0x004F5828L, // the menu also remembers last chosen at 0x004DB6ACL
		SUB_CHARACTER           = 0x004F582CL, // the menu also remembers last chosen at 0x004DB6A8L
		DIFFICULTY              = 0x004F5834L, // the menu also remembers last chosen at 0x004D5984L
		BGM_STR                 = 0x004F9E00L,
		ENEMY_STATE             = 0x004F58ACL,
		SPELL_CARD_ID           = 0x004F5844L,
		LIVES                   = 0x004F5864L,
		BOMBS                   = 0x004F5870L,
		// STAGE                  = 0x004F58A4L, // also found at 0x004F58A8L // bgm checking makes stage unnecessary
		GAME_MODE               = 0x004DB6A0L,
		SCORE                   = 0x004F5830L,
		GAMEOVERS               = 0x004F5838L,

		// EXTRA_SELECT_FLAG       = 0x004F5834L, // unused, is set to 4 when extra was selected and on main menu.
		PRACTICE_SELECT_FLAG    = 0x004F58B8L, // is set to 16 when practice selected, 32 when spell practice selected, and 0 otherwise, on main menu.
	};
};

}