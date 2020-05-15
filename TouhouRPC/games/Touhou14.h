#pragma once
#include "TouhouBase.h"

namespace Touhou14
{

enum GameState
{
	STATE_STAGE = 0,
	STATE_MIDBOSS = 1,
	STATE_BOSS = 2,
	STATE_GAMEOVER = 3,
	STATE_MENU = 4,
	STATE_ENDING = 5,
	STATE_CREDITS = 6,
};

enum Character
{
	CHAR_REIMU = 0,
	CHAR_MARISA = 1,
	CHAR_SAKUYA = 2,
};

enum SubCharacter
{
	SUBCHAR_A = 0,
	SUBCHAR_B = 1,
};

enum Difficulty
{
	DIFFICULTY_EASY = 0,
	DIFFICULTY_NORMAL = 1,
	DIFFICULTY_HARD = 2,
	DIFFICULTY_LUNATIC = 3,
	DIFFICULTY_EXTRA = 4,
};

enum GameMode
{
	GAME_MODE_STANDARD = 1, // main menu, endings, staff roll, and normal gameplay
	GAME_MODE_REPLAY = 2,
	GAME_MODE_CLEAR = 3, // seems to appear after staff roll ends, but goes back to 1 on the name registration screen.
	GAME_MODE_PRACTICE = 4,
	GAME_MODE_SPELLPRACTICE = 5,
};

class Touhou14 :
	public TouhouBase
{
public:
	Touhou14(PROCESSENTRY32W* pe32);
	~Touhou14();

	// Inherited from TouhouBase
	virtual void readDataFromGameProcess() override;
	virtual void setGameName(std::string& name) override;
	virtual void setGameInfo(std::string& info) override;
	virtual void setLargeImageInfo(std::string& icon, std::string& text) override;
	virtual void setSmallImageInfo(std::string& icon, std::string& text) override;

protected:
	int spellCardID{ 0 };
	int lives{ 0 };
	int bombs{ 0 };
	int gameMode{ 0 };
	int score{ 0 };
	int gameOvers{ 0 };

	int mainMenuSub{ -1 };
	int mainMenuDisplayStateA{ -1 };
	int mainMenuDisplayStateB{ -1 };
	int bgm{ 1 };

	bool showScoreInsteadOfRes = false;

private:
	// addresses correct for v1.00b
	enum address
	{
		MENU_POINTER            = 0x004DB6A4L,
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
	};
};

}