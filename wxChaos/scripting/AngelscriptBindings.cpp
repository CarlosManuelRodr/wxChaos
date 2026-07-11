// ReSharper disable CppParameterMayBeConst
// ReSharper disable CppParameterMayBeConstPtrOrRef
// ReSharper disable CppDFAUnusedValue
#include <cassert>
#include <limits>
#include "AngelscriptBindings.h"
#include "AngelscriptConfigurationEngine.h"
#include "AppPaths.h"
#include "TextUtils.h"
#include "Complex.h"
#include "Fractal.h"

using namespace std;

bool** asSetMap;
double** asColorMap;
static constexpr double InvalidColor = std::numeric_limits<double>::max();
static constexpr asPWORD OrbitFractalUserData = 2000;
static constexpr asPWORD ScriptOptionsUserData = 2001;
static string name;
static string documentationPath;
static ScriptCategory scriptCategory;
static double minX, maxX, minY;
static int defaultIter;
static bool defaultIterSet = false;
static bool minXSet = false, maxXSet = false, minYSet = false;
static bool juliaVarietySet = false;
bool thereIsConsoleText = false;
bool asRedrawAlways = false;
bool extColor = true;
bool noSetMap = false;
string consoleText;

std::vector<ScriptData> GetValidUserScripts()
{
    vector<ScriptData> output;
    vector<string> scriptFiles = AppPaths::ScriptFilenames();

    for (auto & scriptFile : scriptFiles)
    {
        AngelscriptConfigurationEngine configEngine;
        const string filePath = AppPaths::ScriptFileStd(wxString::FromUTF8(scriptFile.c_str()));

        if (!configEngine.CompileFromPath(filePath))
            continue;

        if (configEngine.Execute())
        {
            const ScriptData scriptData = configEngine.GetScriptData();
            output.push_back(scriptData);
        }
    }
    return output;
}

std::vector<ScriptData> GetAllUserScripts()
{
    vector<ScriptData> output;
    vector<string> scriptFiles = AppPaths::ScriptFilenames();

    for (auto & scriptFile : scriptFiles)
    {
        AngelscriptConfigurationEngine configEngine;
        const string filePath = AppPaths::ScriptFileStd(wxString::FromUTF8(scriptFile.c_str()));

        if (!configEngine.CompileFromPath(filePath))
        {
            auto errorScript = ScriptData(ScriptDataType::Error);
            errorScript.file = filePath;
            output.push_back(errorScript);
            continue;
        }

        if (configEngine.Execute())
        {
            const ScriptData scriptData = configEngine.GetScriptData();
            output.push_back(scriptData);
        }
        else
        {
            auto errorScript = ScriptData(ScriptDataType::Error);
            errorScript.file = filePath;
            output.push_back(errorScript);
        }
    }
    return output;
}

int CompileScriptFromPath(asIScriptEngine* engine, const string& filePath)
{
    FILE* f = nullptr;
    const errno_t err = fopen_s(&f, filePath.c_str(), "rb");
    if (err != 0 || f == nullptr)
        return -1;

    fseek(f, 0, SEEK_END);
    const int len = ftell(f);
    fseek(f, 0, SEEK_SET);

    string script;
    script.resize(len);
    const size_t c = fread(&script[0], len, 1, f);
    fclose(f);

    if (c == 0)
        return -1;

    asIScriptModule* mod = engine->GetModule(nullptr, asGM_ALWAYS_CREATE);
    int r = mod->AddScriptSection("script", &script[0], len);
    if (r < 0)
        return -1;

    r = mod->Build();
    if (r < 0)
        return -1;

    return 0;
}

// Script accessible functions
static void asSetFractalName(string& str)
{
    name = str;
}

static void asSetDocumentationPath(string& str)
{
    documentationPath = str;
}

static void asSetCategory(string& str)
{
    if (str == "Complex")
        scriptCategory = ScriptCategory::Complex;
    else if (str == "NumMet")
        scriptCategory = ScriptCategory::NumMet;
    else if (str == "Physic")
        scriptCategory = ScriptCategory::Physic;
    else
        scriptCategory = ScriptCategory::Other;
}

