#include "UI.h"
#include "Input Controls//Error Box.h"
#include "Game Controls//Action Bar.h"

UIElement::UIElement ( void )
{
	sprite = nullptr;
	shader = nullptr;
	bLoaded = false;
	inputBuffer = nullptr;
	if ( OnInput ( ) )
		inputBuffer = (LPSTR)HX_SAFE_ALLOC ( 512 );
}
UIElement::~UIElement ( void )
{
	HX_SAFE_REMOVEREF ( sprite );
	HX_SAFE_REMOVEREF ( shader );
	HX_SAFE_FREE ( inputBuffer );

	for ( UINT i = 0; i < childV.size ( ); i++ )
		delete childV[i];
}
void UIElement::SetFade ( float fFade )
{
	if ( shader )
		sprite->GetMaterial()->SetConstantBufferVariableFloat ( "gAlpha", fFade );
	fAlpha = fFade * 255.0f;
}
void UIElement::SetPosition ( int x, int y )
{
	if ( sprite )
		sprite->SetPosition ( x, y );
}
void UIElement::SetSize ( int sizeX, int sizeY )
{
	if ( sprite )
		sprite->SetSize ( sizeX, sizeY );
	
	if ( shader )
	{
		float insSize[2] = { sizeX, sizeY };
		sprite->GetMaterial()->SetConstantBufferVariableFloatArray ( "gBoxSize", 2, insSize );
	}
}
UIElement* UIElement::GetParent ( void ) const
{
	return parent;
}
void UIElement::SetParent ( UIElement* p )
{
	parent = p;
}
UIElement* UIElement::GetChild ( UINT index ) const
{
	if ( index >= childV.size ( ) )
		return nullptr;
	
	return childV[index];
}
void UIElement::AddChild ( UIElement* child )
{
	childV.push_back ( child );
}
void UIElement::RemoveChild ( UIElement* child )
{
	for ( UINT i = 0; i < childV.size ( ); i++ )
		if ( childV[i] == child )
			childV.erase ( childV.begin ( ) + i );
}
void UIElement::RemoveChild ( UINT index )
{
	if ( index < childV.size ( ) )
		childV.erase ( childV.begin ( ) + index );
}
UINT UIElement::GetNumChildren ( void ) const
{
	return childV.size ( );
}

Action* UserInterface::draggedAction = nullptr;
vector<UIElement*> UserInterface::UI;
UIElement* UserInterface::focus = nullptr;

