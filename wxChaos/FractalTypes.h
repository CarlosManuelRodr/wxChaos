/**
* @file FractalTypes.h
* @brief Defines each type of fractal.
*
* @author Carlos Manuel Rodriguez y Martinez
* @copyright GNU Public License.
* @date 7/19/2012
*/

#pragma once
#ifndef _fractalTypes
#define _fractalTypes

#include "FractalClasses.h"
#include "AngelScriptFunc.h"

/**
* @class RenderMandelbrot
* @brief Threaded Mandelbrot rendering routines.
*/
class RenderMandelbrot : public RenderFractal
{
	double c_im;
	double c_re;
	double Z_re;
	double Z_im;
	double Z_re2;
	double Z_im2;
	int buddhaRandomP;
	int bd;

	bool insideSet;

public:
	void Render();
	void SpecialRender();

	///@brief Number of random points that will launch in Buddhabrot mode.
	///@param n Number of points.
	void SetBuddhaRandomP(int n);
	RenderMandelbrot();

	int AskProgress();
};

/**
* @class Mandelbrot
* @brief Handles the RenderMandelbrot threads.
*/
class Mandelbrot : public Fractal
{
	RenderMandelbrot *myRender;
	int buddhaRandomP;
public:
	void Render();																///< Launch threads.
	void DrawOrbit();
	wxString AskInfo();
	wxString AskInfo( double real, double imag, int iter = 0 );
	wxString SaveOrbit(double real, double imag, int iter, wxString filepath);
	void SpecialSaveRenderJob(std::ofstream &dataFile);
	void CopyOptFromPanel();
	void PreRender();
	void PreDrawMaps();
	void ChangeThreadNumber();
	Mandelbrot( sf::RenderWindow *Window );
	Mandelbrot( int width, int height );
	~Mandelbrot();
};

/**
* @class RenderMandelbrotZN
* @brief Threaded MandelbrotZN rendering routines.
*/
class RenderMandelbrotZN : public RenderFractal
{
private:

	double c_im;
	double c_re;
	double Z_re;
	double Z_im;
	double Z_re2;
	double Z_im2;
	int n;
	double bailout;

	bool insideSet;

public:
	void Render();						// Render dentro de la thread.
	void SpecialRender();				// SpecialRender dentro de la thread.
	void SetParams(int _n, double _bailout);
	RenderMandelbrotZN();
};


/**
* @class MandelbrotZN
* @brief Handles the RenderMandelbrotZN threads.
*/
class MandelbrotZN : public Fractal
{
	int n;
	double bailout;
	RenderMandelbrotZN *myRender;
public:
	void Render();
	void DrawOrbit();
	wxString AskInfo();
	wxString AskInfo( double real, double imag, int iter = 0 );
	wxString SaveOrbit(double real, double imag, int iter, wxString filepath);
	void SpecialSaveRenderJob(std::ofstream &dataFile);
	void CopyOptFromPanel();
	void ChangeThreadNumber();
	MandelbrotZN( sf::RenderWindow *Window );
	MandelbrotZN( int width, int height );
	~MandelbrotZN();
};

/**
* @class RenderJulia
* @brief Threaded Julia rendering routines.
*/
class RenderJulia : public RenderFractal
{
private:
	double c_im;
	double c_re;
	double Z_re;
	double Z_im;
	double Z_re2;
	double Z_im2;
	bool insideSet;

public:
	void Render();
	void SpecialRender();
	RenderJulia();
};

/**
* @class Julia
* @brief Handles the RenderJulia threads.
*/
class Julia: public Fractal
{
	RenderJulia *myRender;
public:
	Julia( sf::RenderWindow *Window );
	Julia( int width, int height );
	~Julia();
	void ChangeThreadNumber();
	void Render();
	void DrawOrbit();
	wxString AskInfo();
	wxString AskInfo( double real, double imag, int iter = 0 );
};

/**
* @class RenderJuliaZN
* @brief Threaded JuliaZN rendering routines.
*/
class RenderJuliaZN : public RenderFractal
{
private:
	double c_im;
	double c_re;
	double Z_re;
	double Z_im;
	double Z_re2;
	double Z_im2;
	int n;
	double bailout;
	bool insideSet;

public:
	void Render();
	void SpecialRender();
	RenderJuliaZN();
	void SetParams(int _n, double _bailout);
};

