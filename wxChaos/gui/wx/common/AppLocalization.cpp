#include "common/AppLocalization.h"
#include "AppPaths.h"

std::unique_ptr<wxLocale> AppLocalization::_locale;
AppLanguage AppLocalization::_currentLanguage = AppLanguage::English;

AppLanguage AppLocalization::ResolveLanguage(const AppLanguage language)
{
    if (language != AppLanguage::System)
        return language;

    const int systemLanguage = wxLocale::GetSystemLanguage();
    const wxLanguageInfo* info = wxLocale::GetLanguageInfo(systemLanguage);
    if (info != nullptr && info->CanonicalName.StartsWith("es"))
        return AppLanguage::Spanish;

    return AppLanguage::English;
}

int AppLocalization::ToWxLanguage(const AppLanguage language)
{
    switch (ResolveLanguage(language))
    {
        case AppLanguage::Spanish:
            return wxLANGUAGE_SPANISH;
        case AppLanguage::English:
        case AppLanguage::System:
        default:
            return wxLANGUAGE_ENGLISH;
    }
}

wxString AppLocalization::DocumentationLanguageCode(const AppLanguage language)
{
    switch (ResolveLanguage(language))
    {
        case AppLanguage::Spanish:
            return "es";
        case AppLanguage::English:
        case AppLanguage::System:
        default:
            return "en";
    }
}

void AppLocalization::Initialize(const AppLanguage language)
{
    _currentLanguage = ResolveLanguage(language);
    _locale = std::make_unique<wxLocale>();

    if (!_locale->Init(ToWxLanguage(_currentLanguage), wxLOCALE_DONT_LOAD_DEFAULT))
    {
        _currentLanguage = AppLanguage::English;
        _locale = std::make_unique<wxLocale>();
        _locale->Init(wxLANGUAGE_ENGLISH, wxLOCALE_DONT_LOAD_DEFAULT);
    }

    wxLocale::AddCatalogLookupPathPrefix(AppPaths::ResourceFile({"Locale"}));
    _locale->AddCatalog("wxchaos");
}

AppLanguage AppLocalization::CurrentLanguage()
{
    return _currentLanguage;
}
