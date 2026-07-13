#pragma once

#include "Fractal.h"

/**
 * @class VectorFractal
 * @brief Base for fractals built from world-coordinate SFML drawing primitives.
 */
class VectorFractal : public Fractal
{
    bool**& SetMapStorage() override;
    bool** SetMapStorage() const override;
    double**& ColorMapStorage() override;
    double** ColorMapStorage() const override;
    RenderThreadPool& RenderPoolStorage() override;
    const RenderThreadPool& RenderPoolStorage() const override;
    std::vector<LineData>& OrbitLinesStorage() override;
    const std::vector<LineData>& OrbitLinesStorage() const override;

    void DrawPrimitives(sf::RenderTarget& target) const;

protected:
    void RedrawMaps() override { _refreshImage = true; }

public:
    VectorFractal(unsigned int width, unsigned int height);

    bool IsVectorFractal() const override { return true; }
    void Resize(unsigned int width, unsigned int height) override;
    void PrepareRender(Vector2Int reusedMapOffset = {0, 0}) override;
    void ReuseRenderedMaps(Vector2Int reusedMapOffset) override {}
    void PrepareDisplayColorLookup() override {}
    bool HasDisplayPixelColor(unsigned int x, unsigned int y) const override { return false; }
    sf::Color GetRenderedPixelColor(unsigned int x, unsigned int y) const override { return sf::Color::White; }
    bool SupportsAntiAliasing() const override { return false; }
    void SetAntiAliasingScale(unsigned int scale) override {}
    void RefreshAnimatedColors(sf::Image& image) override {}
    void ClearOrbitLines() override {}
    const std::vector<LineData>& GetOrbitLines() const override;
    PointSample GetPointSample(unsigned int x, unsigned int y) const override { return {false, 0.0, false}; }
    int GetRenderProgress() const override { return 100; }
    void PauseContinue() override {}
    bool StopRender() override { return false; }
    bool IsRendering() override { return false; }
    bool** GetSetMap() const override { return nullptr; }
    sf::Image GetRenderedImage() override;
    bool SaveBmp(const std::string& filename) override;
};