/**
* @class JuliaZN
* @brief Handles the RenderJuliaZN threads.
*/
class JuliaZN: public Fractal
{
	RenderJuliaZN *myRender;
	int n;
	double bailout;

public:
	JuliaZN( sf::RenderWindow *Window );
	JuliaZN( int width, int height );
	~JuliaZN();
	void ChangeThreadNumber();
	void SpecialSaveRenderJob(std::ofstream &dataFile);
	void CopyOptFromPanel();
	void Render();
	void DrawOrbit();
	wxString AskInfo();
	wxString AskInfo( double real, double imag, int iter = 0 );
};

/**
* @class RenderNewton
* @brief Threaded Newton rendering routines.
*/
class RenderNewton : public RenderFractal
{
private:
	double re, im;
	double tmp;
	double d;
	double re2, im2;
	double minStep;

	double re_ant, im_ant;
public:
	RenderNewton();
	void Render();
	void SpecialRender();
	void SetParams(double _minStep);
};

/**
* @class Newton
* @brief Handles the RenderNewton threads.
*/
class Newton : public Fractal
{
	RenderNewton *myRender;
	double minStep;
public:
	void Render();
	Newton( sf::RenderWindow *Window );
	Newton( int width, int height );
	~Newton();
	void ChangeThreadNumber();
	void DrawOrbit();
	wxString AskInfo();
	wxString AskInfo( double real, double imag, int iter = 0 );
	void SpecialSaveRenderJob(std::ofstream &dataFile);
	void CopyOptFromPanel();
};

/**
* @class RenderSinoidal
* @brief Threaded Sinoidal rendering routines.
*/
class RenderSinoidal : public RenderFractal
{
public:
	RenderSinoidal();
	void Render();
	void SpecialRender();
};

/**
* @class Sinoidal
* @brief Handles the RenderSinoidal threads.
*/
class Sinoidal : public Fractal
{
	RenderSinoidal *myRender;
public:
	void Render();
	Sinoidal( sf::RenderWindow *Window );
	Sinoidal( int width, int height );
	~Sinoidal();
	void ChangeThreadNumber();
	void DrawOrbit();
	wxString AskInfo();
	wxString AskInfo( double real, double imag, int iter = 0 );
};

/**
* @class RenderMagnet
* @brief Threaded Magnet rendering routines.
*/
class RenderMagnet : public RenderFractal
{
public:
	RenderMagnet();
	void Render();
};

/**
* @class Magnet
* @brief Handles the RenderMagnet threads.
*/

class Magnet : public Fractal
{
	RenderMagnet *myRender;
public:
	void Render();
	Magnet( sf::RenderWindow *Window );
	Magnet( int width, int height );
	~Magnet();
	void ChangeThreadNumber();
	void DrawOrbit();
	wxString AskInfo();
	wxString AskInfo( double real, double imag, int iter = 0 );
};

/**
* @class RenderManowar
* @brief Threaded Manowar rendering routines.
*/
class RenderManowar : public RenderFractal
{
public:
	RenderManowar();
	void Render();
	void SpecialRender();
};

/**
* @class Manowar
* @brief Handles the RenderManowar threads.
*/
class Manowar : public Fractal
{
	RenderManowar *myRender;
public:
	void Render();
	Manowar( sf::RenderWindow *Window );
	Manowar( int width, int height );
	~Manowar();
	void ChangeThreadNumber();
	void DrawOrbit();
	wxString AskInfo();
	wxString AskInfo( double real, double imag, int iter = 0 );
};

/**
* @class RenderManowarJulia
* @brief Threaded ManowarJulia rendering routines.
*/
class RenderManowarJulia : public RenderFractal
{
public:
	RenderManowarJulia();
	void Render();
	void SpecialRender();
};

/**
* @class ManowarJulia
* @brief Handles the RenderManowarJulia threads.
*/
class ManowarJulia : public Fractal
{
	RenderManowarJulia *myRender;
public:
	void Render();
	ManowarJulia( sf::RenderWindow *Window );
	ManowarJulia( int width, int height );
	~ManowarJulia();
	void ChangeThreadNumber();
	void DrawOrbit();
	wxString AskInfo();
	wxString AskInfo( double real, double imag, int iter = 0 );
};

