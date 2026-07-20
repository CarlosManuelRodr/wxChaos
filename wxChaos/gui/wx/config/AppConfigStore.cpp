#include <algorithm>
#include <fstream>
#include <map>
#include <utility>
#include <wx/fileconf.h>
#include "AppConfigStore.h"
#include "coloring/ColorPalette.h"
#include "Version.h"
#include "../common/AppTheme.h"

std::string AppConfigStore::DefaultColorStyle()
{
    ColorPalette palette;
    palette.SetStyle(ClassicMandelbrot);
    return palette.grad;
}

AppConfig::AppConfig()
{
    ColorPalette palette;
    palette.SetStyle(ResolveColorStyle(colorStyle));
    paletteSize = palette.paletteSize;
    colorCycleLength = palette.colorCycleLength;
    colorStyleGrad = palette.grad;
}

ColorPaletteTypes AppConfig::ResolveColorStyle(const ColorPaletteTypes style)
{
    if (style == System)
        return AppTheme::IsDark() ? SunsetDrive : Retro;
    return style;
}

std::string AppConfigStore::Trim(const std::string& value)
{
    const auto first = std::find_if_not(value.begin(), value.end(), [](const unsigned char ch) {
        return std::isspace(ch) != 0;
    });
    const auto last = std::find_if_not(value.rbegin(), value.rend(), [](const unsigned char ch) {
        return std::isspace(ch) != 0;
    }).base();

    if (first >= last)
        return "";

    return {first, last};
}

std::string AppConfigStore::ToLower(std::string value)
{
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return value;
}

bool AppConfigStore::FileExists(const std::string& filename)
{
    std::ifstream file(filename.c_str());
    return file.is_open();
}

bool AppConfigStore::HasIniSections(const std::string& filename)
{
    std::ifstream file(filename.c_str());
    std::string line;

    while (std::getline(file, line))
    {
        const std::string trimmed = Trim(line);
        if (!trimmed.empty() && trimmed[0] == '[')
            return true;
    }

    return false;
}

std::map<std::string, std::string> AppConfigStore::ReadLegacyConfig(const std::string& filename)
{
    std::map<std::string, std::string> values;
    std::ifstream file(filename.c_str());
    std::string line;

    while (std::getline(file, line))
    {
        const std::string trimmed = Trim(line);
        if (trimmed.empty() || trimmed.find("//") == 0 || trimmed[0] == '#')
            continue;

        const std::string::size_type equalPos = trimmed.find('=');
        if (equalPos == std::string::npos)
            continue;

        values[Trim(trimmed.substr(0, equalPos))] = Trim(trimmed.substr(equalPos + 1));
    }

    return values;
}

bool AppConfigStore::ReadBool(const std::map<std::string, std::string>& values, const std::string& key, bool defaultValue)
{
    const auto it = values.find(key);
    if (it == values.end())
        return defaultValue;

    const std::string value = ToLower(Trim(it->second));
    if (value == "true" || value == "1" || value == "yes" || value == "on")
        return true;
    if (value == "false" || value == "0" || value == "no" || value == "off")
        return false;

    return defaultValue;
}

int AppConfigStore::ReadInt(const std::map<std::string, std::string>& values, const std::string& key, int defaultValue)
{
    const auto it = values.find(key);
    if (it == values.end())
        return defaultValue;

    try
    {
        const std::string trimmed = Trim(it->second);
        size_t parsedChars = 0;
        const int value = std::stoi(trimmed, &parsedChars);
        return parsedChars == trimmed.size() ? value : defaultValue;
    }
    catch (...)
    {
        return defaultValue;
    }
}

std::string AppConfigStore::ReadString(const std::map<std::string, std::string>& values, const std::string& key, const std::string& defaultValue)
{
    const auto it = values.find(key);
    return it == values.end() ? defaultValue : it->second;
}

