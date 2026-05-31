void Configure()
{
	SetFractalName("Henon Map (script)");
	SetCategory("Other");
	SetMinX(-1.5);
	SetMaxX(1.5);
	SetMinY(-0.4);
	SetDefaultIter(20000);
	SetRedrawAlways(true);
	SetExtColorMode(false);
}

void Render()
{
	if(threadIndex == 0)
	{
		double x = 0.5;
		double y = 0.2;
		double tempX;
		double alpha = 1.4;
		double beta = 0.3;
		int coordX, coordY;
		
		for(int i=0; i<maxIter; i++)
		{
			coordX = ((x-minX)/xFactor);
			coordY = ((maxY-y)/yFactor);
			if((coordX >= 0 && coordX < screenWidth) && (coordY >= 0 && coordY < screenHeight))
			{
				SetPoint(coordX, coordY, true, 0);
			}
			tempX = x;
			x = y + 1 - alpha*x*x;
			y = beta*tempX;
		}
	}
}