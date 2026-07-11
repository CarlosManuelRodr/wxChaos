void Configure()
{
    SetFractalName("Newton z^8+15z^4-16 = 0");
    SetDocumentationPath("script_newton_polynomial.html");
    SetCategory("NumMet");
    SetMinX(-5);
    SetMaxX(5);
    SetMinY(-4);
    SetEnableSetMap(false);
    const string minStepLabel = GetCurrentLocale() == "es" ? "Paso mínimo: " : "Minimum step: ";
    AddDoubleOption("minStep", minStepLabel, 0.01);
}

void Render()
{
    complex z, z_prev;
    complex fifteen = complex(15,0);
    complex sixteen = complex(16,0);
    complex eight = complex(8,0);
    complex sixty = complex(60,0);
    double c_im;
    int n;
    int slope;
    const double minStep = GetDoubleOption("minStep");
    for(int y=ho; y<hf; y++)
    {
        c_im = maxY - y*yFactor;
        for(int x=wo; x<wf; x++)
        {
            z = complex(minX + x*xFactor, c_im);
            for(n=0; n<maxIterations; n++)
            {
                z_prev = z;
                z = z - (pow(z,8)+fifteen*pow(z,4)-sixteen)/(eight*pow(z,7)+sixty*pow(z,3));
                if((z_prev.real() - minStep < z.real() && z_prev.real() + minStep > z.real()) && (z_prev.imag() - minStep < z.imag() && z_prev.imag() + minStep > z.imag()))
                {
                    break;
                }
            }
            if(z.real() != 0)
            {
                slope = z.imag()/z.real();
                if(slope < 1 && slope > 0.0)
                {
                    SetPoint(x, y, false, n + 37);
                }
                else
                {
                    SetPoint(x, y, false, n + 1);
                }
            }
            else
            {
                if(z.imag() > 0)
                {
                    SetPoint(x, y, false, n + 1);
                }
                else
                {
                    SetPoint(x, y, false, n + 37);
                }
            }
        }
    }
}

void DrawOrbit()
{
    complex z(orbitX, orbitY);
    const complex fifteen(15, 0);
    const complex sixteen(16, 0);
    const complex eight(8, 0);
    const complex sixty(60, 0);
    const double minStep = GetDoubleOption("minStep");

    for(uint n = 0; n < maxIterations; n++)
    {
        const complex previous = z;
        z = z - (pow(z, 8) + fifteen * pow(z, 4) - sixteen) / (eight * pow(z, 7) + sixty * pow(z, 3));
        DrawLine(previous.real(), previous.imag(), z.real(), z.imag(), 0, 255, 0);
        if(abs_r(previous.real() - z.real()) < minStep && abs_r(previous.imag() - z.imag()) < minStep)
            break;
    }
}
