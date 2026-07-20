#pragma once

#include <map>
#include <string>
#include <wx/string.h>
#include "coloring/ColorPaletteTypes.h"
#include "../common/AppAppearance.h"
#include "../common/AppLanguage.h"
#include "config/TutorialStatus.h"
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
    static constexpr int MinimumTargetFrameRate = 30;
    static constexpr int DefaultTargetFrameRate = 120;

    AppConfig();

    ///@brief Resolves the System palette to a concrete palette based on the current system theme.
    ///@param style Color style to resolve.
    ///@return Concrete color palette (never System).
    static ColorPaletteTypes ResolveColorStyle(ColorPaletteTypes style);

    FractalType type = FractalType::Mandelbrot;
    bool automaticIterations = true;
    int paletteSize = 0;
    int colorCycleLength = 0;
    ColorPaletteTypes colorStyle = System; ///< Default gradient preset, or CustomGradient for user-edited gradients.
    std::string colorStyleGrad;
    int antiAliasingScale = 1;
    bool constantWindow = false;
    bool commandConsole = false;
    bool juliaMode = false;
    bool colorPaletteWindow = false;
    bool colorFractal = true;
    bool colorSet = true;
    bool showWelcomeOnStartup = true;                ///< Opens the welcome guide whenever wxChaos starts.
    AppAppearance appearance = AppAppearance::System; ///< Application appearance preference.
    AppLanguage language = AppLanguage::System;       ///< Application language preference.
    int targetFrameRate = DefaultTargetFrameRate;     ///< Target SFML canvas presentation frequency in hertz.
    int zoomStepPercent = 50;                         ///< Percent zoomed in by one mouse-wheel notch.
    int zoomInertiaMilliseconds = 685;                ///< Temporary zoom preview easing duration.
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

    ///@brief Updates whether the welcome guide should open at startup.
    ///@param showWelcomeOnStartup True to open the welcome guide whenever wxChaos starts.
    void SetShowWelcomeOnStartup(bool showWelcomeOnStartup) const;

    ///@brief Updates the guided tutorial lifecycle state.
    void SetTutorialStatus(TutorialStatus tutorialStatus) const;

    ///@brief Loads the guided tutorial lifecycle state.
    ///@return Persisted state, or Pending when it is absent or invalid.
    [[nodiscard]] TutorialStatus LoadTutorialStatus() const;

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
    static int NormalizeAntiAliasingScale(int scale);
    static const std::map<std::string, FractalType>& FractalTypes();
    ///@brief Returns the persisted names for gradient color style presets.
    static const std::map<std::string, ColorPaletteTypes>& ColorStyles();
    ///@brief Returns the persisted names for application appearance preferences.
    static const std::map<std::string, AppAppearance>& Appearances();
    ///@brief Returns the persisted names for application language preferences.
    static const std::map<std::string, AppLanguage>& Languages();
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
    static AppLanguage LanguageFromString(const std::string& value, AppLanguage defaultValue);
    static std::string LanguageToString(AppLanguage language);
    static TutorialStatus TutorialStatusFromString(const std::string& value, TutorialStatus defaultValue);
    static std::string TutorialStatusToString(TutorialStatus status);
    ///@brief Identifies whether a saved gradient string matches a built-in preset.
    ///@param gradient Serialized gradient string.
    ///@return Matching preset, or CustomGradient when no preset matches.
    static ColorPaletteTypes InferColorStyleFromGradient(const std::string& gradient);
    static AppConfig LoadLegacyConfig(const std::string& filename);
    static wxString ToWxString(const std::string& value);

    std::string filename;
};
