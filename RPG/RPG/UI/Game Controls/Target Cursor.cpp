#include "Target Cursor.h"

TargetCursor::TargetCursor ( void ) : UIElement ( )
{
	img = nullptr;
}
TargetCursor::~TargetCursor ( void )
{
	HX_SAFE_REMOVEREF ( img );
}
void TargetCursor::Load ( hxCore* const core )
{
	fAlpha = 255.0f;

	img = new hxImage ( 0, core );
	img->Load ( "Data\\Target Cursor.png" );

	sprite = new hxSprite ( 0, core );
	sprite->GetMaterial()->SetImage ( img );
}
bool TargetCursor::Update ( float fDeltaTime )
{
	return true;
}