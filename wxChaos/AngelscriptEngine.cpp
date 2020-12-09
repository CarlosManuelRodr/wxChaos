#include "AngelscriptEngine.h"
#include "StringFuncs.h"
#include <fstream>
#include <cmath>
#include <assert.h>
using namespace std;

// Variables accesible from the scripts.
bool** asSetMap;
int** asColorMap;
string name;
ScriptCategory scriptCategory;
double minX, maxX, minY;
int defaultIter;
bool defaultIterSet = false;
bool minXSet = false, maxXSet = false, minYSet = false;
bool juliaVarietySet = false;
bool thereIsConsoleText = false;
bool asRedrawAlways = false;
bool extColor = true;
bool noSetMap = false;
string consoleText;

int CompileScriptFromPath(asIScriptEngine *engine, const string filePath)
{
    int r;

    // Opens the script file.
    FILE *f = fopen(filePath.c_str(), "rb");
    if(f == 0)
        return -1;
    
    fseek(f, 0, SEEK_END);
    int len = ftell(f);
    fseek(f, 0, SEEK_SET);

    string script;
    script.resize(len);
    int c =    fread(&script[0], len, 1, f);
    fclose(f);

    if(c == 0) 
        return -1;

    // Creates module.
    asIScriptModule *mod = engine->GetModule(0, asGM_ALWAYS_CREATE);
    r = mod->AddScriptSection("script", &script[0], len);
    if(r < 0) 
        return -1;
    
    // Compiles the script.
    r = mod->Build();
    if(r < 0)
        return -1;

    return 0;
}

// Functions accesible from the script.
void asSetFractalName(string &str)
{
    name = str;
}
void asSetCategory(string &str)
{
    if(str == "Complex")
        scriptCategory = ScriptCategory::Complex;
    else if(str == "NumMet")
        scriptCategory = ScriptCategory::NumMet;
    else if(str == "Physic")
        scriptCategory = ScriptCategory::Physic;
    else
        scriptCategory = ScriptCategory::Other;
}
void asSetMinX(double _minX)
{
    minX = _minX;
    minXSet = true;
}
void asSetMaxX(double _maxX)
{
    maxX = _maxX;
    maxXSet = true;
}
void asSetMinY(double _minY)
{
    minY = _minY;
    minYSet = true;
}
void asSetDefaultIter(int _defaultIter)
{
    defaultIter = _defaultIter;
    defaultIterSet = true;
}
void asSetJuliaVariety(bool mode)
{
    juliaVarietySet = mode;
}
void asSetRedrawAlways(bool mode)
{
    asRedrawAlways = mode;
}
void asSetExtColorMode(bool mode)
{
    extColor = mode;
}
void asNoSetMap(bool mode)
{
    noSetMap = mode;
}
void asSetPoint(int x, int y, bool setVal, int colorVal)
{
    asSetMap[x][y] = setVal;
    asColorMap[x][y] = colorVal;
}
void asPrintString(string &str)
{
    if(consoleText.size() == 0)
        consoleText = str;
    else
        consoleText += str;

    thereIsConsoleText = true;
}
void asPrintInt(int num)
{
    if(consoleText.size() == 0)
        consoleText = str_num_to_string(num);
    else
        consoleText += str_num_to_string(num);

    thereIsConsoleText = true;
}
void asPrintFloat(double num)
{
    if(consoleText.size() == 0)
        consoleText = str_num_to_string(num);
    else
        consoleText += str_num_to_string(num);

    thereIsConsoleText = true;
}
void asPrintComplex(const Complex &num)
{
    string temp = str_num_to_string(num.complexNum.real());

    if(num.complexNum.imag() >= 0)
        temp += "+i";
    else
        temp += "-i";

    temp += str_num_to_string((double)abs(num.complexNum.imag()));

    if(consoleText.size() == 0)
        consoleText = temp;
    else
        consoleText += temp;

    thereIsConsoleText = true;
}

