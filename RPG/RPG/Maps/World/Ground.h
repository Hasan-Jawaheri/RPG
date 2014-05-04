#ifndef _WORLD_GROUND_H_
#define _WORLD_GROUND_H_

#include "..//Map.h"

class Ground : public WorldObject {

	hxEffect* fx;

public:
	Ground ( UINT id );
	~Ground ( void );

	virtual void Load ( void );
	virtual void Destroy ( void );
	virtual bool Update ( float fDeltaTime );
};

#endif //_WORLD_GROUND_H_