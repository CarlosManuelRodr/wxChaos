#include "docs/FractalDocumentation.h"
#include "AppPaths.h"
#include "common/AppLocalization.h"
#include <wx/filename.h>

wxString FractalDocumentation::ResolveBundledDocumentFile(const wxString& relativePath, const AppLanguage language)
{
    (void) language;

    if (relativePath.empty())
        return wxEmptyString;

    wxFileName defaultFile;
    defaultFile.AssignDir(AppPaths::AppDirectory({"Resources", "Documents"}));

    wxString remaining = relativePath;
    while (remaining.Contains("/"))
    {
        const wxString directory = remaining.BeforeFirst('/');
        if (!directory.empty() && directory != ".")
            defaultFile.AppendDir(directory);
        remaining = remaining.AfterFirst('/');
    }

    defaultFile.SetFullName(remaining);
    defaultFile.Normalize(wxPATH_NORM_DOTS | wxPATH_NORM_TILDE);
    return defaultFile.GetFullPath();
}

wxString FractalDocumentation::ResolveScriptDocumentFile(const std::string& documentationPath)
{
    ScriptData scriptData;
    scriptData.documentationPath = documentationPath;
    return GetDocumentFile(scriptData, AppLocalization::CurrentLanguage());
}

wxString FractalDocumentation::GetDocumentFile(const ScriptData& scriptData, const AppLanguage language)
{
    if (scriptData.documentationPath.empty())
        return wxEmptyString;

    wxString path = wxString::FromUTF8(scriptData.documentationPath.c_str());
    path.Replace("\\", "/");

    if (wxFileName fileName(path); fileName.IsAbsolute())
        return fileName.GetFullPath();

    if (path.StartsWith("Resources/Documents/"))
        path = path.Mid(wxString("Resources/Documents/").length());
    else if (path.StartsWith("Documents/"))
        path = path.Mid(wxString("Documents/").length());

    return ResolveBundledDocumentFile(path, language);
}

wxString FractalDocumentation::GetDocumentFilename(const FractalType type)
{
    switch (type)
    {
        case FractalType::Mandelbrot: return "mandelbrot.html";
        case FractalType::MandelbrotZN: return "mandelbrot_zn.html";
        case FractalType::Julia: return "julia.html";
        case FractalType::JuliaZN: return "julia_zn.html";
        case FractalType::NewtonRaphsonMethod: return "newton.html";
        case FractalType::Sinusoidal: return "sinusoidal.html";
        case FractalType::Magnetic: return "magnet.html";
        case FractalType::Jellyfish: return "jellyfish.html";
        case FractalType::Manowar: return "manowar.html";
        case FractalType::ManowarJulia: return "manowar_julia.html";
        case FractalType::SierpinskiTriangle: return "sierpinski_triangle.html";
        case FractalType::FixedPoint1: return "fixed_point_sin.html";
        case FractalType::FixedPoint2: return "fixed_point_cos.html";
        case FractalType::FixedPoint3: return "fixed_point_tan.html";
        case FractalType::FixedPoint4: return "fixed_point_z2.html";
        case FractalType::Tricorn: return "tricorn.html";
        case FractalType::BurningShip: return "burning_ship.html";
        case FractalType::BurningShipJulia: return "burning_ship_julia.html";
        case FractalType::Fractory: return "fractory.html";
        case FractalType::Cell: return "cell.html";
        case FractalType::LogisticMap: return "logistic_map.html";
        case FractalType::HenonMap: return "henon_map.html";
        case FractalType::DoublePendulum: return "double_pendulum.html";
        case FractalType::UserDefinedEscapeTime: return "user_defined_escape_time.html";
        case FractalType::UserDefinedFixedPoint: return "user_defined_fixed_point.html";
        case FractalType::UserDefinedNewtonRaphson: return "user_defined_newton.html";
        default: return wxEmptyString;
    }
}

bool FractalDocumentation::HasDocumentation(const FractalType type)
{
    return !GetDocumentFilename(type).empty();
}

wxString FractalDocumentation::GetDocumentFile(const FractalType type)
{
    return GetDocumentFile(type, AppLocalization::CurrentLanguage());
}

wxString FractalDocumentation::GetDocumentFile(const FractalType type, const AppLanguage language)
{
    const wxString filename = GetDocumentFilename(type);
    return filename.empty() ? wxEmptyString : ResolveBundledDocumentFile(filename, language);
}

bool FractalDocumentation::HasDocumentation(const ScriptData& scriptData)
{
    return !scriptData.documentationPath.empty();
}

wxString FractalDocumentation::GetDocumentFile(const ScriptData& scriptData)
{
    return GetDocumentFile(scriptData, AppLocalization::CurrentLanguage());
}
