#include "CommandConsole.h"

#include <algorithm>
#include <limits>
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include "AppPaths.h"
#include "global.h"

wxDEFINE_EVENT(wxEVT_COMMAND_CONSOLE_CLOSED, wxCommandEvent);
using namespace std;

CommandConsole::CommandConsole(FractalCanvas* fractalCanvas, std::function<void()> reloadScripts,
                               wxWindow* parent, const wxWindowID id, const wxString& title,
                               const wxPoint& pos, const wxSize& size, const long style)
    : wxFrame(parent, id, title, pos, size, style),
      _fractalCanvas(fractalCanvas),
      _reloadScripts(std::move(reloadScripts)),
      _historyIndex(0)
{
    this->SetSizeHints(wxSize(620, 380), wxDefaultSize);
    this->SetIcon(wxIcon(AppPaths::ResourceFile({wxT("icon.ico")}), wxBITMAP_TYPE_ICO));

    const auto panel = new wxPanel(this);
    const auto panelSizer = new wxBoxSizer(wxVERTICAL);
    _output = new wxRichTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                                 wxTE_READONLY | wxTE_MULTILINE | wxBORDER_NONE);
    panelSizer->Add(_output, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10);

    const auto inputSizer = new wxBoxSizer(wxHORIZONTAL);
    _prompt = new wxStaticText(panel, wxID_ANY, wxT(">"));
    inputSizer->Add(_prompt, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 8);
    _input = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                            wxTE_PROCESS_ENTER | wxTE_PROCESS_TAB);
    inputSizer->Add(_input, 1, wxEXPAND | wxRIGHT, 8);
    _enterButton = new wxButton(panel, wxID_ANY, wxT("Run"));
    inputSizer->Add(_enterButton, 0, wxEXPAND);
    panelSizer->Add(inputSizer, 0, wxEXPAND | wxALL, 10);

    panel->SetSizer(panelSizer);
    const auto frameSizer = new wxBoxSizer(wxVERTICAL);
    frameSizer->Add(panel, 1, wxEXPAND);
    this->SetSizer(frameSizer);

    _commandNames = {
        wxT("AskInfo("), wxT("SetBoundaries("), wxT("Redraw()"), wxT("Abort()"),
        wxT("DrawLine("), wxT("DrawCircle("), wxT("DeleteFigures()"),
        wxT("SetIterations("), wxT("GetIterations()"), wxT("ReloadScripts()"),
        wxT("Help()"), wxT("Clear()")
    };

    this->ApplyDarkTheme();
    this->WriteWelcome();
    this->CentreOnParent();
    _input->SetFocus();

    this->Bind(wxEVT_CLOSE_WINDOW, &CommandConsole::OnClose, this);
    _input->Bind(wxEVT_KEY_DOWN, &CommandConsole::OnKeyDown, this);
    _enterButton->Bind(wxEVT_BUTTON, &CommandConsole::OnEnter, this);
}

void CommandConsole::ApplyDarkTheme()
{
    const wxColour frameBackground(12, 17, 27);
    const wxColour controlBackground(17, 24, 39);
    const wxColour foreground(226, 232, 240);
    const wxFont font = wxFontInfo(10).Family(wxFONTFAMILY_TELETYPE).FaceName(wxT("Consolas"));

    this->SetBackgroundColour(frameBackground);
    _output->GetParent()->SetBackgroundColour(frameBackground);
    _output->SetBackgroundColour(controlBackground);
    _output->SetForegroundColour(foreground);
    _output->SetFont(font);
    _input->SetBackgroundColour(controlBackground);
    _input->SetForegroundColour(foreground);
    _input->SetFont(font);
    _prompt->SetForegroundColour(wxColour(52, 211, 153));
    _prompt->SetFont(font);
    _enterButton->SetBackgroundColour(wxColour(31, 41, 55));
    _enterButton->SetForegroundColour(foreground);
}

void CommandConsole::WriteWelcome()
{
    WriteText(wxT("wxChaos ") + wxString::FromUTF8(APP_VERSION) + wxT(" command console\n"), wxColour(96, 165, 250));
    WriteText(wxT("Type Help() for commands. Named and positional arguments are supported.\n\n"),
              wxColour(148, 163, 184));
    WriteText(wxT("Ready.\n"), wxColour(226, 232, 240));
}

