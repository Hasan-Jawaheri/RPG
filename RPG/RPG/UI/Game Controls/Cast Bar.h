#ifndef _UI_CASTBAR_H_
#define _UI_CASTBAR_H_

#include "..//UI.h"

class CastBar : public UIElement
{
	float fCast;
	float fSpellTime;
	//class Action* spell;
	hxImage* image;
	bool bOver;
	hxImage* coverImg;

public:
	CastBar ( void );
	~CastBar ( void );
	
	virtual void Load ( hxCore* const core );
	virtual bool Update ( float fDeltaTime );

	//bool Cast ( float fTime, Action* sp, UINT targetGUID );
	void Cast ( hxImage* img, float fTime );
	float GetCurTime ( void ) const;
	//class Action* GetCastingSpell ( void ) const;

	virtual bool OnInput ( void ) { return false; }
	virtual bool OnTab ( void ) { return false; }
	virtual bool OnEnter ( void ) { return false; }
	virtual bool OnEscape ( void ) { return false; }
	virtual bool OnFocus ( void ) { return false; }
};

#endif //_UI_CASTBAR_H_
