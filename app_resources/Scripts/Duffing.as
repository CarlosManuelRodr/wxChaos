void Configure()
{
    SetFractalName("Duffing map");
    SetDocumentationPath("script_duffing_map.html");
    SetCategory("Other");
    SetCoordinateSystem("x", "y");
    SetMinX(-2.86192);
    SetMaxX(2.82077);
    SetMinY(-1.81804);
    SetDefaultIter(20000);
    SetRedrawAlways(true);
    SetExtColorMode(false);
    AddDoubleOption("a", "a: ", 2.75);
    SetDimensionCalculatorEnabled(true);
    SetDimensionCalculatorPreset(
        -2.20, 2.20, -2.10,  // Min X, Max X, Min Y
        50000,               // Iterations
        "5*x",               // Division function
        1, 100,              // Function range
        5000                 // Image size
    );
}

void Render()
{
    if(threadIndex == 0)
    {
        double x = 0.5;
        double y = 0.2;
        double tempX;
        const double a = GetDoubleOption("a");
        double b = 0.2;
        int coordX, coordY;
        
        for(int i=0; i<maxIterations; i++)
        {
            coordX = ((x-minX)/xFactor);
            coordY = ((maxY-y)/yFactor);
            if((coordX >= 0 && coordX < screenWidth) && (coordY >= 0 && coordY < screenHeight))
                SetPoint(coordX, coordY, true, 0);

            tempX = x;
            x = y;
            y = -b*tempX + a*y - y*y*y;
        }
    }
}