int AppConfigStore::NormalizeAntiAliasingScale(const int scale)
{
    switch (scale)
    {
        case 2:
        case 4:
            return scale;
        default:
            return 1;
    }
}

const std::map<std::string, FractalType>& AppConfigStore::FractalTypes()
{
    static const std::map<std::string, FractalType> fractalTypes = {
        { "Mandelbrot", FractalType::Mandelbrot },
        { "MandelbrotZN", FractalType::MandelbrotZM },
        { "Julia", FractalType::Julia },
        { "JuliaZN", FractalType::JuliaZM },
        { "Newton", FractalType::NewtonRaphsonMethod },
        { "Sinusoidal", FractalType::Sinusoidal },
        { "Sinoidal", FractalType::Sinusoidal },
        { "Magnet", FractalType::Magnetic },
        { "Jellyfish", FractalType::Jellyfish },
        { "Medusa", FractalType::Jellyfish },
        { "Manowar", FractalType::Manowar },
        { "JManowar", FractalType::ManowarJulia },
        { "Sierpinski_Triangle", FractalType::SierpinskiTriangle },
        { "Sierpinsky_Triangle", FractalType::SierpinskiTriangle },
        { "Koch_Snowflake", FractalType::KochSnowflake },
        { "Vector_Sierpinski_Triangle", FractalType::VectorSierpinskiTriangle },
        { "Sierpinski_Carpet", FractalType::SierpinskiCarpet },
        { "FixedPoint1", FractalType::FixedPoint1 },
        { "FixedPoint2", FractalType::FixedPoint2 },
        { "FixedPoint3", FractalType::FixedPoint3 },
        { "FixedPoint4", FractalType::FixedPoint4 },
        { "Tricorn", FractalType::Tricorn },
        { "Burning_Ship", FractalType::BurningShip },
        { "Burning_Ship_Julia", FractalType::BurningShipJulia },
        { "Fractory", FractalType::Fractory },
        { "Cell", FractalType::Cell },
        { "Logistic", FractalType::LogisticMap },
        { "Logistic_Map", FractalType::LogisticMap },
        { "LogisticMap", FractalType::LogisticMap },
        { "Henon_Map", FractalType::HenonMap },
        { "HenonMap", FractalType::HenonMap },
        { "Double_Pendulum", FractalType::DoublePendulum },
        { "User_Defined_Escape_Time", FractalType::UserDefinedEscapeTime },
        { "User_Defined_Fixed_Point", FractalType::UserDefinedFixedPoint },
        { "User_Defined_Newton_Raphson", FractalType::UserDefinedNewtonRaphson }
    };

    return fractalTypes;
}

const std::map<std::string, ColorPaletteTypes>& AppConfigStore::ColorStyles()
{
    static const std::map<std::string, ColorPaletteTypes> colorStyles = {
        { "Retro", Retro },
        { "Hakim", Hakim },
        { "Aquamarine", Aquamarine },
        { "PastelDream", PastelDream },
        { "RoseGold", RoseGold },
        { "Gunmetal", Gunmetal },
        { "SunsetDrive", SunsetDrive },
        { "AuroraBorealis", AuroraBorealis },
        { "Vaporwave", Vaporwave },
        { "DeepOcean", DeepOcean },
        { "Ember", Ember },
        { "RainbowFire", RainbowFire },
        { "CoolBlue", CoolBlue },
        { "StarryNight", StarryNight },
        { "MoonlitGarden", MoonlitGarden },
        { "CoralReef", CoralReef },
        { "DesertMirage", DesertMirage },
        { "PolarEmber", PolarEmber },
        { "JadeTemple", JadeTemple },
        { "ClassicMandelbrot", ClassicMandelbrot },
        { "Custom", CustomGradient },
        { "System", System }
    };

    return colorStyles;
}

const std::map<std::string, AppAppearance>& AppConfigStore::Appearances()
{
    static const std::map<std::string, AppAppearance> appearances = {
        { "System", AppAppearance::System },
        { "Light", AppAppearance::Light },
        { "Dark", AppAppearance::Dark }
    };

    return appearances;
}

