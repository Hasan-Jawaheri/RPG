#include "Chat.h"

bool ChatEditBox::Update ( float fDeltaTime )
{
	if ( UserInterface::GetFocus ( ) == this )
		sprite->GetMaterial()->SetConstantBufferVariableFloat ( "gAlpha", 1.0f );
	else
		sprite->GetMaterial()->SetConstantBufferVariableFloat ( "gAlpha", 0.3f );
	return EditBox::Update ( fDeltaTime );
}
bool ChatEditBox::OnEscape ( void )
{
	strcpy_s ( inputBuffer, 512, "" );
	if ( GetParent ( ) )
		UserInterface::SetFocus ( GetParent ( ) );
	return false;
}

Chatbox::Chatbox ( void ) : UIElement ( )
{
}
Chatbox::~Chatbox ( void )
{
	for ( UINT i = 0; i < messageV.size ( ); i++ )
		delete messageV[i];
}
void Chatbox::Load ( hxCore* const core )
{
	shader = new hxPixelShader ( core );
	shader->LoadFromFile ( "Data\\ChatboxShader.HLSL", HX_MAKEPROFILE ( 4, 0 ), "PS" );
	
	sprite = new hxSprite ( 0, core );
	sprite->GetMaterial()->SetImage ( nullptr );
	sprite->GetMaterial()->SetPixelShader ( shader );
	sprite->GetMaterial()->SetConstantBufferVariableFloat ( "gAlpha", 0.0f );
	sprite->SetPriority ( 0 );
	SetSize ( 250, 120 );
}
void Chatbox::OnResize ( UINT width, UINT height )
{
}
bool Chatbox::Update ( float fDeltaTime )
{
	hxCore* core = sprite->GetCorePtr ( );
	int printX = sprite->GetPositionX ( ) + 1;
	int printY = sprite->GetPositionY ( ) - 5 + sprite->GetSizeY ( ) - 20;
	
	for ( UINT i = messageV.size ( ) - 1; i >= 0 && i != UINT_MAX; i-- )
	{
		char msg[256];
		sprintf_s ( msg, "[%s]: %s", messageV[i]->sender, messageV[i]->message );
		core->Renderer->RenderText ( msg, printX, printY, 3, hxColor ( 120, 170, 250, fAlpha ) );

		printY -= 17;
		if ( printY <= sprite->GetPositionY ( ) )
			break;
	}

	return true;
}
void Chatbox::AddMessage ( LPCSTR message, LPCSTR owner )
{
	messageV.push_back ( new Message ( owner, message ) );
}