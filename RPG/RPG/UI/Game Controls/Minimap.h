#ifndef _UI_MINIMAP_H_
#define _UI_MINIMAP_H_

#include "..//UI.h"

class Minimap : public UIElement
{
	hxRenderTarget* rt;
	hxImage* rtImg;
	hxCamera* mapCam;

public:
	Minimap ( void );
	~Minimap ( void );
	
	virtual void Load ( hxCore* const core );
	virtual bool Update ( float fDeltaTime );
	
	virtual bool OnInput ( void ) { return false; }
	virtual bool OnTab ( void ) { return false; }
	virtual bool OnEnter ( void ) { return false; }
	virtual bool OnEscape ( void ) { return false; }
	virtual bool OnFocus ( void ) { return false; }
};

#endif //_UI_MINIMAP_H_
