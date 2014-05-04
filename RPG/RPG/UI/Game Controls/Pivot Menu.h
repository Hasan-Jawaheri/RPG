#ifndef _UI_PIVOTMENU_H_
#define _UI_PIVOTMENU_H_

#include "..//UI.h"

class PivotMenu : public UIElement
{
protected:
	hxMesh* pivotMesh;
	hxEffect* pivotFX;
	hxObject* pivot;
	int sx, sy;

private:
	int dx, dy;
	int ox, oy;
	bool bOpen;
	hxImage* tabTex;
	LPCSTR tabFile;

	int posX, posY;
	int posXsm, posYsm;
	float fCurAngle, fMinAngle;
	float fAng;

public:
	PivotMenu ( int dirX, int dirY, int sX, int sY, int offsetX, int offsetY, float ang, LPCSTR tabFilename );
	~PivotMenu ( void );
	
	virtual void Load ( hxCore* const core );
	virtual void SetFade ( float fFade );
	virtual void OnResize ( UINT width, UINT height );
	virtual bool Update ( float fDeltaTime );
	
	void Open ( void ) { bOpen = true; }
	void Close ( void ) { bOpen = false; }
	bool IsOpen ( void ) { return bOpen; }
	void SetMinAngle ( float fAngle );
	
	virtual bool OnInput ( void ) { return false; }
	virtual void OnMouseButton ( int mx, int my, bool bDown );
	virtual bool OnTab ( void ) { return false; }
	virtual bool OnEnter ( void ) {  return false; }
	virtual bool OnEscape ( void ) { return true; }
	virtual bool OnFocus ( void ) { return false; }
	
	virtual int GetPositionX ( void ) const;
	virtual int GetPositionY ( void ) const;
	virtual int GetSizeX ( void ) const;
	virtual int GetSizeY ( void ) const;
};

#endif //_UI_PIVOTMENU_H_
