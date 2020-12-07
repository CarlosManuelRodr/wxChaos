/**
* @file AngelScriptFunc.h
* @brief Contains definitions of functions used by the user scripts.
*
* These are the functions callable by the user scripts. Also are declared procedures to
* compile scripts and register it's functions. At last is defined a structure to do complex number operations.
*
* @copyright GNU Public License.
* @author Carlos Manuel Rodriguez y Martinez
* @date 7/18/2012
*/

#pragma once
#ifndef _angelScriptFunc
#define _angelScriptFunc
#include <wx/wx.h>
#include <vector>
#include <cmath>
#include <complex>
#include "angelscript/include/angelscript.h"
#include "angelscript/add_on/scriptstdstring/scriptstdstring.h"

/**
* @enum ScriptCategory
* @brief Category of the script fractal.
*/
enum class ScriptCategory
{
    Complex,
    NumMet,
    Physic,
    Other
};

/**
* @struct ScriptData
* @brief Configuration data retrieved from the script.
*
* This is used later by the routine that renders the fractal and the routine that draws the menu elements
* in the MainFrame.
*/
struct ScriptData
{
    std::string file;
    std::string name;
    ScriptCategory cat;
    double minX, maxX, minY;
    int defaultIter;
    bool juliaVariety;
    bool redrawAlways;
    bool extColor;
    bool noSetMap;
};

// Variables accesible from the scripts.
extern std::vector<ScriptData> scriptDataVect;
extern bool** asSetMap;
extern int** asColorMap;
extern std::string name;
extern ScriptCategory cat;
extern double minX, maxX, minY;
extern int defaultIter;
extern bool minXSet, maxXSet, minYSet, juliaVarietySet, asRedrawAlways, extColor;
extern bool defaultIterSet;
extern bool noSetMap;

///@brief Compiles angelscript file.
///@param engine Pointer to the angelscript engine which will hold the script.
///@param filepath Path to the script.
int CompileScriptFromPath(asIScriptEngine* engine, const std::string filePath);

///@brief Register the functions used by the scripts.
///@param engine Pointer to the angelscript engine which will hold the script.
void RegisterAsFunctions(asIScriptEngine* engine);

void ClearConsoleText();   ///< Clears the variable used to send text to the console.

// Functions accesible from the script.
void asSetFractalName(std::string &str);
void asSetCategory(std::string &str);
void asSetMinX(double _minX);
void asSetMaxX(double _maxX);
void asSetMinY(double _minY);
void asSetDefaultIter(int _defaultIter);
void asSetJuliaVariety(bool mode);
void asSetRedrawAlways(bool mode);
void asSetExtColorMode(bool mode);
void asNoSetMap(bool mode);
void asSetPoint(int x, int y, bool setVal, int colorVal);
void asPrintString(std::string &str);
void asPrintInt(int num);
void asPrintFloat(double num);

void PushScriptData(std::string fileName);
void MessageCallback(const asSMessageInfo *msg, void *param);

/**
* @struct Complex
* @brief Wrapper of a complex number used to do complex number arithmetic in a script.
*/
struct Complex
{
    Complex();
    Complex(const Complex& other);
    Complex(const std::complex<double>& other);
    Complex(double r, double i = 0);

    // Assignment operator.
    Complex &operator=(const Complex& other);

    // Compound assigment operators.
    Complex &operator+=(const Complex& other);
    Complex &operator-=(const Complex& other);
    Complex &operator*=(const Complex& other);
    Complex &operator/=(const Complex& other);

    double norm() const;
    double squaredNorm() const;

    // Comparison.
    bool operator==(const Complex& other) const;
    bool operator!=(const Complex& other) const;

    // Math operators.
    Complex operator+(const Complex& other) const;
    Complex operator-(const Complex& other) const;
    Complex operator*(const Complex& other) const;
    Complex operator/(const Complex& other) const;

    // Complex values.
    std::complex<double> complexNum;
    double real();
    double imag();
};

void asPrintComplex(const Complex& num);

///@brief Register the real number functions in the script engine.
///@param engine Pointer to the angelscript engine which will hold the script.
void RegisterScriptMathReal(asIScriptEngine* engine);

///@brief Register the complex number class in the script engine.
///@param engine Pointer to the angelscript engine which will hold the script.
void RegisterScriptMathComplex(asIScriptEngine* engine);


class AngelscriptRenderEngine
{
private:
    asIScriptEngine* engine;
    asIScriptContext* ctx;
    wxString errorInfo;
public:
    AngelscriptRenderEngine();
    ~AngelscriptRenderEngine();

    bool RegisterGlobalVariable(const char* declaration, void* pointer);
    bool CompileFromPath(std::string path);
    bool Execute();
    void Abort();
};

#endif