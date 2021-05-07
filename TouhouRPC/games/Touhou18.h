#pragma once
#include "TouhouMainGameBase.h"
#include <cstring>

namespace Touhou18
{

enum GameMode
{
	GAME_MODE_STANDARD = 1, // main menu, endings, staff roll, and normal gameplay
	GAME_MODE_REPLAY = 2,
	GAME_MODE_CLEAR = 3, // seems to appear after staff roll ends, but goes back to 1 on the name registration screen.
	GAME_MODE_PRACTICE = 4,
	GAME_MODE_SPELLPRACTICE = 5,
};

class Touhou18Trial :
	public TouhouMainGameBase
{
public:
	Touhou18Trial(PROCESSENTRY32W const& pe32);


	// Inherited from TouhouBase
	int64_t getClientId() const override { return 823266075659599892; };
	const char* getGameName() const override { return "Touhou 18 - Unconnected Marketeers - Trial"; }

	void readDataFromGameProcess() override;

	// Inherited from TouhouMainGameBase
	std::string getMidbossName() const override;
	std::string getBossName() const override;
	std::string const& getBGMName() const override;

protected:
	int spellCardID{ -1 };
	int gameMode{ 0 };
	int bgm{ 1 };
	bool seenMidboss{ false };

private:
	// addresses correct for v0.02a
	enum address {
		CHARACTER                   = 0x004C112CL,
		DIFFICULTY                  = 0x004C1138L,
		STAGE                       = 0x004C1114L,
		MENU_POINTER                = 0x004C338CL,
		BGM_STR                     = 0x005442CCL,
		ENEMY_STATE_POINTER         = 0x004C3230L,
		STAGE_STATE                 = 0x004C111CL,
		LIVES                       = 0x004C1180L,
		BOMBS                       = 0x004C1190L,
		SCORE                       = 0x004C1134L,
		GAMEOVERS                   = 0x004C113CL,
		GAME_MODE                   = 0x004C3388L,
	};
};

class Touhou18 :
	public TouhouMainGameBase
{
public:
	Touhou18(PROCESSENTRY32W const& pe32);


	// Inherited from TouhouBase
	int64_t getClientId() const override { return 823266075659599892; };
	const char* getGameName() const override { return "Touhou 18 - Unconnected Marketeers"; }

	void readDataFromGameProcess() override;

	// Inherited from TouhouMainGameBase
	std::string getMidbossName() const override;
	std::string getBossName() const override;
	std::string const& getSpellCardName() const override;
	std::string const& getBGMName() const override;
	std::string getCustomResources() const override;

protected:
	int spellCardID{ -1 };
	int gameMode{ 0 };
	int bgm{ 1 };
	int money{ 0 };
	bool seenMidboss{ false };

private:
	// addresses correct for v1.00a
	enum address
	{
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

}