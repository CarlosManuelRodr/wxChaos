void Configure()
{
	SetFractalName("Hello world");
	SetCategory("Other");
	NoSetMap(true);
}

void Render()
{
	if(threadIndex == 0)
	{
		PrintString("Hello world\n");
		PrintInt(34);
		PrintString("\n");
		PrintFloat(0.02);
		PrintString("\n");
		complex z(0.23, 389);
		PrintComplex(z);
	}
}