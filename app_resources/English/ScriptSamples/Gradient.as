void Configure()
{
	SetFractalName("Gradient");
	SetCategory("Other");
}

void Render()
{
	int color;
	if(threadIndex == 0)
	{
		for(int y=0; y<screenHeight; y++)
		{
			color = (float(y)/screenHeight)*paletteSize;
			for(int x=0; x<screenWidth; x++)
			{
				SetPoint(x, y, false, color);
			}
		}
	}
}