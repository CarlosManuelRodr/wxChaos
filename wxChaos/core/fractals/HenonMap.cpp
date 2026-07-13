#include "HenonMap.h"
#include "Translation.h"

HenonMap::HenonMap(const unsigned int width, const unsigned int height) : RasterFractal(width, height)
{
    _type = FractalType::HenonMap;
    _algorithm = RenderingAlgorithmType::ChaoticMap;
    _availableAlg.push_back(RenderingAlgorithmType::ChaoticMap);

    _threadNumber = 1;
    _redrawAlways = true;
    ConfigureIterationDefaults(20000, 1000);

    _minX = -1.5;
    _maxX = 1.5;
    _minY = -1.0;
    _maxY = 0.4;
    _xFactor = (_maxX - _minX) / (_screenWidth - 1);
    _yFactor = (_maxY - _minY) / (_screenHeight - 1);
    SetPreciseView(PreciseRect(Rect(_minX, _minY, _maxX, _maxY)));

    _panelOpt.SetForceShow(true);
    _panelOpt.LinkDouble(PanelOptionType::TextCtrl, WXCHAOS_TRANSLATE_NOOP("Alpha: "), &_alpha, "1.4");
    _panelOpt.LinkDouble(PanelOptionType::TextCtrl, WXCHAOS_TRANSLATE_NOOP("Beta: "), &_beta, "0.3");
    _panelOpt.LinkDouble(PanelOptionType::TextCtrl, WXCHAOS_TRANSLATE_NOOP("X0: "), &_x0, "0.5");
    _panelOpt.LinkDouble(PanelOptionType::TextCtrl, WXCHAOS_TRANSLATE_NOOP("Y0: "), &_y0, "0.2");
    _alpha = 1.4;
    _beta = 0.3;
    _x0 = 0.5;
    _y0 = 0.2;

    SetExteriorColorMode(false);
    _myRender = new HenonMapRenderer[_threadNumber];
}

HenonMap::~HenonMap()
{
    this->StopRender();
    delete[] _myRender;
}

void HenonMap::Render()
{
    _myRender[0].SetParams(_alpha, _beta, _x0, _y0);
    this->SetRendererBounds<HenonMapRenderer>(_myRender);
}

void HenonMap::CopyOptionFromPanel()
{
    _alpha = *_panelOpt.GetDoubleValue(0);
    _beta = *_panelOpt.GetDoubleValue(1);
    _x0 = *_panelOpt.GetDoubleValue(2);
    _y0 = *_panelOpt.GetDoubleValue(3);
}
