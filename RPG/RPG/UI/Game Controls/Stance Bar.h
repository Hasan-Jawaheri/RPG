#ifndef _UI_STANCEBAR_H_
#define _UI_STANCEBAR_H_

#include "..//UI.h"
#include "Pivot Menu.h"

class StanceBar : public PivotMenu
{
	hxImage* bar;
	hxImage* stances[3];
	hxSprite* stanceSprs[3];
	UINT curStance;

public:
	StanceBar ( void );
	~StanceBar ( void );
	
	virtual void Load ( hxCore* const core );
	virtual bool Update ( float fDeltaTime );

	void SetStance ( UINT stance );
	UINT GetCurrentStance ( void ) const;
	hxImage* GetCurrentImage ( void ) const;

	virtual bool OnInput ( void ) { return false; }
	virtual bool OnTab ( void ) { return false; }
	virtual bool OnEscape ( void ) { return false; }
	virtual bool OnFocus ( void ) { return false; }
	virtual bool OnEnter ( void ) { return false; }

	virtual void SetPosition ( int x, int y );
	
	virtual int GetPositionX ( void ) const;
	virtual int GetPositionY ( void ) const;
};

#endif //_UI_STANCEBAR_H_
