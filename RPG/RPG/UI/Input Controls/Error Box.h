#ifndef _UI_ERRORBOX_H_
#define _UI_ERRORBOX_H_

#include "..//UI.h"
#include "..//Input Controls//Button.h"

class ErrorBox : public UIElement
{
	LPSTR errorMsg;
	bool bClick;

public:
	ErrorBox ( LPCSTR error );
	~ErrorBox ( void ) { HX_SAFE_DELETE ( errorMsg ); }
	
	virtual void Load ( hxCore* const core );
	virtual void OnResize ( UINT width, UINT height );
	virtual bool Update ( float fDeltaTime );
	
	virtual bool OnInput ( void ) { return false; }
	virtual bool OnTab ( void ) { return false; }
	virtual bool OnEnter ( void ) { bClick = true; return false; }
	virtual bool OnEscape ( void ) { bClick = true; return false; }
	virtual bool IsPassword ( void ) { return false; }
	virtual bool OnFocus ( void ) { return true; }
	virtual bool OnLoseFocus ( void ) { return false; } //never lose focus
	virtual bool OnDisableAllUpdates ( void ) { return true; } //disable updating all other elements
	
	//in order not to allow the UI manager to pick it for mouse events
	virtual int GetPosZ ( void ) const { return 10; }
	virtual int GetSizeX ( void ) const { return 0; }
	virtual int GetSizeY ( void ) const { return 0; }
};
class ErrorButton : public MenuButton
{
public:
	ErrorButton ( int o1, int o2, LPCSTR _text ) : MenuButton ( o1, o2, _text ) { };
	virtual int GetPosZ ( void ) const { return 10; }
};

#endif //_UI_ERRORBOX_H_
