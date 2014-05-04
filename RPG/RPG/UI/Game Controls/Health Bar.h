#ifndef _UI_HEALTHBAR_H_
#define _UI_HEALTHBAR_H_

#include "..//UI.h"
#include "Pivot Menu.h"

class HealthBar : public PivotMenu
{
	bool bReversed;
	float fOffset;
	hxImage* bar;

public:
	HealthBar ( bool bReverse );
	~HealthBar ( void );
	
	virtual void Load ( hxCore* const core );
	virtual bool Update ( float fDeltaTime );

	void SetHealth ( float fPercentage );
	void SetPower ( float fPercentage );
	void SetPowerColor ( float pwrR, float pwrG, float pwrB );

	virtual bool OnInput ( void ) { return false; }
	virtual bool OnTab ( void ) { return false; }
	virtual bool OnEscape ( void ) { return false; }
	virtual bool OnFocus ( void ) { return false; }

	virtual void SetPosition ( int x, int y );
	
	virtual int GetPositionX ( void ) const;
	virtual int GetPositionY ( void ) const;
	virtual int GetSizeX ( void ) const;
	virtual int GetSizeY ( void ) const;
};

#endif //_UI_HEALTHBAR_H_
