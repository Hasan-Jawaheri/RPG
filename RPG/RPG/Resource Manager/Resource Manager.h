#ifndef _RESOURCE_MANAGER_H_
#define _RESOURCE_MANAGER_H_

#include "..//Common.h"

//maximum number of resources loaded at one time
#define MAX_NUM_RESOURCES 2000
#define HASHTABLE_SIZE 500
#define DBG_RES_MGR

struct MODEL {
	char name[64];
	UINT meshes[4];
	UINT textures[4];
	UINT effect;
	UINT material;
	UINT rigidBody;
};
struct ALLOCATED_MODEL {
	hxObject* obj;
	hxRigidBody* rb;
};
struct ALLOCATED_UNIT {
	UINT ID;
	hxObject* obj;
	hxSkeleton* skeleton;
};

class ResourceManager {

	//
	// Loader data
	//
	static hxFile* dMapFile;
	static FILE* modelsFile;
	static UINT curObjID;
	static UINT curUnitAnimID;
	
	//
	// Loading utilities
	//
	static hxMesh* _allocMesh ( UINT index );
	static hxImage* _allocImage ( UINT index );
	static hxEffect* _allocEffect ( UINT index );
	static hxGBufferMaterial* _allocMaterial ( UINT index );
	static hxRigidBody* _allocRigidBody ( UINT index );
	static hxSkeleton* _allocSkeleton ( UINT index );

	static void _releaseMesh ( UINT index );
	static void _releaseImage ( UINT index );
	static void _releaseEffect ( UINT index );
	static void _releaseMaterial ( UINT index );
	static void _releaseRigidBody ( UINT index );
	static void _releaseSkeleton ( hxSkeleton* s, UINT index );
	
/*#define _loadResource(r) static hx##r* _load##r ( LPCSTR name, UINT id = 0 );

	_loadResource ( Object );
	_loadResource ( Image );
	_loadResource ( Mesh );
	_loadResource ( Effect );
	_loadResource ( GBufferMaterial );
	_loadResource ( Skeleton );
	_loadResource ( Particles );
	_loadResource ( Sound );
	_loadResource ( RigidBody );

	//
	// Raw resource Allocation
	//
#define AllocResource(r) \
	static hx##r* Alloc##r ( LPCSTR name, bool bEmpty = false ); \
	static hx##r* Alloc##r ( UINT ID, bool bEmpty = false );

	AllocResource ( Object );
	AllocResource ( Image );
	AllocResource ( Mesh );
	AllocResource ( Effect );
	AllocResource ( GBufferMaterial );
	AllocResource ( Skeleton );
	AllocResource ( Particles );
	AllocResource ( Sound );
	AllocResource ( RigidBody );

	//
	// Resource Releasing
	//
#define ReleaseResource(r) static void Release##r ( hxBase* resource, bool bPurge = false );

	ReleaseResource ( Object );
	ReleaseResource ( Image );
	ReleaseResource ( Mesh );
	ReleaseResource ( Effect );
	ReleaseResource ( GBufferMaterial );
	ReleaseResource ( Skeleton );
	ReleaseResource ( Particles );
	ReleaseResource ( Sound );
	ReleaseResource ( RigidBody );*/

public:
	static hxCore* core;
	static void Init ( hxCore* core );
	static void Cleanup ( void );

	static void GetModelName ( UINT modelID, LPSTR name, UINT ccMax );

	//
	// Game asset allocation
	//
	static ALLOCATED_MODEL AllocModel ( UINT modelID );
	static void ReleaseModel ( hxObject* model, UINT modelID );
	static ALLOCATED_UNIT AllocUnit ( UINT modelID, UINT skeletonID );
	static void ReleaseUnit ( ALLOCATED_UNIT unit, UINT modelID, UINT skeletonID );
};

#endif //_RESOURCE_MANAGER_H_