const std::map<std::string, AppLanguage>& AppConfigStore::Languages()
{
    static const std::map<std::string, AppLanguage> languages = {
        { "System", AppLanguage::System },
        { "English", AppLanguage::English },
        { "Spanish", AppLanguage::Spanish },
        { "Japanese", AppLanguage::Japanese }
    };

    return languages;
}

// ReSharper disable once CppDFAConstantParameter
FractalType AppConfigStore::FractalTypeFromString(const std::string& value, const FractalType defaultValue)
{
    const auto it = FractalTypes().find(value);
    return it == FractalTypes().end() ? defaultValue : it->second;
}

std::string AppConfigStore::FractalTypeToString(FractalType type)
{
    if (type == FractalType::Sinusoidal)
        return "Sinusoidal";
    if (type == FractalType::Jellyfish)
        return "Jellyfish";
    if (type == FractalType::SierpinskiTriangle)
        return "Sierpinski_Triangle";
    if (type == FractalType::KochSnowflake)
        return "Koch_Snowflake";
    if (type == FractalType::VectorSierpinskiTriangle)
        return "Vector_Sierpinski_Triangle";
    if (type == FractalType::SierpinskiCarpet)
        return "Sierpinski_Carpet";
    if (type == FractalType::LogisticMap)
        return "Logistic_Map";
    if (type == FractalType::HenonMap)
        return "Henon_Map";

    for (const auto& item : FractalTypes())
    {
        if (item.second == type)
            return item.first;
    }

    return "Mandelbrot";
}

ColorPaletteTypes AppConfigStore::ColorStyleFromString(const std::string& value, const ColorPaletteTypes defaultValue)
{
    const auto it = ColorStyles().find(value);
    return it == ColorStyles().end() ? defaultValue : it->second;
}

std::string AppConfigStore::ColorStyleToString(const ColorPaletteTypes type)
{
    for (const auto& item : ColorStyles())
    {
        if (item.second == type)
            return item.first;
    }

    return "Retro";
}

// ReSharper disable once CppDFAConstantParameter
AppAppearance AppConfigStore::AppearanceFromString(const std::string& value, const AppAppearance defaultValue)
{
    const auto it = Appearances().find(value);
    return it == Appearances().end() ? defaultValue : it->second;
}

std::string AppConfigStore::AppearanceToString(const AppAppearance appearance)
{
    for (const auto& item : Appearances())
    {
        if (item.second == appearance)
            return item.first;
    }

    return "System";
}

AppLanguage AppConfigStore::LanguageFromString(const std::string& value, const AppLanguage defaultValue)
{
    const auto it = Languages().find(value);
    return it == Languages().end() ? defaultValue : it->second;
}

std::string AppConfigStore::LanguageToString(const AppLanguage language)
{
    for (const auto& item : Languages())
    {
        if (item.second == language)
            return item.first;
    }

    return "System";
}

TutorialStatus AppConfigStore::TutorialStatusFromString(const std::string& value,
                                                        const TutorialStatus defaultValue)
{
    const std::string normalized = ToLower(Trim(value));
    if (normalized == "pending")
        return TutorialStatus::Pending;
    if (normalized == "completed")
        return TutorialStatus::Completed;
    if (normalized == "dismissed")
        return TutorialStatus::Dismissed;
    return defaultValue;
}

std::string AppConfigStore::TutorialStatusToString(const TutorialStatus status)
{
    switch (status)
    {
        case TutorialStatus::Completed:
            return "Completed";
        case TutorialStatus::Dismissed:
            return "Dismissed";
        case TutorialStatus::Pending:
        default:
            return "Pending";
    }
}

ColorPaletteTypes AppConfigStore::InferColorStyleFromGradient(const std::string& gradient)
{
    for (const auto& item : ColorStyles())
    {
        if (item.second == CustomGradient || item.second == System)
            continue;

        ColorPalette palette;
        palette.SetStyle(item.second);
        if (palette.grad == gradient)
            return item.second;
    }

    return CustomGradient;
}