ScriptData FetchScriptData(const string& fileName)
{
    ScriptData dat;
    dat.scriptCategory = scriptCategory;
    dat.file = fileName;
    dat.name = name;
    if(minXSet)
        dat.minX = minX;
    else 
        dat.minX = -2;
    if(maxXSet)
        dat.maxX = maxX;
    else
        dat.maxX = 2;
    if(minYSet)
        dat.minY = minY;
    else
        dat.minY = -2;

    if(defaultIterSet)
        dat.defaultIter = defaultIter;
    else
        dat.defaultIter = 100;

    dat.juliaVariety = juliaVarietySet;
    dat.redrawAlways = asRedrawAlways;
    dat.extColor = extColor;
    dat.noSetMap = noSetMap;

    minXSet = false;
    maxXSet = false;
    minYSet = false;
    defaultIterSet = false;
    juliaVarietySet = false;
    asRedrawAlways = false;
    noSetMap = false;
    extColor = true;
    
    return dat;
}
void MessageCallback(const asSMessageInfo* msg, void* param)
{
    const char *type = "ERR ";

    if( msg->type == asMSGTYPE_WARNING ) 
        type = "WARN";
    else if( msg->type == asMSGTYPE_INFORMATION ) 
        type = "INFO";

    consoleText = "";
    consoleText += msg->section;
    consoleText += " (";
    consoleText += str_num_to_string(msg->row);
    consoleText += ", ";
    consoleText += str_num_to_string(msg->col);
    consoleText += ") : ";
    consoleText += type;
    consoleText += " : ";
    consoleText += msg->message;
    consoleText += "\n";
    thereIsConsoleText = true;
}
void RegisterWxChaosInterface(asIScriptEngine *engine)
{
    int r;
    r = engine->RegisterGlobalFunction("void SetFractalName(string &in)", asFUNCTION(asSetFractalName), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void SetCategory(string &in)", asFUNCTION(asSetCategory), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void SetPoint(int, int, bool, int)", asFUNCTION(asSetPoint), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void SetMinX(double)", asFUNCTION(asSetMinX), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void SetMaxX(double)", asFUNCTION(asSetMaxX), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void SetMinY(double)", asFUNCTION(asSetMinY), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void SetDefaultIter(int)", asFUNCTION(asSetDefaultIter), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void PrintString(string &in)", asFUNCTION(asPrintString), asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("void PrintInt(int)", asFUNCTION(asPrintInt), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void PrintFloat(double)", asFUNCTION(asPrintFloat), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void PrintComplex(const complex &in)", asFUNCTION(asPrintComplex), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void SetJuliaVariety(bool)", asFUNCTION(asSetJuliaVariety), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void SetRedrawAlways(bool)", asFUNCTION(asSetRedrawAlways), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void SetExtColorMode(bool)", asFUNCTION(asSetExtColorMode), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void NoSetMap(bool)", asFUNCTION(asNoSetMap), asCALL_CDECL); assert(r >= 0);
}

// Complex number.
Complex::Complex()
{
    complexNum = std::complex<double>(0,0);
}
Complex::Complex(const Complex &other)
{
    complexNum = other.complexNum;
}
Complex::Complex(const std::complex<double> &other)
{
    complexNum = other;
}
Complex::Complex(double _r, double _i)
{
    complexNum = std::complex<double>(_r, _i);
}
bool Complex::operator==(const Complex &o) const
{
    return (complexNum == o.complexNum);
}
bool Complex::operator!=(const Complex &o) const
{
    return !(*this == o);
}
Complex &Complex::operator=(const Complex &other)
{
    complexNum = other.complexNum;
    return *this;
}
Complex &Complex::operator+=(const Complex &other)
{
    complexNum += other.complexNum;
    return *this;
}
Complex &Complex::operator-=(const Complex &other)
{
    complexNum -= other.complexNum;
    return *this;
}
Complex &Complex::operator*=(const Complex &other)
{
    *this = *this * other;
    return *this;
}
Complex &Complex::operator/=(const Complex &other)
{
    *this = *this / other;
    return *this;
}
double Complex::norm() const
{
    return std::norm(complexNum);
}
double Complex::squaredNorm() const
{
    return complexNum.real()*complexNum.real() + complexNum.imag()*complexNum.imag();
}
Complex Complex::operator+(const Complex &other) const
{
    return Complex(complexNum + other.complexNum);
}
Complex Complex::operator-(const Complex &other) const
{
    return Complex(complexNum - other.complexNum);
}
Complex Complex::operator*(const Complex &other) const
{
    return Complex(complexNum*other.complexNum);
}
Complex Complex::operator/(const Complex &other) const
{
    return Complex(complexNum/other.complexNum);;
}
double Complex::real()
{
    return complexNum.real();
}
double Complex::imag()
{
    return complexNum.imag();
}

// Complex functions.
Complex cpxPow(const Complex &base, const int &exponent)
{
    return Complex(pow(base.complexNum, exponent));
}
Complex cpxPow(const Complex &base, const double &exponent)
{
    return Complex(pow(base.complexNum, exponent));
}
Complex cpxPow(const Complex &base, const Complex &exponent)
{
    return Complex(pow(base.complexNum, exponent.complexNum));
}
Complex cpxSqrt(const Complex &z)
{
    return sqrt(z.complexNum);
}
Complex cpxSin(const Complex &z)
{
    return sin(z.complexNum);
}
Complex cpxCsc(const Complex &z)
{
    return complex<double>(1,0)/sin(z.complexNum);
}
Complex cpxCos(const Complex &z)
{
    return cos(z.complexNum);
}
Complex cpxSec(const Complex &z)
{
    return complex<double>(1,0)/cos(z.complexNum);
}
Complex cpxTan(const Complex &z)
{
    return tan(z.complexNum);
}
Complex cpxCot(const Complex &z)
{
    return complex<double>(1,0)/tan(z.complexNum);
}
Complex cpxSinh(const Complex &z)
{
    return sinh(z.complexNum);
}
Complex cpxCosh(const Complex &z)
{
    return cosh(z.complexNum);
}
Complex cpxTanh(const Complex &z)
{
    return tanh(z.complexNum);
}
Complex cpxExp(const Complex &z)
{
    return exp(z.complexNum);
}
Complex cpxLog(const Complex &z)
{
    return log(z.complexNum);
}
Complex cpxLog10(const Complex &z)
{
    return log10(z.complexNum);
}


// Complex constructors.
static void ComplexDefaultConstructor(Complex *self)
{
    new(self) Complex();
}
static void ComplexCopyConstructor(const Complex &other, Complex *self)
{
    new(self) Complex(other);
}
static void ComplexInitConstructor(double r, double i, Complex *self)
{
    new(self) Complex(r,i);
}

// Register complex numbers.
void RegisterScriptMathReal(asIScriptEngine *engine)
{
    int r;
    // Trigonometric functions.
    r = engine->RegisterGlobalFunction("double cos_r(double)", asFUNCTIONPR(cos, (double), double), asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("double sin_r(double)", asFUNCTIONPR(sin, (double), double), asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("double tan_r(double)", asFUNCTIONPR(tan, (double), double), asCALL_CDECL); assert( r >= 0 );

    r = engine->RegisterGlobalFunction("double acos_r(double)", asFUNCTIONPR(acos, (double), double), asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("double asin_r(double)", asFUNCTIONPR(asin, (double), double), asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("double atan_r(double)", asFUNCTIONPR(atan, (double), double), asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("double atan2_r(double,double)", asFUNCTIONPR(atan2, (double, double), double), asCALL_CDECL); assert( r >= 0 );

    // Hyberbolic functions.
    r = engine->RegisterGlobalFunction("double cosh_r(double)", asFUNCTIONPR(cosh, (double), double), asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("double sinh_r(double)", asFUNCTIONPR(sinh, (double), double), asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("double tanh_r(double)", asFUNCTIONPR(tanh, (double), double), asCALL_CDECL); assert( r >= 0 );

    // Exponential and logarithmic functions.
    r = engine->RegisterGlobalFunction("double log_r(double)", asFUNCTIONPR(log, (double), double), asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("double log10_r(double)", asFUNCTIONPR(log10, (double), double), asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("double exp_r(double)", asFUNCTIONPR(exp, (double), double), asCALL_CDECL); assert( r >= 0 );

    // Power functions.
    r = engine->RegisterGlobalFunction("double pow_r(double, double)", asFUNCTIONPR(pow, (double, double), double), asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("double sqrt_r(double)", asFUNCTIONPR(sqrt, (double), double), asCALL_CDECL); assert( r >= 0 );

    // Nearest integer, absolute value, and remainder functions.
    r = engine->RegisterGlobalFunction("double ceil_r(double)", asFUNCTIONPR(ceil, (double), double), asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("double abs_r(double)", asFUNCTIONPR(fabs, (double), double), asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("double floor_r(double)", asFUNCTIONPR(floor, (double), double), asCALL_CDECL); assert( r >= 0 );
}

void RegisterScriptMathComplex(asIScriptEngine *engine)
{
    int r;

    r = engine->RegisterObjectType("complex", sizeof(Complex), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CAK | asOBJ_APP_CLASS_ALLFLOATS); assert( r >= 0 );

    // Constructors.
    r = engine->RegisterObjectBehaviour("complex", asBEHAVE_CONSTRUCT,  "void f()",                    asFUNCTION(ComplexDefaultConstructor), asCALL_CDECL_OBJLAST); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("complex", asBEHAVE_CONSTRUCT,  "void f(const complex &in)",   asFUNCTION(ComplexCopyConstructor), asCALL_CDECL_OBJLAST); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("complex", asBEHAVE_CONSTRUCT,  "void f(double, double i = 0)",  asFUNCTION(ComplexInitConstructor), asCALL_CDECL_OBJLAST); assert( r >= 0 );

    // Operator overload.
    r = engine->RegisterObjectMethod("complex", "complex &opAddAssign(const complex &in)", asMETHODPR(Complex, operator+=, (const Complex &), Complex&), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("complex", "complex &opSubAssign(const complex &in)", asMETHODPR(Complex, operator-=, (const Complex &), Complex&), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("complex", "complex &opMulAssign(const complex &in)", asMETHODPR(Complex, operator*=, (const Complex &), Complex&), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("complex", "complex &opDivAssign(const complex &in)", asMETHODPR(Complex, operator/=, (const Complex &), Complex&), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("complex", "bool opEquals(const complex &in) const", asMETHODPR(Complex, operator==, (const Complex &) const, bool), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("complex", "complex opAdd(const complex &in) const", asMETHODPR(Complex, operator+, (const Complex &) const, Complex), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("complex", "complex opSub(const complex &in) const", asMETHODPR(Complex, operator-, (const Complex &) const, Complex), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("complex", "complex opMul(const complex &in) const", asMETHODPR(Complex, operator*, (const Complex &) const, Complex), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("complex", "complex opDiv(const complex &in) const", asMETHODPR(Complex, operator/, (const Complex &) const, Complex), asCALL_THISCALL); assert( r >= 0 );

    // Methods.
    r = engine->RegisterObjectMethod("complex", "double norm() const", asMETHOD(Complex,norm), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("complex", "double real() const", asMETHOD(Complex,real), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("complex", "double imag() const", asMETHOD(Complex,imag), asCALL_THISCALL); assert( r >= 0 );

    // Functions.
    r = engine->RegisterGlobalFunction("complex pow(const complex &in, const int &in)", asFUNCTIONPR(cpxPow, (const Complex &, const int &), Complex), asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("complex pow(const complex &in, const double &in)", asFUNCTIONPR(cpxPow, (const Complex &, const double &), Complex), asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("complex pow(const complex &in, const complex &in)", asFUNCTIONPR(cpxPow, (const Complex &, const Complex &), Complex), asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("complex sqrt(const complex &in)", asFUNCTIONPR(cpxSqrt, (const Complex &), Complex), asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("complex sin(const complex &in)", asFUNCTIONPR(cpxSin, (const Complex &), Complex), asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("complex cos(const complex &in)", asFUNCTIONPR(cpxCos, (const Complex &), Complex), asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("complex tan(const complex &in)", asFUNCTIONPR(cpxTan, (const Complex &), Complex), asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("complex csc(const complex &in)", asFUNCTIONPR(cpxCsc, (const Complex &), Complex), asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("complex sec(const complex &in)", asFUNCTIONPR(cpxSec, (const Complex &), Complex), asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("complex cot(const complex &in)", asFUNCTIONPR(cpxCot, (const Complex &), Complex), asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("complex sinh(const complex &in)", asFUNCTIONPR(cpxSinh, (const Complex &), Complex), asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("complex cosh(const complex &in)", asFUNCTIONPR(cpxCosh, (const Complex &), Complex), asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("complex tanh(const complex &in)", asFUNCTIONPR(cpxTanh, (const Complex &), Complex), asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("complex exp(const complex &in)", asFUNCTIONPR(cpxExp, (const Complex &), Complex), asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("complex log(const complex &in)", asFUNCTIONPR(cpxLog, (const Complex &), Complex), asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("complex log10(const complex &in)", asFUNCTIONPR(cpxLog10, (const Complex &), Complex), asCALL_CDECL); assert( r >= 0 );
}

// AngelscriptConfigurationEngine implementation.
AngelscriptConfigurationEngine::AngelscriptConfigurationEngine()
{
    engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
    if (engine == 0)
    {
        errorInfo = wxT("Failed to create script engine.");
        return;
    }

    engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);

    // Configure the script engine.
    RegisterStdString(engine);
    RegisterScriptMathComplex(engine);
    RegisterScriptMathReal(engine);
    RegisterWxChaosInterface(engine);

    // Register this to avoid compiler complains.
    int intVar;
    double dblVar;
    engine->RegisterGlobalProperty("double minX", &dblVar);
    engine->RegisterGlobalProperty("double maxX", &dblVar);
    engine->RegisterGlobalProperty("double minY", &dblVar);
    engine->RegisterGlobalProperty("double maxY", &dblVar);
    engine->RegisterGlobalProperty("double xFactor", &dblVar);
    engine->RegisterGlobalProperty("double yFactor", &dblVar);
    engine->RegisterGlobalProperty("double kReal", &dblVar);
    engine->RegisterGlobalProperty("double kImaginary", &dblVar);
    engine->RegisterGlobalProperty("int ho", &intVar);
    engine->RegisterGlobalProperty("int hf", &intVar);
    engine->RegisterGlobalProperty("int wo", &intVar);
    engine->RegisterGlobalProperty("int wf", &intVar);
    engine->RegisterGlobalProperty("int maxIter", &intVar);
    engine->RegisterGlobalProperty("int threadIndex", &intVar);
    engine->RegisterGlobalProperty("int screenWidth", &intVar);
    engine->RegisterGlobalProperty("int screenHeight", &intVar);
    engine->RegisterGlobalProperty("int paletteSize", &intVar);

    ctx = nullptr;
}
AngelscriptConfigurationEngine::~AngelscriptConfigurationEngine()
{
    if (engine != nullptr)
    {
        engine->Release();
        engine = nullptr;
    }
}

bool AngelscriptConfigurationEngine::CompileFromPath(std::string path)
{
    // Compile the script code.
    const int r = CompileScriptFromPath(engine, path);
    filePath = path;

    if (r < 0)
    {
        engine->Release();
        engine = nullptr;
        errorInfo = wxT("Compile error in file: " + path);
        return false;
    }
    return true;
}
bool AngelscriptConfigurationEngine::Execute()
{
    int r;
    asIScriptContext* ctx = engine->CreateContext();
    if (ctx == 0)
    {
        engine->Release();
        engine = nullptr;
        return false;
    }


    // Find the function for the function we want to execute.
    asIScriptFunction* renderFunc = engine->GetModule(0)->GetFunctionByDecl("void Configure()");
    if (renderFunc == 0)
    {
        ctx->Release();
        engine->Release();
        engine = nullptr;
        return false;
    }

    // Prepare the script context with the function we wish to execute.
    r = ctx->Prepare(renderFunc);
    if (r < 0)
    {
        ctx->Release();
        engine->Release();
        engine = nullptr;
        return false;
    }

    // Execute the function.
    r = ctx->Execute();
    ctx->Release();
    engine->Release();
    engine = nullptr;
    configuration = FetchScriptData(filePath);
    return true;
}
ScriptData AngelscriptConfigurationEngine::GetScriptData()
{
    return configuration;
}

// AngelscriptRenderEngine implementation.
AngelscriptRenderEngine::AngelscriptRenderEngine()
{
    // Creates script engine.
    engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
    engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);
    engine->SetEngineProperty(asEP_AUTO_GARBAGE_COLLECT, false);
    engine->SetEngineProperty(asEP_BUILD_WITHOUT_LINE_CUES, true);
    if (engine == 0)
    {
        errorInfo = wxT("Failed to create script engine.");
        return;
    }

    // Configures script engine.
    RegisterStdString(engine);
    RegisterScriptMathReal(engine);
    RegisterScriptMathComplex(engine);
    RegisterWxChaosInterface(engine);

    ctx = nullptr;
}
AngelscriptRenderEngine::~AngelscriptRenderEngine()
{
    if (engine != nullptr)
    {
        engine->Release();
        engine = nullptr;
    }
}

bool AngelscriptRenderEngine::RegisterGlobalVariable(const char* declaration, void* pointer)
{
    const int r = engine->RegisterGlobalProperty(declaration, pointer);
    if (r < 0)
    {
        engine->Release();
        engine = nullptr;
        errorInfo = wxT("Error while registering global variable.");
        return false;
    }

    return true;
}
bool AngelscriptRenderEngine::CompileFromPath(string path)
{
    // Compile the script code.
    const int r = CompileScriptFromPath(engine, path);
    if (r < 0)
    {
        engine->Release();
        engine = nullptr;
        errorInfo = wxT("Compile error.");
        return false;
    }
    return true;
}
bool AngelscriptRenderEngine::Execute()
{
    // Create a context that will execute the script.
    ctx = engine->CreateContext();
    if (ctx == 0)
    {
        errorInfo = wxT("Failed to create the context.");
        engine->Release();
        return false;
    }

    // Find the function for the function we want to execute.
    asIScriptFunction* renderFunc = engine->GetModule(0)->GetFunctionByDecl("void Render()");
    if (renderFunc == 0)
    {
        errorInfo = wxT("The function 'Render' was not found.");
        ctx->Release();
        engine->Release();
        engine = nullptr;
        ctx = nullptr;
        return false;
    }

    // Prepare the script context with the function we wish to execute.
    int r = ctx->Prepare(renderFunc);
    if (r < 0)
    {
        errorInfo = wxT("Failed to prepare the context.");
        ctx->Release();
        engine->Release();
        return false;
    }
    ctx->Execute();

    ctx->Release();
    engine->Release();
    engine = nullptr;
    ctx = nullptr;
    asThreadCleanup();

    return true;
}
void AngelscriptRenderEngine::Abort()
{
    if (ctx != nullptr)
        ctx->Abort();
}