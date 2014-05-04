#include "Action Bar.h"
#include "Cast Bar.h"

Action::Action ( hxCore* const core, hxImage* icon, hxPixelShader* shader )
{
	spr = new hxSprite ( 0, core );
	spr->GetMaterial()->SetPixelShader ( shader );
	spr->GetMaterial()->SetImage ( icon );
	spr->SetPriority ( 1 );
	img = icon;
	fCooldown = 0.0f;
}
Action::~Action ( void )
{
	HX_SAFE_REMOVEREF ( spr );
}
void Action::Update ( float fDeltaTime )
{
	fCooldown = 1 - GetCooldownPercentage ( );

	spr->GetMaterial()->SetConstantBufferVariableFloat ( "gCast", fCooldown );

	if ( fCooldown <= 1.0f )
	{
		float fCosAng;
		if ( fCooldown < 0.5f )
			fCosAng = 2 - (cosf ( HX_PI * fCooldown * 2.0f ) + 1.0f);
		else
			fCosAng = 2 - (cosf ( HX_PI * (fCooldown-0.5f) * 2.0f ) + 1) + 10.0f;

		spr->GetMaterial()->SetConstantBufferVariableFloat ( "gCosAngle", fCosAng );
	}
}
ActionButton::ActionButton ( hxImage* bkg, hxImage* holderImg )
{
	action = nullptr;
	sprite = new hxSprite ( 0, bkg->GetCorePtr ( ) );
	sprite->GetMaterial()->SetImage ( bkg );
	sprite->SetPriority ( 0 );
	holder = new hxSprite ( 0, holderImg->GetCorePtr ( ) );
	holder->GetMaterial()->SetImage ( holderImg );
	holder->SetPriority ( 3 );
	bLocked = false;
	fHolderAlpha = 0.0f;
	initClickX = -1;
}
ActionButton::~ActionButton ( void )
{
	HX_SAFE_REMOVEREF ( sprite );
	HX_SAFE_REMOVEREF ( holder );
}
void ActionButton::Update ( float fDeltaTime )
{
	bMagnify = false;
	if ( action )
	{
		if ( initClickX != -1 )
		{
			int mx = sprite->GetCorePtr()->InputDevice->MouseX ( );
			int my = sprite->GetCorePtr()->InputDevice->MouseY ( );
			if ( (abs ( initClickX - mx) > 5 || abs ( initClickY - my ) > 5) && !bLocked )
			{
				initClickX = initClickY = -1;
				action->GetSprite()->SetSize ( 32, 32 );
				UserInterface::Drag ( action );
				action = nullptr;
			}
		}
	}
	else if ( UserInterface::IsDragging ( ) ) //if something is dragged on top of us, show the holder magnified
	{
		int mx = sprite->GetCorePtr()->InputDevice->MouseX ( );
		int my = sprite->GetCorePtr()->InputDevice->MouseY ( );
		if ( IsHighlighted ( mx, my, nullptr ) )
		{
			bMagnify = true;
		}
	}

	if ( bMagnify )
	{
		holder->Show ( );
		fHolderAlpha += 2500.0f * fDeltaTime;
		if ( fHolderAlpha > 255.0f )
			fHolderAlpha = 255.0f;
	}
	else
	{
		fHolderAlpha -= 1500.0f * fDeltaTime;
		if ( fHolderAlpha < 0.0f )
		{
			if ( !action )
				holder->Hide ( );
			fHolderAlpha = 0.0f;
		}
	}
	
	float fSize = 40 + (10.0f*fHolderAlpha/255.0f);
	holder->SetSize ( fSize, fSize );
	holder->SetPosition ( sprite->GetPositionX ( ) - (fSize-40.0f) / 2.0f, sprite->GetPositionY ( ) - (fSize-40.0f) / 2.0f );
	if ( !action )
		holder->GetMaterial()->SetAlpha ( fHolderAlpha );
	else
	{
		holder->GetMaterial()->SetAlpha ( 255.0f );
		action->Update ( fDeltaTime );
	}
}
void ActionButton::SetPosition ( int x, int y )
{
	sprite->SetPosition ( x, y );
	holder->SetPosition ( x, y );
	if ( action )
		action->GetSprite()->SetPosition ( sprite->GetPositionX ( ) + 4, sprite->GetPositionY ( ) + 4 );
}
void ActionButton::SetAlpha ( BYTE alpha )
{
	sprite->GetMaterial()->SetAlpha ( alpha );
	holder->GetMaterial()->SetAlpha ( alpha );
	if ( action )
		action->GetSprite()->GetMaterial()->SetAlpha ( alpha );
}
void ActionButton::SetAction ( Action* act )
{
	if ( action )
		UserInterface::Drag ( action );
	action = act;
	action->GetSprite()->SetPosition ( sprite->GetPositionX ( ) + 4, sprite->GetPositionY ( ) + 4 );
}
void ActionButton::DoAction ( CastBar* cb, UINT targetGUID )
{
	if ( action )
	{
		action->GetSprite()->SetSize ( 32, 32 );
		action->GetSprite()->SetPosition ( sprite->GetPositionX ( ) + 4, sprite->GetPositionY ( ) + 4 );
		if ( action->GetCooldownPercentage ( ) < 0.0001f )
		{
			/*if ( !cb->Cast ( action->GetCastTime ( ), action, targetGUID ) )
			{
				//UNABLE TO CAST
			}
			else
			{
				//started casting, tell the server so we know if we have to stop for some reason
				/*char* mem[4];
				UINT id = action->GetID ( );
				memcpy ( mem, (char*)&id, 4 );
				NetworkManager::AddPermissionParam ( mem, 4 );
				memcpy ( mem, (char*)&targetGUID, 4 );
				NetworkManager::AddPermissionParam ( mem, 4 );
				NetworkManager::RequestPermissions ( NETCODE_ACTION, NETCODE_ACTION_CASTBEG );*
			}*/
		}
		else
		{
			//COOLDOWN
		}
	}
}
void ActionButton::SetInitialClick ( int mx, int my )
{
	if ( action )
	{
		initClickX = mx;
		initClickY = my;
		action->GetSprite()->SetSize ( 28, 28 );
		action->GetSprite()->SetPosition ( sprite->GetPositionX ( ) + 6, sprite->GetPositionY ( ) + 6 );
	}
}
void ActionButton::SetFinalClick ( int mx, int my, CastBar* cb, UINT targetGUID )
{
	initClickX = initClickY = -1;
	DoAction ( cb, targetGUID );
}
void ActionButton::Lock ( void )
{
	bLocked = true;
}
void ActionButton::Unlock ( void )
{
	bLocked = false;
}
bool ActionButton::IsHighlighted ( int mx, int my, hxSprite* highlight )
{
	int posX = sprite->GetPositionX ( );
	int posY = sprite->GetPositionY ( );
	int sizeX = sprite->GetSizeX ( );
	int sizeY = sprite->GetSizeY ( );

	if ( mx > posX && mx < posX + sizeX && my > posY && my < posY + sizeY )
	{
		if ( highlight )
			highlight->SetPosition ( posX, posY );
		return true;
	}

	return false;
}
ActionBar::ActionBar ( float fWidthRatio )
{
	fWidth = fWidthRatio;
	castBar = nullptr;
	targetGUID = 0;
}
ActionBar::~ActionBar ( void )
{
	HX_SAFE_REMOVEREF ( actionButtonImg );
	HX_SAFE_REMOVEREF ( actionButtonHImg );
	HX_SAFE_REMOVEREF ( actionHolderImage );
	HX_SAFE_REMOVEREF ( highlight );
	for ( UINT i = 0; i < 10; i++ )
		HX_SAFE_DELETE ( actionButton[i] );
}
void ActionBar::Load ( hxCore* const core )
{
	shader = new hxPixelShader ( core );
	shader->LoadFromFile ( "Data\\ActionBarShader.HLSL", HX_MAKEPROFILE ( 4, 0 ), "PS" );

	sprite = new hxSprite ( 0, core );
	sprite->GetMaterial()->SetImage ( nullptr );
	sprite->GetMaterial()->SetPixelShader ( shader );
	sprite->GetMaterial()->SetConstantBufferVariableFloat ( "gAlpha", 1.0f );
	sprite->SetPriority ( 4 );
	fAlpha = 255.0f;
	
	actionButtonImg = new hxImage ( 0, core );
	actionButtonImg->LoadFromHXI ( "Data\\ActionButton.HXI" );
	actionHolderImage = new hxImage ( 0, core );
	actionHolderImage->LoadFromHXI ( "Data\\ActionHolder.HXI" );
	for ( UINT i = 0; i < 10; i++ )
		actionButton[i] = new ActionButton ( actionButtonImg, actionHolderImage );
	hxImage* img = new hxImage ( 0, core );
	img->LoadFromHXI ( "Data\\testspell.HXI" );
	
	actionButtonHImg = new hxImage ( 0, core );
	actionButtonHImg->LoadFromHXI ( "Data\\ActionButton_H.HXI" );
	highlight = new hxSprite ( 0, core );
	highlight->SetPriority ( 2 );
	highlight->GetMaterial()->SetImage ( actionButtonHImg );

	OnResize ( core->WindowsDevice->GetWindowWidth ( ), core->WindowsDevice->GetWindowHeight ( ) );
}
void ActionBar::SetFade ( float fFade )
{
	UIElement::SetFade ( fFade );
	for ( UINT i = 0; i < 10; i++ )
		actionButton[i]->SetAlpha ( fFade * 255 );
}
void ActionBar::OnResize ( UINT width, UINT height )
{
	float fSpacing = (float)fWidth / 12.0f;
	for ( UINT i = 0; i < 10; i++ )
		actionButton[i]->SetPosition ( width / 2.0f - fWidth / 2.0f + (fSpacing+3) * (i+1) -
		actionButtonImg->GetWidth ( ) / 2.0f + 5, height - 45 );
	SetSize (  fWidth, 50 );
	SetPosition ( width / 2.0f - (fWidth) / 2.0f, height - 50 );
}
bool ActionBar::Update ( float fDeltaTime )
{
	//if mouse is not inside the action bar then remove highlight
	int mx = sprite->GetCorePtr()->InputDevice->MouseX ( );
	int my = sprite->GetCorePtr()->InputDevice->MouseY ( );
	if ( !(mx > sprite->GetPositionX ( ) && mx < sprite->GetPositionX ( ) + sprite->GetSizeX ( ) &&
		my > sprite->GetPositionY ( ) && my < sprite->GetPositionY ( ) + sprite->GetSizeY ( )) )
		highlight->Hide ( );
	
	for ( UINT i = 0; i < 10; i++ )
		actionButton[i]->Update ( fDeltaTime );

	return true;
}
void ActionBar::SetCastbar ( CastBar* cb )
{
	castBar = cb;
}
void ActionBar::SetAction ( UINT index, Action* action )
{
	if ( index < 10 )
		actionButton[index]->SetAction ( action );
}
void ActionBar::SetTargetGUID ( UINT target )
{
	targetGUID = target;
}
void ActionBar::Lock ( void )
{
	for ( UINT i = 0; i < 10; i++ )
		actionButton[i]->Lock ( );
}
void ActionBar::Unlock ( void )
{
	for ( UINT i = 0; i < 10; i++ )
		actionButton[i]->Unlock ( );
}
void ActionBar::DoAction ( UINT index )
{
	if ( index < 10 )
		actionButton[index]->DoAction ( castBar, targetGUID );
}
void ActionBar::OnMouseButton ( int mx, int my, bool bDown )
{
	for ( UINT i = 0; i < 10; i++ )
		if ( actionButton[i]->IsHighlighted ( mx, my, highlight ) )
		{
			if ( bDown )
			{
				if ( Action* dragged = UserInterface::RemoveDrag ( ) )
					actionButton[i]->SetAction ( dragged );
				else
					actionButton[i]->SetInitialClick ( mx, my );
			}
			else
				actionButton[i]->SetFinalClick ( mx, my, castBar, targetGUID );
		}
}
void ActionBar::OnMouseMove ( int mx, int my )
{
	highlight->Show ( );
	for ( UINT i = 0; i < 10; i++ )
		if ( actionButton[i]->IsHighlighted ( mx, my, highlight ) )
			return;
	highlight->Hide ( );
}