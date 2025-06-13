#pragma once
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"

/*
* @class FractalHandler
* @brief Handles the fractal memory management.
*/
class FractalHandler
{
    Mandelbrot* mandelbrot;
    MandelbrotZN* mandelbrotZN;
    Julia* julia;
    JuliaZN* juliaZN;
    Newton* newton;
    Sinoidal* sinoidal;
    Magnet* magnet;
    Medusa* medusa;
    Manowar* manowar;
    ManowarJulia* manowarJulia;
    SierpTriangle* sierpinskyTriangle;
    FixedPoint1* fixedPoint1;
    FixedPoint2* fixedPoint2;
    FixedPoint3* fixedPoint3;
    FixedPoint4* fixedPoint4;
    Tricorn* tricorn;
    BurningShip* burningShip;
    BurningShipJulia* burningShipJulia;
    Fractory* fractory;
    Cell* cell;
    DPendulum* dPendulum;
    UserDefined* userDefined;
    FPUserDefined* fpUserDefined;
    ScriptFractal* scriptFractal;

    Fractal* target;
    FractalType type;

public:
    FractalHandler();
    ~FractalHandler();

    void CreateFractal(FractalType _type, sf::RenderWindow* Window);
    void CreateFractal(FractalType _type, int width, int height);
    void CreateScriptFractal(sf::RenderWindow* Window, ScriptData scriptData);
    void CreateScriptFractal(int width, int height, ScriptData scriptData);
    void CreateScriptFractal(int width, int height, string scriptPath);
    void SetFormula(FormulaOpt formula);
    void DeleteFractal();
    Fractal* GetFractalPtr();
    FractalType GetType();
};
