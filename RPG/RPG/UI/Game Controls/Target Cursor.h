#ifndef _UI_TARGETCURSOR_H_
#define _UI_TARGETCURSOR_H_

#include "..//UI.h"

class TargetCursor : public UIElement
{
	hxImage* img;

public:
	TargetCursor ( void );
	~TargetCursor ( void );
	
	virtual void Load ( hxCore* const core );
	virtual bool Update ( float fDeltaTime );
	
	virtual bool OnInput ( void ) { return false; }
	virtual bool OnTab ( void ) { return false; }
	virtual bool OnEnter ( void ) { return false; }
	virtual bool OnEscape ( void ) { return false; }
	virtual bool OnFocus ( void ) { return false; }
	
	virtual int GetSizeX ( void ) const { return 0; }
	virtual int GetSizeY ( void ) const { return 0; }
};

#endif //_UI_TARGETCURSOR_H_