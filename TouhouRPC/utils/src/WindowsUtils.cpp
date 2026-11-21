#include "WindowsUtils.hpp"

#include <memory>
#include <memoryapi.h>
#include <string>

#include "Log.hpp"

int ReadProcessMemoryInt(HANDLE processHandle, TouhouAddress address, int size) {
    int result{ 0 };
    if (ReadProcessMemory(processHandle, reinterpret_cast<LPCVOID>(address), &result, size, nullptr) == 0) {
        Log::error("Error while reading process memory at {:x}", address);
    }
    return result;
};

float ReadProcessMemoryFloat(HANDLE processHandle, TouhouAddress address, int size) {
    float result{ 0.0F };
    if (ReadProcessMemory(processHandle, reinterpret_cast<LPCVOID>(address), &result, size, nullptr) == 0) {
        Log::error("Error while reading process memory at {:x}", address);
    }
    return result;
};


std::string ReadProcessMemoryString(HANDLE processHandle, TouhouAddress address, int size) {
    const auto result{ std::make_unique<char[]>(size) };
    if (ReadProcessMemory(processHandle, reinterpret_cast<LPCVOID>(address), result.get(), size, nullptr) == 0) {
        Log::error("Error while reading process memory at {:x}", address);
    }
    return result.get();
};