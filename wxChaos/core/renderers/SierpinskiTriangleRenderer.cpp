#include "SierpinskiTriangleRenderer.h"
using namespace std;

SierpinskiTriangleRenderer::SierpinskiTriangleRenderer() = default;

void SierpinskiTriangleRenderer::Render()
{
    const auto renderPixel = [this](const auto& pixelRe, const auto& pixelIm)
    {
        auto zRe = pixelRe;
        auto zIm = pixelIm;
        bool insideSet = true;
        unsigned int iterations = 0;

        for (unsigned n = 0; n < _maxIter; n++)
        {
            if (zRe * zRe + zIm * zIm > decltype(zRe)(4))
            {
                insideSet = false;
                break;
            }

            if (zIm > decltype(zIm)(0.5))
            {
                zRe *= decltype(zRe)(2);
                zIm = decltype(zIm)(2) * zIm - decltype(zIm)(1);
            }
            else if (zRe > decltype(zRe)(0.5))
            {
                zRe = decltype(zRe)(2) * zRe - decltype(zRe)(1);
                zIm *= decltype(zIm)(2);
            }
            else
            {
                zRe *= decltype(zRe)(2);
                zIm *= decltype(zIm)(2);
            }

            iterations = n;
        }

        if (insideSet)
            _setMap[_x][_y] = true;

        _colorMap[_x][_y] = iterations;
    };

    RenderPixelsByPrecision(renderPixel);
}
