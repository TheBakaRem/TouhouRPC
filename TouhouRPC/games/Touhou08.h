#pragma once
#include "TouhouBase.h"

class Touhou08 :
	public TouhouBase
{
public:
	Touhou08(PROCESSENTRY32W const& pe32);
	~Touhou08();

	// Inherited from TouhouBase
	int64_t getClientId() const override { return 684365704220508169; };
	const char* getGameName() const override { return "Touhou 08 - Imperishable Night"; }

	void readDataFromGameProcess() override;

	void setGameName(std::string& name) const override;
	void setGameInfo(std::string& info) const override;
	void setLargeImageInfo(std::string& icon, std::string& text) const override;
	void setSmallImageInfo(std::string& icon, std::string& text) const override;

protected:
	bool isBoss;
	int bossStateChange; // how many times isBoss has changed
	int oldStageFrames; // to compare with stageFrames, >0 -> in-game
	int spellCardID;

	// Stages in IN are complicated
	const std::string stageName[9] = { // Cao Minh BIG BRAIN
		"Stage 1",
		"Stage 2",
		"Stage 3",
		"Stage 4A",
		"Stage 4B",
		"Stage 5",
		"Stage 6A",
		"Stage 6B",
		"Extra Stage"
	};

	// screw stage 5, 6B, extra for having a different midboss than boss
	const std::string midBossName[9] = {
		"Wriggle",
		"Mystia",
		"Keine",
		"Reimu",
		"Marisa",
		"Tewi",
		"Eirin",
		"Eirin",
		"Keine"
	};
	const std::string bossName[9] = {
		"Wriggle",
		"Mystia",
		"Keine",
		"Reimu",
		"Marisa",
		"Reisen",
		"Eirin",
		"Kaguya",
		"Mokou"
	};

private:
	enum address {
		CHARACTER = 0x0164D0B1,
		DIFFICULTY = 0x0160F538,
		SCORE = 0x0160F510,
		STAGE = 0x004E4850,
		BOSS_APPEARANCE = 0x018B89B8,
		SPELL_CARD_ID = 0x004EA678,
		STAGE_FRAMES = 0x0164D0AC,
		PLAYING_MUSIC = 0x018BCB70, // determine if we're practicing a spell or not
		// if either changes, we are sure to be in music room
		MUSIC_ROOM_STATE_1 = 0x017CF53C, // chosen but not playing
		MUSIC_ROOM_STATE_2 = 0x017CF540, // actually playing
	};
};