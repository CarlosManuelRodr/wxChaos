#include "SystemUtilities.h"

#include <algorithm>
#include <thread>

#ifdef _WIN32
#include <windows.h>
#endif

namespace Platform
{
    int ProcessorCount()
    {
        return static_cast<int>(std::max(1U, std::thread::hardware_concurrency()));
    }

    void EnableHighDpiSupport()
    {
#ifdef _WIN32
        const HMODULE user32 = LoadLibraryW(L"user32.dll");
        if (user32)
        {
            using SetProcessDpiAwarenessContextFn = BOOL(WINAPI*)(HANDLE);
            const auto setDpiAwarenessContext = reinterpret_cast<SetProcessDpiAwarenessContextFn>(
                GetProcAddress(user32, "SetProcessDpiAwarenessContext"));

            if (setDpiAwarenessContext && setDpiAwarenessContext(reinterpret_cast<HANDLE>(-4)))
            {
                FreeLibrary(user32);
                return;
            }
        }

        if (const HMODULE shcore = LoadLibraryW(L"Shcore.dll"))
        {
            enum class ProcessDpiAwarenessValue
            {
                PerMonitorDpiAware = 2
            };

            using SetProcessDpiAwarenessFn = HRESULT(WINAPI*)(ProcessDpiAwarenessValue);
            const auto setProcessDpiAwareness = reinterpret_cast<SetProcessDpiAwarenessFn>(
                GetProcAddress(shcore, "SetProcessDpiAwareness"));

            if (setProcessDpiAwareness)
                setProcessDpiAwareness(ProcessDpiAwarenessValue::PerMonitorDpiAware);

            FreeLibrary(shcore);
            if (user32)
                FreeLibrary(user32);
            return;
        }

        if (user32)
        {
            using SetProcessDPIAwareFn = BOOL(WINAPI*)();
            const auto setProcessDPIAware = reinterpret_cast<SetProcessDPIAwareFn>(
                GetProcAddress(user32, "SetProcessDPIAware"));

            if (setProcessDPIAware)
                setProcessDPIAware();

            FreeLibrary(user32);
        }
#endif
    }
}
