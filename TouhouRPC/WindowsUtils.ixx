module;

#include <Windows.h>
#include <memoryapi.h>

export module WindowsUtils;

import <stdexcept>;
import <memory>;
import Log;


// Declaring a custom Address type that changes size based on x64 or x86 architecture.
// This removes warning C4312.
#if _WIN64
export using TouhouAddress = uint64_t;
#else
export using TouhouAddress = uint32_t;
#endif // _WIN64

export int ReadProcessMemoryInt(HANDLE processHandle, TouhouAddress address, int size = 4);
export float ReadProcessMemoryFloat(HANDLE processHandle, TouhouAddress address, int size = 4);
export std::string ReadProcessMemoryString(HANDLE processHandle, TouhouAddress address, int size = 4);

module : private;

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