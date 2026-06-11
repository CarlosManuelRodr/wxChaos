#pragma once
#include "FractalTypes.h"
#include "../scripting/ScriptData.h"

/*
* @class FractalHandler
* @brief Interface class to instantiate fractals.
*/
class FractalHandler
{
    Mandelbrot* _mandelbrot;
    MandelbrotZN* _mandelbrotZN;
    Julia* _julia;
    JuliaZN* _juliaZN;
    Newton* _newton;
    Sinoidal* _sinoidal;
    Magnet* _magnet;
    Medusa* _medusa;
    Manowar* _manowar;
    ManowarJulia* _manowarJulia;
    SierpinskyTriangle* _sierpinskyTriangle;
    FixedPoint1* _fixedPoint1;
    FixedPoint2* _fixedPoint2;
    FixedPoint3* _fixedPoint3;
    FixedPoint4* _fixedPoint4;
    Tricorn* _tricorn;
    BurningShip* _burningShip;
    BurningShipJulia* _burningShipJulia;
    Fractory* _fractory;
    Cell* _cell;
    DoublePendulum* _doublePendulum;
    UserDefined* _userDefined;
    FPUserDefined* _fpUserDefined;
    ScriptFractal* _scriptFractal;

    Fractal* _target;
    FractalType _type;

public:
    FractalHandler();
    ~FractalHandler();

    void CreateFractal(FractalType type, unsigned int width, unsigned int height);
    void CreateScriptFractal(unsigned int width, unsigned int height, const ScriptData& scriptData);
    void CreateScriptFractal(unsigned int width, unsigned int height, const std::string& scriptPath);
    void SetFormula(FormulaOptions formula) const;
    void DeleteFractal();
    Fractal* GetFractalPtr() const;
    FractalType GetType() const;
};
