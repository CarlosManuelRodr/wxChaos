#include <doctest/doctest.h>

#include "wx/common/PanelOptions.h"

TEST_CASE("PanelOptions starts empty and not forced visible")
{
    const PanelOptions options;

    CHECK(options.GetElementsSize() == 0);
    CHECK_FALSE(options.GetForceShow());
}

TEST_CASE("PanelOptions stores linked integer option metadata")
{
    int iterations = 100;
    PanelOptions options;

    options.LinkInt(PanelOptionType::Spin, "Iterations", &iterations, "100");

    CHECK(options.GetElementsSize() == 1);
    CHECK(options.GetPanelOptType(0) == PanelOptionType::Spin);
    CHECK(options.GetLinkType(0) == LinkTo::ToInt);
    CHECK(options.GetLabelElement(0) == "Iterations");
    CHECK(options.GetDefault(0) == "100");
    CHECK(options.GetIntElement(0) == &iterations);
}

TEST_CASE("PanelOptions stores linked double option metadata")
{
    double exponent = 2.0;
    PanelOptions options;

    options.LinkDbl(PanelOptionType::TextCtrl, "Exponent", &exponent, "2.0");

    CHECK(options.GetElementsSize() == 1);
    CHECK(options.GetPanelOptType(0) == PanelOptionType::TextCtrl);
    CHECK(options.GetLinkType(0) == LinkTo::ToDouble);
    CHECK(options.GetLabelElement(0) == "Exponent");
    CHECK(options.GetDefault(0) == "2.0");
    CHECK(options.GetDoubleElement(0) == &exponent);
}

TEST_CASE("PanelOptions stores linked bool option metadata")
{
    bool enabled = true;
    PanelOptions options;

    options.LinkBool(PanelOptionType::CheckBox, "Enabled", &enabled, "true");

    CHECK(options.GetElementsSize() == 1);
    CHECK(options.GetPanelOptType(0) == PanelOptionType::CheckBox);
    CHECK(options.GetLinkType(0) == LinkTo::ToBool);
    CHECK(options.GetLabelElement(0) == "Enabled");
    CHECK(options.GetDefault(0) == "true");
    CHECK(options.GetBoolElement(0) == &enabled);
}

TEST_CASE("PanelOptions force-show flag can be toggled")
{
    PanelOptions options;

    options.SetForceShow(true);
    CHECK(options.GetForceShow());

    options.SetForceShow(false);
    CHECK_FALSE(options.GetForceShow());
}
