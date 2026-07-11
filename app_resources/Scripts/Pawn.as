void Configure()
{
    SetFractalName("Pawn");
    SetDocumentationPath("script_pawn.html");
    SetCategory("Complex");
    SetMinX(-1.22);
    SetMaxX(-0.86033);
    SetMinY(-0.11782);
}

void Render()
{
    complex z, c, temp;
    complex one(1,0);
    double c_im;
    int n;
    int i;
    bool insideSet;
    for(int y=ho; y<hf; y++)
    {
        c_im = maxY - y*yFactor;
        for(int x=wo; x<wf; x++)
        {
            z = c = complex(minX + x*xFactor, c_im);
            insideSet = true;

            for(n=0; n<maxIterations; n++)
            {
                z = pow(z,n) + c;
                
                if(z.real()*z.real() + z.imag()*z.imag() > 4)
                {
                    insideSet = false;
                    break;
                }
            }
            if(insideSet)
            {
                SetPoint(x, y, true, n);
            }
            else
            {
                SetPoint(x, y, false, n);
            }
        }
    }
}

void DrawOrbit()
{
    complex z(orbitX, orbitY);
    const complex c = z;

    for(uint n = 0; n < maxIterations; n++)
    {
        const complex previous = z;
        z = pow(z, int(n)) + c;
        DrawLine(previous.real(), previous.imag(), z.real(), z.imag(), 0, 255, 0);
        if(z.real() * z.real() + z.imag() * z.imag() > 4)
            break;
    }
}
