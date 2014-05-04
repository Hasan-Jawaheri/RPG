#ifndef _DUNGEON_ARENA_H_
#define _DUNGEON_ARENA_H_

#include "..//Map.h"

class Arena : public Map {
public:
	Arena ( void );
	~Arena ( void );

	virtual void Init ( void );
	virtual void Update ( float fDeltaTime );
	virtual void OnLoadObject ( Object* obj );
};

#endif //_DUNGEON_ARENA_H_