#include "TouhouBase.h"
#include <iostream>
#include <Windows.h>
#include <memoryapi.h>
#include <TlHelp32.h>
#include <handleapi.h>
#include <processthreadsapi.h>

// CHANGE INITIALIZER
TouhouBase::TouhouBase(PROCESSENTRY32W* pe32)
{
    // Default value initialization
    character = 0;
    characterSub = 0;
    difficulty = 0;
    stage = 0;
    gameState = 0;
    stageFrames = 0;
    gameStateFrames = 0;
    menuState = 1;
    
    // Process opening
    this->processEntry = pe32;
    processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, processEntry->th32ProcessID);
    if (processHandle != nullptr) {
        std::wcout << "Read access granted! " << std::endl;
        linkedToProcess = true;
    }
    else {
        std::wcout << "Error: Read access not granted. " << std::endl;
        linkedToProcess = false;
    }
}

TouhouBase::~TouhouBase()
{
	CloseHandle(processHandle);
	delete processEntry;
	processEntry = nullptr;
}

bool TouhouBase::isStillRunning()
{
    DWORD running;
    if (GetExitCodeProcess(processHandle, &running)) {
        if (running == STILL_ACTIVE) {
            return true;
        }
    }
    return false;
}
