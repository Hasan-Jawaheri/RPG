#include "Edit Box.h"

DWORD EditBox::dwCursorTimer = 0;
EditBox::EditBox ( void ) : UIElement ( )
{
	bCursor = false;
}
EditBox::~EditBox ( void )
{
}
void EditBox::Load ( hxCore* const core )
{
	shader = new hxPixelShader ( core );
	shader->LoadFromFile ( "Data\\ChatInsShader.HLSL", HX_MAKEPROFILE ( 4, 0 ), "PS" );

	sprite = new hxSprite ( 0, core );
	sprite->GetMaterial()->SetImage ( nullptr );
	sprite->GetMaterial()->SetPixelShader ( shader );
	sprite->GetMaterial()->SetConstantBufferVariableFloat ( "gAlpha", 0.0f );
	sprite->SetPriority ( 0 );
}
void EditBox::OnResize ( UINT width, UINT height )
{
}
bool EditBox::Update ( float fDeltaTime )
{
	if ( UserInterface::GetFocus ( ) == this )
	{
		DWORD curTime = timeGetTime ( );
		if ( dwCursorTimer + 500 < curTime )
		{
			if ( bCursor )
			{
				inputBuffer[strlen(inputBuffer)-1] = '\0';
				bCursor = false;
			}
			else
			{
				sprintf_s ( inputBuffer, 512, "%s%c", inputBuffer, '|' );
				bCursor = true;
			}
			dwCursorTimer = curTime;
		}
	}
	else if ( bCursor )
	{
		inputBuffer[strlen(inputBuffer)-1] = '\0';
		bCursor = false;
	}

	hxCore* core = sprite->GetCorePtr ( );
	int printX = sprite->GetPositionX ( ) + 2;
	int printY = sprite->GetPositionY ( ) - 2;

	UINT numCharsMax = sprite->GetSizeX ( ) / 8;
	char* outputBuffer = new char[numCharsMax+1];
	outputBuffer[numCharsMax] = '\0';
	if ( strlen ( inputBuffer ) > numCharsMax )
	{
		if ( IsPassword ( ) )
			for ( UINT i = 0; i < numCharsMax; i++ )
				outputBuffer[i] = '*';
		else
			memcpy ( outputBuffer, inputBuffer + (strlen(inputBuffer)-numCharsMax), numCharsMax );
	}
	else
	{
		if ( IsPassword ( ) )
		{
			for ( UINT i = 0; i < strlen ( inputBuffer ); i++ )
				outputBuffer[i] = '*';
			outputBuffer[strlen(inputBuffer)] = '\0';
			if ( bCursor )
				outputBuffer[strlen(inputBuffer)-1] = '|';
		}
		else
			strcpy ( outputBuffer, inputBuffer );
	}
	core->Renderer->RenderText ( outputBuffer, printX, printY, FONT_CONSOLAS_16, hxColor ( 0.45, 0.6, 1.0, fAlpha ) );
	delete[] outputBuffer;

	return true;
}
void EditBox::SetText ( LPCSTR text )
{
	if ( bCursor ) //always remove cursor when the user tries to do it himself
	{
		inputBuffer[strlen(inputBuffer)-1] = '\0';
		bCursor = false;
	}

	strcpy_s ( inputBuffer, 512, text );
}
void EditBox::GetText ( LPSTR text, UINT ccMax )
{
	if ( bCursor ) //always remove cursor when the user tries to do it himself
	{
		inputBuffer[strlen(inputBuffer)-1] = '\0';
		bCursor = false;
	}

	strcpy_s ( text, min ( ccMax, 512 ), inputBuffer );
}
bool EditBox::OnInput ( void )
{
	if ( bCursor )
	{
		inputBuffer[strlen(inputBuffer)-1] = '\0';
		bCursor = false;
		dwCursorTimer = timeGetTime ( ) + 500;
	}

	return true;
}