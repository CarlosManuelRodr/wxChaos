#pragma once

#include <mutex>

class AngelscriptRuntime
{
    static std::once_flag _prepareFlag;

public:
    AngelscriptRuntime();
    ~AngelscriptRuntime() = default;

    AngelscriptRuntime(const AngelscriptRuntime&) = delete;
    AngelscriptRuntime& operator=(const AngelscriptRuntime&) = delete;
};
