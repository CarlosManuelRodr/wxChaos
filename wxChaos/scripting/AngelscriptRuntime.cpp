#include "AngelscriptRuntime.h"
#include <angelscript.h>

std::once_flag AngelscriptRuntime::_prepareFlag;

AngelscriptRuntime::AngelscriptRuntime()
{
    std::call_once(_prepareFlag, [] {
        asPrepareMultithread();
    });
}
