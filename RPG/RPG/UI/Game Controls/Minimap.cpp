#include "Minimap.h"

Minimap::Minimap ( void ) : UIElement ( )
{
	rt = nullptr;
	rtImg = nullptr;
}
Minimap::~Minimap ( void )
{
	HX_SAFE_REMOVEREF ( rt );
	HX_SAFE_REMOVEREF ( rtImg );
}
void Minimap::Load ( hxCore* const core )
{
	fAlpha = 255.0f;

	rt = new hxRenderTarget ( 0, core );
	rt->AddRenderTarget ( 150, 150 );
	rt->SetClearColor ( 127, 127, 127 );
	rtImg = new hxImage ( 0, core );
	rtImg->BindToRenderTarget ( rt );

	mapCam = new hxCamera ( 0, core );

	sprite = new hxSprite ( 0, core );
	sprite->GetMaterial()->SetImage ( rtImg );
	sprite->SetPriority ( 0 );
}
bool Minimap::Update ( float fDeltaTime )
{
	rt->GetCorePtr()->WindowsDevice->CreateViewport ( 1, 0, 0, 150, 150 );
	rt->GetCorePtr()->WindowsDevice->SetViewportCamera ( 1, mapCam );
	rt->GetCorePtr()->WindowsDevice->SetCurrentViewportID ( 1 );
	rt->Begin ( );
	rt->End ( );
	rt->GetCorePtr()->WindowsDevice->DeleteViewport ( 1 );

	return true;
}