#ifndef _MAIN_H_ //header guard
#define _MAIN_H_

#include <HasX11_vs12.h>
#include <fstream>
#include <fbxsdk.h>
#pragma comment ( lib, "libfbxsdk-mt.lib" )
using std::fstream;

//******************************************************************************************
//This is the main class that inherits from HasX11. This is the "container" of the game
//******************************************************************************************
class Application : public HasX11 {

public:
	Application ( void ); //constructor
	
	//***************************************************************************************
	//This is called once at the beginning of the application asking it to initialize
	//***************************************************************************************
	bool Setup ( void );
	
	//***************************************************************************************
	//This is called every frame asking the application to update
	//This occurs before rendering and rendering should NOT be done here
	//***************************************************************************************
	bool Loop ( float fDeltaTime );

	//***************************************************************************************
	//This is called once at the end of the program if it passes the Setup function
	//***************************************************************************************
	void Cleanup ( void );
};

#endif //_MAIN_H_