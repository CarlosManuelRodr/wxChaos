void Configure()
{
    SetFractalName("Secant method z^3 - 1 = 0");
    SetDocumentationPath("script_secant_polynomial.html");
    SetCategory("NumMet");
    SetMinX(-5.09166);
    SetMaxX(4.72931);
    SetMinY(-3.26225);
    SetEnableSetMap(false);
}

void Render()
{
    complex z, z_prev, temp;
    double c_im;
    double tolerance = 0.001;
    double slope;
    int n;
    for(int y=ho; y<hf; y++)
    {
        c_im = maxY - y*yFactor;
        for(int x=wo; x<wf; x++)
        {
            z = complex(minX + x*xFactor, c_im);
            z_prev = z;
            z = pow(z,3)-z;
            for(n=0; n<maxIterations; n++)
            {
                temp = z;
                z = z - (z-z_prev)/((pow(z,3)-z)-(pow(z_prev,2)-z_prev));
                z_prev = z;
                if((z_prev.real() - tolerance < z.real() && z_prev.real() + tolerance > z.real()) && (z_prev.imag() - tolerance < z.imag() && z_prev.imag() + tolerance > z.imag()))
                {
                    break;
                }
            }
            if(z.real() > 0 && z.imag() > 0)
                SetPoint(x, y, false, n + 1);
            else if(z.real() <= 0 && z.imag() > 0)
                SetPoint(x, y, false, n + 27);
            else if(z.real() <= 0 && z.imag() < 0)
                SetPoint(x, y, false, n + 37);
            else
                SetPoint(x, y, false, n + 47);
        }
    }
}

void DrawOrbit()
{
    complex z(orbitX, orbitY);
    complex previous = z;
    z = pow(z, 3) - z;
    const double tolerance = 0.001;

    for(uint n = 0; n < maxIterations; n++)
    {
        const complex current = z;
        z = z - (z - previous) / ((pow(z, 3) - z) - (pow(previous, 2) - previous));
        DrawLine(current.real(), current.imag(), z.real(), z.imag(), 0, 255, 0);
        if(abs_r(current.real() - z.real()) < tolerance && abs_r(current.imag() - z.imag()) < tolerance)
            break;
        previous = current;
    }
}
