#pragma once

#include <functional>
#include <map>
#include <optional>
#include <vector>
#include <wx/button.h>
#include <wx/frame.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include "canvas/FractalCanvas.h"

wxDECLARE_EVENT(wxEVT_COMMAND_CONSOLE_CLOSED, wxCommandEvent);

/**
* @class CommandConsole
* @brief Text command window for manipulating the active fractal session.
*
* CommandConsole parses typed commands, writes themed console output, dispatches
* view/render/color/script actions to FractalCanvas, and exposes callbacks for
* script reloads and Julia-mode opening.
*/
class CommandConsole : public wxFrame
{
    struct ParsedCommand
    {
        wxString name;
        std::vector<wxString> positionalArguments;
        std::map<wxString, wxString> namedArguments;
    };

    FractalCanvas* _fractalCanvas;
    std::function<void()> _reloadScripts;
    std::function<bool(double, double)> _openJuliaMode;
    wxRichTextCtrl* _output;
    wxStaticText* _prompt;
    wxTextCtrl* _input;
    wxButton* _enterButton;
    std::vector<wxString> _history;
    std::vector<wxString> _commandNames;
    size_t _historyIndex;

    /// @brief Returns the console color used for ordinary output.
    /// @return Theme-aware primary console text color.
    static wxColour OutputColor();

    /// @brief Returns the console color used for prompts.
    /// @return Theme-aware prompt color.
    static wxColour PromptColor();

    /// @brief Returns the console color used for errors.
    /// @return Theme-aware error color.
    static wxColour ErrorColor();

    /// @brief Returns the console color used for secondary text.
    /// @return Theme-aware muted text color.
    static wxColour MutedColor();

    /// @brief Returns the console color used for informational headings.
    /// @return Theme-aware informational text color.
    static wxColour InfoColor();
    void WriteWelcome() const;
    void WriteText(const wxString& text, const wxColour& color) const;
    void RunCommand();
    [[nodiscard]] wxString Execute(const ParsedCommand& command) const;
    static std::optional<ParsedCommand> Parse(const wxString& text, wxString& error);
    static std::optional<wxString> FindArgument(const ParsedCommand& command,
                                                std::initializer_list<const char*> names,
                                                size_t positionalIndex);
    static std::optional<double> ReadDouble(const ParsedCommand& command,
                                            std::initializer_list<const char*> names,
                                            size_t positionalIndex, wxString& error);
    static std::optional<unsigned int> ReadUnsigned(const ParsedCommand& command,
                                                    std::initializer_list<const char*> names,
                                                    size_t positionalIndex, wxString& error);
    static sf::Color ReadColor(const ParsedCommand& command, size_t positionalIndex, wxString& error);
    static std::optional<bool> ReadBool(const ParsedCommand& command,
                                        std::initializer_list<const char*> names,
                                        size_t positionalIndex, wxString& error);
    static wxString HelpText();

    void OnClose(wxCloseEvent& event);
    void OnEnter(wxCommandEvent& event);
    void OnKeyDown(wxKeyEvent& event);

public:
    /// @brief Applies the currently selected application appearance to the console.
    void ApplyTheme();

    CommandConsole(FractalCanvas* fractalCanvas, std::function<void()> reloadScripts,
                   std::function<bool(double, double)> openJuliaMode, wxWindow* parent,
                   wxWindowID id = wxID_ANY, const wxString& title = "Command console",
                   const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(760, 520),
                   long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);
};
