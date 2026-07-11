void Configure()
{
    SetFractalName("Newton cos(z) - z = 0");
    SetDocumentationPath("script_newton_cos.html");
    SetCategory("NumMet");
    SetMinX(-11.1389);
    SetMaxX(-4.3754);
    SetMinY(-2.17419);
    SetEnableSetMap(false);
    AddDoubleOption("minStep", "Min step: ", 0.01);
}

void Render()
{
    complex z, z_prev;
    complex one = complex(1,0);
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
                z = z + (cos(z)-z)/(sin(z)+one);
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
                    SetPoint(x, y, false, n + 1);
                }
                else if(slope > 1)
                {
                    SetPoint(x, y, false, n + 37);
                }
                else
                {
                    SetPoint(x, y, false, n + 57);
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
    const complex one(1, 0);
    const double minStep = GetDoubleOption("minStep");

    for(uint n = 0; n < maxIterations; n++)
    {
        const complex previous = z;
        z = z + (cos(z) - z) / (sin(z) + one);
        DrawLine(previous.real(), previous.imag(), z.real(), z.imag(), 0, 255, 0);
        if(abs_r(previous.real() - z.real()) < minStep && abs_r(previous.imag() - z.imag()) < minStep)
            break;
    }
}
