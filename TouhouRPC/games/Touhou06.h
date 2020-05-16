#pragma once
#include "TouhouBase.h"
class Touhou06 :
	public TouhouBase
{
public:
	Touhou06(PROCESSENTRY32W const& pe32);
	~Touhou06();



	// Inherited from TouhouBase
	int64_t getClientId() const override { return 684365704220508169; };
	const char* getGameName() const override { return "Touhou 06 - Embodiment of Scarlet Devil"; }

	virtual void readDataFromGameProcess() override;
	virtual void setGameName(std::string& name) const override;
	virtual void setGameInfo(std::string& info) const override;
	virtual void setLargeImageInfo(std::string& icon, std::string& text) const override;
	virtual void setSmallImageInfo(std::string& icon, std::string& text) const override;

protected:
	int gameState2;

private:
	enum address {
		CHARACTER = 0x0069D4BDL,
		SUBWEAPON = 0x0069D4BEL,
		DIFFICULTY = 0x0069BCB0L,
		STAGE = 0x00487B48L,
		GAME_STATE = 0x004B974CL,
		GAME_STATE_2 = 0x0069BC57L,
		CHECK_IN_MENU_VALUE = 0x006DC8F8L, // If != 0, we are in the menu 
		MENU_STATE = 0x006DC8B0L
	};
};

