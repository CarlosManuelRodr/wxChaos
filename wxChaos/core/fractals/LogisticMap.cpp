#include "LogisticMap.h"

LogisticMap::LogisticMap(const unsigned int width, const unsigned int height) : Fractal(width, height)
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
    _panelOpt.LinkDouble(PanelOptionType::TextCtrl, "Seed: ", &_logisticSeed, "0.25");
    _panelOpt.LinkBool(PanelOptionType::CheckBox, "Stabilize point: ", &_stabilizePoint, "true");
    _logisticSeed = 0.25;
    _stabilizePoint = true;

    SetExteriorColorMode(false);
    _myRender = new LogisticMapRenderer[_threadNumber];
}

LogisticMap::~LogisticMap()
{
    this->StopRender();
    delete[] _myRender;
}

void LogisticMap::Render()
{
    _myRender[0].SetParams(_logisticSeed, _stabilizePoint);
    this->SetRendererBounds<LogisticMapRenderer>(_myRender);
}

void LogisticMap::CopyOptionFromPanel()
{
    _logisticSeed = *_panelOpt.GetDoubleValue(0);
    _stabilizePoint = *_panelOpt.GetBoolValue(0);
}
