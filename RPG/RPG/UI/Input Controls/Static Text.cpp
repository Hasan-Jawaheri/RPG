#include "Static Text.h"

StaticText::StaticText ( LPCSTR _text ) : UIElement ( )
{
	text = _text;
}
StaticText::~StaticText ( void )
{
}
void StaticText::Load ( hxCore* const core )
{
	sprite = new hxSprite ( 0, core );
}
bool StaticText::Update ( float fDeltaTime )
{
	sprite->GetCorePtr()->Renderer->RenderText ( text, sprite->GetPositionX ( ), sprite->GetPositionY ( ), fAlpha, 3 );
	return true;
}