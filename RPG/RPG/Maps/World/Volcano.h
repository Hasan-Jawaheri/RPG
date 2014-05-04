#ifndef _WORLD_VOLCANO_H_
#define _WORLD_VOLCANO_H_

#include "..//Map.h"

class Volcano : public WorldObject {

	hxEffect* fx;

public:
	Volcano ( UINT id );
	~Volcano ( void );

	virtual void Load ( void );
	virtual void Destroy ( void );
	virtual bool Update ( float fDeltaTime );
};

#endif //_WORLD_VOLCANO_H_