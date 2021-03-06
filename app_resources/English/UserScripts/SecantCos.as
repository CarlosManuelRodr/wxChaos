/* Categorias de script: Complex, NumMet, Physic, Other.
*/

void Configure()
{
    SetFractalName("Secant method cos(z) - 1 = 0");
    SetCategory("NumMet");
    SetMinX(-25.2866);
    SetMaxX(11.6554);
    SetMinY(-11.0199);
    NoSetMap(true);
}

void Render()
{
    complex z, z_prev, temp;
    double c_im;
    int n;
    double slope;
    double tolerance = 0.001;
    for(int y=ho; y<hf; y++)
    {
        c_im = maxY - y*yFactor;
        for(int x=wo; x<wf; x++)
        {
            z = complex(minX + x*xFactor, c_im);
            z_prev = z;
            z = cos(z)-z;
            for(n=0; n<maxIter; n++)
            {
                temp = z;
                z = z - (z-z_prev)/((cos(z)-z)-(cos(z_prev)-z_prev));
                z_prev = z;
                if(
					(z_prev.real() - tolerance < z.real() && z_prev.real() + tolerance > z.real()) 
					&& (z_prev.imag() - tolerance < z.imag() && z_prev.imag() + tolerance > z.imag())
				  )
				{
                    break;
				}
            }
            if(z.real() != 0)
            {
                slope = z.imag() / z.real();
                if(slope < 1 && slope > 0.0)
                    SetPoint(x, y, false, n + 37);
                else
                    SetPoint(x, y, false, n + 1);
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