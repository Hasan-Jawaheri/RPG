#ifndef _DUNGEON_TEST_H_
#define _DUNGEON_TEST_H_

#include "..//Map.h"

class TestMap : public Map {
public:
	TestMap ( void );
	~TestMap ( void );

	virtual void Update ( float fDeltaTime );
	virtual void OnLoadObject ( Object* obj );
};

#endif //_DUNGEON_TEST_H_