void UserInterface::Init ( hxCore* core ) {
	core->Renderer->CreateFont ( FONT_CONSOLAS_16, "Consolas", 15.0f );
}
UIElement* UserInterface::PrintError ( hxCore* const core, LPCSTR errorMessage )
{
	ErrorBox* errBox = new ErrorBox ( errorMessage );
	MenuButton* errorButton = new ErrorButton ( 140, -8, "Ok" );
	AddUIElement ( errBox, nullptr );
	AddUIElement ( errorButton, errBox );
	Load ( core );
	SetFocus ( errBox );
	return errBox;
}
void UserInterface::AddUIElement ( UIElement* element, UIElement* parent )
{
	element->SetParent ( parent );
	if ( parent )
		parent->AddChild ( element );
	UI.push_back ( element );

	if ( UI.size ( ) == 1 )
		focus = element;
}
void UserInterface::RemoveUIElement ( UIElement* element )
{
	for ( int i = 0; i < UI.size ( ); i++ )
	{
		if ( UI[i] == element )
		{
			if ( focus == element )
			{
				bool bFoundFocus = false;
				for ( UINT n = 0; n < UI.size ( ); n++ )
					if ( UI[n] != focus && UI[n]->OnFocus ( ) )
					{
						focus = UI[n];
						bFoundFocus = true;
						break;
					}
				if ( !bFoundFocus )
					focus = nullptr;
			}
			UI.erase ( UI.begin ( ) + i );
			for ( UINT n = 0; n < element->childV.size ( ); n++ )
				RemoveUIElement ( element->childV[n] );
		}
	}
}
void UserInterface::Load ( hxCore* c )
{
	for ( UINT i = 0; i < UI.size ( ); i++ )
	{
		if ( !UI[i]->bLoaded )
			UI[i]->Load ( c );
		UI[i]->bLoaded = true;
	}
}
void UserInterface::Terminate ( void )
{
	for ( UINT i = 0; i < UI.size ( ); i++ )
	{
		while ( UI[i]->GetNumChildren ( ) ) UI[i]->RemoveChild ( (UINT)0 ); //remove all children for safe deleting of the elements
		delete UI[i];
	}
	UI.clear ( );
	focus = nullptr;
}
void UserInterface::Update ( float fDeltaTime )
{
	//update dragging
	if ( draggedAction )
	{
		int mx = draggedAction->GetSprite()->GetCorePtr()->InputDevice->MouseX ( );
		int my = draggedAction->GetSprite()->GetCorePtr()->InputDevice->MouseY ( );
		draggedAction->GetSprite()->SetPosition ( mx - draggedAction->GetImage()->GetWidth ( ) / 2,
			my - draggedAction->GetImage()->GetHeight ( ) / 2 );
	}

	//check for update disablers
	for ( int i = 0; i < UI.size ( ); i++ )
	{
		if ( UI[i]->OnDisableAllUpdates ( ) )
		{
			if ( !UI[i]->Update ( fDeltaTime ) )
			{
				UIElement* element = UI[i];
				RemoveUIElement ( element );
				delete element;
				i--;
			}
			else
			{
				//update children too
				for ( int n = 0; n < UI[i]->GetNumChildren ( ); n++ )
				{
					if ( !UI[i]->GetChild(n)->Update ( fDeltaTime ) )
					{
						UIElement* element = UI[i]->GetChild ( n );
						UI[i]->RemoveChild ( n );
						RemoveUIElement ( element );
						delete element;
						n--;
					}
				}
			}
			return;
		}
	}

	for ( int i = 0; i < UI.size ( ); i++ )
	{
		if ( !UI[i]->Update ( fDeltaTime ) )
		{
			UIElement* element = UI[i];
			RemoveUIElement ( element );
			delete element;
			i--;
		}
	}
}
void UserInterface::SetFade ( float fFade )
{
	for ( UINT i = 0; i < UI.size ( ); i++ )
		UI[i]->SetFade ( fFade );
}
void UserInterface::OnResize ( UINT width, UINT height )
{
	for ( UINT i = 0; i < UI.size ( ); i++ )
		UI[i]->OnResize ( width, height );
}
void UserInterface::SetFocus ( UIElement* element )
{
	focus = element;
}
UIElement* UserInterface::GetFocus ( void )
{
	return focus;
}
UIElement* UserInterface::GetElementAt ( int mx, int my )
{
	vector<UIElement*> possibilities;
	for ( int i = 0; i < UI.size ( ); i++ )
	{
		float posX = UI[i]->GetPositionX ( );
		float posY = UI[i]->GetPositionY ( );
		float sX = UI[i]->GetSizeX ( );
		float sY = UI[i]->GetSizeY ( );
		if ( mx > UI[i]->GetPositionX ( ) && mx < UI[i]->GetPositionX ( ) + UI[i]->GetSizeX ( ) &&
				my > UI[i]->GetPositionY ( ) && my < UI[i]->GetPositionY ( ) + UI[i]->GetSizeY ( ) )
				possibilities.push_back ( UI[i] );
	}
	if ( possibilities.size ( ) )
	{
		int lowestZ = possibilities[0]->GetPosZ ( );
		int index = 0;
		//return priority to the one without input support
		for ( UINT i = 1; i < possibilities.size ( ); i++ )
			if ( lowestZ > possibilities[i]->GetPosZ ( ) && possibilities[i]->GetPosZ ( ) >= 0 )
			{
				lowestZ = possibilities[i]->GetPosZ ( );
				index = i;
			}
		if ( lowestZ >= 0 )
			return possibilities[index];
	}

	for ( int i = 0; i < UI.size ( ); i++ )
	{
		if ( UI[i]->GetSizeX ( ) == 0 && UI[i]->GetSizeY ( ) == 0 )
			return UI[i];
	}

	return nullptr;
}
void UserInterface::Drag ( Action* action )
{
	if ( draggedAction )
		HX_SAFE_DELETE ( draggedAction );

	draggedAction = action;
	draggedAction->GetSprite()->SetPriority ( 10 ); //while dragging, use top priority
}
bool UserInterface::IsDragging ( void )
{
	return draggedAction != nullptr;
}
Action* UserInterface::RemoveDrag ( void )
{
	Action* ret = draggedAction;
	draggedAction = nullptr;
	if ( ret )
		ret->GetSprite()->SetPriority ( 1 ); //dragging is over, back to normal priority
	return ret;
}