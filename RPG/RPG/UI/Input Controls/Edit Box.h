#ifndef _UI_EDITBOX_H_
#define  _UI_EDITBOX_H_

#include "..//UI.h"

class EditBox : public UIElement
{
	bool bCursor;
	static DWORD dwCursorTimer;

public:

	EditBox ( void );
	~EditBox ( void );

	virtual void Load ( hxCore* const core );
	virtual void OnResize ( UINT width, UINT height );
	virtual bool Update ( float fDeltaTime );

	void SetText ( LPCSTR text );
	void GetText ( LPSTR text, UINT ccMax );

	virtual bool OnInput ( void );
};

class PasswordEditBox : public EditBox
{
public:
	virtual bool OnEnter ( void ) { return true; }
	virtual bool IsPassword ( void ) { return true; }
};

#endif //_UI_EDITBOX_H_
