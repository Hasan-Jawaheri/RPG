#ifndef _DUNGEON_DESERT_ARENA_H_
#define _DUNGEON_DESERT_ARENA_H_

#include "..//Map.h"

class DesertArena : public Map {
public:
	DesertArena ( void );
	~DesertArena ( void );

	virtual void Init ( void );
	virtual void Update ( float fDeltaTime );
	virtual void OnLoadObject ( Object* obj );
};

#endif //_DUNGEON_DESERT_ARENA_H_