#pragma once

#include <Windows.h>
#include <memoryapi.h>
#include <string>
#include <cstdint>

using TouhouAddress = std::uintptr_t;

int ReadProcessMemoryInt(HANDLE processHandle, TouhouAddress address, int size = 4);
float ReadProcessMemoryFloat(HANDLE processHandle, TouhouAddress address, int size = 4);
std::string ReadProcessMemoryString(HANDLE processHandle, TouhouAddress address, int size = 4);
