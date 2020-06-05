#pragma once
#include "TouhouMainGameBase.h"

namespace Touhou08
{

class Touhou08 :
	public TouhouMainGameBase
{
public:
	Touhou08(PROCESSENTRY32W const& pe32);
	~Touhou08();

	// Inherited from TouhouBase
	int64_t getClientId() const override { return 712068017172905984; };
	const char* getGameName() const override { return "Touhou 08 - Imperishable Night"; }

	void readDataFromGameProcess() override;

	// Inherited from TouhouMainGameBase
	std::string getStageName() const override;
	std::string getMidbossName() const override;
	std::string getBossName() const override;
	std::string const& getSpellCardName() const override;
	std::string const& getBGMName() const override;

protected:
	bool isBoss;
	int bossStateChange; // how many times isBoss has changed
	int oldStageFrames; // to compare with stageFrames, >0 -> in-game
	int spellCardID;
	int bgm{ 1 }; // correct in music room only

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
		"Wriggle Nightbug",
		"Mystia Lorelei",
		"Keine Kamishirasawa",
		"Reimu Hakurei",
		"Marisa Kirisame",
		"Tewi Inaba",
		"Eirin Yagokoro",
		"Eirin Yagokoro",
		"Keine Kamishirasawa"
	};
	const std::string bossName[9] = {
		"Wriggle Nightbug",
		"Mystia Lorelei",
		"Keine Kamishirasawa",
		"Reimu Hakurei",
		"Marisa Kirisame",
		"Reisen Udongein Inaba",
		"Eirin Yagokoro",
		"Kaguya Houraisan",
		"Fujiwara no Mokou"
	};

private:
	// addresses correct for v1.00d
	enum address {
		CHARACTER              = 0x0164D0B1, // 2 byte
		DIFFICULTY             = 0x0160F538, // 1 byte
		SCORE                  = 0x0160F510,
		STAGE                  = 0x004E4850,
		BOSS_APPEARANCE        = 0x018B89B8, // 1 byte
		SPELL_CARD_ID          = 0x004EA678, // 1 byte
		STAGE_FRAMES           = 0x0164D0AC,
		MUSIC_ROOM_CURSOR      = 0x017CF53CL,
		MUSIC_ROOM_TRACK       = 0x017CF540L, // the actually playing track

		MENU_MODE              = 0x017CE8B0L, // 1 on most menus, 2 in-game, 5 in player data, 8 in music room

		// 1 byte bitset
		// when in practice mode (including on main menu), bit 0 is set
		// when on title screen demo, bits 1 and 3 are set (3 for being a replay)
		// when unpaused, bit 2 is set
		// when in replay, bit 3 is set
		// when the boss is present in spell practice, bit 8 is set. this is kind of unreliable since it goes away if the boss is killed
		// otherwise, none set
		STAGE_MODE             = 0x0164D0B4L,
		STAGE_MODE_PRACTICE_FLAG = 1,
		STAGE_MODE_DEMO_FLAG = 2,
		STAGE_MODE_PAUSE_FLAG = 4,
		STAGE_MODE_REPLAY_FLAG = 8,
		STAGE_MODE_SPELL_PRACTICE_FLAG = 128,

		PLAYER_POINTER         = 0x0160F510L, // score at offset 00 (int); lives at offset 74 (float); bombs at offset 80 (float); game overs offset 28 (byte)
	};
};

}