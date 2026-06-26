#pragma once

#include <map>
#include <string>
#include <wx/string.h>
#include "../AppAppearance.h"
#include "coloring/ColorPaletteTypes.h"
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
    AppConfig();

    FractalType type = FractalType::Mandelbrot;
    int maxIterations = 100;
    bool automaticIterations = false;
    int paletteSize = 0;
    int colorCycleLength = 0;
    ColorPaletteTypes colorStyle = ClassicMandelbrot; ///< Default gradient preset, or CustomGradient for user-edited gradients.
    std::string colorStyleGrad;
    bool constantWindow = false;
    bool commandConsole = false;
    bool juliaMode = false;
    bool colorPaletteWindow = false;
    bool colorFractal = true;
    bool colorSet = true;
    bool firstUse = false;
    AppAppearance appearance = AppAppearance::System; ///< Application appearance preference.
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
    [[nodiscard]] AppConfig Load() const;

    ///@brief Saves configuration values using the current sectioned INI format.
    ///@param config Configuration values to write.
    void Save(const AppConfig& config) const;

    ///@brief Updates the first-use flag in the configuration file.
    ///@param firstUse New value for the first-use flag.
    void SetFirstUse(bool firstUse) const;

    ///@brief Updates whether the command console should open at startup.
    void SetCommandConsole(bool commandConsole) const;

private:
    static std::string DefaultColorStyle();
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
    ///@brief Returns the persisted names for application appearance preferences.
    static const std::map<std::string, AppAppearance>& Appearances();
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
    ///@brief Converts a persisted appearance name to its enum value.
    ///@param value Persisted appearance name.
    ///@param defaultValue Value returned when the name is not recognized.
    ///@return Parsed appearance preference.
    static AppAppearance AppearanceFromString(const std::string& value, AppAppearance defaultValue);
    ///@brief Converts an appearance enum value to its persisted name.
    ///@param appearance Appearance preference to serialize.
    ///@return Persisted appearance name.
    static std::string AppearanceToString(AppAppearance appearance);
    ///@brief Identifies whether a saved gradient string matches a built-in preset.
    ///@param gradient Serialized gradient string.
    ///@return Matching preset, or CustomGradient when no preset matches.
    static ColorPaletteTypes InferColorStyleFromGradient(const std::string& gradient);
    static AppConfig LoadLegacyConfig(const std::string& filename);
    static wxString ToWxString(const std::string& value);

    std::string filename;
};
