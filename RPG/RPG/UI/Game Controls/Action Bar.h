#ifndef _UI_ACTIONBAR_H_
#define _UI_ACTIONBAR_H_

#include "..//UI.h"

class Action
{
	hxImage* img;
	hxSprite* spr;
	float fCooldown;

public:
	Action ( hxCore* const core, hxImage* icon, hxPixelShader* shader );
	~Action ( void );
	
	hxSprite* GetSprite ( void ) const { return spr; }
	hxImage* GetImage ( void ) const { return img; }
	
	virtual UINT GetID ( void ) const = 0;
	virtual float GetCooldownPercentage ( void ) const = 0;
	virtual float GetCastTime ( void ) const = 0;
	virtual float GetCastColor ( UINT index ) const { if ( index >= 6 ) return 0; float col[] = {01,0.5,0.1,0.4,1,0.4}; return col[index]; }
	virtual bool GetTargetDependency ( void ) const { return true; }
	virtual void Begin ( UINT targetGUID ) = 0;
	
	void Update ( float fDeltaTime );
};

class ActionButton
{
	hxSprite* sprite;
	hxSprite* holder;
	Action* action;
	int initClickX, initClickY;
	bool bLocked;

	float fHolderAlpha;
	bool bMagnify;

public:
	ActionButton ( hxImage* bkg, hxImage* holder );
	~ActionButton ( void );

	void Update ( float fDeltaTime );
	void SetPosition ( int x, int y );
	void SetAlpha ( BYTE alpha );
	void SetAction ( Action* action );
	void DoAction ( class CastBar* cb, UINT targetGUID );
	void SetInitialClick ( int mx, int my );
	void SetFinalClick ( int mx, int my, class CastBar* cb, UINT targetGUID );

	void Lock ( void );
	void Unlock ( void );

	bool IsHighlighted ( int mx, int my, hxSprite* highlight );
};
class ActionBar : public UIElement
{
	float fWidth;
	
	hxImage* actionButtonImg;
	hxImage* actionButtonHImg;
	hxImage* actionHolderImage;
	hxSprite* highlight;
	ActionButton* actionButton[10];
	class CastBar* castBar;
	UINT targetGUID;

public:
	ActionBar ( float fWidthRatio );
	~ActionBar ( void );
	
	virtual void Load ( hxCore* const core );
	virtual void SetFade ( float fFade );
	virtual void OnResize ( UINT width, UINT height );
	virtual bool Update ( float fDeltaTime );

	void SetCastbar ( class CastBar* cb );
	void DoAction ( UINT index );
	void SetAction ( UINT index, Action* action );
	void SetTargetGUID ( UINT target );
	void Lock ( void );
	void Unlock ( void );
	
	virtual bool OnInput ( void ) { return false; }
	virtual void OnMouseButton ( int mx, int my, bool bDown );
	virtual void OnMouseMove ( int mx, int my );
	virtual bool OnTab ( void ) { return false; }
	virtual bool OnEnter ( void ) { return false; }
	virtual bool OnEscape ( void ) { return false; }
	virtual bool OnFocus ( void ) { return false; }
};

#endif //_UI_ACTIONBAR_H_
