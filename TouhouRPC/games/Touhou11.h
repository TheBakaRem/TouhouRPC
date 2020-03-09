#pragma once
#include "TouhouBase.h"
class Touhou11 :
	public TouhouBase
{
public:
	Touhou11(PROCESSENTRY32W* pe32);
	~Touhou11();

	// Inherited from TouhouBase
	virtual void readDataFromGameProcess() override;
	virtual void setGameName(std::string & name) override;
	virtual void setGameInfo(std::string & info) override;
	virtual void setLargeImageInfo(std::string & icon, std::string & text) override;
	virtual void setSmallImageInfo(std::string & icon, std::string & text) override;

private:
	enum address {
		CHARACTER = 0x004A5710L,
		SUB_CHARACTER = 0x004A5714L,
		DIFFICULTY = 0x004A5720L,
		STAGE = 0x004A5728L,
		STAGE_FRAMES = 0x004A5734L,
		GAME_STATE = 0x004A5730L,
		GAME_STATE_FRAMES = 0x004A5738L,
		MENU_POINTER = 0x004A8ECC
	};
};