AppConfig AppConfigStore::LoadLegacyConfig(const std::string& filename)
{
    AppConfig config;
    const std::map<std::string, std::string> values = ReadLegacyConfig(filename);

    config.type = FractalTypeFromString(ReadString(values, "FRACTAL_TYPE", "Mandelbrot"), config.type);
    config.automaticIterations = ReadBool(values, "AUTOMATIC_ITERATIONS", config.automaticIterations);
    config.paletteSize = ReadInt(values, "PALETTE_SIZE", config.paletteSize);
    config.colorCycleLength = ReadInt(values, "COLOR_CYCLE_LENGTH", config.colorCycleLength);
    config.colorStyleGrad = ReadString(values, "COLOR_STYLE", config.colorStyleGrad);
    if (config.colorStyleGrad.find("rgb(") == std::string::npos)
        config.colorStyleGrad = DefaultColorStyle();
    config.colorStyle = InferColorStyleFromGradient(config.colorStyleGrad);
    config.antiAliasingScale = NormalizeAntiAliasingScale(
        ReadInt(values, "ANTI_ALIASING_SCALE", config.antiAliasingScale));

    config.constantWindow = ReadBool(values, "CONSTANT_WINDOW", config.constantWindow);
    config.commandConsole = ReadBool(values, "COMMAND_CONSOLE", config.commandConsole);
    config.juliaMode = ReadBool(values, "JULIA_MODE", config.juliaMode);
    config.colorPaletteWindow = ReadBool(values, "COLOR_PALETTE_WINDOW", config.colorPaletteWindow);
    config.colorFractal = ReadBool(values, "COLOR_FRACTAL", config.colorFractal);
    config.colorSet = ReadBool(values, "COLOR_SET", config.colorSet);
    config.showWelcomeOnStartup =
        ReadBool(values, "SHOW_WELCOME_ON_STARTUP", config.showWelcomeOnStartup);
    config.targetFrameRate = std::max(
        AppConfig::MinimumTargetFrameRate, ReadInt(values, "TARGET_FRAME_RATE", config.targetFrameRate));
    config.zoomStepPercent = std::clamp(ReadInt(values, "ZOOM_STEP_PERCENT", config.zoomStepPercent), 1, 95);
    config.zoomInertiaMilliseconds = std::clamp(ReadInt(values, "ZOOM_INERTIA_MS", config.zoomInertiaMilliseconds), 0, 1000);
    if (values.find("APPEARANCE") != values.end())
        config.appearance = AppearanceFromString(ReadString(values, "APPEARANCE", "System"), config.appearance);
    else if (values.find("DARK_THEME") != values.end())
        config.appearance = ReadBool(values, "DARK_THEME", false) ? AppAppearance::Dark : AppAppearance::Light;
    if (values.find("LANGUAGE") != values.end())
        config.language = LanguageFromString(ReadString(values, "LANGUAGE", "System"), config.language);

    return config;
}

wxString AppConfigStore::ToWxString(const std::string& value)
{
    return wxString::FromUTF8(value.c_str());
}

AppConfigStore::AppConfigStore(std::string filename) : filename(std::move(filename)) {}

