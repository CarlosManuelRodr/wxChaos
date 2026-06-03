#pragma once

#include <vector>
#include <string>
#include <angelscript.h>
#include "ScriptData.h"

extern bool** asSetMap;
extern unsigned int** asColorMap;
extern bool thereIsConsoleText;
extern std::string consoleText;

std::vector<ScriptData> GetValidUserScripts();
std::vector<ScriptData> GetAllUserScripts();

int CompileScriptFromPath(asIScriptEngine* engine, const std::string& filePath);
void RegisterScriptMathReal(asIScriptEngine* engine);
void RegisterScriptMathComplex(asIScriptEngine* engine);
void RegisterWxChaosInterface(asIScriptEngine* engine);
void MessageCallback(const asSMessageInfo* msg, void* param);
ScriptData FetchScriptData(const std::string& fileName);

