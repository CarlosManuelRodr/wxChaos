#include "LogisticMap.h"
#include <limits>
#include "Translation.h"

LogisticMap::LogisticMap(const unsigned int width, const unsigned int height) : RasterFractal(width, height)
{
    _type = FractalType::LogisticMap;
    _algorithm = RenderingAlgorithmType::ChaoticMap;
    _availableAlg.push_back(RenderingAlgorithmType::ChaoticMap);

    _threadNumber = 1;
    _redrawAlways = true;
    ConfigureIterationDefaults(500, 100);

    _minX = 2.8;
    _maxX = 4.0;
    _minY = 0.24;
    _maxY = 1.0;
    _xFactor = (_maxX - _minX) / (_screenWidth - 1);
    _yFactor = (_maxY - _minY) / (_screenHeight - 1);
    SetPreciseView(PreciseRect(Rect(_minX, _minY, _maxX, _maxY)));

    _panelOpt.SetForceShow(true);
    _panelOpt.LinkDouble(PanelOptionType::TextCtrl, WXCHAOS_TRANSLATE_NOOP("Seed: "), &_logisticSeed, "0.25");
    _panelOpt.LinkBool(PanelOptionType::CheckBox, WXCHAOS_TRANSLATE_NOOP("Stabilize point: "), &_stabilizePoint, "true");
    _logisticSeed = 0.25;
    _stabilizePoint = true;

    SetExteriorColorMode(false);
    _myRender = new LogisticMapRenderer[_threadNumber];
}

LogisticMap::~LogisticMap()
{
    this->RasterFractal::StopRender();
    delete[] _myRender;
}

void LogisticMap::Render()
{
    _myRender[0].SetParams(_logisticSeed, _stabilizePoint);
    this->SetRendererBounds<LogisticMapRenderer>(_myRender);
}

wxString LogisticMap::InspectPoint(const double x, const double y, const std::optional<unsigned int> iterations) const
{
    const unsigned int maxIterations = std::max(1U, iterations.value_or(_maxIterations));
    const double a = x;
    double orbitValue = _logisticSeed;

    if (_stabilizePoint)
    {
        for (unsigned int n = 0; n < maxIterations; n++)
        {
            orbitValue = a * orbitValue * (1.0 - orbitValue);
            if (!std::isfinite(orbitValue))
                break;
        }
    }

    bool exponentIsNegativeInfinity = false;
    bool exponentIsValid = std::isfinite(orbitValue);
    double derivativeLogSum = 0.0;
    for (unsigned int n = 0; n < maxIterations && exponentIsValid && !exponentIsNegativeInfinity; n++)
    {
        orbitValue = a * orbitValue * (1.0 - orbitValue);
        if (!std::isfinite(orbitValue))
        {
            exponentIsValid = false;
            break;
        }

        const double derivativeMagnitude = std::abs(a * (1.0 - 2.0 * orbitValue));
        if (derivativeMagnitude == 0.0)
        {
            exponentIsNegativeInfinity = true;
            break;
        }
        if (!std::isfinite(derivativeMagnitude))
        {
            exponentIsValid = false;
            break;
        }

        derivativeLogSum += std::log(derivativeMagnitude);
        if (!std::isfinite(derivativeLogSum))
        {
            exponentIsValid = false;
            break;
        }
    }

    wxString output;
    output << "Fractal: " << GetName() << "\n"
           << "Coordinates: (" << FormatNumber(x) << ", " << FormatNumber(y) << ")\n"
           << "Parameter a: " << FormatNumber(a) << "\n"
           << "Seed: " << FormatNumber(_logisticSeed) << "\n"
           << "Stabilized: " << (_stabilizePoint ? "yes" : "no") << "\n"
           << "Maximum iterations: " << maxIterations << "\n";

    if (!exponentIsValid)
    {
        output << "Lyapunov exponent: could not be calculated\n"
               << "Result: orbit left the finite numeric range";
        return output;
    }

    const double lyapunovExponent = exponentIsNegativeInfinity
                                        ? -std::numeric_limits<double>::infinity()
                                        : derivativeLogSum / maxIterations;
    output << "Lyapunov exponent: " << FormatNumber(lyapunovExponent) << "\n";

    constexpr double neutralTolerance = 1e-12;
    if (lyapunovExponent > neutralTolerance)
        output << "Result: chaotic";
    else if (lyapunovExponent < -neutralTolerance)
        output << "Result: stable or periodic";
    else
        output << "Result: neutral or bifurcation boundary";

    return output;
}

void LogisticMap::CopyOptionFromPanel()
{
    _logisticSeed = *_panelOpt.GetDoubleValue(0);
    _stabilizePoint = *_panelOpt.GetBoolValue(0);
}
