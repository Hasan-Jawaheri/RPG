#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "..//Common.h"

enum ITEMSLOT {
	SLOT_HEAD = 0,
	SLOT_NECK = 1,
	SLOT_SHOULDERS = 2,
	SLOT_CHEST = 3,
	SLOT_BRACERS = 4,
	SLOT_GLOVES = 5,
	SLOT_BELT = 6,
	SLOT_PANTS = 7,
	SLOT_BOOTS = 8,
	SLOT_RING1 = 9,
	SLOT_RING2 = 10,
	SLOT_TRINKET1 = 11,
	SLOT_TRINKET2 = 12,
	SLOT_WEAPON1 = 13,
	SLOT_WEAPON2 = 14,
	SLOT_RANGED = 15,
	SLOT_SPECIAL = 16
};
#define NUM_ITEM_SLOTS 16

/*class Resource {
	int _refCount;

public:
	Resource ( void );
	~Resource ( void );

	void AddReference ( void );
	void RemoveReference ( void );
};
#define _ADD_RES(x) class x : public Resource, public hx##x { public: x ( UINT ID, hxCore* c ) : hx##x ( ID, c ) { } }
namespace RES {
	_ADD_RES ( Image );
	_ADD_RES ( Skeleton );
	_ADD_RES ( Mesh );
	_ADD_RES ( GBufferMaterial );
	_ADD_RES ( Effect );
};*/

class Object {
	friend class ResourceManager;
	friend class Unit;

protected:
	UINT __ID;
	hxObject* obj;

	enum TYPE { TYPE_NONE,			// default
					TYPE_STATIC,		// static (decoration)
					TYPE_DYNAMIC,		// dynamic environment (e.g. elevators)
					TYPE_UNIT,			// a unit (creature/player/boss)
					TYPE_EQUIPMENT }	// equipment (sword, chestplate, ...)
	type;

public:
	Object ( UINT ID, TYPE t ); //id = 0, t = TYPE_NONE
	virtual ~Object ( void );

	virtual void Load ( void );
	virtual void Destroy ( void );
	virtual bool Update ( float fDeltaTime );

	virtual hxOrientationDevice* GetOriDev ( void ) const;
	TYPE GetType ( void ) const;
};

/*class ResourceManager {
	friend class Object;
	static vector<Object*> objects;

	struct DEFRESOURCES {
		hxMesh* defMesh;
		hxGBufferMaterial* defMat;
		hxImage* defTex;
		hxSkeleton* defAnim;
		
		hxMesh* skyboxMesh;
		hxMaterial* skyboxMat;
		hxEffect* skyboxFX;
		hxImage* skyboxTex;

		hxMesh* terrainMesh;
		hxGBufferMaterial* terrainMat;
		hxEffect* terrainFX;
		hxImage* terrainTex;
		hxPixelShader* terrainPS;
	};
	static DEFRESOURCES defResources;

	static class Dungeon* curDungeon;
	static hxFile* dMapFile;

public:
	static hxCore* core;

	static void LoadDefaultResources ( hxCore* const c );
	static void Initialize ( LPCSTR mapFile, Object* player, class Dungeon* dun );
	static void LoadSkybox ( Object* const& obj );
	static void LoadTerrain ( Object* const& obj );
	static void DestroySkybox ( void );
	static void DestroyTerrain ( void );

	static void GetObjectNameByID ( UINT modelID, LPSTR name, UINT ccMax );
	static void LoadObject ( Object* const& obj, UINT modelID );

	static void LoadUnit ( class Unit* const& unit, UINT meshID, UINT skeletonID );

	static void LoadEquipment ( Object* const& obj, ITEMSLOT slot, UINT itemID );
	static void DestroyEquipment ( Object* const& obj );

	static void UpdateObjects ( float fDeltaTime );
	static void Cleanup ( void );
};*/

#endif //_OBJECT_H_