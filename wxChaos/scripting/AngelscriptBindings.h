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
void RegisterOrbitDrawingInterface(asIScriptEngine* engine, Fractal* fractal);
void MessageCallback(const asSMessageInfo* msg, void* param);
ScriptData FetchScriptData(const std::string& fileName);

