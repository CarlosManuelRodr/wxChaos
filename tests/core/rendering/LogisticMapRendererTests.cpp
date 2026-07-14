#include <doctest/doctest.h>
#include <memory>
#include <vector>
#include "renderers/LogisticMapRenderer.h"

namespace
{
    struct RenderMaps
    {
        std::vector<std::unique_ptr<bool[]>> setRows;
        std::vector<std::unique_ptr<double[]>> colorRows;
        std::vector<bool*> setMap;
        std::vector<double*> colorMap;
        unsigned int height;

        RenderMaps(const unsigned int width, const unsigned int height)
            : setRows(width), colorRows(width), setMap(width), colorMap(width), height(height)
        {
            for (unsigned int x = 0; x < width; x++)
            {
                setRows[x] = std::make_unique<bool[]>(height);
                colorRows[x] = std::make_unique<double[]>(height);
                setMap[x] = setRows[x].get();
                colorMap[x] = colorRows[x].get();
            }
        }

        unsigned int CountPlottedPixels() const
        {
            unsigned int plottedPixels = 0;
            for (const auto& row : setRows)
            {
                for (unsigned int y = 0; y < height; y++)
                {
                    if (row[y])
                        plottedPixels++;
                }
            }
            return plottedPixels;
        }
    };

    Options LogisticOptions(const RenderingAlgorithmType algorithm)
    {
        constexpr unsigned int width = 100;
        constexpr unsigned int height = 80;

        Options options;
        options.minX = 2.8;
        options.maxX = 4.0;
        options.minY = -1.0;
        options.maxY = 1.0;
        options.xFactor = (options.maxX - options.minX) / (width - 1);
        options.yFactor = (options.maxY - options.minY) / (height - 1);
        options.maxIterations = 200;
        options.alg = algorithm;
        options.type = FractalType::LogisticMap;
        options.screenWidth = width;
        options.screenHeight = height;
        return options;
    }
}

TEST_CASE("Logistic map renderer plots the bifurcation diagram")
{
    constexpr unsigned int width = 100;
    constexpr unsigned int height = 80;
    RenderMaps maps(width, height);

    LogisticMapRenderer renderer;
    renderer.SetParams(0.25, true);
    renderer.SetOptions(LogisticOptions(RenderingAlgorithmType::ChaoticMap));
    renderer.SetOutputRenderingMaps(maps.setMap.data(), maps.colorMap.data());
    renderer.SetLimits(0, 0, width, height);
    renderer.Render();

    CHECK(maps.CountPlottedPixels() > 0);
}