/**
* @class RenderMedusa
* @brief Threaded Medusa rendering routines.
*/
class RenderMedusa : public RenderFractal
{
public:
	RenderMedusa();
	void Render();
	void SpecialRender();
};

/**
* @class Medusa
* @brief Handles the RenderMedusa threads.
*/
class Medusa : public Fractal
{
	RenderMedusa *myRender;
public:
	void Render();
	Medusa( sf::RenderWindow *Window );
	Medusa( int width, int height );
	~Medusa();
	void ChangeThreadNumber();
	void DrawOrbit();
	wxString AskInfo();
	wxString AskInfo( double real, double imag, int iter = 0 );
};

/**
* @class RenderSierpTriangle
* @brief Threaded SierpTriangle rendering routines.
*/
class RenderSierpTriangle : public RenderFractal
{
public:
	RenderSierpTriangle();
	void Render();
};

/**
* @class SierpTriangle
* @brief Renders a Sierpinsky triangle.
*/
class SierpTriangle : public Fractal
{
	RenderSierpTriangle *myRender;

public:
	void Render();
	SierpTriangle( sf::RenderWindow *Window );
	SierpTriangle( int width, int height );
	wxString AskInfo();
};

// FixedPoint1 z = sin(z)
/**
* @class RenderFixedPoint1
* @brief Threaded FixedPoint1 rendering routines.
*/
class RenderFixedPoint1 : public RenderFractal
{
	double minStep;
public:
	RenderFixedPoint1();
	void Render();
	void SetParams(double _minStep);
};

/**
* @class FixedPoint1
* @brief Handles the RenderFixedPoint1 threads.
*/
class FixedPoint1 : public Fractal
{
	RenderFixedPoint1 *myRender;
	double minStep;
public:
	void Render();
	FixedPoint1( sf::RenderWindow *Window );
	FixedPoint1( int width, int height );
	~FixedPoint1();
	void ChangeThreadNumber();
	void DrawOrbit();
	wxString AskInfo();
	wxString AskInfo( double real, double imag, int iter = 0 );
	void SpecialSaveRenderJob(std::ofstream &dataFile);
	void CopyOptFromPanel();
};

// FixedPoint2 z = cos(z)
/**
* @class RenderFixedPoint2
* @brief Threaded FixedPoint2 rendering routines.
*/
class RenderFixedPoint2 : public RenderFractal
{
	double minStep;
public:
	RenderFixedPoint2();
	void Render();
	void SetParams(double _minStep);
};

/**
* @class FixedPoint2
* @brief Handles the RenderFixedPoint2 threads.
*/
class FixedPoint2 : public Fractal
{
	RenderFixedPoint2 *myRender;
	double minStep;
public:
	void Render();
	FixedPoint2( sf::RenderWindow *Window );
	FixedPoint2( int width, int height );
	~FixedPoint2();
	void ChangeThreadNumber();
	void DrawOrbit();
	wxString AskInfo();
	wxString AskInfo( double real, double imag, int iter = 0 );
	void SpecialSaveRenderJob(std::ofstream &dataFile);
	void CopyOptFromPanel();
};

// FixedPoint3 z = tan(z)
/**
* @class RenderFixedPoint3
* @brief Threaded FixedPoint3 rendering routines.
*/
class RenderFixedPoint3 : public RenderFractal
{
	double minStep;
public:
	RenderFixedPoint3();
	void Render();
	void SetParams(double _minStep);
};

/**
* @class FixedPoint3
* @brief Handles the RenderFixedPoint3 threads.
*/
class FixedPoint3 : public Fractal
{
	RenderFixedPoint3 *myRender;
	double minStep;
public:
	void Render();
	FixedPoint3( sf::RenderWindow *Window );
	FixedPoint3( int width, int height );
	~FixedPoint3();
	void ChangeThreadNumber();
	void DrawOrbit();
	wxString AskInfo();
	wxString AskInfo( double real, double imag, int iter = 0 );
	void SpecialSaveRenderJob(std::ofstream &dataFile);
	void CopyOptFromPanel();
};

