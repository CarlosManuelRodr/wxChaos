#pragma once

#include <memory>
#include "VectorRenderWorker.h"
#include "../Fractal.h"

/**
 * @class VectorFractal
 * @brief Base for fractals built from world-coordinate SFML drawing primitives.
 */
class VectorFractal : public Fractal
{
    std::unique_ptr<VectorRenderWorker> _vectorRenderWorker;

    void DrawPrimitives(sf::RenderTarget& target) const;
    void PublishCompletedGeometry();

protected:
    void RedrawMaps() override { _refreshImage = true; }
    void SetVectorRenderWorker(std::unique_ptr<VectorRenderWorker> worker);
    template<class Worker>
    Worker& GetVectorRenderWorker() { return static_cast<Worker&>(*_vectorRenderWorker); }
    void StartVectorRender();

public:
    VectorFractal(unsigned int width, unsigned int height);
    ~VectorFractal() override;

    bool IsVectorFractal() const override { return true; }
    void Resize(unsigned int width, unsigned int height) override;
    void PrepareRender(Vector2Int reusedMapOffset) override;
    void ReuseRenderedMaps(Vector2Int reusedMapOffset) override {}
    void PrepareDisplayColorLookup() override {}
    bool HasDisplayPixelColor(unsigned int x, unsigned int y) const override { return false; }
    sf::Color GetRenderedPixelColor(unsigned int x, unsigned int y) const override { return sf::Color::White; }
    bool SupportsAntiAliasing() const override { return false; }
    void SetAntiAliasingScale(unsigned int scale) override {}
    unsigned int GetAntiAliasingScale() const override { return 1; }
    void RefreshAnimatedColors(sf::Image& image) override {}
    void ClearOrbitLines() override {}
    const std::vector<LineData>& GetOrbitLines() const override;
    PointSample GetPointSample(unsigned int x, unsigned int y) const override { return {false, 0.0, false}; }
    int GetRenderProgress() const override;
    void PauseContinue() override;
    bool StopRender() override;
    bool IsRendering() override;
    bool** GetSetMap() const override { return nullptr; }
    sf::Image GetRenderedImage() override;
    bool SaveBmp(const std::string& filename) override;
};
