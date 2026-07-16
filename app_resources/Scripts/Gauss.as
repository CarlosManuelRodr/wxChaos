void Configure()
{
    SetFractalName("Gauss map");
    SetDocumentationPath("script_gauss_map.html");
    SetCategory("Other");
    SetMinX(-1.5);
    SetMaxX(1.5);
    SetMinY(-0.4);
    SetRedrawAlways(true);
    SetExtColorMode(false);
    SetDimensionCalculatorEnabled(true);
    SetDimensionCalculatorPreset(
        -1.5, 1.5, -1.25,  // Min X, Max X, Min Y
        500,               // Iterations
        "5*x",             // Division function
        1, 100,            // Function range
        5000               // Image size
    );
}

void Render()
{
    if(threadIndex == 0)
    {
        double a, b, x;
        a = 4.90;
        
        x = 0.25;
        int coordX, coordY;
        
        for(int i=0; i<screenWidth; i++)
        {
            b = minX + i*xFactor;
            for(int n=0; n<maxIterations; n++)
            {
                x = exp_r(-a*pow_r(x,2)) + b;
                coordX = ((b-minX)/xFactor);
                coordY = ((maxY-x)/yFactor);
                if((coordX >= 0 && coordX < screenWidth) && (coordY >= 0 && coordY < screenHeight))
                {
                    SetPoint(coordX, coordY, true, 0);
                }
            }
        }
    }
}