static void asSetMinX(double _minX)
{
    minX = _minX;
    minXSet = true;
}

static void asSetMaxX(double _maxX)
{
    maxX = _maxX;
    maxXSet = true;
}

static void asSetMinY(double _minY)
{
    minY = _minY;
    minYSet = true;
}

static void asSetDefaultIter(int _defaultIter)
{
    defaultIter = _defaultIter;
    defaultIterSet = true;
}

static void asSetJuliaVariety(bool mode)
{
    juliaVarietySet = mode;
}

static void asSetRedrawAlways(bool mode)
{
    asRedrawAlways = mode;
}

static void asSetExtColorMode(bool mode)
{
    extColor = mode;
}

static void asNoSetMap(bool mode)
{
    noSetMap = mode;
}

static void asSetPoint(int x, int y, bool setVal, int colorVal)
{
    asSetMap[x][y] = setVal;
    asColorMap[x][y] = colorVal < 0 ? InvalidColor : static_cast<double>(colorVal);
}

static void asDrawOrbitLine(asIScriptGeneric* generic)
{
    auto* fractal = static_cast<Fractal*>(generic->GetEngine()->GetUserData(OrbitFractalUserData));
    if (fractal == nullptr)
        return;

    const auto colorComponent = [generic](const asUINT argument) {
        return static_cast<sf::Uint8>(std::min<asDWORD>(generic->GetArgDWord(argument), 255));
    };

    fractal->DrawLine(generic->GetArgDouble(0), generic->GetArgDouble(1), generic->GetArgDouble(2),
                      generic->GetArgDouble(3), sf::Color(colorComponent(4), colorComponent(5), colorComponent(6)), true);
}

static ScriptOptions* GetScriptOptions(asIScriptGeneric* generic)
{
    return static_cast<ScriptOptions*>(generic->GetEngine()->GetUserData(ScriptOptionsUserData));
}

static const string& GetOptionStringArgument(asIScriptGeneric* generic, const asUINT index)
{
    return *static_cast<string*>(generic->GetArgObject(index));
}

static void asAddIntegerOption(asIScriptGeneric* generic)
{
    if (ScriptOptions* options = GetScriptOptions(generic))
        options->AddInteger(GetOptionStringArgument(generic, 0), GetOptionStringArgument(generic, 1),
                            static_cast<int>(generic->GetArgDWord(2)));
}

static void asAddDoubleOption(asIScriptGeneric* generic)
{
    if (ScriptOptions* options = GetScriptOptions(generic))
        options->AddDouble(GetOptionStringArgument(generic, 0), GetOptionStringArgument(generic, 1),
                           generic->GetArgDouble(2));
}

static void asAddBoolOption(asIScriptGeneric* generic)
{
    if (ScriptOptions* options = GetScriptOptions(generic))
        options->AddBool(GetOptionStringArgument(generic, 0), GetOptionStringArgument(generic, 1),
                         generic->GetArgByte(2) != 0);
}

static void asGetIntegerOption(asIScriptGeneric* generic)
{
    const ScriptOptions* options = GetScriptOptions(generic);
    generic->SetReturnDWord(options == nullptr ? 0 : options->GetInteger(GetOptionStringArgument(generic, 0)));
}

static void asGetDoubleOption(asIScriptGeneric* generic)
{
    const ScriptOptions* options = GetScriptOptions(generic);
    generic->SetReturnDouble(options == nullptr ? 0.0 : options->GetDouble(GetOptionStringArgument(generic, 0)));
}

static void asGetBoolOption(asIScriptGeneric* generic)
{
    const ScriptOptions* options = GetScriptOptions(generic);
    generic->SetReturnByte(options != nullptr && options->GetBool(GetOptionStringArgument(generic, 0)));
}

static void asPrintString(string& str)
{
    consoleText.empty() ? consoleText = str : consoleText += str;
    thereIsConsoleText = true;
}

static void asPrintInt(int num)
{
    consoleText.empty() ? consoleText = TextUtils::ToString(num) : consoleText += TextUtils::ToString(num);
    thereIsConsoleText = true;
}

