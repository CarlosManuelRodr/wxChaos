#pragma once
#include <vector>
#include <wx/string.h>

/// @brief Describes the UI control used to expose one fractal option.
enum class PanelOptionType { Label, TextCtrl, Spin, SpinDouble, CheckBox };

/// @brief Describes the native value type linked to one editable panel option.
enum class LinkTo { ToInt, ToDouble, ToBool };

/**
 * @class PanelOptions
 * @brief Stores metadata and target links used to build a fractal options panel.
 *
 * Fractal classes populate this object with labels, default display values, control types, and pointers to the
 * underlying settings. FractalOptionsPanel reads the metadata to create controls and writes edited values back through
 * the stored target pointers when the panel is applied.
 */
class PanelOptions
{
    std::vector<PanelOptionType> _type;  ///< UI control type for each option entry.
    std::vector<wxString> _label;        ///< Display label for each option entry.
    std::vector<wxString> _defaults;     ///< Initial text value used when creating controls.
    std::vector<LinkTo> _linkTo;         ///< Linked native value type for each editable entry.
    std::vector<int*> _integerTarget;    ///< Integer target pointers, stored in integer option order.
    std::vector<double*> _doubleTarget;  ///< Double target pointers, stored in double option order.
    std::vector<bool*> _boolTarget;      ///< Boolean target pointers, stored in boolean option order.
    std::vector<double> _increments;     ///< Spin increments for entries that need step controls.
    bool _forceShow;                     ///< Forces the option area visible even when no entries exist.

public:
    /// @brief Creates an empty options definition.
    PanelOptions();

    /**
     * @brief Adds an integer-linked option.
     * @param type UI control type to create.
     * @param labelText Label shown beside or above the control.
     * @param linkInteger Target integer updated when the panel is applied.
     * @param defaultValue Initial control value.
     */
    void LinkInteger(PanelOptionType type, const wxString& labelText, int* linkInteger, const wxString& defaultValue);

    /**
     * @brief Adds a double-linked option.
     * @param type UI control type to create.
     * @param labelText Label shown beside or above the control.
     * @param linkDouble Target double updated when the panel is applied.
     * @param defaultValue Initial control value.
     * @param increment Step amount used by double spin controls.
     */
    void LinkDouble(PanelOptionType type, const wxString& labelText, double* linkDouble, const wxString& defaultValue,
                    double increment = 1.0);

    /**
     * @brief Adds a boolean-linked option.
     * @param type UI control type to create.
     * @param labelText Label shown beside or above the control.
     * @param linkBool Target boolean updated when the panel is applied.
     * @param defaultValue Initial control value, normally "true" or "false".
     */
    void LinkBool(PanelOptionType type, const wxString& labelText, bool* linkBool, const wxString& defaultValue);

    /// @brief Returns the total number of option entries.
    [[nodiscard]] size_t GetElementsSize() const;

    /// @brief Returns the linked native value type for the option at an index.
    [[nodiscard]] LinkTo GetLinkType(unsigned int index) const;

    /// @brief Returns the display label for the option at an index.
    wxString GetLabelValue(unsigned int index);

    /// @brief Returns the integer target pointer at an integer-option index.
    [[nodiscard]] int* GetIntegerValue(unsigned int index) const;

    /// @brief Returns the double target pointer at a double-option index.
    [[nodiscard]] double* GetDoubleValue(unsigned int index) const;

    /// @brief Returns the boolean target pointer at a boolean-option index.
    [[nodiscard]] bool* GetBoolValue(unsigned int index) const;

    /// @brief Returns the spin increment for the option at an index.
    [[nodiscard]] double GetIncrement(unsigned int index) const;

    /// @brief Returns the initial display value for the option at an index.
    wxString GetDefault(unsigned int index);

    /// @brief Returns the UI control type for the option at an index.
    [[nodiscard]] PanelOptionType GetPanelOptionType(unsigned int index) const;

    /// @brief Copies linked target values from another compatible options definition.
    void CopyValuesFrom(const PanelOptions& source);

    /// @brief Sets whether the owning panel should be visible even when there are no entries.
    void SetForceShow(bool mode);

    /// @brief Returns whether the owning panel should be forced visible.
    [[nodiscard]] bool GetForceShow() const;
};
