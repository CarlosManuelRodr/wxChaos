#pragma once

#include <mutex>

/**
 * @class AngelscriptRuntime
 * @brief Ensures AngelScript multithreading support is initialized once per process.
 *
 * AngelScript's multithreading preparation is global. Engine instances keep this
 * lightweight member so every scripting entry path performs the initialization,
 * while std::call_once prevents nested engines from preparing the runtime again.
 */
class AngelscriptRuntime
{
    ///< Process-wide guard for AngelScript multithreading initialization.
    static std::once_flag _prepareFlag;

public:
    /** @brief Initializes AngelScript multithreading support on the first construction. */
    AngelscriptRuntime();
    /** @brief Leaves process-wide runtime cleanup to process termination. */
    ~AngelscriptRuntime() = default;

    /** @brief Runtime guards are unique members and cannot be copied. */
    AngelscriptRuntime(const AngelscriptRuntime&) = delete;
    /** @brief Runtime guards are unique members and cannot be assigned. */
    AngelscriptRuntime& operator=(const AngelscriptRuntime&) = delete;
};
