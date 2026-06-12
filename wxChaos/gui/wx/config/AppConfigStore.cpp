#include <algorithm>
#include <fstream>
#include <map>
#include <utility>
#include <wx/fileconf.h>
#include "AppConfigStore.h"
#include "global.h"

const char* AppConfigStore::DefaultColorStyle()
{
    return "rgb(4,108,164);rgb(136,171,14);rgb(255,255,255);rgb(171,27,27);rgb(61,43,94);rgb(4,108,164);";
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

const std::map<std::string, FractalType>& AppConfigStore::FractalTypes()
{
    static const std::map<std::string, FractalType> fractalTypes = {
        { "Mandelbrot", FractalType::Mandelbrot },
        { "MandelbrotZN", FractalType::MandelbrotZN },
        { "Julia", FractalType::Julia },
        { "JuliaZN", FractalType::JuliaZN },
        { "Newton", FractalType::NewtonRaphsonMethod },
        { "Sinoidal", FractalType::Sinoidal },
        { "Magnet", FractalType::Magnetic },
        { "Medusa", FractalType::Medusa },
        { "Manowar", FractalType::Manowar },
        { "JManowar", FractalType::ManowarJulia },
        { "Sierpinsky_Triangle", FractalType::SierpinskyTriangle },
        { "FixedPoint1", FractalType::FixedPoint1 },
        { "FixedPoint2", FractalType::FixedPoint2 },
        { "FixedPoint3", FractalType::FixedPoint3 },
        { "FixedPoint4", FractalType::FixedPoint4 },
        { "Tricorn", FractalType::Tricorn },
        { "Burning_Ship", FractalType::BurningShip },
        { "Burning_Ship_Julia", FractalType::BurningShipJulia },
        { "Fractory", FractalType::Fractory },
        { "Cell", FractalType::Cell },
        { "Double_Pendulum", FractalType::DoublePendulum },
        { "User_Defined", FractalType::UserDefined },
        { "FPUser_Defined", FractalType::FixedPointUserDefined }
    };

    return fractalTypes;
}

// ReSharper disable once CppDFAConstantParameter
FractalType AppConfigStore::FractalTypeFromString(const std::string& value, const FractalType defaultValue)
{
    const auto it = FractalTypes().find(value);
    return it == FractalTypes().end() ? defaultValue : it->second;
}

std::string AppConfigStore::FractalTypeToString(FractalType type)
{
    for (const auto& item : FractalTypes())
    {
        if (item.second == type)
            return item.first;
    }

    return "Mandelbrot";
}

AppConfig AppConfigStore::LoadLegacyConfig(const std::string& filename)
{
    AppConfig config;
    const std::map<std::string, std::string> values = ReadLegacyConfig(filename);

    config.type = FractalTypeFromString(ReadString(values, "FRACTAL_TYPE", "Mandelbrot"), config.type);
    config.maxIterations = ReadInt(values, "DEFAULT_ITERATION", config.maxIterations);
    config.paletteSize = ReadInt(values, "PALETTE_SIZE", config.paletteSize);
    config.colorStyleGrad = ReadString(values, "COLOR_STYLE", config.colorStyleGrad);
    if (config.colorStyleGrad.find("rgb(") == std::string::npos)
        config.colorStyleGrad = DefaultColorStyle();

    config.constantWindow = ReadBool(values, "CONSTANT_WINDOW", config.constantWindow);
    config.commandConsole = ReadBool(values, "COMMAND_CONSOLE", config.commandConsole);
    config.juliaMode = ReadBool(values, "JULIA_MODE", config.juliaMode);
    config.colorPaletteWindow = ReadBool(values, "COLOR_PALETTE_WINDOW", config.colorPaletteWindow);
    config.colorFractal = ReadBool(values, "COLOR_FRACTAL", config.colorFractal);
    config.colorSet = ReadBool(values, "COLOR_SET", config.colorSet);
    config.firstUse = ReadBool(values, "FIRST_USE", config.firstUse);

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

    wxString fractalType;
    fileConfig.Read("/Fractal/fractal_type", &fractalType, "Mandelbrot");
    config.type = FractalTypeFromString(fractalType.ToStdString(), config.type);

    long intValue = config.maxIterations;
    fileConfig.Read("/Fractal/default_iteration", &intValue, config.maxIterations);
    config.maxIterations = static_cast<int>(intValue);

    intValue = config.paletteSize;
    fileConfig.Read("/Fractal/palette_size", &intValue, config.paletteSize);
    config.paletteSize = static_cast<int>(intValue);

    fileConfig.Read("/Fractal/constant_window", &config.constantWindow, config.constantWindow);
    fileConfig.Read("/Fractal/command_console", &config.commandConsole, config.commandConsole);
    fileConfig.Read("/Fractal/julia_mode", &config.juliaMode, config.juliaMode);
    fileConfig.Read("/Fractal/first_use", &config.firstUse, config.firstUse);
    fileConfig.Read("/Color/palette_window", &config.colorPaletteWindow, config.colorPaletteWindow);
    fileConfig.Read("/Color/fractal", &config.colorFractal, config.colorFractal);
    fileConfig.Read("/Color/set", &config.colorSet, config.colorSet);

    return config;
}

void AppConfigStore::Save(const AppConfig& config) const
{
    wxFileConfig fileConfig(wxEmptyString, wxEmptyString, ToWxString(filename));

    fileConfig.DeleteAll();
    fileConfig.Write("/Fractal/app_version", ToWxString(APP_VERSION));
    fileConfig.Write("/Fractal/color_type", wxString("Gradient"));
    fileConfig.Write("/Fractal/palette_size", static_cast<long>(config.paletteSize));
    fileConfig.Write("/Fractal/color_style", ToWxString(config.colorStyleGrad));
    fileConfig.Write("/Fractal/fractal_type", ToWxString(FractalTypeToString(config.type)));
    fileConfig.Write("/Fractal/default_iteration", static_cast<long>(config.maxIterations));
    fileConfig.Write("/Fractal/constant_window", config.constantWindow);
    fileConfig.Write("/Fractal/julia_mode", config.juliaMode);
    fileConfig.Write("/Fractal/command_console", config.commandConsole);
    fileConfig.Write("/Fractal/first_use", config.firstUse);
    fileConfig.Write("/Color/palette_window", config.colorPaletteWindow);
    fileConfig.Write("/Color/fractal", config.colorFractal);
    fileConfig.Write("/Color/set", config.colorSet);
    fileConfig.Flush();
}

void AppConfigStore::SetFirstUse(bool firstUse) const
{
    AppConfig config = Load();
    config.firstUse = firstUse;
    Save(config);
}

void AppConfigStore::SetCommandConsole(const bool commandConsole) const
{
    AppConfig config = Load();
    config.commandConsole = commandConsole;
    Save(config);
}
