#include <doctest/doctest.h>
#include <memory>
#include <vector>
#include "../../../wxChaos/core/renderers/raster/HenonMapRenderer.h"

TEST_CASE("Henon map renderer plots the configured orbit")
{
    constexpr unsigned int width = 80;
    constexpr unsigned int height = 60;
    std::vector<std::unique_ptr<bool[]>> setRows(width);
    std::vector<std::unique_ptr<double[]>> colorRows(width);
    std::vector<bool*> setMap(width);
    std::vector<double*> colorMap(width);

    for (unsigned int x = 0; x < width; x++)
    {
        setRows[x] = std::make_unique<bool[]>(height);
        colorRows[x] = std::make_unique<double[]>(height);
        setMap[x] = setRows[x].get();
        colorMap[x] = colorRows[x].get();
    }

    Options options;
    options.minX = -1.5;
    options.maxX = 1.5;
    options.minY = -0.4;
    options.maxY = 0.4;
    options.xFactor = (options.maxX - options.minX) / (width - 1);
    options.yFactor = (options.maxY - options.minY) / (height - 1);
    options.maxIterations = 20000;
    options.alg = RenderingAlgorithmType::ChaoticMap;
    options.type = FractalType::HenonMap;
    options.screenWidth = width;
    options.screenHeight = height;

    HenonMapRenderer renderer;
    renderer.SetParams(1.4, 0.3, 0.5, 0.2);
    renderer.SetOptions(options);
    renderer.SetOutputRenderingMaps(setMap.data(), colorMap.data());
    renderer.SetLimits(0, 0, width, height);
    renderer.Render();

    unsigned int plottedPixels = 0;
    for (unsigned int x = 0; x < width; x++)
    {
        for (unsigned int y = 0; y < height; y++)
        {
            if (setRows[x][y])
                plottedPixels++;
        }
    }

    CHECK(plottedPixels > 0);
}
