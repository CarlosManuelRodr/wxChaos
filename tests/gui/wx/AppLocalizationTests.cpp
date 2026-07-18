#include <doctest/doctest.h>

#include "common/AppLocalization.h"

TEST_CASE("AppLocalization maps explicit languages to wx language ids")
{
    CHECK(AppLocalization::ResolveLanguage(AppLanguage::English) == AppLanguage::English);
    CHECK(AppLocalization::ResolveLanguage(AppLanguage::Spanish) == AppLanguage::Spanish);
    CHECK(AppLocalization::ResolveLanguage(AppLanguage::Japanese) == AppLanguage::Japanese);
    CHECK(AppLocalization::ToWxLanguage(AppLanguage::English) == wxLANGUAGE_ENGLISH);
    CHECK(AppLocalization::ToWxLanguage(AppLanguage::Spanish) == wxLANGUAGE_SPANISH);
    CHECK(AppLocalization::ToWxLanguage(AppLanguage::Japanese) == wxLANGUAGE_JAPANESE);
}

TEST_CASE("AppLocalization maps documentation language codes")
{
    CHECK(AppLocalization::DocumentationLanguageCode(AppLanguage::English) == "en");
    CHECK(AppLocalization::DocumentationLanguageCode(AppLanguage::Spanish) == "es");
    CHECK(AppLocalization::DocumentationLanguageCode(AppLanguage::Japanese) == "en");
}
