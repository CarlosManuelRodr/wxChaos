#include <doctest/doctest.h>
#include <wx/filename.h>

#include "common/AppLocalization.h"
#include "docs/FractalDocumentation.h"

TEST_CASE("script documentation is absent by default")
{
    const ScriptData scriptData;

    CHECK_FALSE(FractalDocumentation::HasDocumentation(scriptData));
    CHECK(FractalDocumentation::GetDocumentFile(scriptData).empty());
}

TEST_CASE("Logistic Map documentation is registered")
{
    CHECK(FractalDocumentation::HasDocumentation(FractalType::LogisticMap));
    CHECK(FractalDocumentation::GetDocumentFile(FractalType::LogisticMap).EndsWith("logistic_map.html"));
}

TEST_CASE("localized fractal documentation keeps the canonical document file")
{
    const wxString documentFile = FractalDocumentation::GetDocumentFile(FractalType::Mandelbrot, AppLanguage::Spanish);
    wxString normalizedDocumentFile = documentFile;
    normalizedDocumentFile.Replace("\\", "/");

    CHECK(documentFile.Contains("Documents"));
    CHECK_FALSE(normalizedDocumentFile.Contains("Documents/es"));
    CHECK(documentFile.EndsWith("mandelbrot.html"));
}

TEST_CASE("script documentation path resolves under bundled documents")
{
    ScriptData scriptData;
    scriptData.documentationPath = "script_gallery/duffing.html";

    const wxString documentFile = FractalDocumentation::GetDocumentFile(scriptData);

    CHECK(FractalDocumentation::HasDocumentation(scriptData));
    CHECK(documentFile.Contains("Resources"));
    CHECK(documentFile.Contains("Documents"));
    CHECK(documentFile.Contains("script_gallery"));
    CHECK(documentFile.EndsWith("duffing.html"));
}

TEST_CASE("script documentation path accepts bundled documents prefix")
{
    ScriptData scriptData;
    scriptData.documentationPath = "Documents/duffing.html";

    const wxString documentFile = FractalDocumentation::GetDocumentFile(scriptData);

    CHECK(documentFile.Contains("Resources"));
    CHECK(documentFile.Contains("Documents"));
    CHECK(documentFile.EndsWith("duffing.html"));
}

TEST_CASE("script documentation path preserves absolute paths")
{
    wxFileName absolutePath;
    absolutePath.AssignDir(wxFileName::GetHomeDir());
    absolutePath.SetFullName("script_documentation.html");

    ScriptData scriptData;
    scriptData.documentationPath = std::string(absolutePath.GetFullPath().utf8_string());

    CHECK(FractalDocumentation::GetDocumentFile(scriptData) == absolutePath.GetFullPath());
}
