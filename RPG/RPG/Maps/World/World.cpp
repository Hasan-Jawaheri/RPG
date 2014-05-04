#include "World.h"
#include "Volcano.h"
#include "Lava.h"
#include "Ground.h"

Skybox::Skybox ( UINT ID ) : Object ( ID, TYPE_STATIC ) {
}
Skybox::~Skybox ( void ) {
}
void Skybox::Load ( void ) {
	struct SKYBOXVERTEX {
		hxVector3 pos;
		hxVector3 tex;

		SKYBOXVERTEX ( void ) {}
		SKYBOXVERTEX ( float x, float y, float z, float u, float v, float w )
			: pos ( x, y, z ), tex ( u, v, w ) {
		}
	};

	obj = new hxObject ( 0, APPHANDLE->core );
	SKYBOXVERTEX* vb = new SKYBOXVERTEX[24];
	DWORD* ib = new DWORD[36];

	//fill the vertex buffer data
	vb[0] = SKYBOXVERTEX ( -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 4 );
	vb[1] = SKYBOXVERTEX ( -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 4 );
	vb[2] = SKYBOXVERTEX ( 0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 4 );
	vb[3] = SKYBOXVERTEX ( 0.5f, -0.5f, -0.5f, -1.0f, 1.0f, 4 );
	vb[4] = SKYBOXVERTEX ( -0.5f, -0.5f, 0.5f, -1.0f, 1.0f, 2 );
	vb[5] = SKYBOXVERTEX ( 0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 2 );
	vb[6] = SKYBOXVERTEX ( 0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 2 );
	vb[7] = SKYBOXVERTEX ( -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 2 );
	vb[8] = SKYBOXVERTEX ( -0.5f, 0.5f, -0.5f, 0, -1, 0 );
	vb[9] = SKYBOXVERTEX ( -0.5f, 0.5f, 0.5f, 0, 0, 0 );
	vb[10] = SKYBOXVERTEX ( 0.5f, 0.5f, 0.5f, 1, 0, 0 );
	vb[11] = SKYBOXVERTEX ( 0.5f, 0.5f, -0.5f, 1, -1, 0 );
	vb[12] = SKYBOXVERTEX ( -0.5f, -0.5f, -0.5f, 1.0f, -1.0f, 5 );
	vb[13] = SKYBOXVERTEX ( 0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 5 );
	vb[14] = SKYBOXVERTEX ( 0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 5 );
	vb[15] = SKYBOXVERTEX ( -0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 5 );
	vb[16] = SKYBOXVERTEX ( -0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 1 );
	vb[17] = SKYBOXVERTEX ( -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1 );
	vb[18] = SKYBOXVERTEX ( -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 1 );
	vb[19] = SKYBOXVERTEX ( -0.5f, -0.5f, -0.5f, -1.0f, 1.0f, 1 );
	vb[20] = SKYBOXVERTEX ( 0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 3 );
	vb[21] = SKYBOXVERTEX ( 0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 3 );
	vb[22] = SKYBOXVERTEX ( 0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 3 );
	vb[23] = SKYBOXVERTEX ( 0.5f, -0.5f, 0.5f, -1.0f, 1.0f, 3 );

	//scale the vertices
	for ( int i = 0; i < 24; i++ )
		vb[i].pos *= 10000.0f;
	for ( unsigned int i = 0; i < 24; i++ ) {
		if ( abs ( vb[i].pos.x ) > 0.5f )
			vb[i].pos.x *= 0.998f;
		else if ( abs ( vb[i].pos.y ) > 0.5f )
			vb[i].pos.y *= 0.998f;
		else if ( abs ( vb[i].pos.z ) > 0.5f )
			vb[i].pos.z *= 0.998f;
	}

	//allocate 36 DWORDs for the indices of the cube
	ib = new DWORD[36];

	//fill cube's index data
	ib[0] = 0; ib[1] = 1; ib[2] = 2;
	ib[3] = 0; ib[4] = 2; ib[5] = 3;
	ib[6] = 4; ib[7] = 5; ib[8] = 6;
	ib[9] = 4; ib[10] = 6; ib[11] = 7;
	ib[12] = 8; ib[13] = 9; ib[14] = 10;
	ib[15] = 8; ib[16] = 10; ib[17] = 11;
	ib[18] = 12; ib[19] = 13; ib[20] = 14;
	ib[21] = 12; ib[22] = 14; ib[23] = 15;
	ib[24] = 16; ib[25] = 17; ib[26] = 18;
	ib[27] = 16; ib[28] = 18; ib[29] = 19;
	ib[30] = 20; ib[31] = 21; ib[32] = 22;
	ib[33] = 20; ib[34] = 22; ib[35] = 23;

	mesh = new hxMesh ( 0, APPHANDLE->core );
	mesh->CreateFromBuffers ( vb, ib, 24, 36, false, sizeof SKYBOXVERTEX );
	delete vb;
	delete ib;
	obj->SetMesh ( mesh );

	const char* filenames[6] = {
		"Media\\top.bmp",
		"Media\\left.bmp",
		"Media\\front.bmp",
		"Media\\right.bmp",
		"Media\\back.bmp",
		"Media\\bot.bmp"
	};
	tex = new hxImage ( 0, APPHANDLE->core );
	tex->Load ( filenames, 6 );

	fx = new hxEffect ( 0, APPHANDLE->core );
	fx->LoadFromHXE ( "Media\\skyboxFX.HXE", false );

	material = new hxMaterial ( 0, ResourceManager::core );
	material->SetEffect ( fx );
	material->SetCullingMode ( CULL_FRONT );
	material->SetTexture ( 0, tex );

	obj->SetGBufferMaterial ( nullptr );
	obj->SetMaterial ( material );
}
void Skybox::Destroy ( void ) {
	HX_SAFE_REMOVEREF ( material );
	HX_SAFE_REMOVEREF ( mesh );
	HX_SAFE_REMOVEREF ( fx );
	HX_SAFE_REMOVEREF ( tex );
	HX_SAFE_REMOVEREF ( obj );
}
bool Skybox::Update ( float fDeltaTime ) {
	hxCamera* cam = ResourceManager::core->CameraManager->GetDefaultCamera ( );
	obj->SetPosition ( cam->GetPositionX ( ), 0, cam->GetPositionZ ( ) );
	return true; //never delete the skybox
}