// FixedPoint4 z = z^2
/**
* @class RenderFixedPoint4
* @brief Threaded FixedPoint4 rendering routines.
*/
class RenderFixedPoint4 : public RenderFractal
{
	double minStep;
public:
	RenderFixedPoint4();
	void Render();
	void SetParams(double _minStep);
};

/**
* @class FixedPoint4
* @brief Handles the RenderFixedPoint4 threads.
*/
class FixedPoint4 : public Fractal
{
	RenderFixedPoint4 *myRender;
	double minStep;
public:
	void Render();
	FixedPoint4( sf::RenderWindow *Window );
	FixedPoint4( int width, int height );
	~FixedPoint4();
	void ChangeThreadNumber();
	void DrawOrbit();
	wxString AskInfo();
	wxString AskInfo( double real, double imag, int iter = 0 );
	void SpecialSaveRenderJob(std::ofstream &dataFile);
	void CopyOptFromPanel();
};

/**
* @class RenderTricorn
* @brief Threaded Tricorn rendering routines.
*/
class RenderTricorn : public RenderFractal
{
public:
	RenderTricorn();
	void Render();
};

/**
* @class Tricorn
* @brief Handles the RenderTricorn threads.
*/
class Tricorn : public Fractal
{
	RenderTricorn *myRender;
public:
	Tricorn( sf::RenderWindow *Window );
	Tricorn( int width, int height );
	~Tricorn();
	void ChangeThreadNumber();
	void Render();
	void DrawOrbit();
	wxString AskInfo();
	wxString AskInfo( double real, double imag, int iter = 0 );
};

/**
* @class RenderBurningShip
* @brief Threaded BurningShip rendering routines.
*/
class RenderBurningShip : public RenderFractal
{
public:
	RenderBurningShip();
	void Render();
};

/**
* @class BurningShip
* @brief Handles the RenderBurningShip threads.
*/
class BurningShip : public Fractal
{
	RenderBurningShip *myRender;
public:
	BurningShip( sf::RenderWindow *Window );
	BurningShip( int width, int height );
	~BurningShip();
	void ChangeThreadNumber();
	void Render();
	void DrawOrbit();
	wxString AskInfo();
	wxString AskInfo( double real, double imag, int iter = 0 );
};

/**
* @class RenderBurningShipJulia
* @brief Threaded BurningShipJulia rendering routines.
*/
class RenderBurningShipJulia : public RenderFractal
{
public:
	RenderBurningShipJulia();
	void Render();
};

/**
* @class BurningShipJulia
* @brief Handles the RenderBurningShipJulia threads.
*/
class BurningShipJulia : public Fractal
{
	RenderBurningShipJulia *myRender;
public:
	BurningShipJulia( sf::RenderWindow *Window );
	BurningShipJulia( int width, int height );
	~BurningShipJulia();
	void ChangeThreadNumber();
	void Render();
	void DrawOrbit();
	wxString AskInfo();
	wxString AskInfo( double real, double imag, int iter = 0 );
};


/**
* @class RenderFractory
* @brief Threaded Fractory rendering routines.
*/
class RenderFractory : public RenderFractal
{
public:
	RenderFractory();
	void Render();
};

/**
* @class Fractory
* @brief Handles the RenderFractory threads.
*/
class Fractory : public Fractal
{
	RenderFractory *myRender;
public:
	Fractory( sf::RenderWindow *Window );
	Fractory( int width, int height );
	~Fractory();
	void ChangeThreadNumber();
	void Render();
	void DrawOrbit();
	wxString AskInfo();
	wxString AskInfo( double real, double imag, int iter = 0 );
};

/**
* @class RenderCell
* @brief Threaded Cell rendering routines.
*/
class RenderCell : public RenderFractal
{
	double bailout;
public:
	RenderCell();
	void Render();
	void SetParams(double _bailout);
};

/**
* @class Cell
* @brief Handles the RenderCell threads.
*/
class Cell : public Fractal
{
	RenderCell *myRender;
	double bailout;
public:
	Cell( sf::RenderWindow *Window );
	Cell( int width, int height );
	~Cell();
	void ChangeThreadNumber();
	void Render();
	void DrawOrbit();
	wxString AskInfo();
	wxString AskInfo( double real, double imag, int iter = 0 );
	void SpecialSaveRenderJob(std::ofstream &dataFile);
	void CopyOptFromPanel();
};

