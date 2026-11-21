#pragma once

#include <Windows.h>
#include <memoryapi.h>
#include <TlHelp32.h>
#include <handleapi.h>
#include <vector>
#include <memory>

#include "Touhou06.hpp"
#include "Touhou07.hpp"
#include "Touhou08.hpp"
#include "Touhou09.hpp"
#include "Touhou09_5.hpp"
#include "Touhou10.hpp"
#include "Touhou11.hpp"
#include "Touhou12.hpp"
#include "Touhou12_5.hpp"
#include "Touhou12_8.hpp"
#include "Touhou13.hpp"
#include "Touhou14.hpp"
#include "Touhou14_3.hpp"
#include "Touhou15.hpp"
#include "Touhou16.hpp"
#include "Touhou17.hpp"
#include "Touhou18.hpp"
#include "TouhouBase.hpp"

#include "Log.hpp"


std::unique_ptr<TouhouBase> initializeTouhouGame(bool initLogSilence);