Terrain::Terrain ( UINT ID ) : Object ( ID, TYPE_STATIC ) {
}
Terrain::~Terrain ( void ) {
}
void Terrain::Load ( void ) {
	/*obj = new hxObject ( 0, ResourceManager::core );
	ResourceManager::LoadTerrain ( this );*/
}
void Terrain::Destroy ( void ) {
	/*ResourceManager::DestroyTerrain ( );
	HX_SAFE_REMOVEREF ( obj );*/
}
bool Terrain::Update ( float fDeltaTime ) {
	hxCamera* cam = ResourceManager::core->CameraManager->GetDefaultCamera ( );
	obj->GetGBufferMaterial ( )->SetConstantBufferVariableFloatArray ( "camPos", 3, cam->GetPosition ( ) );
	obj->SetPosition ( cam->GetPositionX ( ), 0, cam->GetPositionZ ( ) );
	return true; //never delete the terrain
}

World::World ( void ) : Map ( ) {
	//load AIs for this map
}
World::~World ( void ) {
}
void World::Init ( void ) {
	Map::Init ( ); //default environment
	sky = new Skybox ( );
	sky->Load ( );
	//ground = new Terrain ( );
	//ground->Load ( );
}
void World::Update ( float fDeltaTime ) {
	Map::Update ( fDeltaTime );
}
void World::OnLoadObject ( Object* obj ) {
	Map::OnLoadObject ( obj );
}
WorldObject* World::CreateObject ( UINT modelID ) {
	char name[256];
	ResourceManager::GetModelName ( modelID, name, 256 );

	if ( strcmp ( name, "volcano" ) == 0 ) {
		return new Volcano ( modelID );
	} else if ( strcmp ( name, "lavalevel" ) == 0 ) {
		return new Lava ( modelID );
	} else if ( strcmp ( name, "terrain" ) == 0 ) {
		return new Ground ( modelID );
	}

	return Map::CreateObject ( modelID );
}