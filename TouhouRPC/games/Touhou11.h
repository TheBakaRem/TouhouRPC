#pragma once
#include "TouhouMainGameBase.h"

namespace Touhou11
{

class Touhou11 :
	public TouhouMainGameBase
{
public:
	Touhou11(PROCESSENTRY32W const& pe32);
	~Touhou11();

	// Inherited from TouhouBase
	int64_t getClientId() const override { return 712067875757752442; };
	const char* getGameName() const override { return "Touhou 11 - Subterranean Animism"; }

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
	enum address {
		CHARACTER             = 0x004A5710L,
		SUB_CHARACTER         = 0x004A5714L,
		DIFFICULTY            = 0x004A5720L,
		STAGE                 = 0x004A5728L,
		STAGE_FRAMES          = 0x004A5734L,
		GAME_STATE            = 0x004A5730L,
		GAME_STATE_FRAMES     = 0x004A5738L,
		MENU_POINTER          = 0x004A8ECCL, // display state at 0x1C, sub menu selection at 0x30, in sub menu at 0xB0
		ENEMY_STATE           = 0x004A8D58L,
		BGM_STR               = 0x004C7F8CL,
		PRACTICE_FLAG         = 0x004A5758L, // 16 when practicing, 0 otherwise
		REPLAY_FLAG           = 0x004C3250L, // 2 when watching replay, 1 otherwise
		LIVES                 = 0x004A5718L,
		POWER                 = 0x004A56E8L, // integer, divide by 20 to get actual power value
		SCORE                 = 0x004A56E4L,
		GAMEOVERS             = 0x004A573CL,
	};
};

}