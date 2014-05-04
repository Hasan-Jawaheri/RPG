#ifndef _DUNGEON_DEMONLORDSLAIR_H_
#define _DUNGEON_DEMONLORDSLAIR_H_

#include "..//Map.h"

class DemonLordsLair : public Map {
public:
	DemonLordsLair ( void );
	~DemonLordsLair ( void );

	virtual void Update ( float fDeltaTime );
	virtual void OnLoadObject ( Object* obj );
};

#endif //_DUNGEON_DEMONLORDSLAIR_H_