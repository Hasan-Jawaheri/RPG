#ifndef _MENU_H_
#define _MENU_H_

#include "..//Common.h"

class Menu : public hxGameState {
public:
	Menu ( HasX11* a );
	~Menu ( void );

	void Load ( void );
	void Update ( float fDeltaTime );
	void Cleanup ( void );
};

#endif //_MENU_H_
