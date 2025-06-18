#include "SystemUtils.h"
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

int Get_Cores()
{
#ifdef _WIN32
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    return sysinfo.dwNumberOfProcessors;
#elif defined(__linux__)
    return sysconf(_SC_NPROCESSORS_ONLN);
#else
    return 1;
#endif
}
