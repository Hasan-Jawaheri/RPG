#ifndef _INTRO_H_
#define _INTRO_H_

#include "..//Common.h"

#define NUMLOGOS 2
#define INTROSPEED 500.0f

class Intro : public hxGameState {
	
	hxImage* logos[NUMLOGOS];	//an array of images to be displayed
	hxSprite* curLogo;			//a sprite that displays the currently shown logo
	unsigned int curLogoIndex;	//index of the current logo
	float curLogoAlpha;			//alpha value of the current logo
	bool bFadingIn;				//fading in or out?

public:
	Intro ( HasX11* a );
	~Intro ( void );

	void Load ( void );
	void Update ( float fDeltaTime );
	void Cleanup ( void );
};

#endif //_INTRO_H_
