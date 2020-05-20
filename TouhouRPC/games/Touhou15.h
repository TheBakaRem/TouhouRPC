#pragma once
#include "TouhouMainGameBase.h"

namespace Touhou15
{

enum GameMode
{
	GAME_MODE_STANDARD = 1, // main menu, endings, staff roll, and normal gameplay
	GAME_MODE_REPLAY = 2,
	GAME_MODE_CLEAR = 3, // seems to appear after staff roll ends, but goes back to 1 on the name registration screen.
	GAME_MODE_PRACTICE = 4,
	GAME_MODE_SPELLPRACTICE = 5,
};

enum GameType
{
	POINT_DEVICE,
	LEGACY,
};

enum ExtraBossState
{
	JUNKO_AND_HECATIA,
	JUNKO,
	HECATIA,
};

class Touhou15 :
	public TouhouMainGameBase
{
public:
	Touhou15(PROCESSENTRY32W const& pe32);
	~Touhou15();



	// Inherited from TouhouBase
	int64_t getClientId() const override { return 712067916862062633; };
	const char* getGameName() const override { return "Touhou 15 - Legacy of Lunatic Kingdom"; }

	void readDataFromGameProcess() override;
	void setSmallImageInfo(std::string& icon, std::string& text) const override;

	// Inherited from TouhouMainGameBase
	std::string getStageName() const override;
	std::string getMidbossName() const override;
	std::string getBossName() const override;
	std::string const& getBGMName() const override;
	std::string getCustomResources() const override;

	bool stateHasChangedSinceLastCheck() override;

protected:
	int gameMode{ GAME_MODE_STANDARD };
	int bgm{ 0 };
	GameType gameType{ POINT_DEVICE };
	int retryCount{ 0 }; // Game retry count for Pointdevice
	int chapterRetryCount{ 0 }; // Chapter retry count for Pointdevice

	int stageChapter{ 0 };
	int currentStageChapterFrames{ 0 };
	ExtraBossState extraBossState{ ExtraBossState::JUNKO_AND_HECATIA };
	ExtraBossState prevExtraBossState{ ExtraBossState::JUNKO_AND_HECATIA }; // for checking if it changed since last time

private:
	enum address {
		CHARACTER                  = 0x004E7404L,
		GAME_TYPE                  = 0x004E9BECL,
		DIFFICULTY                 = 0x004E7410L,
		STAGE                      = 0x004E73F0L,
		STAGE_CHAPTER              = 0x004E73F8L,
		STAGE_CHAPTER_FRAMES       = 0x004E7400L,
		GLOBAL_RETRY_COUNT         = 0x004E7594L,
		CHAPTER_RETRY_COUNT        = 0x004E75B8L,
		MENU_POINTER               = 0x004E9BE0L,
		LIVES                      = 0x004E7450L,
		BOMBS                      = 0x004E745CL,
		SCORE                      = 0x004E740CL,
		GAMEOVERS                  = 0x004E7414L,
		GAME_MODE                  = 0x004E9BDCL,
		EXTRA_FLAG                 = 0x004E7410L, // is set to 4 when extra was selected, 1 otherwise
		PRACTICE_SELECT_FLAG       = 0x004E7794L, // is set to 16 when practice selected, 32 when spell practice selected, and 0 otherwise, on main menu.
		BGM_STR                    = 0x00520224L,
	};
};

}