static void asPrintFloat(double num)
{
    consoleText.empty() ? consoleText = TextUtils::ToString(num) : consoleText += TextUtils::ToString(num);
    thereIsConsoleText = true;
}

void asPrintComplex(const Complex& num)
{
    string temp = TextUtils::ToString(num.complexNum.real());

    temp += num.complexNum.imag() >= 0 ? "+i" : "-i";
    temp += TextUtils::ToString(abs(num.complexNum.imag()));
    consoleText.empty() ? consoleText = temp : consoleText += temp;

    thereIsConsoleText = true;
}

ScriptData FetchScriptData(const string& fileName)
{
    ScriptData dat;
    dat.scriptCategory = scriptCategory;
    dat.file = fileName;
    dat.name = name;
    dat.documentationPath = documentationPath;
    dat.minX = minXSet ? minX : -2;
    dat.maxX = maxXSet ? maxX : 2;
    dat.minY = minYSet ? minY : -2;
    dat.defaultIter = defaultIterSet ? defaultIter : 100;
    dat.juliaVariety = juliaVarietySet;
    dat.redrawAlways = asRedrawAlways;
    dat.extColor = extColor;
    dat.noSetMap = noSetMap;

    minXSet = maxXSet = minYSet = false;
    defaultIterSet = false;
    documentationPath.clear();
    juliaVarietySet = false;
    asRedrawAlways = false;
    noSetMap = false;
    extColor = true;

    return dat;
}

// ReSharper disable once CppParameterNeverUsed
void MessageCallback(const asSMessageInfo* msg, void* param)
{
    auto type = "ERR ";

    if (msg->type == asMSGTYPE_WARNING)
        type = "WARN";
    else if (msg->type == asMSGTYPE_INFORMATION)
        type = "INFO";

    consoleText = "";
    consoleText += msg->section;
    consoleText += " (";
    consoleText += TextUtils::ToString(msg->row);
    consoleText += ", ";
    consoleText += TextUtils::ToString(msg->col);
    consoleText += ") : ";
    consoleText += type;
    consoleText += " : ";
    consoleText += msg->message;
    consoleText += "\n";
    thereIsConsoleText = true;
}

static void ComplexDefaultConstructor(Complex* self)
{
    new(self) Complex();
}

static void ComplexCopyConstructor(const Complex& other, Complex* self)
{
    new(self) Complex(other);
}

static void ComplexInitConstructor(double r, double i, Complex* self)
{
    new(self) Complex(r, i);
}

