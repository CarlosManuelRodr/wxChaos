void Configure()
{
    SetFractalName("Duffing map");
    SetDocumentationPath("script_duffing_map.html");
    SetCategory("Other");
    SetMinX(-2.86192);
    SetMaxX(2.82077);
    SetMinY(-1.81804);
    SetDefaultIter(20000);
    SetRedrawAlways(true);
    SetExtColorMode(false);
    AddDoubleOption("a", "a: ", 2.75);
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
