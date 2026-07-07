#pragma once

#include <memory>
#include <wx/intl.h>
#include "common/AppLanguage.h"

/**
 * @class AppLocalization
 * @brief Owns wxWidgets catalog loading and language resolution.
 */
class AppLocalization
{
public:
    static AppLanguage ResolveLanguage(AppLanguage language);
    static int ToWxLanguage(AppLanguage language);
    static wxString DocumentationLanguageDirectory(AppLanguage language);
    static void Initialize(AppLanguage language);
    static AppLanguage CurrentLanguage();

private:
    static std::unique_ptr<wxLocale> _locale;
    static AppLanguage _currentLanguage;
};
