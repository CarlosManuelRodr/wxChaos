#pragma once

#include <map>
#include <string>
#include <wx/string.h>
#include "ColorPaletteTypes.h"
#include "../../../core/types/FractalType.h"

/**
* @struct AppConfig
* @brief Holds the application options loaded from "config.ini".
*
* This struct centralizes the default configuration values used when the
* configuration file is missing, incomplete, or contains invalid values.
*/
struct AppConfig
{
    FractalType type = FractalType::Mandelbrot;
    int maxIterations = 100;
    int paletteSize = 300;
    ColorPaletteTypes colorStyle = Retro; ///< Default gradient preset, or CustomGradient for user-edited gradients.
    std::string colorStyleGrad = "rgb(4,108,164);rgb(136,171,14);rgb(255,255,255);rgb(171,27,27);rgb(61,43,94);rgb(4,108,164);";
    bool constantWindow = false;
    bool commandConsole = false;
    bool juliaMode = false;
    bool colorPaletteWindow = false;
    bool colorFractal = true;
    bool colorSet = true;
    bool firstUse = false;
    bool darkTheme = false; ///< Uses the application dark appearance when enabled.
};

/**
* @class AppConfigStore
* @brief Loads, saves, and migrates the application configuration file.
*
* Uses wxFileConfig for the current sectioned INI format and can import the
* legacy flat KEY=VALUE format used by older versions of wxChaos.
*/
class AppConfigStore
{
public:
    ///@brief Constructor.
    ///@param filename Path to the configuration file.
    explicit AppConfigStore(std::string  filename);

    ///@brief Loads the configuration file.
    ///@return Parsed configuration values, with defaults used for missing or invalid entries.
    AppConfig Load() const;

    ///@brief Saves configuration values using the current sectioned INI format.
    ///@param config Configuration values to write.
    void Save(const AppConfig& config) const;

    ///@brief Updates the first-use flag in the configuration file.
    ///@param firstUse New value for the first-use flag.
    void SetFirstUse(bool firstUse) const;

    ///@brief Updates whether the command console should open at startup.
    void SetCommandConsole(bool commandConsole) const;

private:
    static const char* DefaultColorStyle();
    static std::string Trim(const std::string& value);
    static std::string ToLower(std::string value);
    static bool FileExists(const std::string& filename);
    static bool HasIniSections(const std::string& filename);
    static std::map<std::string, std::string> ReadLegacyConfig(const std::string& filename);
    static bool ReadBool(const std::map<std::string, std::string>& values, const std::string& key, bool defaultValue);
    static int ReadInt(const std::map<std::string, std::string>& values, const std::string& key, int defaultValue);
    static std::string ReadString(const std::map<std::string, std::string>& values, const std::string& key, const std::string& defaultValue);
    static const std::map<std::string, FractalType>& FractalTypes();
    ///@brief Returns the persisted names for gradient color style presets.
    static const std::map<std::string, ColorPaletteTypes>& ColorStyles();
    static FractalType FractalTypeFromString(const std::string& value, FractalType defaultValue);
    static std::string FractalTypeToString(FractalType type);
    ///@brief Converts a persisted gradient color style name to its enum value.
    ///@param value Persisted color style name.
    ///@param defaultValue Value returned when the name is not recognized.
    ///@return Parsed color style.
    static ColorPaletteTypes ColorStyleFromString(const std::string& value, ColorPaletteTypes defaultValue);
    ///@brief Converts a gradient color style enum value to its persisted name.
    ///@param type Color style to serialize.
    ///@return Persisted color style name.
    static std::string ColorStyleToString(ColorPaletteTypes type);
    ///@brief Identifies whether a saved gradient string matches a built-in preset.
    ///@param gradient Serialized gradient string.
    ///@return Matching preset, or CustomGradient when no preset matches.
    static ColorPaletteTypes InferColorStyleFromGradient(const std::string& gradient);
    static AppConfig LoadLegacyConfig(const std::string& filename);
    static wxString ToWxString(const std::string& value);

    std::string filename;
};
