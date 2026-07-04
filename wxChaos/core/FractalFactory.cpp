#include "FractalFactory.h"
#include "FractalTypes.h"
using namespace std;

FractalFactory::FractalFactory()
{
    _mandelbrot = nullptr;
    _mandelbrotZN = nullptr;
    _julia = nullptr;
    _juliaZN = nullptr;
    _newton = nullptr;
    _sinusoidal = nullptr;
    _magnet = nullptr;
    _sierpinskiTriangle = nullptr;
    _jellyfish = nullptr;
    _manowar = nullptr;
    _manowarJulia = nullptr;
    _fixedPoint1 = nullptr;
    _fixedPoint2 = nullptr;
    _fixedPoint3 = nullptr;
    _fixedPoint4 = nullptr;
    _tricorn = nullptr;
    _burningShip = nullptr;
    _burningShipJulia = nullptr;
    _fractory = nullptr;
    _cell = nullptr;
    _henonMap = nullptr;
    _doublePendulum = nullptr;
    _userDefined = nullptr;
    _fpUserDefined = nullptr;
    _newtonUserDefined = nullptr;
    _scriptFractal = nullptr;
    _fractal = nullptr;
    _fractalType = FractalType::Undefined;
}
FractalFactory::~FractalFactory()
{
    this->DeleteFractal();
}
void FractalFactory::CreateFractal(const FractalType type, const unsigned int width, const unsigned int height)
{
    this->DeleteFractal();
    _fractalType = type;
    switch(type)
    {
        case FractalType::Mandelbrot:
            {
                _fractal = _mandelbrot = new Mandelbrot(width, height);
                break;
            }
        case FractalType::MandelbrotZN:
            {
                _fractal = _mandelbrotZN = new MandelbrotZM(width, height);
                break;
            }
        case FractalType::Julia:
            {
                _fractal = _julia = new Julia(width, height);
                break;
            }
        case FractalType::JuliaZN:
            {
                _fractal = _juliaZN = new JuliaZM(width, height);
                break;
            }
        case FractalType::NewtonRaphsonMethod:
            {
                _fractal = _newton = new Newton(width, height);
                break;
            }
        case FractalType::Sinusoidal:
            {
                _fractal = _sinusoidal = new Sinusoidal(width, height);
                break;
            }
        case FractalType::Magnetic:
            {
                _fractal = _magnet = new Magnet(width, height);
                break;
            }
        case FractalType::Jellyfish:
            {
                _fractal = _jellyfish = new Jellyfish(width, height);
                break;
            }
        case FractalType::Manowar:
            {
                _fractal = _manowar = new Manowar(width, height);
                break;
            }
        case FractalType::ManowarJulia:
            {
                _fractal = _manowarJulia = new ManowarJulia(width, height);
                break;
            }
        case FractalType::SierpinskiTriangle:
            {
                _fractal = _sierpinskiTriangle = new SierpinskiTriangle(width, height);
                break;
            }
        case FractalType::FixedPoint1:
            {
                _fractal = _fixedPoint1 = new FixedPoint1(width, height);
                break;
            }
        case FractalType::FixedPoint2:
            {
                _fractal = _fixedPoint2 = new FixedPoint2(width, height);
                break;
            }
        case FractalType::FixedPoint3:
            {
                _fractal = _fixedPoint3 = new FixedPoint3(width, height);
                break;
            }
        case FractalType::FixedPoint4:
            {
                _fractal = _fixedPoint4 = new FixedPoint4(width, height);
                break;
            }
        case FractalType::Tricorn:
            {
                _fractal = _tricorn = new Tricorn(width, height);
                break;
            }
        case FractalType::BurningShip:
            {
                _fractal = _burningShip = new BurningShip(width, height);
                break;
            }
        case FractalType::BurningShipJulia:
            {
                _fractal = _burningShipJulia = new BurningShipJulia(width, height);
                break;
            }
        case FractalType::Fractory:
            {
                _fractal = _fractory = new Fractory(width, height);
                break;
            }
        case FractalType::Cell:
            {
                _fractal = _cell = new Cell(width, height);
                break;
            }
        case FractalType::HenonMap:
            {
                _fractal = _henonMap = new HenonMap(width, height);
                break;
            }
        case FractalType::DoublePendulum:
            {
                _fractal = _doublePendulum = new DoublePendulum(width, height);
                break;
            }
        case FractalType::UserDefinedEscapeTime:
            {
                _fractal = _userDefined = new UserDefinedEscapeTime(width, height);
                break;
            }
        case FractalType::UserDefinedFixedPoint:
            {
                _fractal = _fpUserDefined = new UserDefinedFixedPoint(width, height);
                break;
            }
        case FractalType::UserDefinedNewtonRaphson:
            {
                _fractal = _newtonUserDefined = new UserDefinedNewton(width, height);
                break;
            }
        default: break;
    }
}
void FractalFactory::CreateScriptFractal(const unsigned int width, const unsigned int height, const ScriptData& scriptData)
{
    _fractal = _scriptFractal = new ScriptFractal(width, height, scriptData);
}
void FractalFactory::CreateScriptFractal(const unsigned int width, const unsigned int height, const string& scriptPath)
{
    _fractal = _scriptFractal = new ScriptFractal(width, height, scriptPath);
}
void FractalFactory::SetFormula(FormulaOptions formula) const
{
    if (_fractalType == FractalType::UserDefinedEscapeTime)
    {
        if (formula.type != FormulaType::Complex)
        {
            formula.type = FormulaType::Complex;
            formula.userFormula = "z = z^2 + c";
        }
        _userDefined->SetFormula(formula);
    }
    else if (_fractalType == FractalType::UserDefinedFixedPoint)
    {
        if (formula.type != FormulaType::FixedPoint)
        {
            formula.type = FormulaType::FixedPoint;
            formula.userFormula = "z = sin(z)";
        }
        _fpUserDefined->SetFormula(formula);
    }
    else if (_fractalType == FractalType::UserDefinedNewtonRaphson)
    {
        if (formula.type != FormulaType::NewtonRaphson)
        {
            formula.type = FormulaType::NewtonRaphson;
            formula.userFormula = "z^3 - 1";
        }
        _newtonUserDefined->SetFormula(formula);
    }
}
void FractalFactory::DeleteFractal()
{
    _fractal = nullptr;
    if (_mandelbrot != nullptr)
    {
        delete _mandelbrot;
        _mandelbrot = nullptr;
    }
    if (_mandelbrotZN != nullptr)
    {
        delete _mandelbrotZN;
        _mandelbrotZN = nullptr;
    }
    if (_julia != nullptr)
    {
        delete _julia;
        _julia = nullptr;
    }
    if (_juliaZN != nullptr)
    {
        delete _juliaZN;
        _juliaZN = nullptr;
    }
    if (_newton != nullptr)
    {
        delete _newton;
        _newton = nullptr;
    }
    if (_sinusoidal != nullptr)
    {
        delete _sinusoidal;
        _sinusoidal = nullptr;
    }
    if (_magnet != nullptr)
    {
        delete _magnet;
        _magnet = nullptr;
    }
    if (_jellyfish != nullptr)
    {
        delete _jellyfish;
        _jellyfish = nullptr;
    }
    if (_manowar != nullptr)
    {
        delete _manowar;
        _manowar = nullptr;
    }
    if (_manowarJulia != nullptr)
    {
        delete _manowarJulia;
        _manowarJulia = nullptr;
    }
    if (_sierpinskiTriangle != nullptr)
    {
        delete _sierpinskiTriangle;
        _sierpinskiTriangle = nullptr;
    }
    if (_fixedPoint1 != nullptr)
    {
        delete _fixedPoint1;
        _fixedPoint1 = nullptr;
    }
    if (_fixedPoint2 != nullptr)
    {
        delete _fixedPoint2;
        _fixedPoint2 = nullptr;
    }
    if (_fixedPoint3 != nullptr)
    {
        delete _fixedPoint3;
        _fixedPoint3 = nullptr;
    }
    if (_fixedPoint4 != nullptr)
    {
        delete _fixedPoint4;
        _fixedPoint4 = nullptr;
    }
    if (_tricorn != nullptr)
    {
        delete _tricorn;
        _tricorn = nullptr;
    }
    if (_burningShip != nullptr)
    {
        delete _burningShip;
        _burningShip = nullptr;
    }
    if (_burningShipJulia != nullptr)
    {
        delete _burningShipJulia;
        _burningShipJulia = nullptr;
    }
    if (_fractory != nullptr)
    {
        delete _fractory;
        _fractory = nullptr;
    }
    if (_cell != nullptr)
    {
        delete _cell;
        _cell = nullptr;
    }
    if (_henonMap != nullptr)
    {
        delete _henonMap;
        _henonMap = nullptr;
    }
    if (_doublePendulum != nullptr)
    {
        delete _doublePendulum;
        _doublePendulum = nullptr;
    }
    if (_userDefined != nullptr)
    {
        delete _userDefined;
        _userDefined = nullptr;
    }
    if (_fpUserDefined != nullptr)
    {
        delete _fpUserDefined;
        _fpUserDefined = nullptr;
    }
    if (_newtonUserDefined != nullptr)
    {
        delete _newtonUserDefined;
        _newtonUserDefined = nullptr;
    }
    if (_scriptFractal != nullptr)
    {
        delete _scriptFractal;
        _scriptFractal = nullptr;
    }
}
Fractal* FractalFactory::GetFractal() const
{
    return _fractal;
}
FractalType FractalFactory::GetType() const
{
    return _fractalType;
}
