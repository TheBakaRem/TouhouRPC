#pragma once
#include "TouhouBase.h"
#include <cstring>
class Touhou17 :
	public TouhouBase
{
public:
	Touhou17(PROCESSENTRY32W const& pe32);
	~Touhou17();



	// Inherited from TouhouBase
	int64_t getClientId() const override { return 684365704220508169; };
	const char* getGameName() const override { return "Touhou 17 - Wily Beast and Weakest Creature"; }

	void readDataFromGameProcess() override;
	void setGameName(std::string& name) const override;
	void setGameInfo(std::string& info) const override;
	void setLargeImageInfo(std::string& icon, std::string& text) const override;
	void setSmallImageInfo(std::string& icon, std::string& text) const override;

protected:
	

private:
	enum address {
		CHARACTER = 0x004B59F4L,
		SUB_CHARACTER = 0x004B59F8L,
		DIFFICULTY = 0x004B5A00L,
		STAGE = 0x004B59DCL,
		MENU_POINTER = 0x004B77F0L,
		BGM_STR = 0x00526B08L,
		ENEMY_STATE_POINTER = 0x004B76A0L
	};
};
