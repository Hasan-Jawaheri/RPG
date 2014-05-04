#ifndef _WORLD_LAVA_H_
#define _WORLD_LAVA_H_

#include "..//Map.h"

class Lava : public WorldObject {

	hxEffect* fx;

public:
	Lava ( UINT id );
	~Lava ( void );

	virtual void Load ( void );
	virtual void Destroy ( void );
	virtual bool Update ( float fDeltaTime );
};

#endif //_WORLD_LAVA_H_