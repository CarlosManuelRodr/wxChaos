void Configure()
{
	SetFractalName("ScriptJulia");
	SetCategory("Complex");
	SetMinX(-1.81818);
	SetMaxX(1.8441);
	SetMinY(-1.16464);
	SetJuliaVariety(true);
}

void Render()
{
	complex z, k;
	float c_im;
	int n;
	int i;
	bool insideSet;
	k = complex(kReal, kImaginary);
	for(int y=ho; y<hf; y++)
	{
		c_im = maxY - y*yFactor;
		for(int x=wo; x<wf; x++)
		{
			z = complex(minX + x*xFactor, c_im);
			insideSet = true;

			for(n=0; n<maxIter; n++)
			{
				z = pow(z,2) + k;
				
				if(z.real()*z.real() + z.imag()*z.imag() > 4)
				{
					insideSet = false;
					break;
				}
			}
			SetPoint(x, y, insideSet, n);
		}
	}
}