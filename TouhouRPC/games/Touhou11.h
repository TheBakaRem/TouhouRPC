#pragma once
#include "TouhouBase.h"
class Touhou11 :
	public TouhouBase
{
public:
	Touhou11(PROCESSENTRY32W const& pe32);
	~Touhou11();

	// Inherited from TouhouBase
	int64_t getClientId() const override { return 684365704220508169; };
	const char* getGameName() const override { return "Touhou 11 - Subterranean Animism"; }

	void readDataFromGameProcess() override;
	void setGameName(std::string & name) const override;
	void setGameInfo(std::string & info) const override;
	void setLargeImageInfo(std::string & icon, std::string & text) const override;
	void setSmallImageInfo(std::string & icon, std::string & text) const override;

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