void CommandConsole::WriteText(const wxString& text, const wxColour& color)
{
    _output->MoveEnd();
    _output->BeginTextColour(color);
    _output->WriteText(text);
    _output->EndTextColour();
    _output->ShowPosition(_output->GetLastPosition());
}

void CommandConsole::RunCommand()
{
    const wxString input = _input->GetValue();
    wxString trimmedInput = input;
    if (trimmedInput.Trim(true).Trim(false).empty())
        return;

    _history.push_back(input);
    _historyIndex = _history.size();
    _input->Clear();
    WriteText(wxT("\n> ") + input + wxT("\n"), wxColour(52, 211, 153));

    wxString error;
    const std::optional<ParsedCommand> command = Parse(input, error);
    if (!command.has_value())
    {
        WriteText(error + wxT("\n"), wxColour(248, 113, 113));
        return;
    }

    const wxString result = Execute(*command);
    WriteText(result + wxT("\n"), result.StartsWith(wxT("Error:")) ? wxColour(248, 113, 113)
                                                                    : wxColour(226, 232, 240));
}

wxString CommandConsole::Execute(const ParsedCommand& command)
{
    Fractal* fractal = _fractalCanvas->GetFractalPtr();
    wxString error;

    if (command.name == wxT("help"))
        return HelpText();
    if (command.name == wxT("clear") || command.name == wxT("clc"))
    {
        _output->Clear();
        return wxT("Console cleared.");
    }
    if (command.name == wxT("reloadscripts"))
    {
        _reloadScripts();
        return wxT("Scripts reloaded.");
    }
    if (command.name == wxT("redraw"))
    {
        _fractalCanvas->GetSFMLFractalPtr()->Redraw();
        return wxT("Redraw requested.");
    }
    if (command.name == wxT("abort"))
    {
        if (!_fractalCanvas->CanAbortRender())
            return wxT("Nothing to abort.");
        _fractalCanvas->AbortRender();
        return wxT("Render aborted.");
    }
    if (command.name == wxT("deletefigures"))
    {
        fractal->ClearGeometryFigures();
        return wxT("Figures deleted.");
    }
    if (command.name == wxT("getiterations"))
        return wxString::Format(wxT("Maximum iterations: %u"), fractal->GetIterations());
    if (command.name == wxT("setiterations"))
    {
        const optional<unsigned int> iterations = ReadUnsigned(command, {"iterations", "value", "n"}, 0, error);
        if (!iterations.has_value())
            return error;
        if (*iterations == 0)
            return wxT("Error: iterations must be greater than zero.");
        _fractalCanvas->GetSFMLFractalPtr()->ChangeIterations(*iterations);
        return wxString::Format(wxT("Maximum iterations set to %u."), *iterations);
    }
    if (command.name == wxT("askinfo"))
    {
        const optional<double> real = ReadDouble(command, {"re", "real", "x"}, 0, error);
        if (!real.has_value()) return error;
        const optional<double> imaginary = ReadDouble(command, {"im", "imaginary", "y"}, 1, error);
        if (!imaginary.has_value()) return error;

        optional<unsigned int> iterations;
        if (FindArgument(command, {"iterations", "maxiter", "n"}, 2).has_value())
        {
            iterations = ReadUnsigned(command, {"iterations", "maxiter", "n"}, 2, error);
            if (!iterations.has_value()) return error;
            if (*iterations == 0) return wxT("Error: iterations must be greater than zero.");
        }
        return _fractalCanvas->InspectPoint(*real, *imaginary, iterations);
    }
    if (command.name == wxT("drawcircle"))
    {
        const optional<double> x = ReadDouble(command, {"x", "re"}, 0, error);
        if (!x.has_value()) return error;
        const optional<double> y = ReadDouble(command, {"y", "im"}, 1, error);
        if (!y.has_value()) return error;
        const optional<double> radius = ReadDouble(command, {"r", "radius"}, 2, error);
        if (!radius.has_value()) return error;
        if (*radius <= 0.0) return wxT("Error: radius must be greater than zero.");
        const sf::Color color = ReadColor(command, error);
        if (!error.empty()) return error;
        fractal->DrawCircle(*x, *y, *radius, color);
        return wxT("Circle drawn.");
    }
    if (command.name == wxT("drawline"))
    {
        const optional<double> x1 = ReadDouble(command, {"x1", "re1"}, 0, error);
        if (!x1.has_value()) return error;
        const optional<double> y1 = ReadDouble(command, {"y1", "im1"}, 1, error);
        if (!y1.has_value()) return error;
        const optional<double> x2 = ReadDouble(command, {"x2", "re2"}, 2, error);
        if (!x2.has_value()) return error;
        const optional<double> y2 = ReadDouble(command, {"y2", "im2"}, 3, error);
        if (!y2.has_value()) return error;
        const sf::Color color = ReadColor(command, error);
        if (!error.empty()) return error;
        fractal->DrawLine(*x1, *y1, *x2, *y2, color);
        return wxT("Line drawn.");
    }
    if (command.name == wxT("setboundaries"))
    {
        const optional<double> minX = ReadDouble(command, {"minx", "minre"}, 0, error);
        if (!minX.has_value()) return error;
        const optional<double> maxX = ReadDouble(command, {"maxx", "maxre"}, 1, error);
        if (!maxX.has_value()) return error;
        const optional<double> minY = ReadDouble(command, {"miny", "minim"}, 2, error);
        if (!minY.has_value()) return error;
        const optional<double> maxY = ReadDouble(command, {"maxy", "maxim"}, 3, error);
        if (!maxY.has_value()) return error;
        if (*minX >= *maxX || *minY >= *maxY)
            return wxT("Error: minimum boundaries must be smaller than maximum boundaries.");
        fractal->SetView({*minX, *minY, *maxX, *maxY});
        _fractalCanvas->GetSFMLFractalPtr()->Redraw();
        return wxT("View boundaries updated.");
    }

    return wxT("Error: unknown command. Type Help() for the command list.");
}