/**
* @class RenderLogisticMap
* @brief Render a logistic map.
*/

class RenderLogisticMap : public RenderFractal
{
	double seed;
	bool stabilizePoint;
	double progress;

public:
	RenderLogisticMap();
	void Render();
	void SetParams(double _seed, bool _stabilizePoint);
	int AskProgress();
};

/**
* @class LogisticMap
* @brief Handles the RenderLogisticMap threads.
*/
class LogisticMap : public Fractal
{
	RenderLogisticMap *myRender;
	double logisticSeed;
	bool stabilizePoint;

public:
	void Render();
	LogisticMap( sf::RenderWindow *Window );
	LogisticMap( int width, int height );
	wxString AskInfo();
	void CopyOptFromPanel();
};

/**
* @class RenderHenonMap
* @brief Render a Henon map.
*/

class RenderHenonMap : public RenderFractal
{
	double alpha, beta;
	double x0, y0;
	int i;
public:
	RenderHenonMap();
	void Render();
	void SetParams(double _alpha, double _beta, double _x0, double _y0);
	int AskProgress();
};

/**
* @class HenonMap
* @brief Handles the RenderHenonMap threads.
*/

class HenonMap : public Fractal
{
	RenderHenonMap *myRender;
	double alpha, beta;
	double x0, y0;
public:
	HenonMap(sf::RenderWindow *Window);
	HenonMap(int width, int height);
	~HenonMap();
	void Render();
	wxString AskInfo();
	void CopyOptFromPanel();
	void MoreIter();
	void LessIter();
};

/**
* @class RenderDPendulum
* @brief Threaded DPendulum rendering routines.
*/
class RenderDPendulum : public RenderFractal
{
	bool th1Bailout, th2Bailout;
	double th1NumBailout, th2NumBailout;
	double m1, m2, l, g;
	double dt;
	bool referenced, rungeKutta;

public:
	RenderDPendulum();
	void Render();
	void SetParams(bool th1B, bool th2B, double th1NB, double th2NB,
					double _dt, double _m1, double _m2, double _l, double _g, bool ref, bool _rungeKutta);
};

/**
* @class DPendulum
* @brief Handles the RenderDPendulum threads.
*/
class DPendulum : public Fractal
{
	bool th1Bailout, th2Bailout;
	double th1NumBailout, th2NumBailout;
	double m1, m2, l, g;
	double dt;
	bool referenced;
	bool rungeKutta;
	RenderDPendulum *myRender;
public:
	void Render();
	DPendulum( sf::RenderWindow *Window );
	DPendulum( int width, int height );
	~DPendulum();
	void ChangeThreadNumber();
	wxString AskInfo();
	wxString AskInfo( double real, double imag, int iter = 0 );
	void DrawOrbit();
	void SpecialSaveRenderJob(std::ofstream &dataFile);
	void CopyOptFromPanel();
	void MoreIter();
	void LessIter();
	void SpecialSaveRoutine(string filename);
};

/**
* @class RenderUserDefined
* @brief Threaded UserDefined rendering routines.
*/
class RenderUserDefined : public RenderFractal
{
	int bailout;
	bool julia;
	wxString errorInfo;
	wxString parserFormula;

public:
	RenderUserDefined();
	void SetFormula( FormulaOpt formula );
	void Render();
	wxString GetErrorInfo();
	void ClearErrorInfo();
	bool IsThereError();
};

/**
* @class UserDefined
* @brief Handles the RenderUserDefined threads.
*/
class UserDefined : public Fractal
{
	RenderUserDefined* myRender;
public:
	UserDefined( sf::RenderWindow* Window );
	UserDefined( int width, int height );
	~UserDefined();
	void ChangeThreadNumber();
	void Render();
	void SetFormula( FormulaOpt formula );		///< Sets user formula.
	wxString AskInfo();
	wxString AskInfo( double real, double imag, int iter = 0 );
	wxString SaveOrbit(double real, double imag, int iter, wxString filepath);
	void DrawOrbit();
	void PostRender();
};

