#pragma once
#include "TouhouBase.h"
class Touhou15 :
	public TouhouBase
{
public:
	Touhou15(PROCESSENTRY32W const& pe32);
	~Touhou15();



	// Inherited from TouhouBase
	int64_t getClientId() const override { return 684365704220508169; };
	const char* getGameName() const override { return "Touhou 15 - Legacy of Lunatic Kingdom"; }

	void readDataFromGameProcess() override;
	void setGameName(std::string& name) const override;
	void setGameInfo(std::string& info) const override;
	void setLargeImageInfo(std::string& icon, std::string& text) const override;
	void setSmallImageInfo(std::string& icon, std::string& text) const override;

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

