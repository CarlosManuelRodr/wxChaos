#pragma once

#include <string>
#include <vector>

/// @brief Native value type stored by a script-defined fractal option.
enum class ScriptOptionType { Integer, Double, Boolean };

/**
 * @struct ScriptOption
 * @brief Describes one arbitrary option declared by a fractal script.
 */
struct ScriptOption
{
    std::string name;       ///< Stable key used by scripts to retrieve the value.
    std::string label;      ///< Label displayed in the fractal options panel.
    ScriptOptionType type;  ///< Native type of the option value.
    int integerValue = 0;   ///< Storage used by integer options.
    double doubleValue = 0; ///< Storage used by double options.
    bool boolValue = false; ///< Storage used by boolean options.
};

/**
 * @class ScriptOptions
 * @brief Owns typed option metadata and values declared by a script's Configure() function.
 */
class ScriptOptions
{
    std::vector<ScriptOption> _options;

    ScriptOption* Find(const std::string& name, ScriptOptionType type);
    [[nodiscard]] const ScriptOption* Find(const std::string& name, ScriptOptionType type) const;

public:
    /// @brief Adds or replaces an integer option.
    void AddInteger(const std::string& name, const std::string& label, int defaultValue);
    /// @brief Adds or replaces a double option.
    void AddDouble(const std::string& name, const std::string& label, double defaultValue);
    /// @brief Adds or replaces a boolean option.
    void AddBool(const std::string& name, const std::string& label, bool defaultValue);

    /// @brief Returns an integer option, or zero when the key and type do not exist.
    [[nodiscard]] int GetInteger(const std::string& name) const;
    /// @brief Returns a double option, or zero when the key and type do not exist.
    [[nodiscard]] double GetDouble(const std::string& name) const;
    /// @brief Returns a boolean option, or false when the key and type do not exist.
    [[nodiscard]] bool GetBool(const std::string& name) const;

    /// @brief Returns the finalized option entries in declaration order.
    [[nodiscard]] const std::vector<ScriptOption>& GetOptions() const;
    /// @brief Returns mutable finalized option entries for linking them to PanelOptions.
    [[nodiscard]] std::vector<ScriptOption>& GetOptions();
};