/**
* @class RenderFPUserDefined
* @brief Threaded FPUserDefined rendering routines.
*/
class RenderFPUserDefined : public RenderFractal
{
	int bailout;
	bool julia;
	double minStep;
	wxString errorInfo;
	wxString parserFormula;

public:
	RenderFPUserDefined();
	void SetFormula( FormulaOpt formula );
	void Render();
	void SetParams(double _minStep);
	wxString GetErrorInfo();
	void ClearErrorInfo();
	bool IsThereError();
};

/**
* @class FPUserDefined
* @brief Handles the RenderUserDefined threads.
*/
class FPUserDefined : public Fractal
{
	RenderFPUserDefined* myRender;
	double minStep;
public:
	FPUserDefined( sf::RenderWindow* Window );
	FPUserDefined( int width, int height );
	~FPUserDefined();
	void ChangeThreadNumber();
	void Render();
	void SetFormula( FormulaOpt formula );		///< Sets user formula.
	wxString AskInfo();
	wxString AskInfo( double real, double imag, int iter = 0 );
	wxString SaveOrbit(double real, double imag, int iter, wxString filepath);
	void DrawOrbit();
	void SpecialSaveRenderJob(std::ofstream &dataFile);
	void CopyOptFromPanel();
	void PostRender();
};

/**
* @class RenderScriptFractal
* @brief Threaded ScriptFractal rendering routines.
*/
class RenderScriptFractal : public RenderFractal
{
	asIScriptEngine *engine;
	asIScriptContext *ctx;
	string path;
	wxString errorInfo;
	int threadIndex;
	bool hasEnded;

public:
	void Render();
	RenderScriptFractal();
	~RenderScriptFractal();
	void SetPath(string scriptPath);
	wxString GetErrorInfo();
	void ClearErrorInfo();
	void PreTerminate();
	bool IsThereError();
	void SetParams(int _threadIndex);
};

/**
* @class ScriptFractal
* @brief Handles the RenderScriptdFractal threads.
*/
class ScriptFractal : public Fractal
{
	string path;
	ScriptData myScriptData;
	RenderScriptFractal *myRender;
public:
	void Render();
	ScriptFractal(sf::RenderWindow *Window, ScriptData scriptData);
	ScriptFractal(int width, int height, ScriptData scriptData);
	ScriptFractal(int width, int height, string scriptPath);
	void ChangeThreadNumber();
	void PostRender();
	void PreRestartRender();
	wxString AskInfo();
	~ScriptFractal();
};

/**
* @class FractalHandler
* @brief Handles the fractal memory management.
*/
class FractalHandler
{
	Mandelbrot *mandelbrot;
	MandelbrotZN *mandelbrotZN;
	Julia *julia;
	JuliaZN *juliaZN;
	Newton *newton;
	Sinoidal *sinoidal;
	Magnet *magnet;
	Medusa *medusa;
	Manowar *manowar;
	ManowarJulia *manowarJulia;
	SierpTriangle *triangulo;
	FixedPoint1 *fixedPoint1;
	FixedPoint2 *fixedPoint2;
	FixedPoint3 *fixedPoint3;
	FixedPoint4 *fixedPoint4;
	Tricorn *tricorn;
	BurningShip *burningShip;
	BurningShipJulia *burningShipJulia;
	Fractory* fractory;
	Cell* cell;
	LogisticMap *logisticMap;
	HenonMap *henonMap;
	DPendulum *dPendulum;
	UserDefined *userDefined;
	FPUserDefined *fpUserDefined;
	ScriptFractal *scriptFractal;

	Fractal *target;
	FRACTAL_TYPE type;

public:
	FractalHandler();
	~FractalHandler();
	void CreateFractal(FRACTAL_TYPE _type, sf::RenderWindow *Window);
	void CreateFractal(FRACTAL_TYPE _type, int width, int height);
	void CreateScriptFractal(sf::RenderWindow *Window, ScriptData scriptData);
	void CreateScriptFractal(int width, int height, ScriptData scriptData);
	void CreateScriptFractal(int width, int height, string scriptPath);
	void SetFormula(FormulaOpt formula);
	void DeleteFractal();
	Fractal* GetTarget();
	FRACTAL_TYPE GetType();
};

#endif