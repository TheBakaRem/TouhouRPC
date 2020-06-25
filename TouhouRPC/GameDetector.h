#pragma once

#include <string>
#include <Windows.h>
#include <memoryapi.h>
#include <TlHelp32.h>
#include <handleapi.h>

// Games includes
#include "games/TouhouBase.h"

#include "games/Touhou06.h"
#include "games/Touhou07.h"
#include "games/Touhou08.h"
#include "games/Touhou09.h"
#include "games/Touhou09_5.h"
#include "games/Touhou10.h"
#include "games/Touhou11.h"
#include "games/Touhou12.h"
#include "games/Touhou12_8.h"
#include "games/Touhou13.h"
#include "games/Touhou14.h"
#include "games/Touhou15.h"
#include "games/Touhou16.h"
#include "games/Touhou17.h"

enum class SupportedGame
{
	EoSD_6,
	PCB_7,
	IN_8,
	PoFV_9,
	StB_9_5,
	MoF_10,
	SA_11,
	UFO_12,
	GFW_12_8,
	TD_13,
	DDC_14,
	LoLK_15,
	HSiFS_16,
	WBaWC_17,

	Invalid,
};

struct ProcessNameGamePair
{
	SupportedGame game;
	const wchar_t* processName;
};

const int PROCESS_NAME_LIST_SIZE = 19;

// Executables name list and associated game
static const ProcessNameGamePair processNameList[PROCESS_NAME_LIST_SIZE] =
{
	{ SupportedGame::EoSD_6, L"eosd.exe" },
	{ SupportedGame::EoSD_6, L"th06e.exe" },
	{ SupportedGame::EoSD_6, L"“Œ•ûg–‚‹½.exe" },
	{ SupportedGame::EoSD_6, L"東方紅魔郷.exe" },
	{ SupportedGame::PCB_7, L"th07.exe" },
	{ SupportedGame::PCB_7, L"th07e.exe" },
	{ SupportedGame::IN_8, L"th08.exe" },
	{ SupportedGame::IN_8, L"th08e.exe" },
	{ SupportedGame::PoFV_9, L"th09.exe" },
	{ SupportedGame::StB_9_5, L"th095.exe" },
	{ SupportedGame::MoF_10, L"th10.exe" },
	{ SupportedGame::SA_11, L"th11.exe" },
	{ SupportedGame::UFO_12, L"th12.exe" },
	{ SupportedGame::GFW_12_8, L"th128.exe" },
	{ SupportedGame::TD_13, L"th13.exe" },
	{ SupportedGame::DDC_14, L"th14.exe" },
	{ SupportedGame::LoLK_15, L"th15.exe" },
	{ SupportedGame::HSiFS_16, L"th16.exe" },
	{ SupportedGame::WBaWC_17, L"th17.exe" },
};

std::unique_ptr<TouhouBase> initializeTouhouGame();