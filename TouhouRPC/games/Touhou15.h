#pragma once
#include "TouhouBase.h"
class Touhou15 :
	public TouhouBase
{
public:
	Touhou15(PROCESSENTRY32W* pe32);
	~Touhou15();



	// Inherited from TouhouBase
	virtual void readDataFromGameProcess() override;
	virtual void setGameName(std::string& name) override;
	virtual void setGameInfo(std::string& info) override;
	virtual void setLargeImageInfo(std::string& icon, std::string& text) override;
	virtual void setSmallImageInfo(std::string& icon, std::string& text) override;

protected:
	int gameType; // Pointdevice or Legacy
	int retryCount; // Game retry count for Pointdevice
	int chapterRetryCount; // Chapter retry count for Pointdevice

private:
	enum address {
		CHARACTER = 0x004E7404L,
		GAME_TYPE = 0x004E9BECL,
		DIFFICULTY = 0x004E7410L,
		STAGE = 0x004E73F0L,
		STAGE_CHAPTER = 0x004E73F8L,
		STAGE_CHAPTER_FRAMES = 0x004E7400L,
		GLOBAL_RETRY_COUNT = 0x004E7594L,
		CHAPTER_RETRY_COUNT = 0x004E75B8L,
		MENU_POINTER = 0x004E9BE0
	};
};

