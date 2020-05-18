#pragma once
#include "TouhouMainGameBase.h"

namespace Touhou16
{

enum GameMode
{
	GAME_MODE_STANDARD = 1, // main menu, endings, staff roll, and normal gameplay
	GAME_MODE_REPLAY = 2,
	GAME_MODE_CLEAR = 3, // seems to appear after staff roll ends, but goes back to 1 on the name registration screen.
	GAME_MODE_PRACTICE = 4,
	GAME_MODE_SPELLPRACTICE = 5,
};

class Touhou16 :
	public TouhouMainGameBase
{
public:
	Touhou16(PROCESSENTRY32W const& pe32);
	~Touhou16();



	// Inherited from TouhouBase
	int64_t getClientId() const override { return 712067956481458197; };
	const char* getGameName() const override { return "Touhou 16 - Hidden Stars in Four Seasons"; }

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
	// addresses correct for v1.00a
	enum address {
		CHARACTER                  = 0x004A57A4L,
		SUB_CHARACTER              = 0x004A57ACL,
		DIFFICULTY                 = 0x004A57B4L,
		STAGE                      = 0x004A5790,
		MENU_POINTER               = 0x004A6F20L,
		BGM_STR                    = 0x004DC0F0L,
		ENEMY_STATE_POINTER        = 0x004A6D98L, // State pointer, add 0x1BC8 to get the boss state
		SPELL_CARD_ID              = 0x004A57C8,
		LIVES                      = 0x004A57F4L,
		BOMBS                      = 0x004A5800L,
		SCORE                      = 0x004A57B0L,
		GAMEOVERS                  = 0x004A57B8L,
		GAME_MODE                  = 0x004A6F1CL,
		EXTRA_FLAG                 = 0x004A57B4L, // is set to 4 when extra was selected, 1 otherwise
		PRACTICE_SELECT_FLAG       = 0x004A5BECL, // is set to 16 when practice selected, 32 when spell practice selected, and 0 otherwise, on main menu.
	};
};

}