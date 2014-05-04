#include "Main.h"
#include "States//Intro.h"
#include "Maps//MapLoader.h"
#include "Entities//Player.h"
#include "Spells//Spell.h"
#include "UI//UI.h"

Application* APPHANDLE;

Application::Application ( void ) : HasX11 ( ) { //call the base constructor
}

bool Application::Setup ( void ) {
	//
	//Initialize the core. This must be done to create the window and initialize it
	//
	int scrW = GetSystemMetrics ( SM_CXSCREEN );
	int scrH = GetSystemMetrics ( SM_CYSCREEN );
#ifdef _DEBUG
	int width = 640;	//640 pixels wide
	int height = 480;	//480 pixels high
#else
	int width = scrW;
	int height = scrH;
#endif
	bool vsync = false;//enable vsync (prevents screen tearing)
	HX_ERROR err = core->Init ( width, height, vsync );
	if ( err != HX_SUCCEED ) { //An error occured. Very rare but should always be checked
		//retrieve the error as a string and print it
		char errorStr[256];
		hxGetError ( err, errorStr, 256 );
		MessageBoxA ( nullptr, errorStr, APPNAME, MB_ICONERROR | MB_OK );
		return false; //returning false from Setup will stop the application
	}
	maxFPS = 0;
#ifndef _DEBUG
	core->WindowsDevice->SetFullScreenState ( true );
#endif
	//renderFlags = RENDERFLAG_NOLIGHTING;

	core->InputDevice->DisableEscapeKeyQuit ( );
	core->CameraManager->GetDefaultCamera()->SetRange ( 1, 10000 );

	core->PhysicsDevice->Init ( 4.0f, 15000, true );
	core->PhysicsDevice->SetGravity ( 0.0f, -12.0f, 0.0f );
	core->PhysicsDevice->Start ( );

	ResourceManager::Init ( core );
	MapLoader::Init ( );
	UserInterface::Init ( core );

	SwitchState ( new Intro ( this ) );

	return true;
}

bool Application::Loop ( float fDeltaTime ) {

	fCurGameTime = core->GetBaseTimer()->GetElapsedTime ( );

	char text[256];
	sprintf_s ( text, 256, "FPS: %.2f//%.2f\nGame Time: %.2f", FPS, maxFPS, fCurGameTime );
	core->Renderer->RenderText ( text, 5, 5, 0, hxColor ( 1.0, 0.0, 0.0 )  );

	if ( curState ) { // we only need this because we destroy state in the windows callback
		UserInterface::Update ( fDeltaTime );
	}

	core->PhysicsDevice->Step ( fDeltaTime );

	//a null state means exit
	return curState != nullptr; //returning true will allow the application to continue
}

void Application::Cleanup ( void ) {
	UserInterface::Terminate ( );
	MapLoader::Cleanup ( );
	ResourceManager::Cleanup ( );
}

