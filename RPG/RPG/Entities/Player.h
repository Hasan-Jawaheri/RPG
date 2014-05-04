#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "..//Common.h"
#include "Unit.h"

class Character : public Unit {
public:
	Character ( UINT ID = 0 );
	virtual ~Character ( void );
};

class Player : public Unit {
public:
	Player ( void );
	virtual ~Player ( void );

	virtual void Load ( void );
	virtual void Destroy ( void );
	virtual bool Update ( float fDeltaTime );
	
	virtual void Equip ( ITEMSLOT slot, UINT itemID );
	virtual void Unequip ( ITEMSLOT slot );
};

#endif //_PLAYER_H_
