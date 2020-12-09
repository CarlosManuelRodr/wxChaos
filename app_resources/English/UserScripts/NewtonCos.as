/* Categorias de script: Complex, NumMet, Physic, Other.
*/

void Configure()
{
    SetFractalName("Newton cos(z) - z = 0");
    SetCategory("NumMet");
    SetMinX(-11.1389);
    SetMaxX(-4.3754);
    SetMinY(-2.17419);
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
                z = z + (cos(z)-z)/(sin(z)+one);
                if((z_prev.real() - tolerance < z.real() && z_prev.real() + tolerance > z.real()) && (z_prev.imag() - tolerance < z.imag() && z_prev.imag() + tolerance > z.imag()))
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