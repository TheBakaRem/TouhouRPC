#include "TouhouBase.h"
#include <iostream>
#include <Windows.h>
#include <memoryapi.h>
#include <TlHelp32.h>
#include <handleapi.h>
#include <processthreadsapi.h>

// CHANGE INITIALIZER
TouhouBase::TouhouBase(PROCESSENTRY32W const& pe32)
{    
    // Process opening
    processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, pe32.th32ProcessID);
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
}

bool TouhouBase::isStillRunning() const
{
    DWORD running;
    return GetExitCodeProcess(processHandle, &running) && running == STILL_ACTIVE;
}

bool TouhouBase::stateHasChangedSinceLastCheck()
{
    bool changed = prevStage != stage;    
    prevStage = stage;
    return changed;
}