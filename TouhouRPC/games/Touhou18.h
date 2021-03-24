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

}