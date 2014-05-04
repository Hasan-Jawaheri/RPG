#ifndef _UI_CHAT_H_
#define _UI_CHAT_H_

#include "..//UI.h"
#include "..//Input Controls//Edit Box.h"

class Chatbox : public UIElement
{
	struct Message
	{
		LPSTR sender;
		LPSTR message;

		Message ( LPCSTR sndr, LPCSTR msg )
		{
			sender = (LPSTR)HX_SAFE_ALLOC ( strlen ( sndr ) + 1 );
			strcpy ( sender, sndr );
			message = (LPSTR)HX_SAFE_ALLOC ( strlen ( msg ) + 1 );
			strcpy ( message, msg );
		}
		~Message ( void )
		{
			HX_SAFE_FREE ( sender );
			HX_SAFE_FREE ( message );
		}
	};

	vector<Message*> messageV;

public:

	Chatbox ( void );
	~Chatbox ( void );

	virtual void Load ( hxCore* const core );
	virtual void OnResize ( UINT width, UINT height );
	virtual bool Update ( float fDeltaTime );
	
	virtual bool OnInput ( void ) { return false; } //does not accept input
	virtual int GetPosZ ( void ) const { return -1; }

	void AddMessage ( LPCSTR message, LPCSTR owner );
};

class ChatEditBox : public EditBox
{
public:
	virtual bool Update ( float fDeltaTime );
	virtual bool OnTab ( void ) { return false; }
	virtual bool OnEscape ( void );
};

#endif //_UI_CHAT_H_
