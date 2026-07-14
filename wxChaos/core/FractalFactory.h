#pragma once
#include "fractals/Mandelbrot.h"
#include "fractals/MandelbrotZM.h"
#include "fractals/Julia.h"
#include "fractals/JuliaZM.h"
#include "fractals/Newton.h"
#include "fractals/Sinusoidal.h"
#include "fractals/Magnet.h"
#include "fractals/Manowar.h"
#include "fractals/ManowarJulia.h"
#include "fractals/Jellyfish.h"
#include "fractals/SierpinskiTriangle.h"
#include "fractals/KochSnowflake.h"
#include "fractals/FixedPoint1.h"
#include "fractals/FixedPoint2.h"
#include "fractals/FixedPoint3.h"
#include "fractals/FixedPoint4.h"
#include "fractals/Tricorn.h"
#include "fractals/BurningShip.h"
#include "fractals/BurningShipJulia.h"
#include "fractals/Fractory.h"
#include "fractals/Cell.h"
#include "fractals/LogisticMap.h"
#include "fractals/HenonMap.h"
#include "fractals/DoublePendulum.h"
#include "fractals/UserDefinedEscapeTime.h"
#include "fractals/UserDefinedFixedPoint.h"
#include "fractals/UserDefinedNewton.h"
#include "fractals/ScriptFractal.h"
#include "../scripting/ScriptData.h"

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
    void SetFormula(FormulaOptions formula) const;
    void DeleteFractal();
    [[nodiscard]] Fractal* GetFractal() const;
    [[nodiscard]] FractalType GetType() const;
};
