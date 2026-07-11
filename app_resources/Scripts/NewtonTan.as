void Configure()
{
    SetFractalName("Newton tan(z) - z = 0");
    SetDocumentationPath("script_newton_tan.html");
    SetCategory("NumMet");
    SetMinX(-4.22205);
    SetMaxX(4.27001);
    SetMinY(-2.69);
    NoSetMap(true);
}

void Render()
{
    complex z, z_prev;
    complex one = complex(1,0);
    double c_im;
    int n;
    int slope;
    double tolerance = 0.01;
    for(int y=ho; y<hf; y++)
    {
        c_im = maxY - y*yFactor;
        for(int x=wo; x<wf; x++)
        {
            z = complex(minX + x*xFactor, c_im);
            for(n=0; n<maxIter; n++)
            {
                z_prev = z;
                z = z - (tan(z)-z)/(pow(sec(z),2)+one);
                if((z_prev.real() - tolerance < z.real() && z_prev.real() + tolerance > z.real()) && (z_prev.imag() - tolerance < z.imag() && z_prev.imag() + tolerance > z.imag()))
                {
                    break;
                }
            }
            if(z.real() < -4.49)
            {
                SetPoint(x, y, false, n + 1);
            }
            else if(z.real() < 0 && z.real() > -4.49)
            {
                SetPoint(x, y, false, n + 20);
            }
            else if(z.real() > 0 && z.real() < 4.49)
            {
                SetPoint(x, y, false, n + 30);
            }
            else
            {
                SetPoint(x, y, false, n + 40);
            }
        }
    }
}

void DrawOrbit()
{
    complex z(orbitX, orbitY);
    const complex one(1, 0);
    const double tolerance = 0.01;

    for(uint n = 0; n < maxIter; n++)
    {
        const complex previous = z;
        z = z - (tan(z) - z) / (pow(sec(z), 2) + one);
        DrawLine(previous.real(), previous.imag(), z.real(), z.imag(), 0, 255, 0);
        if(abs_r(previous.real() - z.real()) < tolerance && abs_r(previous.imag() - z.imag()) < tolerance)
            break;
    }
}
