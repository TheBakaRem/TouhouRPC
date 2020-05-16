#pragma once
#include "TouhouBase.h"

class Touhou16 :
	public TouhouBase
{
public:
	Touhou16(PROCESSENTRY32W const& pe32);
	~Touhou16();



	// Inherited from TouhouBase
	int64_t getClientId() const override { return 684365704220508169; };
	const char* getGameName() const override { return "Touhou 16 - Hidden Stars in Four Seasons"; }

	void readDataFromGameProcess() override;
	void setGameName(std::string& name) const override;
	void setGameInfo(std::string& info) const override;
	void setLargeImageInfo(std::string& icon, std::string& text) const override;
	void setSmallImageInfo(std::string& icon, std::string& text) const override;

protected:
	int spellCardID;

private:
	enum address {
		CHARACTER = 0x004A57A4L,
		SUB_CHARACTER = 0x004A57ACL,
		DIFFICULTY = 0x004A57B4L,
		STAGE = 0x004A5790,
		MENU_POINTER = 0x004A6F20L,
		BGM_STR = 0x004DC0F0L,
		ENEMY_STATE_POINTER = 0x004A6D98L, // State pointer, add 0x1BC8 to get the boss state
		SPELL_CARD_ID = 0x004A57C8
	};
};


