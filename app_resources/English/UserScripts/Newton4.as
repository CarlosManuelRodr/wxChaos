void SetParams()
{
    SetFractalName("Newton z^4 - 1 = 0");
    SetCategory("NumMet");
    SetMinX(-2);
    SetMaxX(2);
    SetMinY(-1.35897);
    NoSetMap(true);
}

void Render()
{
    complex z, z_prev, temp;
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
                z = z - (pow(z,4) - complex(1.0,0.0))/(complex(4,0)*pow(z,3));
                if((z_prev.real() - tolerance < z.real() && z_prev.real() + tolerance > z.real()) && (z_prev.imag() - tolerance < z.imag() && z_prev.imag() + tolerance > z.imag()))
                {
                    break;
                }
            }
            if(z.real() != 0)
            {
                slope = z.imag()/z.real();
                if(slope < 0.7853 && slope > 0.0)
                    SetPoint(x, y, false, n + 1);
                else
                    SetPoint(x, y, false, n + 37);
            }
            else
            {
                if(z.imag() > 0)
                    SetPoint(x, y, false, n + 1);
                else
                    SetPoint(x, y, false, n + 37);
            }
        }
    }
}