AppConfig AppConfigStore::Load() const
{
    AppConfig config;

    if (!FileExists(filename))
    {
        Save(config);
        return config;
    }

    if (!HasIniSections(filename))
    {
        config = LoadLegacyConfig(filename);
        Save(config);
        return config;
    }

    wxFileConfig fileConfig(wxEmptyString, wxEmptyString, ToWxString(filename));

    wxString colorStyle;
    fileConfig.Read("/Fractal/color_style", &colorStyle, ToWxString(config.colorStyleGrad));
    config.colorStyleGrad = colorStyle.ToStdString();
    if (config.colorStyleGrad.find("rgb(") == std::string::npos)
        config.colorStyleGrad = DefaultColorStyle();

    wxString colorStylePreset;
    if (fileConfig.Read("/Fractal/color_style_preset", &colorStylePreset))
        config.colorStyle = ColorStyleFromString(colorStylePreset.ToStdString(), InferColorStyleFromGradient(config.colorStyleGrad));
    else
        config.colorStyle = InferColorStyleFromGradient(config.colorStyleGrad);
    if (config.colorStyle != CustomGradient)
    {
        ColorPalette palette;
        palette.SetStyle(AppConfig::ResolveColorStyle(config.colorStyle));
        config.colorStyleGrad = palette.grad;
    }

    wxString fractalType;
    fileConfig.Read("/Fractal/fractal_type", &fractalType, "Mandelbrot");
    config.type = FractalTypeFromString(fractalType.ToStdString(), config.type);

    long intValue = config.paletteSize;
    fileConfig.Read("/Fractal/automatic_iterations", &config.automaticIterations, config.automaticIterations);

    fileConfig.Read("/Fractal/palette_size", &intValue, config.paletteSize);
    config.paletteSize = static_cast<int>(intValue);

    intValue = config.colorCycleLength;
    fileConfig.Read("/Fractal/color_cycle_length", &intValue, config.colorCycleLength);
    config.colorCycleLength = static_cast<int>(intValue);

    intValue = config.antiAliasingScale;
    fileConfig.Read("/Fractal/anti_aliasing_scale", &intValue, config.antiAliasingScale);
    config.antiAliasingScale = NormalizeAntiAliasingScale(static_cast<int>(intValue));

    fileConfig.Read("/Fractal/constant_window", &config.constantWindow, config.constantWindow);
    fileConfig.Read("/Fractal/command_console", &config.commandConsole, config.commandConsole);
    fileConfig.Read("/Fractal/julia_mode", &config.juliaMode, config.juliaMode);
    fileConfig.Read("/General/show_welcome_on_startup",
                    &config.showWelcomeOnStartup,
                    config.showWelcomeOnStartup);
    fileConfig.Read("/Color/palette_window", &config.colorPaletteWindow, config.colorPaletteWindow);
    fileConfig.Read("/Color/fractal", &config.colorFractal, config.colorFractal);
    fileConfig.Read("/Color/set", &config.colorSet, config.colorSet);

    intValue = config.targetFrameRate;
    fileConfig.Read("/General/target_frame_rate", &intValue, config.targetFrameRate);
    config.targetFrameRate = std::max(AppConfig::MinimumTargetFrameRate, static_cast<int>(intValue));

    intValue = config.zoomStepPercent;
    fileConfig.Read("/Zoom/step_percent", &intValue, config.zoomStepPercent);
    config.zoomStepPercent = std::clamp(static_cast<int>(intValue), 1, 95);

    intValue = config.zoomInertiaMilliseconds;
    fileConfig.Read("/Zoom/inertia_ms", &intValue, config.zoomInertiaMilliseconds);
    config.zoomInertiaMilliseconds = std::clamp(static_cast<int>(intValue), 0, 1000);

    wxString appearance;
    if (fileConfig.Read("/General/appearance", &appearance))
        config.appearance = AppearanceFromString(appearance.ToStdString(), config.appearance);
    else
    {
        bool darkTheme = false;
        if (fileConfig.Read("/General/dark_theme", &darkTheme))
            config.appearance = darkTheme ? AppAppearance::Dark : AppAppearance::Light;
    }

    wxString language;
    if (fileConfig.Read("/General/language", &language))
        config.language = LanguageFromString(language.ToStdString(), config.language);

    return config;
}

