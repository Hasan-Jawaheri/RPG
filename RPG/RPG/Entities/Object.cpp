#include "Object.h"
//#include "Unit.h"
//#include "..//Dungeons//Dungeon.h"

/*vector<class Object*> ResourceManager::objects;
hxCore* ResourceManager::core = nullptr;
ResourceManager::DEFRESOURCES ResourceManager::defResources;
Dungeon* ResourceManager::curDungeon = nullptr;
hxFile* ResourceManager::dMapFile = nullptr;

void ResourceManager::LoadDefaultResources ( hxCore* const c ) {
	HX_SAFE_REMOVEREF ( defResources.defMesh );
	HX_SAFE_REMOVEREF ( defResources.defTex );
	HX_SAFE_REMOVEREF ( defResources.defAnim );

	core = c;
	
	defResources.defMesh = new hxMesh ( 0, core );
	//defResources.defMesh->CreateCylinder ( 5.0f, 10.0f, 0, 15 );
	//defResources.defMesh->ApplyOffset ( 0, 5.0f, 0 );
	defResources.defMesh->LoadFromHXM ( "Media\\Units\\Character.HXM" );

	defResources.defTex = new hxImage ( 0, core );
	defResources.defTex->Load ( "Media\\Units\\Deathstroke_D.bmp" );

	defResources.defAnim = new hxSkeleton ( 0, core );
	defResources.defAnim->LoadFromHXS ( "Media\\Units\\Character.HXS" );

	hxObject* obj = new hxObject ( 0, core );
	defResources.defMat = obj->GetGBufferMaterial ( );
	HX_SAFE_REMOVEREF ( obj );

	defResources.defMat->Texture ( defResources.defTex );
}

void ResourceManager::Initialize ( LPCSTR mapFile, Object* player, class Dungeon* dun ) {
	if ( dMapFile )
		dMapFile->Close ( );

	for ( UINT i = 0; i < objects.size ( ); i++ )
		if ( objects[i] != player &&
			objects[i]->GetType ( ) != Object::TYPE_EQUIPMENT ) // equipment should not be deleted with the map
			delete objects[i];

	dMapFile = new hxFile ( 0, core );
	dMapFile->Initialize ( mapFile, nullptr, true );
	curDungeon = dun;
}

void ResourceManager::LoadSkybox ( Object* const& obj ) {
	struct SKYBOXVERTEX {
		hxVector3 pos;
		hxVector3 tex;

		SKYBOXVERTEX ( void ) { }
		SKYBOXVERTEX ( float x, float y, float z, float u, float v, float w )
			: pos ( x, y, z ), tex ( u, v, w ) { }
	};
	if ( obj ) {
		SKYBOXVERTEX* vb = new SKYBOXVERTEX[24];
		DWORD* ib = new DWORD[36];
		
		//fill the vertex buffer data
		vb[0] = SKYBOXVERTEX(-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,4);
		vb[1] = SKYBOXVERTEX(-0.5f,  0.5f, -0.5f, 0.0f, 0.0f,4);
		vb[2] = SKYBOXVERTEX( 0.5f,  0.5f, -0.5f, -1.0f, 0.0f,4);
		vb[3] = SKYBOXVERTEX( 0.5f, -0.5f, -0.5f, -1.0f, 1.0f,4);
		vb[4] = SKYBOXVERTEX(-0.5f, -0.5f, 0.5f, -1.0f, 1.0f,2);
		vb[5] = SKYBOXVERTEX( 0.5f, -0.5f, 0.5f, 0.0f, 1.0f,2);
		vb[6] = SKYBOXVERTEX( 0.5f,  0.5f, 0.5f, 0.0f, 0.0f,2);
		vb[7] = SKYBOXVERTEX(-0.5f,  0.5f, 0.5f, -1.0f, 0.0f,2);
		vb[8]  = SKYBOXVERTEX(-0.5f, 0.5f, -0.5f,0,-1,0);
		vb[9]  = SKYBOXVERTEX(-0.5f, 0.5f,  0.5f,0,0,0);
		vb[10] = SKYBOXVERTEX( 0.5f, 0.5f,  0.5f,1,0,0);
		vb[11] = SKYBOXVERTEX( 0.5f, 0.5f, -0.5f,1,-1,0);
		vb[12] = SKYBOXVERTEX(-0.5f, -0.5f, -0.5f, 1.0f, -1.0f,5);
		vb[13] = SKYBOXVERTEX( 0.5f, -0.5f, -0.5f, 0.0f, -1.0f,5);
		vb[14] = SKYBOXVERTEX( 0.5f, -0.5f,  0.5f, 0.0f, 0.0f,5);
		vb[15] = SKYBOXVERTEX(-0.5f, -0.5f,  0.5f, 1.0f, 0.0f,5);
		vb[16] = SKYBOXVERTEX(-0.5f, -0.5f,  0.5f, 0.0f, 1.0f,1);
		vb[17] = SKYBOXVERTEX(-0.5f,  0.5f,  0.5f, 0.0f, 0.0f,1);
		vb[18] = SKYBOXVERTEX(-0.5f,  0.5f, -0.5f, -1.0f, 0.0f,1);
		vb[19] = SKYBOXVERTEX(-0.5f, -0.5f, -0.5f, -1.0f, 1.0f,1);
		vb[20] = SKYBOXVERTEX( 0.5f, -0.5f, -0.5f, 0.0f, 1.0f,3);
		vb[21] = SKYBOXVERTEX( 0.5f,  0.5f, -0.5f, 0.0f, 0.0f,3);
		vb[22] = SKYBOXVERTEX( 0.5f,  0.5f,  0.5f, -1.0f, 0.0f,3);
		vb[23] = SKYBOXVERTEX( 0.5f, -0.5f,  0.5f, -1.0f, 1.0f,3);

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
		ib[6] = 4; ib[7]  = 5; ib[8]  = 6;
		ib[9] = 4; ib[10] = 6; ib[11] = 7;
		ib[12] = 8; ib[13] =  9; ib[14] = 10;
		ib[15] = 8; ib[16] = 10; ib[17] = 11;
		ib[18] = 12; ib[19] = 13; ib[20] = 14;
		ib[21] = 12; ib[22] = 14; ib[23] = 15;
		ib[24] = 16; ib[25] = 17; ib[26] = 18;
		ib[27] = 16; ib[28] = 18; ib[29] = 19;
		ib[30] = 20; ib[31] = 21; ib[32] = 22;
		ib[33] = 20; ib[34] = 22; ib[35] = 23;

		defResources.skyboxMesh = new hxMesh ( 0, ResourceManager::core );
		defResources.skyboxMesh->CreateFromBuffers ( vb, ib, 24, 36, false, sizeof SKYBOXVERTEX );
		delete vb;
		delete ib;
		obj->obj->SetMesh ( defResources.skyboxMesh );

		const char* filenames[6] = {
			"Media\\top.bmp",
			"Media\\left.bmp",
			"Media\\front.bmp",
			"Media\\right.bmp",
			"Media\\back.bmp",
			"Media\\bot.bmp"
		};
		defResources.skyboxTex = new hxImage ( 0, ResourceManager::core );
		defResources.skyboxTex->Load ( filenames, 6 );

		defResources.skyboxFX = new hxEffect ( 0, ResourceManager::core );
		defResources.skyboxFX->LoadFromHXE ( "Media\\skyboxFX.HXE", false );

		defResources.skyboxMat = new hxMaterial ( 0, ResourceManager::core );
		defResources.skyboxMat->SetEffect ( defResources.skyboxFX );
		defResources.skyboxMat->SetCullingMode ( CULL_FRONT );
		defResources.skyboxMat->SetTexture ( 0, defResources.skyboxTex );
	
		obj->obj->SetGBufferMaterial ( nullptr );
		obj->obj->SetMaterial ( defResources.skyboxMat );
	}
}

void ResourceManager::LoadTerrain ( Object* const& obj ) {
	if ( obj ) {
		defResources.terrainMesh = new hxMesh ( 0, ResourceManager::core );
		defResources.terrainMesh->CreatePlain ( 1000.0f, 50, 50 );
		obj->obj->SetMesh ( defResources.terrainMesh );

		defResources.terrainTex = new hxImage ( 0, ResourceManager::core );
		defResources.terrainTex->Load ( "Media\\checker.bmp" );

		defResources.terrainPS = new hxPixelShader ( ResourceManager::core );
		defResources.terrainPS->LoadFromFile ( "Media\\groundPS.PS", HX_MAKEPROFILE ( 4, 0 ), "PS" );
		defResources.terrainFX = new hxEffect ( 0, ResourceManager::core );
		defResources.terrainFX->CopyFrom ( obj->obj->GetGBufferMaterial()->GetEffect ( ) );
		defResources.terrainFX->SetPixelShader ( defResources.terrainPS );

		defResources.terrainMat = new hxGBufferMaterial ( 0, ResourceManager::core );
		defResources.terrainMat->SetEffect ( defResources.terrainFX );
		defResources.terrainMat->Texture ( defResources.terrainTex );
	
		obj->obj->SetGBufferMaterial ( defResources.terrainMat );
	}
}

void ResourceManager::DestroySkybox ( void ) {
	if ( defResources.skyboxFX ) {
		HX_SAFE_REMOVEREF ( defResources.skyboxFX );
		HX_SAFE_REMOVEREF ( defResources.skyboxMat );
		HX_SAFE_REMOVEREF ( defResources.skyboxMesh );
		HX_SAFE_REMOVEREF ( defResources.skyboxTex );
	}
}

void ResourceManager::DestroyTerrain ( void ) {
	if ( defResources.terrainFX ) {
		HX_SAFE_REMOVEREF ( defResources.terrainPS );
		HX_SAFE_REMOVEREF ( defResources.terrainFX );
		HX_SAFE_REMOVEREF ( defResources.terrainMat );
		HX_SAFE_REMOVEREF ( defResources.terrainMesh );
		HX_SAFE_REMOVEREF ( defResources.terrainTex );
	}
}

void ResourceManager::GetObjectNameByID ( UINT modelID, LPSTR name, UINT ccMax ) {
	dMapFile->GetObjectNameByID ( modelID, name, 256 );
}

void ResourceManager::LoadObject ( Object* const& obj, UINT modelID ) {
	if ( obj ) {
		if ( modelID == 0 ) { //default model
			//MOVE TO UNIT MANAGER*************************************************************************
			obj->obj = new hxObject ( 0, ResourceManager::core );
			obj->obj->SetMesh ( defResources.defMesh );
			obj->obj->SetGBufferMaterial ( defResources.defMat );
			obj->obj->Scale ( 20, 20, 20 );
		} else {
			//LOAD MODEL FROM FILE
			if ( !core->ObjectManager->GetEntity ( modelID ) ) {
				dMapFile->LoadMeshByID ( modelID );
				dMapFile->LoadImageByID ( modelID );
				dMapFile->LoadMaterialByID ( modelID );
				dMapFile->LoadObjectByID ( modelID );
				dMapFile->LoadRigidBodyByID ( modelID );
				hxRigidBody* rb = core->PhysicsDevice->RigidBodyManager->GetEntity ( modelID );
				if ( rb )
					rb->SetPosition ( core->ObjectManager->GetEntity ( modelID )->GetPosition ( ) );
				obj->obj = core->ObjectManager->GetEntity ( modelID );
				if ( curDungeon )
					curDungeon->OnLoadObject ( obj );
			}
		}
	}
}

void ResourceManager::LoadUnit ( Unit* const& unit, UINT meshID, UINT skeletonID ) {
	static UINT objID = INT_MAX;
	unit->obj = new hxObject ( objID, core );
	unit->__ID = objID;
	//A skeleton/mesh's ID in HasX11 Engine is INT_MAX + unitID
	if ( hxMesh* m = core->MeshManager->GetEntity ( INT_MAX + meshID ) ) {
		unit->obj->SetMesh ( m ); 
	} else {
		char meshFile[64];
		UnitManager::GetMeshFilename ( meshID, meshFile );
		char fullPath[MAX_PATH];
		strcpy_s ( fullPath, MAX_PATH, "Media\\Units\\" );
		strcat_s ( fullPath, MAX_PATH, meshFile );
		m = new hxMesh ( INT_MAX + skeletonID, core );
		m->LoadFromHXM ( fullPath );
		unit->obj->SetMesh ( m );
		m->RemoveReference ( );
	}

	if ( hxSkeleton* s = (hxSkeleton*)core->AnimationManager->GetEntity ( INT_MAX + skeletonID ) ) {
		hxSkeleton* ns = new hxSkeleton ( INT_MAX + skeletonID, core );
		ns->UseAnimationFrames ( s );
		if ( ns->GetBone ( 0, "backbone" ) ) {
			ns->AddSubAnimation ( );
			ns->SetSubAnimationBaseBone ( 1, ns->GetBone(0,"backbone")->GetIndex ( ), 0 );
		}
		unit->obj->AttachAnimation ( ns );
		unit->anim = ns;
	} else {
		char skeletonFile[64];
		UnitManager::GetSkeletonFilename ( skeletonID, skeletonFile );
		char fullPath[MAX_PATH];
		strcpy_s ( fullPath, MAX_PATH, "Media\\Units\\" );
		strcat_s ( fullPath, MAX_PATH, skeletonFile );
		s = new hxSkeleton ( INT_MAX + skeletonID, core );
		s->LoadFromHXS ( fullPath );
		if ( s->GetBone ( 0, "backbone" ) ) {
			s->AddSubAnimation ( );
			s->SetSubAnimationBaseBone ( 1, s->GetBone(0,"backbone")->GetIndex ( ), 0 );
		}
		unit->obj->AttachAnimation ( s );
		unit->anim = s;
	}
	objID++;


	/ *LoadObject ( unit, 0 );
	hxSkeleton* anim = nullptr;
	if ( unitID == 0 ) { //default animation
		anim = new hxSkeleton ( 0, ResourceManager::core );
		anim->UseAnimationFrames ( ResourceManager::defResources.defAnim );
		anim->SetBindingScale ( 20.0f );
		anim->AddSubAnimation ( );
		anim->SetSubAnimationBaseBone ( 1, anim->GetBone(0,"backbone")->GetIndex ( ), 0 );
	} else {
		//LOAD SKELETON FROM FILE
		int x = 0;
	}
	unit->obj->AttachAnimation ( anim );
	unit->anim = anim;* /
}

//MOVE TO UNIT MANAGER***********************************************************************
void ResourceManager::LoadEquipment ( Object* const& obj, ITEMSLOT slot, UINT itemID ) {
	if ( obj ) {
		if ( itemID == 0 ) { //default item *********************
			//THIS MESH IS NOT MONITORED. SHOULD CHANGE THIS SYSTEM
			hxMesh* m = new hxMesh ( 0, core );
			m->LoadFromHXM ( "Media\\Items\\Sword.HXM" );
			m->Scale ( 5.0f );
			obj->obj->SetMesh ( m );
			m->RemoveReference ( );
			hxGBufferMaterial* mat = new hxGBufferMaterial ( 0, core );
			obj->obj->SetGBufferMaterial ( mat );
			hxImage* tex = new hxImage ( 0, core );
			tex->Load ( "Media\\Items\\sword.png" );
			mat->Texture ( tex );
			mat->RemoveReference ( );
			tex->RemoveReference ( );
		} else {
			//LOAD ITEM FROM FILE
		}
	}
}

//FIX AND MOVE TO UNIT MANAGER***********************************************************************
void ResourceManager::DestroyEquipment ( Object* const& obj ) {
}

void ResourceManager::UpdateObjects ( float fDeltaTime ) {
	for ( UINT i = 0; i < objects.size ( ); i++ ) {
		if ( !objects[i]->Update ( fDeltaTime ) ) {
			delete objects[i];
			i--;
		}
	}
	if ( curDungeon )
		curDungeon->Update ( fDeltaTime );
}

void ResourceManager::Cleanup ( void ) {
	for ( UINT i = 0; i < objects.size ( ); i )
		delete objects[i];

	DestroyTerrain ( );
	DestroySkybox ( );
	HX_SAFE_REMOVEREF ( defResources.defMesh );
	HX_SAFE_REMOVEREF ( defResources.defTex );
	HX_SAFE_REMOVEREF ( defResources.defAnim );
	
	if ( dMapFile )
		dMapFile->Close ( );
	if ( curDungeon )
		delete curDungeon;
}*/

Object::Object ( UINT ID, TYPE t ) {
	__ID = ID;
	obj = nullptr;
	type = t;
}

Object::~Object ( void ) {
	Destroy ( );
}

hxOrientationDevice* Object::GetOriDev ( void ) const {
	return (hxOrientationDevice*)obj;
}
Object::TYPE Object::GetType ( void ) const {
	return type;
}

void Object::Load ( void ) {
	obj = new hxObject ( 0, APPHANDLE->core );
}
bool Object::Update ( float fDeltaTime ) {
	return true;
}

void Object::Destroy ( void ) {
	if ( obj ) {
		HX_SAFE_REMOVEREF ( obj );
	}
}