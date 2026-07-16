#pragma once
#include <string>
#include "types/FractalType.h"
#include "FormulaOptions.h"

class Fractal;
class Mandelbrot;
class MandelbrotZM;
class Julia;
class JuliaZM;
class Newton;
class Sinusoidal;
class Magnet;
class Jellyfish;
class Manowar;
class ManowarJulia;
class SierpinskiTriangle;
class KochSnowflake;
class VectorSierpinskiTriangle;
class SierpinskiCarpet;
class FixedPoint1;
class FixedPoint2;
class FixedPoint3;
class FixedPoint4;
class Tricorn;
class BurningShip;
class BurningShipJulia;
class Fractory;
class Cell;
class LogisticMap;
class HenonMap;
class DoublePendulum;
class UserDefinedEscapeTime;
class UserDefinedFixedPoint;
class UserDefinedNewton;
class ScriptFractal;
struct ScriptData;

/**
* @class FractalFactory
* @brief Factory and owner for the currently selected Fractal instance.
*
* FractalFactory creates the requested built-in, user-defined, or script-backed
* fractal at the active render size, forwards formula changes to it, and deletes
* the previous instance when the selection changes.
*/
class FractalFactory
{
    Mandelbrot* _mandelbrot;
    MandelbrotZM* _mandelbrotZN;
    Julia* _julia;
    JuliaZM* _juliaZN;
    Newton* _newton;
    Sinusoidal* _sinusoidal;
    Magnet* _magnet;
    Jellyfish* _jellyfish;
    Manowar* _manowar;
    ManowarJulia* _manowarJulia;
    SierpinskiTriangle* _sierpinskiTriangle;
    KochSnowflake* _kochSnowflake;
    VectorSierpinskiTriangle* _vectorSierpinskiTriangle;
    SierpinskiCarpet* _sierpinskiCarpet;
    FixedPoint1* _fixedPoint1;
    FixedPoint2* _fixedPoint2;
    FixedPoint3* _fixedPoint3;
    FixedPoint4* _fixedPoint4;
    Tricorn* _tricorn;
    BurningShip* _burningShip;
    BurningShipJulia* _burningShipJulia;
    Fractory* _fractory;
    Cell* _cell;
    LogisticMap* _logisticMap;
    HenonMap* _henonMap;
    DoublePendulum* _doublePendulum;
    UserDefinedEscapeTime* _userDefined;
    UserDefinedFixedPoint* _fpUserDefined;
    UserDefinedNewton* _newtonUserDefined;
    ScriptFractal* _scriptFractal;

    Fractal* _fractal;
    FractalType _fractalType;

public:
    FractalFactory();
    ~FractalFactory();

    void CreateFractal(FractalType type, unsigned int width, unsigned int height);
    void CreateScriptFractal(unsigned int width, unsigned int height, const ScriptData& scriptData);
    void CreateScriptFractal(unsigned int width, unsigned int height, const std::string& scriptPath);
    void SetFormula(FormulaOptions formula);
    void DeleteFractal();
    [[nodiscard]] Fractal* GetFractal() const;
    [[nodiscard]] FractalType GetType() const;
};