void AppConfigStore::Save(const AppConfig& config) const
{
    const TutorialStatus tutorialStatus = LoadTutorialStatus();
    wxFileConfig fileConfig(wxEmptyString, wxEmptyString, ToWxString(filename));

    fileConfig.DeleteAll();
    fileConfig.Write("/Fractal/app_version", ToWxString(APP_VERSION));
    fileConfig.Write("/Fractal/color_type", wxString("Gradient"));
    fileConfig.Write("/Fractal/palette_size", static_cast<long>(config.paletteSize));
    fileConfig.Write("/Fractal/color_cycle_length", static_cast<long>(config.colorCycleLength));
    fileConfig.Write("/Fractal/anti_aliasing_scale",
                     static_cast<long>(NormalizeAntiAliasingScale(config.antiAliasingScale)));
    fileConfig.Write("/Fractal/color_style_preset", ToWxString(ColorStyleToString(config.colorStyle)));
    fileConfig.Write("/Fractal/color_style", ToWxString(config.colorStyleGrad));
    fileConfig.Write("/Fractal/fractal_type", ToWxString(FractalTypeToString(config.type)));
    fileConfig.Write("/Fractal/automatic_iterations", config.automaticIterations);
    fileConfig.Write("/Fractal/constant_window", config.constantWindow);
    fileConfig.Write("/Fractal/julia_mode", config.juliaMode);
    fileConfig.Write("/Fractal/command_console", config.commandConsole);
    fileConfig.Write("/General/show_welcome_on_startup", config.showWelcomeOnStartup);
    fileConfig.Write("/General/tutorial_status", ToWxString(TutorialStatusToString(tutorialStatus)));
    fileConfig.Write("/Color/palette_window", config.colorPaletteWindow);
    fileConfig.Write("/Color/fractal", config.colorFractal);
    fileConfig.Write("/Color/set", config.colorSet);
    fileConfig.Write("/General/target_frame_rate",
                     static_cast<long>(std::max(AppConfig::MinimumTargetFrameRate, config.targetFrameRate)));
    fileConfig.Write("/Zoom/step_percent", static_cast<long>(config.zoomStepPercent));
    fileConfig.Write("/Zoom/inertia_ms", static_cast<long>(config.zoomInertiaMilliseconds));
    fileConfig.Write("/General/appearance", ToWxString(AppearanceToString(config.appearance)));
    fileConfig.Write("/General/dark_theme", config.appearance == AppAppearance::Dark);
    fileConfig.Write("/General/language", ToWxString(LanguageToString(config.language)));
    fileConfig.Flush();
}

void AppConfigStore::SetShowWelcomeOnStartup(const bool showWelcomeOnStartup) const
{
    AppConfig config = Load();
    config.showWelcomeOnStartup = showWelcomeOnStartup;
    Save(config);
}

void AppConfigStore::SetTutorialStatus(const TutorialStatus tutorialStatus) const
{
    if (!FileExists(filename))
        Save(AppConfig{});

    wxFileConfig fileConfig(wxEmptyString, wxEmptyString, ToWxString(filename));
    fileConfig.Write("/General/tutorial_status", ToWxString(TutorialStatusToString(tutorialStatus)));
    fileConfig.Flush();
}

TutorialStatus AppConfigStore::LoadTutorialStatus() const
{
    if (!FileExists(filename))
        return TutorialStatus::Pending;

    if (!HasIniSections(filename))
    {
        const std::map<std::string, std::string> values = ReadLegacyConfig(filename);
        return TutorialStatusFromString(
            ReadString(values, "TUTORIAL_STATUS", "Pending"), TutorialStatus::Pending);
    }

    wxFileConfig fileConfig(wxEmptyString, wxEmptyString, ToWxString(filename));
    wxString tutorialStatus;
    if (!fileConfig.Read("/General/tutorial_status", &tutorialStatus))
        return TutorialStatus::Pending;

    return TutorialStatusFromString(tutorialStatus.ToStdString(), TutorialStatus::Pending);
}

void AppConfigStore::SetCommandConsole(const bool commandConsole) const
{
    AppConfig config = Load();
    config.commandConsole = commandConsole;
    Save(config);
}
