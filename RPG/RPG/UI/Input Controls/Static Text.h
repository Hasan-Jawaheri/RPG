#ifndef _UI_STATICTEXT_H_
#define _UI_STATICTEXT_H_

#include "..//UI.h"

class StaticText : public UIElement
{
	LPCSTR text;

public:
	StaticText ( LPCSTR _text );
	~StaticText ( void );
	
	virtual void Load ( hxCore* const core );
	virtual bool Update ( float fDeltaTime );

	virtual bool OnInput ( void ) { return false; }
	virtual bool OnTab ( void ) { return false; }
	virtual bool OnEnter ( void ) { return false; }
	virtual bool OnEscape ( void ) { return false; }
	virtual bool IsPassword ( void ) { return false; }
	virtual bool OnFocus ( void ) { return false; }
};

#endif //_UI_STATICTEXT_H_