int Application::WindowCallback ( HWND wnd, UINT msg, WPARAM w, LPARAM l, bool bBeforeProcessing )
{
	if ( !curState )
		return 0;

	//
	//THE CODE AHEAD IS MOSTLY FOR UI TO TAKE INPUT DIRECTLY FROM WINDOWS
	//
	if ( !bBeforeProcessing )
	{
		int mx = LOWORD ( l );
		int my = HIWORD ( l );

 		if ( msg == WM_SIZE )
		{
			UserInterface::OnResize ( core->WindowsDevice->GetWindowWidth ( ), core->WindowsDevice->GetWindowHeight ( ) );
			//CoreLinker::Resize ( core->WindowsDevice->GetWindowWidth ( ), core->WindowsDevice->GetWindowHeight ( ) );
		}

		if ( msg == WM_MOUSEMOVE )
		{
			UIElement* targetElement = UserInterface::GetElementAt ( mx, my );
			if ( targetElement )
				targetElement->OnMouseMove ( mx, my );
		}
			
		if ( msg == WM_LBUTTONDOWN )
		{
			UIElement* targetElement = UserInterface::GetElementAt ( mx, my );
			if ( targetElement )
			{
				targetElement->OnMouseButton ( mx, my, true );
				if ( targetElement->GetSizeX ( ) != 0 && targetElement->GetSizeY ( ) != 0 )
				{
					bool bGiveFocus = true;
					if ( UserInterface::GetFocus ( ) )
						if ( !UserInterface::GetFocus()->OnLoseFocus ( ) )
						{
							bool bIsChild = false;
							for ( UINT i = 0; i < UserInterface::GetFocus()->GetNumChildren ( ); i++ )
								if ( UserInterface::GetFocus()->GetChild(i) == targetElement )
									bIsChild = true;
							if ( !bIsChild )
								bGiveFocus = false;
						}
						
					if ( targetElement->OnFocus ( ) && bGiveFocus )
						UserInterface::SetFocus ( targetElement );
				}
			}
		}

		if ( msg == WM_LBUTTONUP )
		{
			UIElement* targetElement = UserInterface::GetElementAt ( mx, my );
			if ( targetElement )
			{
				bool bProcceed = true;
				if ( UserInterface::GetFocus ( ) )
					if ( !UserInterface::GetFocus()->OnLoseFocus ( ) )
					{
						bool bIsChild = false;
						for ( UINT i = 0; i < UserInterface::GetFocus()->GetNumChildren ( ); i++ )
							if ( UserInterface::GetFocus()->GetChild(i) == targetElement )
								bIsChild = true;
						if ( !bIsChild )
							bProcceed = false;
					}

				if ( bProcceed )
					targetElement->OnMouseButton ( mx, my, false );
			}
		}

		if ( msg == WM_RBUTTONDOWN )
		{
			UIElement* targetElement = UserInterface::GetElementAt ( mx, my );
			if ( targetElement )
				targetElement->OnMouseButton2 ( mx, my, true );
		}

		if ( msg == WM_RBUTTONUP )
		{
			UIElement* targetElement = UserInterface::GetElementAt ( mx, my );
			if ( targetElement )
				targetElement->OnMouseButton2 ( mx, my, false );
		}

		if ( msg == WM_KEYDOWN && UserInterface::GetFocus ( ) )
		{
			if ( UserInterface::GetFocus()->OnInput ( ) )
				UserInterface::GetFocus()->OnKeydown ( w );
			else //if it does not accept input, see if it's parent does
				if ( UserInterface::GetFocus()->GetParent ( ) )
					if ( UserInterface::GetFocus()->GetParent()->OnInput ( ) )
						UserInterface::GetFocus()->GetParent()->OnKeydown ( w );
		}

		if ( msg == WM_KEYUP && UserInterface::GetFocus ( ) )
		{
			if ( UserInterface::GetFocus()->OnInput ( ) )
				UserInterface::GetFocus()->OnKeyup ( w );
			else //if it does not accept input, see if it's parent does
				if ( UserInterface::GetFocus()->GetParent ( ) )
					if ( UserInterface::GetFocus()->GetParent()->OnInput ( ) )
						UserInterface::GetFocus()->GetParent()->OnKeyup ( w );
		}

		if ( msg == WM_CHAR && UserInterface::GetFocus ( ) )
		{
			int repeat = LOWORD ( l );
			for ( UINT i = 0; i < repeat; i++ )
			{
				if ( w == VK_RETURN )
				{
					UIElement* parent = UserInterface::GetFocus()->GetParent ( );
					if ( UserInterface::GetFocus()->OnEnter ( ) )
					{
						if ( parent )
							parent->OnEnter ( );
						continue;
					}
				}
				else if ( w == VK_ESCAPE )
				{
					if ( UserInterface::GetFocus()->OnEscape ( ) )
					{
						if ( UserInterface::GetFocus()->GetParent ( ) )
							UserInterface::GetFocus()->GetParent()->OnEscape ( );
						continue;
					}
				}
				else if ( w == VK_TAB )
				{
					if ( UserInterface::GetFocus()->OnTab ( ) )
					{
						if ( UserInterface::GetFocus()->GetParent ( ) && UserInterface::GetFocus()->OnLoseFocus ( ) )
						{
							UINT numChildren = UserInterface::GetFocus()->GetParent()->GetNumChildren ( );
							UINT focusIndex = -1;
							for ( UINT i = 0; i <= numChildren; i++ )
							{
								if ( i == numChildren )
									i = 0;

								if ( focusIndex != -1 )
								{
									if ( UserInterface::GetFocus()->GetParent()->GetChild(i)->OnTab ( ) )
									{
										if ( UserInterface::GetFocus()->GetParent()->GetChild(i)->OnFocus ( ) )
											UserInterface::SetFocus ( UserInterface::GetFocus()->GetParent()->GetChild ( i ) );
										break;
									}
								}

								if ( UserInterface::GetFocus()->GetParent()->GetChild ( i ) == UserInterface::GetFocus() )
									focusIndex = i;
							}
						}
						continue;
					}
				}
				else if ( w == VK_BACK )
				{
					if ( strlen ( UserInterface::GetFocus()->GetInputBuffer ( ) ) && UserInterface::GetFocus()->OnInput ( ) )
						UserInterface::GetFocus()->GetInputBuffer()[strlen(UserInterface::GetFocus()->GetInputBuffer ( ))-1] = '\0';
					continue;
				}

				if ( UserInterface::GetFocus()->OnInput ( ) )
				{
					if ( strlen ( UserInterface::GetFocus()->GetInputBuffer ( ) ) <
							UserInterface::GetFocus()->GetInputBufferSize ( ) - 1 )
						sprintf_s ( UserInterface::GetFocus()->GetInputBuffer ( ), UserInterface::GetFocus()->GetInputBufferSize ( ),
							"%s%c", UserInterface::GetFocus()->GetInputBuffer ( ), w );
				}
				else //if it does not accept input, see if it's parent does
				{
					if ( UserInterface::GetFocus()->GetParent ( ) )
						if ( UserInterface::GetFocus()->GetParent()->OnInput ( ) )
						{
							if ( strlen ( UserInterface::GetFocus()->GetParent()->GetInputBuffer ( ) ) <
											UserInterface::GetFocus()->GetParent()->GetInputBufferSize ( ) - 1 )
								sprintf_s ( UserInterface::GetFocus()->GetParent()->GetInputBuffer ( ),
												UserInterface::GetFocus()->GetParent()->GetInputBufferSize ( ),
												"%s%c", UserInterface::GetFocus()->GetParent()->GetInputBuffer ( ), w );
						}
				}
			}
		}
	}

	return 0;
}

//******************************************************************************************
//This function is always needed. Every new HasX11 will be registered by the HasX11 system
//and will be automatically freed. Each new HasX11 class will run on its own thread. This
//can be used for multiuthreading. Note that the HasX11 class is completely generic and can
//run without core->Init and still work, thus you don't need to have 2 windows to use this
//multithreading feature.
//******************************************************************************************
void HasX11Init ( void ) {
	APPHANDLE = new Application; //The base construction, HasX11(), will take care of the rest.
}