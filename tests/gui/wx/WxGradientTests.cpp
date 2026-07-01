#include <doctest/doctest.h>
#include "wx/common/wxGradient.h"

namespace
{
    void CheckColor(const wxColour& color, const unsigned char red, const unsigned char green, const unsigned char blue)
    {
        CHECK(color.Red() == red);
        CHECK(color.Green() == green);
        CHECK(color.Blue() == blue);
    }
}

TEST_CASE("wxGradient interpolates between color stops")
{
    const wxGradient gradient({wxColour(0, 0, 0), wxColour(100, 150, 200)}, 0, 100);

    CheckColor(gradient.GetColorAt(0), 0, 0, 0);
    CheckColor(gradient.GetColorAt(50), 50, 75, 100);
    CheckColor(gradient.GetColorAt(100), 100, 150, 200);
}

TEST_CASE("wxGradient supports fractional positions")
{
    const wxGradient gradient({wxColour(0, 0, 0), wxColour(100, 200, 40)}, 0, 1);

    CheckColor(gradient.GetColorAt(0.25), 25, 50, 10);
    CheckColor(gradient.GetColorAt(0.75), 75, 150, 30);
}

TEST_CASE("wxGradient supports adding inserting editing and removing stops")
{
    wxGradient gradient;

    gradient.AddColorStop(wxColour(0, 0, 0));
    gradient.AddColorStop(wxColour(255, 255, 255));
    gradient.InsertColorStop(1, wxColour(10, 20, 30));
    gradient.EditColorStop(2, wxColour(40, 50, 60));
    gradient.RemoveColorStop(0);

    const std::vector<wxColour> stops = gradient.GetStops();
    REQUIRE(stops.size() == 2);
    CheckColor(stops[0], 10, 20, 30);
    CheckColor(stops[1], 40, 50, 60);
}

TEST_CASE("wxGradient parses and serializes CSS color stops")
{
    wxGradient gradient;

    gradient.FromString("rgb(1,2,3);rgb(4,5,6);");

    const std::vector<wxColour> stops = gradient.GetStops();
    REQUIRE(stops.size() == 2);
    CheckColor(stops[0], 1, 2, 3);
    CheckColor(stops[1], 4, 5, 6);
    CHECK(gradient.ToString().ToStdString() == "rgb(1, 2, 3);rgb(4, 5, 6);");
}

TEST_CASE("wxGradient uses black-to-white stops for empty strings")
{
    wxGradient gradient;

    gradient.FromString(wxEmptyString);

    const std::vector<wxColour> stops = gradient.GetStops();
    REQUIRE(stops.size() == 2);
    CheckColor(stops[0], 0, 0, 0);
    CheckColor(stops[1], 255, 255, 255);
}
