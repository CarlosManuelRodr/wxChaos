#include "../FractalIncludes.h"
#include "../FractalTypes.h"
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"
#include "../FractalUtils.h"
FractalHandler::FractalHandler()
{
    mandelbrot = nullptr;
    mandelbrotZN = nullptr;
    julia = nullptr;
    juliaZN = nullptr;
    newton = nullptr;
    sinoidal = nullptr;
    magnet = nullptr;
    sierpinskyTriangle = nullptr;
    medusa = nullptr;
    manowar = nullptr;
    manowarJulia = nullptr;
    fixedPoint1 = nullptr;
    fixedPoint2 = nullptr;
    fixedPoint3 = nullptr;
    fixedPoint4 = nullptr;
    tricorn = nullptr;
    burningShip = nullptr;
    burningShipJulia = nullptr;
    fractory = nullptr;
    cell = nullptr;
    dPendulum = nullptr;
    userDefined = nullptr;
    fpUserDefined = nullptr;
    scriptFractal = nullptr;
    target = nullptr;
    type = FractalType::Undefined;
}
FractalHandler::~FractalHandler()
{
    this->DeleteFractal();
}
void FractalHandler::CreateFractal(FractalType _type, sf::RenderWindow* Window)
{
    this->DeleteFractal();
    type = _type;
    switch(_type)
    {
    case FractalType::Mandelbrot:
        {
            target = mandelbrot = new Mandelbrot(Window);
            break;
        }
    case FractalType::MandelbrotZN:
        {
            target = mandelbrotZN = new MandelbrotZN(Window);
            break;
        }
    case FractalType::Julia:
        {
            target = julia = new Julia(Window);
            break;
        }
    case FractalType::JuliaZN:
        {
            target = juliaZN = new JuliaZN(Window);
            break;
        }
    case FractalType::NewtonRaphsonMethod:
        {
            target = newton = new Newton(Window);
            break;
        }
    case FractalType::Sinoidal:
        {
            target = sinoidal = new Sinoidal(Window);
            break;
        }
    case FractalType::Magnetic:
        {
            target = magnet = new Magnet(Window);
            break;
        }
    case FractalType::Medusa:
        {
            target = medusa = new Medusa(Window);
            break;
        }
    case FractalType::Manowar:
        {
            target = manowar = new Manowar(Window);
            break;
        }
    case FractalType::ManowarJulia:
        {
            target = manowarJulia = new ManowarJulia(Window);
            break;
        }
    case FractalType::SierpinskyTriangle:
        {
            target = sierpinskyTriangle = new SierpTriangle(Window);
            break;
        }
    case FractalType::FixedPoint1:
        {
            target = fixedPoint1 = new FixedPoint1(Window);
            break;
        }
    case FractalType::FixedPoint2:
        {
            target = fixedPoint2 = new FixedPoint2(Window);
            break;
        }
    case FractalType::FixedPoint3:
        {
            target = fixedPoint3 = new FixedPoint3(Window);
            break;
        }
    case FractalType::FixedPoint4:
        {
            target = fixedPoint4 = new FixedPoint4(Window);
            break;
        }
    case FractalType::Tricorn:
        {
            target = tricorn = new Tricorn(Window);
            break;
        }
    case FractalType::BurningShip:
        {
            target = burningShip = new BurningShip(Window);
            break;
        }
    case FractalType::BurningShipJulia:
        {
            target = burningShipJulia = new BurningShipJulia(Window);
            break;
        }
    case FractalType::Fractory:
        {
            target = fractory = new Fractory(Window);
            break;
        }
    case FractalType::Cell:
        {
            target = cell = new Cell(Window);
            break;
        }
    case FractalType::DoublePendulum:
        {
            target = dPendulum = new DPendulum(Window);
            break;
        }
    case FractalType::UserDefined:
        {
            target = userDefined = new UserDefined(Window);
            break;
        }
    case FractalType::FixedPointUserDefined:
        {
            target = fpUserDefined = new FPUserDefined(Window);
            break;
        }
    default: break;
    }
}
void FractalHandler::CreateFractal(FractalType _type, int width, int height)
{
    this->DeleteFractal();
    type = _type;
    switch(_type)
    {
    case FractalType::Mandelbrot:
        {
            target = mandelbrot = new Mandelbrot(width, height);
            break;
        }
    case FractalType::MandelbrotZN:
        {
            target = mandelbrotZN = new MandelbrotZN(width, height);
            break;
        }
    case FractalType::Julia:
        {
            target = julia = new Julia(width, height);
            break;
        }
    case FractalType::JuliaZN:
        {
            target = juliaZN = new JuliaZN(width, height);
            break;
        }
    case FractalType::NewtonRaphsonMethod:
        {
            target = newton = new Newton(width, height);
            break;
        }
    case FractalType::Sinoidal:
        {
            target = sinoidal = new Sinoidal(width, height);
            break;
        }
    case FractalType::Magnetic:
        {
            target = magnet = new Magnet(width, height);
            break;
        }
    case FractalType::Medusa:
        {
            target = medusa = new Medusa(width, height);
            break;
        }
    case FractalType::Manowar:
        {
            target = manowar = new Manowar(width, height);
            break;
        }
    case FractalType::ManowarJulia:
        {
            target = manowarJulia = new ManowarJulia(width, height);
            break;
        }
    case FractalType::SierpinskyTriangle:
        {
            target = sierpinskyTriangle = new SierpTriangle(width, height);
            break;
        }
    case FractalType::FixedPoint1:
        {
            target = fixedPoint1 = new FixedPoint1(width, height);
            break;
        }
    case FractalType::FixedPoint2:
        {
            target = fixedPoint2 = new FixedPoint2(width, height);
            break;
        }
    case FractalType::FixedPoint3:
        {
            target = fixedPoint3 = new FixedPoint3(width, height);
            break;
        }
    case FractalType::FixedPoint4:
        {
            target = fixedPoint4 = new FixedPoint4(width, height);
            break;
        }
    case FractalType::Tricorn:
        {
            target = tricorn = new Tricorn(width, height);
            break;
        }
    case FractalType::BurningShip:
        {
            target = burningShip = new BurningShip(width, height);
            break;
        }
    case FractalType::BurningShipJulia:
        {
            target = burningShipJulia = new BurningShipJulia(width, height);
            break;
        }
    case FractalType::Fractory:
        {
            target = fractory = new Fractory(width, height);
            break;
        }
    case FractalType::Cell:
        {
            target = cell = new Cell(width, height);
            break;
        }
    case FractalType::DoublePendulum:
        {
            target = dPendulum = new DPendulum(width, height);
            break;
        }
    case FractalType::UserDefined:
        {
            target = userDefined = new UserDefined(width, height);
            break;
        }
    case FractalType::FixedPointUserDefined:
        {
            target = fpUserDefined = new FPUserDefined(width, height);
            break;
        }
    default: break;
    }
}
void FractalHandler::CreateScriptFractal(sf::RenderWindow* Window, ScriptData scriptData)
{
    target = scriptFractal = new ScriptFractal(Window, scriptData);
}
void FractalHandler::CreateScriptFractal(int width, int height, ScriptData scriptData)
{
    target = scriptFractal = new ScriptFractal(width, height, scriptData);
}
void FractalHandler::CreateScriptFractal(int width, int height, string scriptPath)
{
    target = scriptFractal = new ScriptFractal(width, height, scriptPath);
}
void FractalHandler::SetFormula(FormulaOpt formula)
{
    if(type == FractalType::UserDefined)
    {
        if(formula.type != FormulaType::Complex)
        {
            formula.type = FormulaType::Complex;
            formula.userFormula = wxT("z = z^2 + c");
        }
        userDefined->SetFormula(formula);
    }
    else if(type == FractalType::FixedPointUserDefined)
    {
        if(formula.type != FormulaType::FixedPoint)
        {
            formula.type = FormulaType::FixedPoint;
            formula.userFormula = wxT("z = sin(z)");
        }
        fpUserDefined->SetFormula(formula);
    }
}
void FractalHandler::DeleteFractal()
{
    target = nullptr;
    if(mandelbrot != nullptr)
    {
        delete mandelbrot;
        mandelbrot = nullptr;
    }
    if(mandelbrotZN != nullptr)
    {
        delete mandelbrotZN;
        mandelbrotZN = nullptr;
    }
    if(julia != nullptr)
    {
        delete julia;
        julia = nullptr;
    }
    if(juliaZN != nullptr)
    {
        delete juliaZN;
        juliaZN = nullptr;
    }
    if(newton != nullptr)
    {
        delete newton;
        newton = nullptr;
    }
    if(sinoidal != nullptr)
    {
        delete sinoidal;
        sinoidal = nullptr;
    }
    if(magnet != nullptr)
    {
        delete magnet;
        magnet = nullptr;
    }
    if(medusa != nullptr)
    {
        delete medusa;
        medusa = nullptr;
    }
    if(manowar != nullptr)
    {
        delete manowar;
        manowar = nullptr;
    }
    if(manowarJulia != nullptr)
    {
        delete manowarJulia;
        manowarJulia = nullptr;
    }
    if(sierpinskyTriangle != nullptr)
    {
        delete sierpinskyTriangle;
        sierpinskyTriangle = nullptr;
    }
    if(fixedPoint1 != nullptr)
    {
        delete fixedPoint1;
        fixedPoint1 = nullptr;
    }
    if(fixedPoint2 != nullptr)
    {
        delete fixedPoint2;
        fixedPoint2 = nullptr;
    }
    if(fixedPoint3 != nullptr)
    {
        delete fixedPoint3;
        fixedPoint3 = nullptr;
    }
    if(fixedPoint4 != nullptr)
    {
        delete fixedPoint4;
        fixedPoint4 = nullptr;
    }
    if(tricorn != nullptr)
    {
        delete tricorn;
        tricorn = nullptr;
    }
    if(burningShip != nullptr)
    {
        delete burningShip;
        burningShip = nullptr;
    }
    if(burningShipJulia != nullptr)
    {
        delete burningShipJulia;
        burningShipJulia = nullptr;
    }
    if(fractory != nullptr)
    {
        delete fractory;
        fractory = nullptr;
    }
    if(cell != nullptr)
    {
        delete cell;
        cell = nullptr;
    }
    if(dPendulum != nullptr)
    {
        delete dPendulum;
        dPendulum = nullptr;
    }
    if(userDefined != nullptr)
    {
        delete userDefined;
        userDefined = nullptr;
    }
    if(fpUserDefined != nullptr)
    {
        delete fpUserDefined;
        fpUserDefined = nullptr;
    }
    if(scriptFractal != nullptr)
    {
        delete scriptFractal;
        scriptFractal = nullptr;
    }
}
Fractal* FractalHandler::GetFractalPtr()
{
    return target;
}
FractalType FractalHandler::GetType()
{
    return type;
}
