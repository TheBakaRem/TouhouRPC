#pragma once
#include "../DiscordRPC.h"
#include <string>
#include <Windows.h>
#include <memoryapi.h>
#include <TlHelp32.h>
#include <handleapi.h>


class TouhouBase
{
public:
	TouhouBase(PROCESSENTRY32W* pe32);
	~TouhouBase();

	inline bool isLinkedToProcess() { return this->linkedToProcess; };

	bool isStillRunning();

	virtual void readDataFromGameProcess() = 0;

	virtual void setGameName(std::string & name) = 0;
	virtual void setGameInfo(std::string & info) = 0;
	virtual void setLargeImageInfo(std::string & icon, std::string & text) = 0;
	virtual void setSmallImageInfo(std::string & icon, std::string & text) = 0;

protected:
	PROCESSENTRY32W* processEntry;
	HANDLE processHandle;

	int character;
	int characterSub;
	int stage;
	int difficulty;
	int gameState;
	int stageFrames;
	int gameStateFrames;
	int menuState;

private:
	bool linkedToProcess = false;
};

