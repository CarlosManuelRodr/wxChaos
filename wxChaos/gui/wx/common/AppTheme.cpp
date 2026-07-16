#include "common/AppTheme.h"

#include <wx/html/htmlwin.h>
#include <wx/panel.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/settings.h>
#include <wx/stattext.h>
#include <wx/stc/stc.h>
#include <wx/textctrl.h>
#include <wx/toplevel.h>
#include <wx/window.h>
#include <vector>

AppTheme& AppTheme::Instance()
{
    static AppTheme instance;
    return instance;
}

void AppTheme::Install()
{
    AppTheme& instance = Instance();
    if (instance._installed)
        return;

    wxEvtHandler::AddFilter(&instance);
    instance._installed = true;
}

void AppTheme::Uninstall()
{
    AppTheme& instance = Instance();
    if (!instance._installed)
        return;

    wxEvtHandler::RemoveFilter(&instance);
    instance._installed = false;
}

void AppTheme::SetDark(const bool dark)
{
    Instance()._dark = dark;
}

void AppTheme::SetAppearance(const AppAppearance appearance)
{
    SetDark(ResolveDark(appearance));
}

bool AppTheme::ResolveDark(const AppAppearance appearance)
{
    switch (appearance)
    {
        case AppAppearance::Dark:
            return true;
        case AppAppearance::Light:
            return false;
        case AppAppearance::System:
            return wxSystemSettings::GetAppearance().AreAppsDark();
    }

    return false;
}

bool AppTheme::IsDark()
{
    return Instance()._dark;
}

wxColour AppTheme::Background()
{
    return IsDark() ? wxColour(30, 32, 36) : wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);
}

wxColour AppTheme::ControlBackground()
{
    return IsDark() ? wxColour(43, 46, 52) : wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
}

wxColour AppTheme::Foreground()
{
    return IsDark() ? wxColour(232, 234, 237) : wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
}

void AppTheme::ApplyToSingleWindow(wxWindow* window)
{
    if (window == nullptr)
        return;

    const bool customTextControl = dynamic_cast<wxRichTextCtrl*>(window) != nullptr
        || dynamic_cast<wxStyledTextCtrl*>(window) != nullptr
        || dynamic_cast<wxHtmlWindow*>(window) != nullptr;
    const bool genericSurface = dynamic_cast<wxTopLevelWindow*>(window) != nullptr
        || dynamic_cast<wxPanel*>(window) != nullptr
        || dynamic_cast<wxStaticText*>(window) != nullptr;

    if (customTextControl)
    {
        window->SetBackgroundColour(ControlBackground());
        window->SetForegroundColour(Foreground());
    }
    else if (genericSurface)
    {
        window->SetBackgroundColour(Background());
        window->SetForegroundColour(Foreground());
    }

    if (const auto styledText = dynamic_cast<wxStyledTextCtrl*>(window))
    {
        styledText->StyleSetBackground(wxSTC_STYLE_DEFAULT, ControlBackground());
        styledText->StyleSetForeground(wxSTC_STYLE_DEFAULT, Foreground());
        styledText->StyleClearAll();
        styledText->StyleSetBackground(wxSTC_STYLE_LINENUMBER, Background());
        styledText->StyleSetForeground(wxSTC_STYLE_LINENUMBER, Foreground());
        styledText->SetCaretForeground(Foreground());

        styledText->StyleSetForeground(wxSTC_C_COMMENT,
            IsDark() ? wxColour(134, 239, 172) : wxColour(21, 128, 61));
        styledText->StyleSetForeground(wxSTC_C_COMMENTLINE,
            IsDark() ? wxColour(134, 239, 172) : wxColour(21, 128, 61));
        styledText->StyleSetForeground(wxSTC_C_NUMBER,
            IsDark() ? wxColour(196, 181, 253) : wxColour(109, 40, 217));
        styledText->StyleSetForeground(wxSTC_C_PREPROCESSOR,
            IsDark() ? wxColour(147, 197, 253) : wxColour(29, 78, 216));
        styledText->StyleSetForeground(wxSTC_C_STRING,
            IsDark() ? wxColour(253, 186, 116) : wxColour(194, 65, 12));
        styledText->StyleSetForeground(wxSTC_C_WORD,
            IsDark() ? wxColour(125, 211, 252) : wxColour(3, 105, 161));
        styledText->StyleSetForeground(wxSTC_C_IDENTIFIER, Foreground());
    }

    if (const auto richText = dynamic_cast<wxRichTextCtrl*>(window))
    {
        richText->SetDefaultStyle(wxTextAttr(Foreground(), ControlBackground()));
        richText->SetStyle(wxRichTextRange(0, richText->GetLastPosition()),
            wxTextAttr(Foreground(), ControlBackground()));
    }

}

void AppTheme::ApplyToWindow(wxWindow* window)
{
    if (window == nullptr)
        return;

    if (window->GetName() == "wxChaosSfmlCanvas")
        return;

    std::vector<wxWindow*> children;
    children.reserve(window->GetChildren().GetCount());
    for (wxWindow* child : window->GetChildren())
        children.push_back(child);

    ApplyToSingleWindow(window);
    for (wxWindow* child : children)
        ApplyToWindow(child);

    window->Refresh();
}

void AppTheme::ApplyToAllWindows()
{
    std::vector<wxWindow*> windows;
    windows.reserve(wxTopLevelWindows.GetCount());
    for (wxWindow* window : wxTopLevelWindows)
        windows.push_back(window);

    for (wxWindow* window : windows)
        ApplyToWindow(window);
}

int AppTheme::FilterEvent(wxEvent& event)
{
    if (event.GetEventType() == wxEVT_SHOW)
    {
        const auto showEvent = dynamic_cast<wxShowEvent*>(&event);
        if (!_applying && showEvent != nullptr && showEvent->IsShown())
        {
            if (auto* window = dynamic_cast<wxTopLevelWindow*>(event.GetEventObject()))
            {
                window->CallAfter([window] {
                    AppTheme& instance = Instance();
                    if (instance._applying || window->IsBeingDeleted())
                        return;

                    instance._applying = true;
                    ApplyToWindow(window);
                    instance._applying = false;
                });
            }
        }
    }

    return Event_Skip;
}