optional<CommandConsole::ParsedCommand> CommandConsole::Parse(const wxString& text, wxString& error)
{
    wxString commandText = text;
    commandText.Trim(true).Trim(false);
    const int openParenthesis = commandText.Find(wxT('('));
    const int closeParenthesis = commandText.Find(wxT(')'), true);
    if (openParenthesis <= 0 || closeParenthesis != static_cast<int>(commandText.length()) - 1)
    {
        error = wxT("Error: expected Command(argument=value, ...).");
        return nullopt;
    }

    ParsedCommand command;
    command.name = commandText.Left(openParenthesis).Lower();
    command.name.Trim(true).Trim(false);
    wxString arguments = commandText.Mid(openParenthesis + 1, closeParenthesis - openParenthesis - 1);
    arguments.Trim(true).Trim(false);
    if (arguments.empty())
        return command;

    while (!arguments.empty())
    {
        const int comma = arguments.Find(wxT(','));
        wxString argument = comma == wxNOT_FOUND ? arguments : arguments.Left(comma);
        arguments = comma == wxNOT_FOUND ? wxEmptyString : arguments.Mid(comma + 1);
        argument.Trim(true).Trim(false);
        arguments.Trim(true).Trim(false);
        if (argument.empty())
        {
            error = wxT("Error: empty argument.");
            return nullopt;
        }

        const int equals = argument.Find(wxT('='));
        if (equals == wxNOT_FOUND)
            command.positionalArguments.push_back(argument);
        else
        {
            wxString name = argument.Left(equals).Lower();
            wxString value = argument.Mid(equals + 1);
            name.Trim(true).Trim(false);
            value.Trim(true).Trim(false);
            if (name.empty() || value.empty())
            {
                error = wxT("Error: invalid named argument.");
                return nullopt;
            }
            if (!command.namedArguments.emplace(name, value).second)
            {
                error = wxT("Error: duplicate argument '") + name + wxT("'.");
                return nullopt;
            }
        }
    }
    return command;
}

optional<wxString> CommandConsole::FindArgument(const ParsedCommand& command,
                                                 const initializer_list<const char*> names,
                                                 const size_t positionalIndex)
{
    for (const char* name : names)
    {
        const auto found = command.namedArguments.find(wxString::FromUTF8(name));
        if (found != command.namedArguments.end())
            return found->second;
    }
    if (positionalIndex < command.positionalArguments.size())
        return command.positionalArguments[positionalIndex];
    return nullopt;
}

optional<double> CommandConsole::ReadDouble(const ParsedCommand& command,
                                             const initializer_list<const char*> names,
                                             const size_t positionalIndex, wxString& error)
{
    const optional<wxString> value = FindArgument(command, names, positionalIndex);
    if (!value.has_value())
    {
        error = wxT("Error: missing numeric argument.");
        return nullopt;
    }
    double number;
    if (!value->ToDouble(&number))
    {
        error = wxT("Error: '") + *value + wxT("' is not a number.");
        return nullopt;
    }
    return number;
}

