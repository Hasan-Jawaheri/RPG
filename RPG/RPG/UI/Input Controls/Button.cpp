#include "Button.h"

MenuButton::MenuButton ( int o1, int o2, LPCSTR _text )
{
	offsets[0] = o1;
	offsets[1] = o2;
	text = _text;
	bClicked = false;
}
MenuButton::~MenuButton ( void )
{
}
void MenuButton::Load ( hxCore* const core )
{
	shader = new hxPixelShader ( core );
	shader->LoadFromFile ( "Data\\MenuShader.HLSL", HX_MAKEPROFILE ( 4, 0 ), "PS" );
	fAlpha = 255.0f;
	fAlphaOffset = 0.5f;

	sprite = new hxSprite ( 0, core );
	sprite->GetMaterial()->SetImage ( nullptr );
	sprite->GetMaterial()->SetPixelShader ( shader );
	sprite->GetMaterial()->SetConstantBufferVariableFloat ( "gAlpha", 0.0f );
	sprite->SetPriority ( 0 );
	sprite->SetSize ( 300, 40 );
}
void MenuButton::SetFade ( float fFade )
{
	sprite->GetMaterial()->SetConstantBufferVariableFloat ( "gAlpha", max ( (fFade-fAlphaOffset), 0 ) );
	fAlpha = fFade * 255.0f;
}
void MenuButton::OnResize ( UINT width, UINT height )
{
}
bool MenuButton::Update ( float fDeltaTime )
{
	hxCore* core = sprite->GetCorePtr ( );

	if ( core->InputDevice->MouseX ( ) > sprite->GetPositionX ( ) && core->InputDevice->MouseY ( ) > sprite->GetPositionY ( ) &&
		core->InputDevice->MouseX ( ) < sprite->GetPositionX ( ) + sprite->GetSizeX ( ) &&
		core->InputDevice->MouseY ( ) < sprite->GetPositionY ( ) + sprite->GetSizeY ( ) )
	{
		if ( fAlphaOffset > 0.15f )
			fAlphaOffset -= 5.0f * fDeltaTime;
		if ( core->InputDevice->MouseClick ( MOUSE_LEFT ) )
		{
			if ( fAlphaOffset > 0.0f )
				fAlphaOffset -= 5.0f * fDeltaTime;
		}
		else if ( fAlphaOffset < 0.15f )
			fAlphaOffset += 5.0f * fDeltaTime;
		if ( fAlphaOffset < 0.0f )
			fAlphaOffset = 0.0f;
		if ( fAlphaOffset > 0.3f )
			fAlphaOffset = 0.3f;
	}
	else if ( fAlphaOffset < 0.3f )
		fAlphaOffset += fAlphaOffset / 15.0f;
	
	sprite->GetMaterial()->SetConstantBufferVariableFloat ( "gAlpha", max ( fAlpha / 255.0f - fAlphaOffset, 0 ) );

	core->Renderer->RenderText ( text, sprite->GetPositionX ( ) + offsets[0], sprite->GetPositionY ( ) + offsets[1], max ( fAlpha - fAlphaOffset * 255.0f, 0 ), 1 );

	return true;
}
void MenuButton::OnMouseButton ( int mx, int my, bool bDown )
{
	if ( !bDown )
	{
		bClicked = true;
		if ( GetParent ( ) )
			GetParent()->OnEnter ( ); //send ENTER message to parent, the parent should do the OnClick of this
		else
			OnClick ( );
	}
}