#pragma once
#include "../DiscordRPC.h"
#include "GameStrings.h"
#include <string>
#include <Windows.h>
#include <memoryapi.h>
#include <TlHelp32.h>
#include <handleapi.h>


class TouhouBase
{
public:
	TouhouBase(PROCESSENTRY32W const& pe32);
	virtual ~TouhouBase();

	bool isLinkedToProcess() const { return linkedToProcess; };

	bool isStillRunning() const;

	// Methods to override
	virtual int64_t getClientId() const = 0;
	virtual const char* getGameName() const = 0;

	virtual void readDataFromGameProcess() = 0;

	virtual void setGameName(std::string & name) const = 0;
	virtual void setGameInfo(std::string & info) const = 0;
	virtual void setLargeImageInfo(std::string & icon, std::string & text) const = 0;
	virtual void setSmallImageInfo(std::string & icon, std::string & text) const = 0;

protected:
	HANDLE processHandle;

	int character{ 0 };
	int characterSub{ 0 };
	int stage{ 0 };
	int difficulty{ 0 };
	int gameState{ 0 };
	int stageFrames{ 0 };
	int gameStateFrames{ 0 };
	int menuState{ 1 };

private:
	bool linkedToProcess = false;
};

