#pragma once
#include "TouhouBase.h"
#include <cstring>
class Touhou17 :
	public TouhouBase
{
public:
	Touhou17(PROCESSENTRY32W* pe32);
	~Touhou17();



	// Inherited from TouhouBase
	virtual void readDataFromGameProcess() override;
	virtual void setGameName(std::string& name) override;
	virtual void setGameInfo(std::string& info) override;
	virtual void setLargeImageInfo(std::string& icon, std::string& text) override;
	virtual void setSmallImageInfo(std::string& icon, std::string& text) override;

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
