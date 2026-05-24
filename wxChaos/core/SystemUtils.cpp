#include "SystemUtils.h"
#include <windows.h>

int Get_Cores()
{
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    return sysinfo.dwNumberOfProcessors;
}
