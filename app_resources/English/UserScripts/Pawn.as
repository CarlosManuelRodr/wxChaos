void Configure()
{
    SetFractalName("Pawn");
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

            for(n=0; n<maxIter; n++)
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