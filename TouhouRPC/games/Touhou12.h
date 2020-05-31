#pragma once
#include "TouhouMainGameBase.h"

namespace Touhou12
{

class Touhou12 :
	public TouhouMainGameBase
{
public:
	Touhou12(PROCESSENTRY32W const& pe32);
	~Touhou12();

	// Inherited from TouhouBase
	int64_t getClientId() const override { return 716678778755219508; };
	const char* getGameName() const override { return "Touhou 12 - Undefined Fantastic Object"; }

	void readDataFromGameProcess() override;

	// Inherited from TouhouMainGameBase
	std::string getMidbossName() const override;
	std::string getBossName() const override;
	std::string const& getBGMName() const override;

protected:
	int bgm{ 1 };
	float power{ 0.0f };

private:
	enum address {
		CHARACTER             = 0x004B0C90L,
		SUB_CHARACTER         = 0x004B0C94L,
		DIFFICULTY            = 0x004B0CA8L,
		STAGE                 = 0x004B0CB0L,
		STAGE_FRAMES          = 0x004B0CBCL,
		GAME_STATE            = 0x004B0CB8L,
		GAME_STATE_FRAMES     = 0x004B0CC0L,
		MENU_POINTER          = 0x004B4530L, // sub menu selection at 0x34, in sub menu at 0xB4
		ENEMY_STATE           = 0x004B43B8L, // boss flag at 0x1594
		BGM_STR_1             = 0x004D3658L, // currently playing outside of stage, or stage theme in-stage
		BGM_STR_2             = 0x004D3758L, // boss theme
		PRACTICE_FLAG         = 0x004B0CE0L, // 16 when practicing, 0 otherwise
		REPLAY_FLAG           = 0x004CF468L, // 2 when watching replay, 0 otherwise
		LIVES                 = 0x004B0C98L,
		BOMBS                 = 0x004B0CA0L,
		SCORE                 = 0x004B0C44L,
		GAMEOVERS             = 0x004B0CC4L,
		PAUSE_POINTER         = 0x004D4754L, // ok dunno what this obj actually is but at 0x34 it says whether game is paused or not
	};
};

}