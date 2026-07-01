#include "docs/FractalDocumentation.h"
#include "AppPaths.h"

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
    const wxString filename = GetDocumentFilename(type);
    return filename.empty() ? wxEmptyString : AppPaths::ResourceFile({"Documents", filename});
}
