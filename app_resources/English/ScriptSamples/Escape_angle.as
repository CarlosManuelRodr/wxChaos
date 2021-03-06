void Configure()
{
	SetFractalName("Escape_angle");
	SetCategory("Complex");
	SetMinX(-2.52);
	SetMaxX(1.16);
	SetMinY(-1.121);
	SetJuliaVariety(false);
}

void Render()
{
	complex z, c;
	float c_im;
	int n;
	int i;
	bool insideSet;
	for(int y=ho; y<hf; y++)
	{
		c_im = maxY - y*yFactor;
		for(int x=wo; x<wf; x++)
		{
			c = z = complex(minX + x*xFactor, c_im);
			insideSet = true;

			for(n=0; n<maxIter; n++)
			{
				z = pow(z,2) + c;
				
				if(z.real()*z.real() + z.imag()*z.imag() > 4)
				{
					insideSet = false;
					break;
				}
			}
			if(z.real() > 0 && z.imag() > 0)
			{
				SetPoint(x, y, false, n + 1);
			}
			else if(z.real() <= 0 && z.imag() > 0)
			{
				SetPoint(x, y, false, n + 27);
			}
			else if(z.real() <= 0 && z.imag() < 0)
			{
				SetPoint(x, y, false, n + 37);
			}
			else
			{
				SetPoint(x, y, false, n + 47);
			}
		}
	}
}