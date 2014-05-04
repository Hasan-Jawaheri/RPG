#ifndef _MAP_H_
#define _MAP_H_

#include "..//Main.h"
#include "..//Entities//Object.h"
#include "MapLoader.h"

class WorldObject : public Object {
	friend class MapLoader;
	UINT modelID;
	hxVector3 loadingPos;

public:
	WorldObject ( UINT modelID = 0 );
	~WorldObject ( void );

	virtual void Load ( void );
	virtual void Destroy ( void );
	virtual bool Update ( float fDeltaTime );

	void SetLoadingPosition ( hxVector3 pos );
};

class Map {
	static Map* curMap;
	
protected:
	hxCore* core;
	vector<Object*> objects;

public:
	Map ( void );
	~Map ( void );

	// Initialize the environment (lighting/fog/etc...)
	virtual void Init ( void );
	// Update the map. Called every frame
	virtual void Update ( float fDeltaTime );
	// Do any action upon loading an object, such as setting its' AI
	virtual void OnLoadObject ( Object* obj );
	// Do any action upon destroying an object
	virtual void OnDestroyObject ( Object* obj );
	// Create an instance of the class representing the modelID
	virtual WorldObject* CreateObject ( UINT modelID );
	// Do any cleaning up before switching/destroying map
	virtual void Cleanup ( void );

	// GLOBAL FUNCIONS
	// Get/Set the current global map
	static Map* Get ( void );
	static void Set ( UINT mapID );
};

#endif //_MAP_H_