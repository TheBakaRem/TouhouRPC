#include "WindowsUtils.hpp"

int ReadProcessMemoryInt(HANDLE processHandle, TouhouAddress address, int size) {
    int result{ 0 };
    if (ReadProcessMemory(processHandle, reinterpret_cast<LPCVOID>(address), &result, size, NULL) == 0) {
        Log::error("Error while reading process memory at {:x}", address);
    }
    return result;
};

float ReadProcessMemoryFloat(HANDLE processHandle, TouhouAddress address, int size) {
    float result{ 0.0f };
    if (ReadProcessMemory(processHandle, reinterpret_cast<LPCVOID>(address), &result, size, NULL) == 0) {
        Log::error("Error while reading process memory at {:x}", address);
    }
    return result;
};


std::string ReadProcessMemoryString(HANDLE processHandle, TouhouAddress address, int size) {
    auto result{ std::make_unique_for_overwrite<char[]>(size) };
    if (ReadProcessMemory(processHandle, reinterpret_cast<LPCVOID>(address), result.get(), size, NULL) == 0) {
        Log::error("Error while reading process memory at {:x}", address);
    }
    return std::string(result.get());
};