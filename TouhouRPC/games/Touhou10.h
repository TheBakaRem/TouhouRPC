#pragma once
#include "TouhouMainGameBase.h"

namespace Touhou10
{

class Touhou10 :
	public TouhouMainGameBase
{
public:
	Touhou10(PROCESSENTRY32W const& pe32);
	~Touhou10();

	// Inherited from TouhouBase
	int64_t getClientId() const override { return 716759035571077171; };
	const char* getGameName() const override { return "Touhou 10 - Mountain of Faith"; }

	void readDataFromGameProcess() override;

	// Inherited from TouhouMainGameBase
	std::string getMidbossName() const override;
	std::string getBossName() const override;
	std::string const& getBGMName() const override;
	std::string getCustomResources() const override;

protected:
	int bgm{ 1 };
	float power{ 0.0f };

private:
	// addresses correct for v1.00a
	enum address {
		CHARACTER             = 0x00474C68L,
		SUB_CHARACTER         = 0x00474C6CL,
		DIFFICULTY            = 0x00474C74L,
		STAGE                 = 0x00474C7CL,
		STAGE_FRAMES          = 0x00474C88L,
		GAME_STATE            = 0x00474C84L,
		GAME_STATE_FRAMES     = 0x00474C8CL,
		MENU_POINTER          = 0x0047784CL, // sub menu selection at 0x30, in sub menu at 0xB0
		BGM_STR_1             = 0x0049669CL, // currently playing or stage theme when in stage
		PRACTICE_FLAG         = 0x00474CA0L, // 16 when practicing, 0 otherwise
		REPLAY_FLAG           = 0x00491C00L, // 2 when watching replay, 1 otherwise
		LIVES                 = 0x00474C70L,
		POWER                 = 0x00474C48L, // integer, divide by 20 to get actual power value
		SCORE                 = 0x00474C44L,
		GAMEOVERS             = 0x00474C90L,
	};
};

}