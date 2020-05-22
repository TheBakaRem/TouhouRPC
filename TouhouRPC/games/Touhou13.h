#pragma once
#include "TouhouMainGameBase.h"

namespace Touhou13
{

enum GameMode
{
	GAME_MODE_STANDARD = 1, // main menu, endings, staff roll, and normal gameplay
	GAME_MODE_REPLAY = 2,
	GAME_MODE_CLEAR = 3, // seems to appear after staff roll ends, but goes back to 1 on the name registration screen.
	GAME_MODE_PRACTICE = 4,
	GAME_MODE_SPELLPRACTICE = 5,
};

class Touhou13 :
	public TouhouMainGameBase
{
public:
	Touhou13(PROCESSENTRY32W const& pe32);
	~Touhou13();

	// Inherited from TouhouBase
	int64_t getClientId() const override { return 712836601407078410; };
	const char* getGameName() const override { return "Touhou 13 - Ten Desires"; }

	void readDataFromGameProcess() override;

	// Inherited from TouhouMainGameBase
	std::string getMidbossName() const override;
	std::string getBossName() const override;
	std::string const& getSpellCardName() const override;
	std::string const& getBGMName() const override;

protected:
	int spellCardID{ -1 };
	int gameMode{ 0 };
	int bgm{ 1 };

private:
	// addresses correct for v1.00c
	enum address
	{
		MENU_POINTER            = 0x004C22E0L, // offsets and data detailed in code
		CHARACTER               = 0x004BE7B8L,
		DIFFICULTY              = 0x004BE7C4L,
		BGM_STR                 = 0x004E13C0L,
		ENEMY_STATE             = 0x004BE824L,
		SPELL_CARD_ID           = 0x004BE7D4L,
		LIVES                   = 0x004BE7F4L,
		BOMBS                   = 0x004BE800L,
		STAGE                   = 0x004BE81CL,
		GAME_MODE               = 0x004DC670L,
		SCORE                   = 0x004BE7C0L,
		GAMEOVERS               = 0x004BE7C8L,
		PRACTICE_SELECT_FLAG    = 0x004BE830L, // is set to 16 when practice selected, 32 when spell practice selected, and 0 otherwise, on main menu.
	};
};

}