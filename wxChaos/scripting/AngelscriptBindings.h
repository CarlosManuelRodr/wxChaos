#pragma once

#include <vector>
#include <string>
#include <angelscript.h>
#include "ScriptData.h"

class Fractal;

extern bool** asSetMap;
extern double** asColorMap;
extern bool thereIsConsoleText;
extern std::string consoleText;

std::vector<ScriptData> GetValidUserScripts();
std::vector<ScriptData> GetAllUserScripts();

int CompileScriptFromPath(asIScriptEngine* engine, const std::string& filePath);
void RegisterScriptMathReal(asIScriptEngine* engine);
void RegisterScriptMathComplex(asIScriptEngine* engine);
void RegisterWxChaosInterface(asIScriptEngine* engine);
/**
 * @brief Registers the DrawLine() function used by scripted orbit entry points.
 * @param engine AngelScript engine receiving the binding.
 * @param fractal Fractal that records drawn lines, or nullptr for compile-only engines.
 */
void RegisterOrbitDrawingInterface(asIScriptEngine* engine, Fractal* fractal);
/**
 * @brief Registers functions for declaring and retrieving arbitrary typed script options.
 * @param engine AngelScript engine receiving the option interface.
 * @param options Host-owned option registry used by the engine.
 */
void RegisterScriptOptionsInterface(asIScriptEngine* engine, ScriptOptions* options);
void MessageCallback(const asSMessageInfo* msg, void* param);
ScriptData FetchScriptData(const std::string& fileName);

