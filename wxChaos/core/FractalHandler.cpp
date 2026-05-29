#include "FractalHandler.h"
#include "FractalTypes.h"
using namespace std;

FractalHandler::FractalHandler()
{
    _mandelbrot = nullptr;
    _mandelbrotZN = nullptr;
    _julia = nullptr;
    _juliaZN = nullptr;
    _newton = nullptr;
    _sinoidal = nullptr;
    _magnet = nullptr;
    _sierpinskyTriangle = nullptr;
    _medusa = nullptr;
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
    _doublePendulum = nullptr;
    _userDefined = nullptr;
    _fpUserDefined = nullptr;
    _scriptFractal = nullptr;
    _target = nullptr;
    _type = FractalType::Undefined;
}
FractalHandler::~FractalHandler()
{
    this->DeleteFractal();
}
void FractalHandler::CreateFractal(FractalType type, int width, int height)
{
    this->DeleteFractal();
    _type = type;
    switch(type)
    {
    case FractalType::Mandelbrot:
        {
            _target = _mandelbrot = new Mandelbrot(width, height);
            break;
        }
    case FractalType::MandelbrotZN:
        {
            _target = _mandelbrotZN = new MandelbrotZN(width, height);
            break;
        }
    case FractalType::Julia:
        {
            _target = _julia = new Julia(width, height);
            break;
        }
    case FractalType::JuliaZN:
        {
            _target = _juliaZN = new JuliaZN(width, height);
            break;
        }
    case FractalType::NewtonRaphsonMethod:
        {
            _target = _newton = new Newton(width, height);
            break;
        }
    case FractalType::Sinoidal:
        {
            _target = _sinoidal = new Sinoidal(width, height);
            break;
        }
    case FractalType::Magnetic:
        {
            _target = _magnet = new Magnet(width, height);
            break;
        }
    case FractalType::Medusa:
        {
            _target = _medusa = new Medusa(width, height);
            break;
        }
    case FractalType::Manowar:
        {
            _target = _manowar = new Manowar(width, height);
            break;
        }
    case FractalType::ManowarJulia:
        {
            _target = _manowarJulia = new ManowarJulia(width, height);
            break;
        }
    case FractalType::SierpinskyTriangle:
        {
            _target = _sierpinskyTriangle = new SierpinskyTriangle(width, height);
            break;
        }
    case FractalType::FixedPoint1:
        {
            _target = _fixedPoint1 = new FixedPoint1(width, height);
            break;
        }
    case FractalType::FixedPoint2:
        {
            _target = _fixedPoint2 = new FixedPoint2(width, height);
            break;
        }
    case FractalType::FixedPoint3:
        {
            _target = _fixedPoint3 = new FixedPoint3(width, height);
            break;
        }
    case FractalType::FixedPoint4:
        {
            _target = _fixedPoint4 = new FixedPoint4(width, height);
            break;
        }
    case FractalType::Tricorn:
        {
            _target = _tricorn = new Tricorn(width, height);
            break;
        }
    case FractalType::BurningShip:
        {
            _target = _burningShip = new BurningShip(width, height);
            break;
        }
    case FractalType::BurningShipJulia:
        {
            _target = _burningShipJulia = new BurningShipJulia(width, height);
            break;
        }
    case FractalType::Fractory:
        {
            _target = _fractory = new Fractory(width, height);
            break;
        }
    case FractalType::Cell:
        {
            _target = _cell = new Cell(width, height);
            break;
        }
    case FractalType::DoublePendulum:
        {
            _target = _doublePendulum = new DoublePendulum(width, height);
            break;
        }
    case FractalType::UserDefined:
        {
            _target = _userDefined = new UserDefined(width, height);
            break;
        }
    case FractalType::FixedPointUserDefined:
        {
            _target = _fpUserDefined = new FPUserDefined(width, height);
            break;
        }
    default: break;
    }
}
void FractalHandler::CreateScriptFractal(int width, int height, ScriptData scriptData)
{
    _target = _scriptFractal = new ScriptFractal(width, height, scriptData);
}
void FractalHandler::CreateScriptFractal(int width, int height, string scriptPath)
{
    _target = _scriptFractal = new ScriptFractal(width, height, scriptPath);
}
void FractalHandler::SetFormula(FormulaOpt formula) const
{
    if (_type == FractalType::UserDefined)
    {
        if (formula.type != FormulaType::Complex)
        {
            formula.type = FormulaType::Complex;
            formula.userFormula = wxT("z = z^2 + c");
        }
        _userDefined->SetFormula(formula);
    }
    else if (_type == FractalType::FixedPointUserDefined)
    {
        if (formula.type != FormulaType::FixedPoint)
        {
            formula.type = FormulaType::FixedPoint;
            formula.userFormula = wxT("z = sin(z)");
        }
        _fpUserDefined->SetFormula(formula);
    }
}
void FractalHandler::DeleteFractal()
{
    _target = nullptr;
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
    if (_sinoidal != nullptr)
    {
        delete _sinoidal;
        _sinoidal = nullptr;
    }
    if (_magnet != nullptr)
    {
        delete _magnet;
        _magnet = nullptr;
    }
    if (_medusa != nullptr)
    {
        delete _medusa;
        _medusa = nullptr;
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
    if (_sierpinskyTriangle != nullptr)
    {
        delete _sierpinskyTriangle;
        _sierpinskyTriangle = nullptr;
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
    if (_scriptFractal != nullptr)
    {
        delete _scriptFractal;
        _scriptFractal = nullptr;
    }
}
Fractal* FractalHandler::GetFractalPtr() const
{
    return _target;
}
FractalType FractalHandler::GetType() const
{
    return _type;
}