void RegisterScriptMathReal(asIScriptEngine* engine)
{
    // ReSharper disable once CppJoinDeclarationAndAssignment
    int r;
    r = engine->RegisterGlobalFunction("double cos_r(double)", asFUNCTIONPR(cos, (double), double), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("double sin_r(double)", asFUNCTIONPR(sin, (double), double), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("double tan_r(double)", asFUNCTIONPR(tan, (double), double), asCALL_CDECL); assert(r >= 0);

    r = engine->RegisterGlobalFunction("double acos_r(double)", asFUNCTIONPR(acos, (double), double), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("double asin_r(double)", asFUNCTIONPR(asin, (double), double), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("double atan_r(double)", asFUNCTIONPR(atan, (double), double), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("double atan2_r(double,double)", asFUNCTIONPR(atan2, (double, double), double), asCALL_CDECL); assert(r >= 0);

    r = engine->RegisterGlobalFunction("double cosh_r(double)", asFUNCTIONPR(cosh, (double), double), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("double sinh_r(double)", asFUNCTIONPR(sinh, (double), double), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("double tanh_r(double)", asFUNCTIONPR(tanh, (double), double), asCALL_CDECL); assert(r >= 0);

    r = engine->RegisterGlobalFunction("double log_r(double)", asFUNCTIONPR(log, (double), double), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("double log10_r(double)", asFUNCTIONPR(log10, (double), double), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("double exp_r(double)", asFUNCTIONPR(exp, (double), double), asCALL_CDECL); assert(r >= 0);

    r = engine->RegisterGlobalFunction("double pow_r(double, double)", asFUNCTIONPR(pow, (double, double), double), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("double sqrt_r(double)", asFUNCTIONPR(sqrt, (double), double), asCALL_CDECL); assert(r >= 0);

    r = engine->RegisterGlobalFunction("double ceil_r(double)", asFUNCTIONPR(ceil, (double), double), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("double abs_r(double)", asFUNCTIONPR(fabs, (double), double), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("double floor_r(double)", asFUNCTIONPR(floor, (double), double), asCALL_CDECL); assert(r >= 0);
}

void RegisterScriptMathComplex(asIScriptEngine* engine)
{
    // ReSharper disable once CppJoinDeclarationAndAssignment
    int r;

    r = engine->RegisterObjectType("complex", sizeof(Complex), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CAK | asOBJ_APP_CLASS_ALLFLOATS); assert(r >= 0);

    r = engine->RegisterObjectBehaviour("complex", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(ComplexDefaultConstructor), asCALL_CDECL_OBJLAST); assert(r >= 0);
    r = engine->RegisterObjectBehaviour("complex", asBEHAVE_CONSTRUCT, "void f(const complex &in)", asFUNCTION(ComplexCopyConstructor), asCALL_CDECL_OBJLAST); assert(r >= 0);
    r = engine->RegisterObjectBehaviour("complex", asBEHAVE_CONSTRUCT, "void f(double, double i = 0)", asFUNCTION(ComplexInitConstructor), asCALL_CDECL_OBJLAST); assert(r >= 0);

    r = engine->RegisterObjectMethod("complex", "complex &opAddAssign(const complex &in)", asMETHODPR(Complex, operator+=, (const Complex &), Complex&), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectMethod("complex", "complex &opSubAssign(const complex &in)", asMETHODPR(Complex, operator-=, (const Complex &), Complex&), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectMethod("complex", "complex &opMulAssign(const complex &in)", asMETHODPR(Complex, operator*=, (const Complex &), Complex&), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectMethod("complex", "complex &opDivAssign(const complex &in)", asMETHODPR(Complex, operator/=, (const Complex &), Complex&), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectMethod("complex", "bool opEquals(const complex &in) const", asMETHODPR(Complex, operator==, (const Complex &) const, bool), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectMethod("complex", "complex opAdd(const complex &in) const", asMETHODPR(Complex, operator+, (const Complex &) const, Complex), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectMethod("complex", "complex opSub(const complex &in) const", asMETHODPR(Complex, operator-, (const Complex &) const, Complex), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectMethod("complex", "complex opMul(const complex &in) const", asMETHODPR(Complex, operator*, (const Complex &) const, Complex), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectMethod("complex", "complex opDiv(const complex &in) const", asMETHODPR(Complex, operator/, (const Complex &) const, Complex), asCALL_THISCALL); assert(r >= 0);

    r = engine->RegisterObjectMethod("complex", "double norm() const", asMETHOD(Complex, norm), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectMethod("complex", "double real() const", asMETHOD(Complex, real), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectMethod("complex", "double imag() const", asMETHOD(Complex, imag), asCALL_THISCALL); assert(r >= 0);

    r = engine->RegisterGlobalFunction("complex pow(const complex &in, const int &in)", asFUNCTIONPR(cpxPow, (const Complex &, const int &), Complex), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("complex pow(const complex &in, const double &in)", asFUNCTIONPR(cpxPow, (const Complex &, const double &), Complex), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("complex pow(const complex &in, const complex &in)", asFUNCTIONPR(cpxPow, (const Complex &, const Complex &), Complex), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("complex sqrt(const complex &in)", asFUNCTIONPR(cpxSqrt, (const Complex &), Complex), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("complex sin(const complex &in)", asFUNCTIONPR(cpxSin, (const Complex &), Complex), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("complex cos(const complex &in)", asFUNCTIONPR(cpxCos, (const Complex &), Complex), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("complex tan(const complex &in)", asFUNCTIONPR(cpxTan, (const Complex &), Complex), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("complex csc(const complex &in)", asFUNCTIONPR(cpxCsc, (const Complex &), Complex), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("complex sec(const complex &in)", asFUNCTIONPR(cpxSec, (const Complex &), Complex), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("complex cot(const complex &in)", asFUNCTIONPR(cpxCot, (const Complex &), Complex), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("complex sinh(const complex &in)", asFUNCTIONPR(cpxSinh, (const Complex &), Complex), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("complex cosh(const complex &in)", asFUNCTIONPR(cpxCosh, (const Complex &), Complex), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("complex tanh(const complex &in)", asFUNCTIONPR(cpxTanh, (const Complex &), Complex), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("complex exp(const complex &in)", asFUNCTIONPR(cpxExp, (const Complex &), Complex), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("complex log(const complex &in)", asFUNCTIONPR(cpxLog, (const Complex &), Complex), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("complex log10(const complex &in)", asFUNCTIONPR(cpxLog10, (const Complex &), Complex), asCALL_CDECL); assert(r >= 0);
}

void RegisterWxChaosInterface(asIScriptEngine* engine)
{
    // ReSharper disable once CppJoinDeclarationAndAssignment
    int r;
    r = engine->RegisterGlobalFunction("void SetFractalName(string &in)", asFUNCTION(asSetFractalName), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void SetDocumentationPath(string &in)", asFUNCTION(asSetDocumentationPath), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void SetCategory(string &in)", asFUNCTION(asSetCategory), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void SetPoint(int, int, bool, int)", asFUNCTION(asSetPoint), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void SetMinX(double)", asFUNCTION(asSetMinX), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void SetMaxX(double)", asFUNCTION(asSetMaxX), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void SetMinY(double)", asFUNCTION(asSetMinY), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void SetDefaultIter(int)", asFUNCTION(asSetDefaultIter), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void PrintString(string &in)", asFUNCTION(asPrintString), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void PrintInt(int)", asFUNCTION(asPrintInt), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void PrintFloat(double)", asFUNCTION(asPrintFloat), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void PrintComplex(const complex &in)", asFUNCTION(asPrintComplex), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void SetJuliaVariety(bool)", asFUNCTION(asSetJuliaVariety), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void SetRedrawAlways(bool)", asFUNCTION(asSetRedrawAlways), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void SetExtColorMode(bool)", asFUNCTION(asSetExtColorMode), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void NoSetMap(bool)", asFUNCTION(asNoSetMap), asCALL_CDECL); assert(r >= 0);
}

void RegisterOrbitDrawingInterface(asIScriptEngine* engine, Fractal* fractal)
{
    engine->SetUserData(fractal, OrbitFractalUserData);

    const int r = engine->RegisterGlobalFunction(
        "void DrawLine(double, double, double, double, uint red = 0, uint green = 0, uint blue = 0)",
        asFUNCTION(asDrawOrbitLine), asCALL_GENERIC);
    assert(r >= 0);
}

void RegisterScriptOptionsInterface(asIScriptEngine* engine, ScriptOptions* options)
{
    engine->SetUserData(options, ScriptOptionsUserData);

    int r = engine->RegisterGlobalFunction("void AddIntegerOption(const string &in, const string &in, int)",
                                           asFUNCTION(asAddIntegerOption), asCALL_GENERIC); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void AddDoubleOption(const string &in, const string &in, double)",
                                       asFUNCTION(asAddDoubleOption), asCALL_GENERIC); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void AddBoolOption(const string &in, const string &in, bool)",
                                       asFUNCTION(asAddBoolOption), asCALL_GENERIC); assert(r >= 0);
    r = engine->RegisterGlobalFunction("int GetIntegerOption(const string &in)",
                                       asFUNCTION(asGetIntegerOption), asCALL_GENERIC); assert(r >= 0);
    r = engine->RegisterGlobalFunction("double GetDoubleOption(const string &in)",
                                       asFUNCTION(asGetDoubleOption), asCALL_GENERIC); assert(r >= 0);
    r = engine->RegisterGlobalFunction("bool GetBoolOption(const string &in)",
                                       asFUNCTION(asGetBoolOption), asCALL_GENERIC); assert(r >= 0);
}
