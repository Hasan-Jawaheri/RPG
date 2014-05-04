#include "Intro.h"
#include "Menu.h"

Intro::Intro ( HasX11* a ) : hxGameState ( a ) {
}

Intro::~Intro ( void ) {
}

void Intro::Load ( void ) {

	core->Renderer->GetPrimaryRenderTarget()->SetClearColor ( 0, 0, 0 );
	
	const char* logoFilenames[NUMLOGOS] = {
		"Media\\havok_logo.jpg",
		"Media\\HasX11 Engine.png"
	};

	//load images using the above filenames
	for ( unsigned int i = 0; i < NUMLOGOS; i ++ ) {
		logos[i] = new hxImage ( 0, core ); //allocate a new image
		logos[i]->Load ( logoFilenames[i] ); //load the image from file
	}

	//setup the sprite to display the current logo
	curLogo = new hxSprite ( 0, core );

	//now set the initial values to start showing the logos
	curLogoIndex = 0; //start with the first image, index 0
	bFadingIn = true;
	curLogoAlpha = -100.0f; //fade in - start with -100 alpha to idle for a while
}

void Intro::Update ( float fDeltaTime ) {
	//
	//The concept behind fDeltaTime is the following:
	//Sometimes the FPS is not constant. Which means FPS could be 60 (60 frames
	//rendered each second, 60 calls to Update), and then it drops to 30 (30 calls
	//to Update only). So if we increment values in the loop by a constant, lets
	//say 5, the value will be incremented by 60*5=300 every second under 60 FPS,
	//while, when the FPS drops for whatever reason, the value will increment by
	//30*5=150 every second under 30 FPS. In other words, we get different outcomes
	//depending on the FPS. So we introduce the concept of fDeltaTime which is
	//calculated such that if you multiply it by the increment amount, lets say
	//5*fDeltaTime, then the value will increment by ~5 every second under any FPS.
	//Notice fDeltaTime is not very accurate but it provides a decent simple solution.
	//

	//We allow the alpha to range from 0 to 350 although the maximum alpha (fully opaque
	//image) is 255 because we want the image to stay at full alpha for a while
	//and to be specific it will remain at full alpha for (350-255)//100 seconds.
	//But when we set the alpha value to the sprite we cap it at 255 so the range
	//255 to 350 is always given to the sprite as 255, but we just use it for the delay
	//Fading at the rate of INTROSPEED alpha per second
	if ( bFadingIn ) //fading in
	{
		curLogoAlpha += INTROSPEED * fDeltaTime;
		if ( curLogoAlpha > 350.0f ) //excceeded maximum alpha, now fade out
		{
			curLogoAlpha = 255.0f; //start fading out from 255
			bFadingIn = false; //start fading out
		}
	}
	else //fading out
	{
		curLogoAlpha -= INTROSPEED * fDeltaTime;
		float fMinAlpha = 0.0f;
		if ( curLogoAlpha < fMinAlpha ) //less than the minimum, now fade in a new image
		{
			curLogoAlpha = 0.0f;
			bFadingIn = true;
			curLogoIndex++; //jump to the new logo
			if ( curLogoIndex >= NUMLOGOS ) //no more logos! switch to the game state!
			{
				app->SwitchState ( new Menu ( app ) );
				return; //dont execute any further, we're done.
			}
		}
	}
		
	//
	//Set the sprite image and alpha as calculated above.
	//We don't need to change the image every fame (it is not the most efficient way..)
	//instead we can only switch at the beginning and when we increment curLogoIndex.
	//However it is being done like this for simplicity.
	//
	curLogo->GetMaterial()->SetImage ( logos[curLogoIndex] );
	//alpha ranges from -100 to 350 so cap it at 255 because its the maximum alpha
	//and 0 because it's the minimum
	curLogo->GetMaterial()->SetAlpha ( (BYTE)max ( min ( curLogoAlpha, 255 ), 0 ) );

	//position the sprite in the center of the screen
	int windowWidth = core->WindowsDevice->GetWindowWidth ( );
	int windowHeight = core->WindowsDevice->GetWindowHeight ( );
	int imageWidth = logos[curLogoIndex]->GetWidth ( );
	int imageHeight = logos[curLogoIndex]->GetHeight ( );
	float xpos = windowWidth / 2 - imageWidth / 2;
	float ypos = windowHeight / 2 - imageHeight / 2;
	curLogo->SetPosition ( xpos, ypos );
	curLogo->SetSize ( logos[curLogoIndex]->GetWidth ( ),
								logos[curLogoIndex]->GetHeight ( ) );
}

void Intro::Cleanup ( void ) {
	//freeing resources with HX_SAFE_REMOVEREF
	for ( unsigned int i = 0; i < NUMLOGOS; i++ )
		HX_SAFE_REMOVEREF ( logos[i] );
	
	HX_SAFE_REMOVEREF ( curLogo );
}