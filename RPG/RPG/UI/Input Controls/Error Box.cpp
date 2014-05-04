#include "Error Box.h"

ErrorBox::ErrorBox ( LPCSTR error )
{
	errorMsg = (LPSTR)HX_SAFE_ALLOC ( strlen ( error ) + 1 );
	strcpy ( errorMsg, error );
	bClick = false;
}
void ErrorBox::Load ( hxCore* const core )
{
	shader = new hxPixelShader ( core );
	shader->LoadFromFile ( "Data\\ErrorShader.HLSL", HX_MAKEPROFILE ( 4, 0 ), "PS" );
	shader->SetConstantBufferVariableFloat ( "gAlpha", 1.0f );

	sprite = new hxSprite ( 0, core );
	sprite->GetMaterial()->SetImage ( nullptr );
	sprite->GetMaterial()->SetPixelShader ( shader );
	sprite->SetPriority ( 0 );

	SetSize ( 400, 200 );
	OnResize ( core->WindowsDevice->GetWindowWidth ( ), core->WindowsDevice->GetWindowHeight ( ) );
}
void ErrorBox::OnResize ( UINT width, UINT height )
{
	SetPosition ( width / 2 - 200, height / 2 - 100 );
}
bool ErrorBox::Update ( float fDeltaTime )
{
	int x = sprite->GetPositionX ( );
	int y = sprite->GetPositionY ( );
	if ( GetNumChildren ( ) )
		GetChild(0)->SetPosition ( x + sprite->GetSizeX ( ) / 2 - 150, y + sprite->GetSizeY ( ) / 2 + 50 );

	sprite->GetCorePtr()->Renderer->RenderText ( errorMsg, x + 20, y + 20, 3, hxColor ( 255, 0, 0, 255 ) );
	return !bClick;
}