optional<unsigned int> CommandConsole::ReadUnsigned(const ParsedCommand& command,
                                                     const initializer_list<const char*> names,
                                                     const size_t positionalIndex, wxString& error)
{
    const optional<wxString> value = FindArgument(command, names, positionalIndex);
    if (!value.has_value())
    {
        error = wxT("Error: missing integer argument.");
        return nullopt;
    }
    unsigned long number;
    if (!value->ToULong(&number) || number > numeric_limits<unsigned int>::max())
    {
        error = wxT("Error: '") + *value + wxT("' is not a valid non-negative integer.");
        return nullopt;
    }
    return static_cast<unsigned int>(number);
}

sf::Color CommandConsole::ReadColor(const ParsedCommand& command, wxString& error)
{
    const optional<wxString> redValue = FindArgument(command, {"red"}, numeric_limits<size_t>::max());
    const optional<wxString> greenValue = FindArgument(command, {"green"}, numeric_limits<size_t>::max());
    const optional<wxString> blueValue = FindArgument(command, {"blue"}, numeric_limits<size_t>::max());
    const bool hasColor = redValue.has_value() || greenValue.has_value() || blueValue.has_value();
    if (!hasColor)
        return sf::Color::Black;

    unsigned int red = 0;
    unsigned int green = 0;
    unsigned int blue = 0;
    auto readComponent = [&error](const optional<wxString>& value, unsigned int& component)
    {
        if (!value.has_value())
            return true;
        unsigned long parsed;
        if (!value->ToULong(&parsed) || parsed > 255)
        {
            error = wxT("Error: color components must be integers between 0 and 255.");
            return false;
        }
        component = static_cast<unsigned int>(parsed);
        return true;
    };

    if (!readComponent(redValue, red) || !readComponent(greenValue, green) || !readComponent(blueValue, blue))
        return {};
    return {static_cast<sf::Uint8>(red), static_cast<sf::Uint8>(green), static_cast<sf::Uint8>(blue)};
}

wxString CommandConsole::HelpText()
{
    return wxT(
        "AskInfo(re, im, iterations=optional)\n"
        "DrawCircle(x, y, r, red=0, green=0, blue=0)\n"
        "DrawLine(x1, y1, x2, y2, red=0, green=0, blue=0)\n"
        "DeleteFigures()\n"
        "SetBoundaries(minX, maxX, minY, maxY)\n"
        "SetIterations(iterations)\n"
        "GetIterations()\n"
        "Redraw()\n"
        "Abort()\n"
        "ReloadScripts()\n"
        "Clear() or Clc()\n"
        "Help()\n\n"
        "Examples:\n"
        "DrawCircle(x=0.1, y=0.3, r=1.3)\n"
        "AskInfo(re=-1.3, im=7.3)"
    );
}

void CommandConsole::OnClose(wxCloseEvent&)
{
    wxQueueEvent(GetParent(), new wxCommandEvent(wxEVT_COMMAND_CONSOLE_CLOSED));
    this->Destroy();
}

void CommandConsole::OnEnter(wxCommandEvent&)
{
    RunCommand();
}

void CommandConsole::OnKeyDown(wxKeyEvent& event)
{
    if (event.GetKeyCode() == WXK_RETURN)
    {
        RunCommand();
        return;
    }
    if (event.GetKeyCode() == WXK_UP)
    {
        if (_historyIndex > 0)
        {
            _input->SetValue(_history[--_historyIndex]);
            _input->SetInsertionPointEnd();
        }
        return;
    }
    if (event.GetKeyCode() == WXK_DOWN)
    {
        if (_historyIndex + 1 < _history.size())
            _input->SetValue(_history[++_historyIndex]);
        else
        {
            _historyIndex = _history.size();
            _input->Clear();
        }
        _input->SetInsertionPointEnd();
        return;
    }
    if (event.GetKeyCode() == WXK_TAB)
    {
        const wxString current = _input->GetValue().Lower();
        for (const wxString& command : _commandNames)
        {
            if (command.Lower().StartsWith(current))
            {
                _input->SetValue(command);
                _input->SetInsertionPointEnd();
                break;
            }
        }
        return;
    }
    event.Skip();
}
