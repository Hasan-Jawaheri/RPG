#ifndef _DUNGEON_WORLD_H_
#define _DUNGEON_WORLD_H_

#include "..//Map.h"

class Skybox : public Object {

	hxPixelShader* ps;
	hxEffect* fx;
	hxMesh* mesh;
	hxImage* tex;
	hxMaterial* material;

public:
	Skybox ( UINT ID = 0 );
	~Skybox ( void );

	virtual void Load ( void );
	virtual void Destroy ( void );
	virtual bool Update ( float fDeltaTime );
};

class Terrain : public Object {

	hxPixelShader* ps;
	hxEffect* fx;

public:
	Terrain ( UINT ID = 0 );
	~Terrain ( void );

	virtual void Load ( void );
	virtual void Destroy ( void );
	virtual bool Update ( float fDeltaTime );
};

class World : public Map {
	Skybox* sky;
	Terrain* ground;

public:
	World ( void );
	~World ( void );

	virtual void Init ( void );
	virtual void Update ( float fDeltaTime );
	virtual void OnLoadObject ( Object* obj );
	virtual WorldObject* CreateObject ( UINT modelID );
};

#endif //_DUNGEON_WORLD_H_