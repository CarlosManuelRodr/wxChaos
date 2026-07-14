#pragma once
#include "fractals/raster/Mandelbrot.h"
#include "fractals/raster/MandelbrotZM.h"
#include "fractals/raster/Julia.h"
#include "fractals/raster/JuliaZM.h"
#include "fractals/raster/Newton.h"
#include "fractals/raster/Sinusoidal.h"
#include "fractals/raster/Magnet.h"
#include "fractals/raster/Manowar.h"
#include "fractals/raster/ManowarJulia.h"
#include "fractals/raster/Jellyfish.h"
#include "fractals/raster/SierpinskiTriangle.h"
#include "fractals/vector/KochSnowflake.h"
#include "fractals/raster/FixedPoint1.h"
#include "fractals/raster/FixedPoint2.h"
#include "fractals/raster/FixedPoint3.h"
#include "fractals/raster/FixedPoint4.h"
#include "fractals/raster/Tricorn.h"
#include "fractals/raster/BurningShip.h"
#include "fractals/raster/BurningShipJulia.h"
#include "fractals/raster/Fractory.h"
#include "fractals/raster/Cell.h"
#include "fractals/raster/LogisticMap.h"
#include "fractals/raster/HenonMap.h"
#include "fractals/raster/DoublePendulum.h"
#include "fractals/raster/UserDefinedEscapeTime.h"
#include "fractals/raster/UserDefinedFixedPoint.h"
#include "fractals/raster/UserDefinedNewton.h"
#include "fractals/raster/ScriptFractal.h"
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
