#ifndef _USER_INTERFACE_H_
#define _USER_INTERFACE_H_

#include "..//Main.h"

class UIElement
{
	friend class UserInterface;

	UIElement* parent;
	vector<UIElement*> childV;
	bool bLoaded;

protected:
	hxSprite* sprite;
	hxPixelShader* shader;
	float fAlpha;
	LPSTR inputBuffer;

public:
	UIElement ( void );
	~UIElement ( void );
	
	UIElement* GetParent ( void ) const;
	void SetParent ( UIElement* p );
	UIElement* GetChild ( UINT index ) const;
	void AddChild ( UIElement* child );
	void RemoveChild ( UIElement* child );
	void RemoveChild ( UINT index );
	UINT GetNumChildren ( void ) const;

	virtual void Load ( hxCore* const core ) { }
	virtual void SetFade ( float fFade );
	virtual void OnResize ( UINT width, UINT height ) { }
	virtual bool Update ( float fDeltaTime ) { return true; };
	
	virtual bool OnInput ( void ) { return true; }
	virtual void OnKeydown ( short key ) { }
	virtual void OnKeyup ( short key ) { }
	virtual void OnMouseMove ( int mx, int my ) { }
	virtual void OnMouseButton ( int mx, int my, bool bDown ) { }
	virtual void OnMouseButton2 ( int mx, int my, bool bDown ) { }
	virtual bool OnTab ( void ) { return true; }
	virtual bool OnEnter ( void ) { return true; }
	virtual bool OnEscape ( void ) { return true; }
	virtual bool IsPassword ( void ) { return false; }
	virtual bool OnFocus ( void ) { return true; }
	virtual bool OnLoseFocus ( void ) { return true; }
	virtual bool OnDisableAllUpdates ( void ) { return false; }
	LPSTR GetInputBuffer ( void ) { return inputBuffer; }
	UINT GetInputBufferSize ( void ) const { return 512; }
	
	virtual int GetPosZ ( void ) const { return 1; }
	virtual void SetPosition ( int x, int y );
	virtual int GetPositionX ( void ) const { if ( sprite ) return sprite->GetPositionX ( ); return 0; }
	virtual int GetPositionY ( void ) const { if ( sprite ) return sprite->GetPositionY ( ); return 0; }
	virtual void SetSize ( int sizeX, int sizeY );
	virtual int GetSizeX ( void ) const { if ( sprite ) return sprite->GetSizeX ( ); return 0; }
	virtual int GetSizeY ( void ) const { if ( sprite ) return sprite->GetSizeY ( ); return 0; }
};

class UserInterface
{
	static class Action* draggedAction;
	static vector<UIElement*> UI;
	static UIElement* focus;

public:
	
	static void Init ( hxCore* core );
	static void Update ( float fDeltaTime );
	static void Terminate ( void );
	static void Load ( hxCore* c );
	static void SetFade ( float fFade );
	static void OnResize ( UINT width, UINT height );
	static UIElement* PrintError ( hxCore* const core, LPCSTR errorMessage );
	static void AddUIElement ( UIElement* element, UIElement* parent );
	static void RemoveUIElement ( UIElement* element );
	static void SetFocus ( UIElement* element );
	static UIElement* GetFocus ( void );
	static UIElement* GetElementAt ( int mx, int my );

	static void Drag ( Action* action );
	static bool IsDragging ( void );
	static Action* RemoveDrag ( void );
};


#endif //_USER_INTERFACE_H_
