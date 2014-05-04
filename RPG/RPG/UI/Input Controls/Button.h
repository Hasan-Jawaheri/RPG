#ifndef _UI_BUTTON_H_
#define _UI_BUTTON_H_

#include "..//UI.h"

class MenuButton : public UIElement
{
	int offsets[2];
	LPCSTR text;
	float fAlphaOffset;
	bool bClicked;

public:
	MenuButton ( int o1, int o2, LPCSTR _text );
	~MenuButton ( void );
	
	virtual void Load ( hxCore* const core );
	virtual void SetFade ( float fFade );
	virtual void OnResize ( UINT width, UINT height );
	virtual bool Update ( float fDeltaTime );

	virtual bool OnInput ( void ) { return false; }
	virtual bool OnTab ( void ) { return false; }
	virtual bool OnEnter ( void ) { return true; }
	virtual bool OnEscape ( void ) { return false; }
	virtual void OnMouseButton ( int mx, int my, bool bDown );
	virtual bool OnFocus ( void ) { return false; } //cannot gain focus

	virtual void OnClick ( void ) { };
	bool IsClicked ( void ) { if ( bClicked ) { bClicked = !bClicked; return !bClicked; } return bClicked; }
};

#endif //_UI_BUTTON_H_
