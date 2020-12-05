/* Categorias de script: Complex, NumMet, Physic, Other.
*/

void SetParams()
{
    SetFractalName("Newton z^8+15z^4-16 = 0");
    SetCategory("NumMet");
    SetMinX(-5);
    SetMaxX(5);
    SetMinY(-4);
    NoSetMap(true);
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
                z = z - (pow(z,8)+fifteen*pow(z,4)-sixteen)/(eight*pow(z,7)+sixty*pow(z,3));
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