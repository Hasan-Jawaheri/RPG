#include "Main.h"
#include "FParser.h"
#include "File Formats.h"

#ifdef IOS_REF
	#undef  IOS_REF
	#define IOS_REF (*(pManager->GetIOSettings()))
#endif

struct VERTEX {
	hxVector3 pos;
	hxVector3 tang;
	hxVector3 norm;
	hxVector2 uv;
};
struct MESHDATA {
	vector<VERTEX> vb;
	vector<DWORD> ib;
	vector<void*> ab; //unused
	bool bTangents;
	LPCSTR name;
	UINT ID;
	hxVector3 pos;
	hxVector3 vmin;
	hxVector3 vmax;
};
struct mapHeader {
	UINT mapWidth;
	UINT mapHeight;
	UINT xChunks;
	UINT yChunks;
	vector<UINT> chunks;
	UINT meshesPos;
	UINT lightsPos;
	UINT unitsPos;
	hxVector3 spawnPos;
};
struct mapChunk {
	UINT size;
	//last 2 bits are used for type: 00 = object, 01 = light, 10 = unit
	vector<UINT> objectIDs;
};
struct MESH {
	hxVector3 pos;
	UINT modelID;
};
struct LIGHT {
	char name[64];
	hxVector3 pos;
	hxVector3 tar;
	hxColor col;
	float fRange;
	BYTE type;
};
struct UNIT {
	hxVector3 pos;
	float fAng;
	UINT unitID;
};
struct ANIMATIONRANGE {
	char name[16];
	UINT params[3];
};
#define UNITDATASIZENOVECTOR 148
struct UNITDATA {
	char name[64];
	char AIName[64];
	UINT modelID;
	UINT skeletonID;
	UINT health;
	UINT mana;
	float scale;
	vector<ANIMATIONRANGE> animationData;
};
struct BASICLOADEDUNIT {
	char name[64];
	UINT ID;
};
struct EQUIPMENTDATA {
	char name[64];
	UINT modelID[2];
	UINT damage[2];
	UINT armor;
	char type, slot;
};

struct MODEL {
	char name[64];
	UINT meshes[4];
	UINT textures[4];
	UINT effect;
	UINT material;
	UINT rigidBody;
};

vector<BASICLOADEDUNIT> basicLoadedUnits;

vector<char*> imagesToLoad;
vector<char*> meshesToLoad;
vector<char*> skeletonsToLoad;
vector<hxRigidBody*> rigidBodies;
vector<MODEL> models;

MESHDATA ParseMesh ( FbxMesh* pMesh, float WORLDSCALE );
void CalculateTangents ( vector<VERTEX>& vb );
hxMesh* CreateMesh ( hxCore* core, MESHDATA data );

void InitializeSdkObjects ( FbxManager*& pManager, FbxScene*& pScene );
void DestroySdkObjects ( FbxManager* pManager, bool pExitStatus );
bool LoadScene ( FbxManager* pManager, FbxDocument* pScene, const char* pFilename );

//==================================
#define HEADERSIZE pow ( 2, 16 )//==
//#define WORLDSCALE  3.0f      //==
//#define WORLDCHUNKSX 1        //==
//#define WORLDCHUNKSY 1        //==
//==================================

UINT curImageID = 50000;
UINT curMeshID = 50000;
UINT curSkeletonID = 50000;

UINT __GetMeshFileIndex ( LPCSTR filename ) {
	for ( UINT i = 0; i < meshesToLoad.size ( ); i++ )
		if ( strcmp ( meshesToLoad[i], filename ) == 0 )
			return i;
	char* c = new char[64];
	strcpy_s ( c, 64, filename );
	meshesToLoad.push_back ( c );
	return meshesToLoad.size ( ) - 1;
}
UINT __GetTextureFileIndex ( LPCSTR filename ) {
	for ( UINT i = 0; i < imagesToLoad.size ( ); i++ )
		if ( strcmp ( imagesToLoad[i], filename ) == 0 )
			return i;
	char* c = new char[64];
	strcpy_s ( c, 64, filename );
	imagesToLoad.push_back ( c );
	return imagesToLoad.size ( ) - 1;
}
UINT GetModelID ( LPCSTR modelname, LPCSTR tex = nullptr, LPCSTR meshname = nullptr, UINT rigidBodyIndex = -1 ) {
	for ( UINT i = 0; i < models.size ( ); i++ ) {
		if ( strcmp ( modelname, models[i].name ) == 0 )
			return i;
	}

	MODEL m;
	strcpy_s ( m.name, 64, modelname );
	for ( UINT i = 0; i < 4; i++ ) {
		m.meshes[i] = -1;
		m.textures[i] = -1;
	}
	m.effect = -1;
	m.rigidBody = rigidBodyIndex;
	m.material = -1;

	m.meshes[0] = __GetMeshFileIndex ( meshname == nullptr? modelname : meshname );
	m.textures[0] = __GetTextureFileIndex ( tex == nullptr ? modelname : tex );

	models.push_back ( m );
	return models.size ( ) - 1;
}

UINT GetSkeletonFileIndex ( LPCSTR filename ) {
	for ( UINT i = 0; i < skeletonsToLoad.size ( ); i++ )
		if ( strcmp ( skeletonsToLoad[i], filename ) == 0 )
				return i;
	char* c = new char[64];
	strcpy_s ( c, 64, filename );
	skeletonsToLoad.push_back ( c );
	return skeletonsToLoad.size ( ) - 1;
}

UNITDATA CreateUnit ( Query* header, Query* q ) {
	UNITDATA ret;
	strcpy_s ( ret.name, 64, "" );
	strcpy_s ( ret.AIName, 64, "" );
	ret.health = 100;
	ret.mana = 100;
	ret.scale = 100;
	ret.modelID = -1;

	for ( UINT i = 0; i < NUM_UNIT_FIELDS; i++ ) {
		char* att = (char*)header->GetField ( i );
		char* params = (char*)q->GetField ( i );
		if ( params == nullptr && q->GetFieldType ( i ) == FIELD_STRING )
			continue;

		if ( strcmp ( att, "name" ) == 0 ) {
			strcpy_s ( ret.name, 64, params );
		} else if ( strcmp ( att, "modelID" ) == 0 ) {
			ret.modelID = (int)params;
		} else if ( strcmp ( att, "model" ) == 0 ) {
			ret.modelID = GetModelID ( params );
		} else if ( strcmp ( att, "skeleton" ) == 0 ) {
			ret.skeletonID = GetSkeletonFileIndex ( params );
		} else if ( strcmp ( att, "AI" ) == 0 ) {
			strcpy_s ( ret.AIName, 64, params );
		} else if ( strcmp ( att, "health" ) == 0 ) {
			ret.health = (int)params;
		} else if ( strcmp ( att, "mana" ) == 0 ) {
			ret.mana = (int)params;
		} else if ( strcmp ( att, "scale" ) == 0 ) {
			memcpy ( &ret.scale, (void*)&params, 4 );
		} else {
			if ( strlen ( att ) > strlen ( "anim_" ) ) {
				if ( att[0] == 'a' && att[1] == 'n' && att[2] == 'i' && att[3] == 'm' && att[4] == '_' ) {
					ANIMATIONRANGE r;
					strcpy_s ( r.name, 16, &att[5] );
					ZeroMemory ( &r.params, 3 * sizeof UINT );
					UINT curParam = 0;
					UINT paramPos = 0;
					for ( UINT i = 0; i <= strlen ( params ); i++ ) {
						if ( params[i] == '-' || params[i] == '\0' ) {
							char oldC = params[i];
							params[i] = '\0';
							r.params[curParam++] = atol ( &params[paramPos] );
							paramPos = i + 1;
							params[i] = oldC;
						}
					}
					ret.animationData.push_back ( r );
				}
			}
		}
	}

	return ret;
}
void CreateUnitData ( LPCSTR outFile, FbxScene* pScene, hxCore* core ) {
	Query* header = nullptr; {
		FParser fp;
		fp.Initialize ( "units.csv", new HeaderQuery ( NUM_UNIT_FIELDS ) );
		header = fp.ParseLine ( 0 );
	}
	vector<UNITDATA> units;
	FParser fp;
	fp.Initialize ( "units.csv", new UnitsQuery ( ) );
	for ( UINT i = 1; i < fp.GetNumLines ( ); i++ ) {
		Query* q = fp.ParseLine ( i );
		units.push_back ( CreateUnit ( header, q ) );
		delete q;
	}
	delete header;

	fstream file;
	file.open ( outFile, ios::out | ios::binary );
	if ( file.is_open ( ) ) {
		//chunks start after the header
		UINT headerSize = sizeof ( UINT ) * ( 1 + units.size ( ) );
		UINT curChunkPos = headerSize;
		UINT s = units.size ( );
		file.write ( (char*)&s, sizeof UINT );
		for ( UINT i = 0; i < units.size ( ); i++ ) {
			file.write ( (char*)&curChunkPos, sizeof UINT );
			curChunkPos += UNITDATASIZENOVECTOR + sizeof UINT + units[i].animationData.size ( ) * sizeof ANIMATIONRANGE;
		}
		for ( UINT i = 0; i < units.size ( ); i++ ) {
			BASICLOADEDUNIT blu;
			blu.ID = i;
			strcpy_s ( blu.name, 64, units[i].name );
			basicLoadedUnits.push_back ( blu );
			file.write ( (char*)&units[i], UNITDATASIZENOVECTOR );
			s = units[i].animationData.size ( );
			file.write ( (char*)&s, sizeof UINT );
			for ( UINT j = 0; j < s; j++ )
				file.write ( (char*)&units[i].animationData[j], sizeof ANIMATIONRANGE );
		}
		file.close ( );
	}
}

EQUIPMENTDATA CreateItem ( Query* header, Query* q ) {
	EQUIPMENTDATA ret;
	char texName[256] = { '\0' };
	strcpy_s ( ret.name, 64, "" );
	ret.armor = 0;
	ret.damage[0] = 0;
	ret.damage[1] = 0;
	ret.type = 0;
	ret.slot = 16;
	ret.modelID[0] = -1;
	ret.modelID[1] = -1;

	if ( q->GetField ( 5 ) != nullptr ) {
		strcpy ( texName, (char*)q->GetField ( 5 ) );
	}

	for ( UINT i = 0; i < NUM_EQUIPMENT_FIELDS; i++ ) {
		char* att = (char*)header->GetField ( i );
		char* params = (char*)q->GetField ( i );
		if ( params == nullptr && q->GetFieldType ( i ) == FIELD_STRING )
			continue;

		if ( strcmp ( att, "name" ) == 0 ) {
			strcpy_s ( ret.name, 64, params );
		} else if ( strcmp ( att, "modelID" ) == 0 ) {
			ret.modelID[0] = (int)params;
		} else if ( strcmp ( att, "model" ) == 0 ) {
			if ( strcmp ( texName, "" ) == 0 )
				strcpy ( texName, params );
			ret.modelID[0] = GetModelID ( params, texName );
		} else if ( strcmp ( att, "modelL" ) == 0 ) {
			if ( strcmp ( texName, "" ) == 0 )
				strcpy ( texName, params );
			ret.modelID[0] = GetModelID ( params, texName );
		} else if ( strcmp ( att, "modelR" ) == 0 ) {
			if ( strcmp ( texName, "" ) == 0 )
				strcpy ( texName, params );
			ret.modelID[1] = GetModelID ( params, texName );
		} else if ( strcmp ( att, "slot" ) == 0 ) {
			ret.slot = (int)params;
		} else if ( strcmp ( att, "type" ) == 0 ) {
			ret.type = (int)params;
		} else if ( strcmp ( att, "armor" ) == 0 ) {
			ret.armor = (int)params;
		} else if ( strcmp ( att, "damage" ) == 0 ) {
			sscanf ( params, "%u-%u", &ret.damage[0], &ret.damage[1] );
		} else {
		}
	}

	return ret;
}
void CreateEquipmentData ( LPCSTR outFile, FbxScene* pScene, hxCore* core ) {
	Query* header = nullptr; {
		FParser fp;
		fp.Initialize ( "equipment.csv", new HeaderQuery ( NUM_EQUIPMENT_FIELDS ) );
		header = fp.ParseLine ( 0 );
	}
	vector<EQUIPMENTDATA> items;
	FParser fp;
	fp.Initialize ( "equipment.csv", new EquipmentQuery ( ) );
	for ( UINT i = 1; i < fp.GetNumLines ( ); i++ ) {
		Query* q = fp.ParseLine ( i );
		items.push_back ( CreateItem ( header, q ) );
		delete q;
	}
	delete header;

	fstream file;
	file.open ( outFile, ios::out | ios::binary );
	if ( file.is_open ( ) ) {
		for ( UINT i = 0; i < items.size ( ); i++ ) {
			file.write ( (char*)&items[i], sizeof ( EQUIPMENTDATA ) );
		}
		file.close ( );
	}
}

UINT GetUnitIndex ( LPCSTR name ) {
	for ( UINT i = 0; i < basicLoadedUnits.size ( ); i++ )
		if ( strcmp ( name, basicLoadedUnits[i].name ) == 0 )
			return basicLoadedUnits[i].ID;
	return -1;
}

void CreateMapData ( LPCSTR fileName, LPCSTR _mapName, FbxScene* pScene, hxCore* core,
	float WORLDSCALE, int WORLDCHUNKSX, int WORLDCHUNKSY ) {
	
	FbxAxisSystem::DirectX.ConvertScene ( pScene );
	FbxAxisSystem::ECoordSystem sys = pScene->GetGlobalSettings ( ).GetAxisSystem ( ).GetCoorSystem ( );

	FbxNode* lNode = pScene->GetRootNode ( );

	char outFile[MAX_PATH], mapName[MAX_PATH];
	strcpy_s ( outFile, MAX_PATH, fileName );
	for ( UINT i = 0; i < strlen ( outFile ); i++ )
	if ( outFile[i] == '.' )
		outFile[i] = '\0';
	for ( UINT i = 0; i < strlen ( _mapName ) + 1; i++ ) {
		mapName[i] = _mapName[i];
		if ( mapName[i] == '.' ) {
			mapName[i] = '\0';
			break;
		}
	}
				
	if ( lNode ) {
		vector<MESHDATA> meshesData;
		/*vector<hxMesh*> meshes;
		vector<hxImage*> images;
		vector<hxEffect*> effects;
		vector<hxGBufferMaterial*> materials;
		vector<hxObject*> objects;
		vector<hxRigidBody*> rigidBodies;*/
		vector<LIGHT> lights;
		vector<UNIT> units;
		hxVector3 spawnPoint;
		const int nameMaxSize = 1024;
		for ( int i = 0; i < lNode->GetChildCount ( ); i++ ) {
			FbxNode* pNode = lNode->GetChild ( i );
			FbxNodeAttribute::EType lAttributeType;
			if ( pNode->GetNodeAttribute ( ) != NULL ) {
				lAttributeType = pNode->GetNodeAttribute()->GetAttributeType ( );
				if ( lAttributeType == FbxNodeAttribute::eMesh ) {
					FbxMesh* lMesh = (FbxMesh*) pNode->GetNodeAttribute ( );
					MESHDATA mesh = ParseMesh ( lMesh, WORLDSCALE );
					if ( mesh.vb.size ( ) ) {
						if ( mesh.ib.size ( ) ) {
							if ( !mesh.bTangents )
								CalculateTangents ( mesh.vb );

							char uniqueMeshName[64];
							sprintf_s ( uniqueMeshName, 64, "_%s_%u_", mapName, i + 1 );
							mesh.name = (char*)pNode->GetName ( );
							mesh.ID = GetModelID ( uniqueMeshName, mesh.name, uniqueMeshName, rigidBodies.size ( ) );
							meshesData.push_back ( mesh );
							hxMesh* m = CreateMesh ( core, mesh );
							m->SetName ( uniqueMeshName );
							hxRigidBody* rb = new hxRigidBody ( 0, core );
							rb->SetName ( mesh.name );
							rb->BuildFromMeshes ( RIGIDBODY_COMPLEX, &m, 1, true, false );
							rigidBodies.push_back ( rb );
							
							/*hxImage* img = nullptr;
							char imgFile[256];
							char imgFile2[256];
							strcpy_s ( imgFile, 256, mesh.name );
							strcpy_s ( imgFile2, 256, mesh.name );
							strcat_s ( imgFile, 256, ".bmp" );
							strcat_s ( imgFile2, 256, ".png" );
							FILE* fptr;
							fopen_s ( &fptr, imgFile, "rb" );
							if ( fptr ) {
								fclose ( fptr );
								img = new hxImage ( mesh.ID, core );
								img->Load ( imgFile );
							} else {
								fopen_s ( &fptr, imgFile2, "rb" );
								if ( fptr ) {
									fclose ( fptr );
									img = new hxImage ( mesh.ID, core );
									img->Load ( imgFile2 );
								}
							}
							if ( img ) {
								img->SetName ( mesh.name );
								images.push_back ( img );
							}

							hxGBufferMaterial* mat = new hxGBufferMaterial ( mesh.ID, core );
							mat->SetName ( mesh.name );
							mat->Texture ( img );
							materials.push_back ( mat );
							hxObject* obj = new hxObject ( mesh.ID, core );
							obj->SetName ( mesh.name );
							obj->SetMesh ( meshes[meshes.size()-1] );
							obj->SetGBufferMaterial ( mat );
							obj->SetPosition ( mesh.pos );
							objects.push_back ( obj );*/
						}
					}
				} else if ( lAttributeType == FbxNodeAttribute::eLight ) {
					LIGHT light;
					strcpy_s ( light.name, 64, pNode->GetName ( ) );
					FbxLight* l = (FbxLight*) pNode->GetNodeAttribute ( );
					FbxDouble3 col = l->Color.Get ( );
					light.col = hxColor ( col[0], col[1], col[2] );
					FbxDouble3 pos = pNode->LclTranslation.Get ( );
					pos[0] *= WORLDSCALE; pos[1] *= WORLDSCALE; pos[2] *= WORLDSCALE;
					light.pos = hxVector3 ( pos[0], pos[1], pos[2] );
					UINT type = l->LightType.Get ( );
					light.type = type;
					if ( type == 2 ) { //spot
						FbxDouble3 tar = pNode->GetTarget()->LclTranslation.Get ( );
						tar[0] *= WORLDSCALE; tar[1] *= WORLDSCALE; tar[2] *= WORLDSCALE;
						light.tar = hxVector3 ( tar[0], tar[1], tar[2] );
						light.fRange = hxVec3Length ( light.pos - light.tar );
					} else if ( type == 1 ) { //directional
					} else { //point
						FbxDouble range = l->DecayStart.Get ( );
						light.fRange = range;
					}
					lights.push_back ( light );
				} else {
					FbxDouble3 pos = pNode->LclTranslation.Get ( );
					pos[0] *= WORLDSCALE; pos[1] *= WORLDSCALE; pos[2] *= WORLDSCALE;
					char command[1024];
					strcpy_s ( command, 1024, pNode->GetName ( ) );
					if ( strcmp ( command, "spawn_point" ) == 0 ) {
						spawnPoint = hxVector3 ( pos[0], pos[1], pos[2] );
					} else {
						int colonPos = -1;
						for ( UINT i = 0; i < strlen ( command ) && colonPos == -1; i++ )
							if ( command[i] == ':' )
								colonPos = i;
						if ( colonPos != -1 ) {
							char cmd[32];
							char param[256];
							memcpy ( cmd, command, colonPos );
							cmd[colonPos] = '\0';
							strcpy_s ( param, 256, command + colonPos + 1 );
							if ( strcmp ( cmd, "unit" ) == 0 ) {
								UNIT u;
								u.pos = hxVector3 ( pos[0], pos[1], pos[2] );
								u.fAng = 0.0f;
								u.unitID = GetUnitIndex ( param );
								units.push_back ( u );
							} else {
							}
						}
					}
				}
			}
		}

		//
		// Map file: object positions for a map
		//
		mapHeader mapData;
		hxVector3 absMin = hxVector3 ( FLT_MAX, 0, FLT_MAX );
		hxVector3 absMax = -absMin;
		for ( UINT i = 0; i < meshesData.size ( ); i++ )
		{
			MESHDATA m = meshesData[i];
			absMin.x = min ( absMin.x, m.pos.x + m.vmin.x );
			absMin.z = min ( absMin.z, m.pos.z + m.vmin.z );
			absMax.x = max ( absMax.x, m.pos.x + m.vmax.x );
			absMax.z = max ( absMax.z, m.pos.z + m.vmax.z );
		}
		for ( UINT i = 0; i < lights.size ( ); i++ )
		{
			LIGHT l = lights[i];
			absMin.x = min ( absMin.x, l.pos.x - l.fRange / 2 );
			absMin.z = min ( absMin.z, l.pos.z - l.fRange / 2 );
			absMax.x = max ( absMax.x, l.pos.x + l.fRange / 2 );
			absMax.z = max ( absMax.z, l.pos.z + l.fRange / 2 );
		}
		//shift the system to be all positive 0-based
		spawnPoint -= absMin;
		for ( UINT i = 0; i < meshesData.size ( ); i++ ) {
			meshesData[i].pos -= absMin;
			//meshesData[i].vmin -= absMin; <--- those are relative to position
			//meshesData[i].vmax -= absMin;
		}
		/*for ( int j = 0; j < objects.size ( ); j++ )      mal awwal
			objects[j]->SetPosition ( objects[j]->GetPosition ( ) - absMin );*/
		for ( UINT i = 0; i < lights.size ( ); i++ ) {
			lights[i].pos -= absMin;
			lights[i].tar -= absMin;
		}
		for ( UINT i = 0; i < units.size ( ); i++ )
			units[i].pos -= absMin;
		mapData.spawnPos = spawnPoint;
		mapData.mapWidth = absMax.x - absMin.x;
		mapData.mapHeight = absMax.z - absMin.z;
		mapData.xChunks = WORLDCHUNKSX;
		mapData.yChunks = WORLDCHUNKSY;

		float xchunksize = mapData.mapWidth / mapData.xChunks;
		float ychunksize = mapData.mapHeight / mapData.yChunks;
		vector<mapChunk> chunks ( mapData.xChunks * mapData.yChunks );
		for ( UINT i = 0; i < chunks.size ( ); i++ )
			chunks[i].size = 0;
		for ( UINT ind = 0; ind < meshesData.size ( ) + lights.size ( ) + units.size ( ); ind++ )
		{
			UINT ID;
			hxVector2 vx;
			hxVector2 vy;
			if ( ind < meshesData.size ( ) ) {
				MESHDATA m = meshesData[ind];
				vx = hxVector2 ( m.vmin.x, m.vmax.x ) + hxVector2 ( m.pos.x, m.pos.x );
				vy = hxVector2 ( m.vmin.z, m.vmax.z ) + hxVector2 ( m.pos.z, m.pos.z );
				ID = ind | 0x00000000;
			} else if ( ind < meshesData.size ( ) + lights.size ( ) ) {
				LIGHT l = lights[ind-meshesData.size ( )];
				vx = hxVector2 ( -l.fRange / 2, l.fRange / 2 ) + hxVector2 ( l.pos.x, l.pos.x );
				vy = hxVector2 ( -l.fRange / 2, l.fRange / 2 ) + hxVector2 ( l.pos.z, l.pos.z );
				ID = (ind-meshesData.size ( )) | 0x40000000;
			} else {
				UNIT u = units[ind-(meshesData.size ( ) + lights.size ( ))];
				vx = hxVector2 ( u.pos.x, u.pos.x );
				vy = hxVector2 ( u.pos.z, u.pos.z );
				ID = (ind-(meshesData.size ( ) + lights.size ( ))) | 0x80000000;
			}
			float X1 = vx.x / xchunksize; //floor it
			float X2 = vx.y / xchunksize; //floor it
			float Y1 = vy.x / ychunksize; //floor it
			float Y2 = vy.y / ychunksize; //floor it
			for ( UINT i = floor ( X1 ); i < floor ( X2 ) + (X2-(int)X2 > 0.01f ? 1 : 0); i++ ) {
				for ( UINT j = floor ( Y1 ); j < floor ( Y2 ) + (Y2-(int)Y2 > 0.01f ? 1 : 0); j++ ) {
					UINT index = j * mapData.xChunks + i;
					chunks[index].objectIDs.push_back ( ID );
					chunks[index].size += 4;
				}
			}
		}
		//chunks start after the header
		UINT headerSize = sizeof ( UINT ) * ( 7 + chunks.size ( ) ) + sizeof hxVector3;
		UINT curChunkPos = headerSize;
		for ( UINT i = 0; i < chunks.size ( ); i++ ) {
			mapData.chunks.push_back ( curChunkPos );
			curChunkPos += chunks[i].size + sizeof UINT;
		}
		mapData.meshesPos = curChunkPos;
		mapData.lightsPos = mapData.meshesPos + meshesData.size ( ) * sizeof MESH;
		mapData.unitsPos = mapData.lightsPos + lights.size ( ) * sizeof LIGHT;

		strcat_s ( outFile, 256, ".map" );

		FILE* fp;
		fopen_s ( &fp, outFile, "wb" );
		if ( fp ) {
			fwrite ( (char*)&mapData, sizeof ( UINT ), 4, fp );
			for ( UINT i = 0; i < mapData.chunks.size ( ); i++ )
				fwrite ( (char*)&mapData.chunks[i], sizeof ( UINT ), 1, fp );
			fwrite ( (char*)&mapData.meshesPos, sizeof ( UINT ), 1, fp );
			fwrite ( (char*)&mapData.lightsPos, sizeof ( UINT ), 1, fp );
			fwrite ( (char*)&mapData.unitsPos, sizeof ( UINT ), 1, fp );
			fwrite ( (char*)&mapData.spawnPos, sizeof ( hxVector3 ), 1, fp );
			for ( UINT i = 0; i < chunks.size ( ); i++ ) {
				fwrite ( (char*)&chunks[i].size, sizeof ( UINT ), 1, fp );
				for ( UINT j = 0; j < chunks[i].objectIDs.size ( ); j++ )
					fwrite ( (char*)&chunks[i].objectIDs[j], sizeof ( UINT ), 1, fp );
			}
			for ( UINT i = 0; i < meshesData.size ( ); i++ ) {
				MESH m;
				m.pos = meshesData[i].pos;
				m.modelID = meshesData[i].ID;
				fwrite ( (char*)&m, sizeof ( MESH ), 1, fp );
			}
			for ( UINT i = 0; i < lights.size ( ); i++ )
				fwrite ( (char*)&lights[i], sizeof ( LIGHT ), 1, fp );
			for ( UINT i = 0; i < units.size ( ); i++ )
				fwrite ( (char*)&units[i], sizeof ( UNIT ), 1, fp );
			fclose ( fp );
		}
	}
}

void CreateModelData ( hxCore* core, LPCSTR modelsFile, LPCSTR assetsFile ) {
	//
	// First file: models file
	//
	FILE* fp = nullptr;
	fopen_s ( &fp, modelsFile, "wb" );
	if ( fp ) {
		UINT size = models.size ( );
		fwrite ( &size, 4, 1, fp );
		fwrite ( &models[0], sizeof MODEL, size, fp );
		fclose ( fp );
	}

	//
	// Second file: raw object data
	//
	hxImage* checker = new hxImage ( 0, core );
	checker->Load ( "checker.bmp" );

	UINT headerSize = HEADERSIZE;
	DeleteFileA ( assetsFile );
	hxFile* f = new hxFile ( 0, core );
	f->Initialize ( assetsFile, &headerSize );
	vector<hxMesh*> meshes;
	vector<hxImage*> images;
	vector<hxEffect*> effects;
	vector<hxGBufferMaterial*> materials;
	vector<hxSkeleton*> skeletons;
	for ( UINT i = 0; i < meshesToLoad.size ( ); i++ ) {
		if ( meshesToLoad[i][0] != '_' ) {
			hxMesh* m = new hxMesh ( curMeshID++, core );
			m->SetName ( meshesToLoad[i] );
			char filename[256];
			strcpy_s ( filename, 256, meshesToLoad[i] );
			strcat_s ( filename, 256, ".HXM" );
			if ( m->LoadFromHXM ( filename ) != HX_SUCCEED )
				m->CreateCube ( 2.0f );
			meshes.push_back ( m );
		} else {
			meshes.push_back ( core->MeshManager->GetEntity ( meshesToLoad[i] ) );
		}
	}
	for ( UINT i = 0; i < imagesToLoad.size ( ); i++ ) {
		char imgFile[256];
		char imgFile2[256];
		char imgFile3[256];
		strcpy_s ( imgFile, 256, imagesToLoad[i] );
		strcpy_s ( imgFile2, 256, imagesToLoad[i] );
		strcpy_s ( imgFile3, 256, imagesToLoad[i] );
		strcat_s ( imgFile, 256, ".bmp" );
		strcat_s ( imgFile2, 256, ".png" );
		strcat_s ( imgFile3, 256, ".jpg" );
		fopen_s ( &fp, imgFile, "rb" );
		hxImage* img = nullptr;
		if ( fp ) {
			fclose ( fp );
			img = new hxImage ( curImageID++, core );
			img->Load ( imgFile );
		} else {
			fopen_s ( &fp, imgFile2, "rb" );
			if ( fp ) {
				fclose ( fp );
				img = new hxImage ( curImageID++, core );
				img->Load ( imgFile2 );
			} else {
				fopen_s ( &fp, imgFile3, "rb" );
				if ( fp ) {
					fclose ( fp );
					img = new hxImage ( curImageID++, core );
					img->Load ( imgFile3 );
				}
			}
		}
		if ( img ) {
			img->SetName ( imagesToLoad[i] );
			images.push_back ( img );
		} else {
			images.push_back ( checker );
		}
	}
	for ( UINT i = 0; i < skeletonsToLoad.size ( ); i++ ) {
		char filename[256];
		strcpy_s ( filename, 256, skeletonsToLoad[i] );
		strcat_s ( filename, 256, ".HXS" );
		hxSkeleton* s = new hxSkeleton ( curSkeletonID++, core );
		s->SetName ( skeletonsToLoad[i] );
		s->LoadFromHXS ( filename );
		skeletons.push_back ( s );
	}
	for ( int j = 0; j < meshes.size ( ); j++ )
		f->SaveMesh ( meshes[j] );
	for ( int j = 0; j < images.size ( ); j++ )
		f->SaveImage ( images[j] );
	for ( int j = 0; j < effects.size ( ); j++ )
		f->SaveEffect ( effects[j] );
	for ( int j = 0; j < materials.size ( ); j++ )
		f->SaveMaterial ( materials[j] );
	for ( int j = 0; j < rigidBodies.size ( ); j++ )
		f->SaveRigidBody ( rigidBodies[j] );
	for ( int j = 0; j < skeletons.size ( ); j++ )
		f->SaveSkeleton ( skeletons[j] );
	f->Close ( );
}

void CombineMapFiles ( LPCSTR outFile ) {
	FILE* fptr = nullptr;
	fopen_s ( &fptr, outFile, "wb" );
	if ( fptr ) {
		vector<UINT> fileSizes;
		vector<char*> mapNames;
		vector<char*> fileData;
		UINT totalSize = 0;

		_WIN32_FIND_DATAA data;
		HANDLE h = FindFirstFileA ( "Output//*.map", &data );
		if ( h ) {
			do {
				char filename[256];
				strcpy_s ( filename, 256, "Output//" );
				strcat_s ( filename, 256, data.cFileName );
				FILE* m;
				fopen_s ( &m, filename, "rb" );
				if ( m ) {
					fseek ( m, 0, SEEK_END );
					UINT pos = ftell ( m );
					fileSizes.push_back ( pos );
					totalSize += pos;

					char* name = new char[64];
					for ( UINT i = 0; i < strlen ( data.cFileName ) + 1; i++ ) {
						name[i] = data.cFileName[i];
						if ( name[i] == '.' ) {
							name[i] = '\0';
							break;
						}
					}
					mapNames.push_back ( name );

					char* d = new char[pos];
					fseek ( m, 0, SEEK_SET );
					fread ( d, 1, pos, m );
					fileData.push_back ( d );
					fclose ( m );
					DeleteFileA ( filename );
				}
			} while ( FindNextFileA ( h, &data ) );
		}

		UINT numMaps = fileSizes.size ( );
		UINT curPosInFile = 4 + (numMaps * (4 + 64));
		fwrite ( &numMaps, 4, 1, fptr );
		for ( UINT i = 0; i < fileSizes.size ( ); i++ ) {
			fwrite ( &curPosInFile, 4, 1, fptr );
			curPosInFile += fileSizes[i];
			fwrite ( mapNames[i], 1, 64, fptr );
		}
		for ( UINT i = 0; i < fileData.size ( ); i++ )
			fwrite ( fileData[i], fileSizes[i], 1, fptr );
		fclose ( fptr );

		for ( UINT i = 0; i < mapNames.size ( ); i++ )
			delete mapNames[i];
		for ( UINT i = 0; i < fileData.size ( ); i++ )
			delete fileData[i];
	}
}

Application::Application ( void ) : HasX11 ( ) { //call the base constructor
	//TODO: Add anything that you need in the constructor
}

bool Application::Setup ( void ) {
	//
	//Initialize the core. This must be done to create the window and initialize it
	//
	int width = 640;	//640 pixels wide
	int height = 480;	//480 pixels high
	bool vsync = true;//enable vsync (prevents screen tearing)
	HX_ERROR err = core->Init ( width, height, vsync );
	if ( err != HX_SUCCEED ) { //An error occured. Very rare but should always be checked
		//retrieve the error as a string and print it
		char errorStr[256];
		hxGetError ( err, errorStr, 256 );
		MessageBoxA ( nullptr, errorStr, "My Application", MB_ICONERROR | MB_OK );
		return false; //returning false from Setup will stop the application
	}
	core->PhysicsDevice->Init ( 1.0f, 20000.0f, /*true*/false );
	core->PhysicsDevice->Start ( );
	core->PhysicsDevice->RigidBodyManager->EnableMoppSaving ( );
	
	FbxManager* pManager = nullptr;
	FbxScene* pScene = nullptr;
	InitializeSdkObjects ( pManager, pScene );

	CreateUnitData ( "Output//units", pScene, core );
	CreateEquipmentData ( "Output//equipment", pScene, core );

	_WIN32_FIND_DATAA data;
	HANDLE h = FindFirstFileA ( "*.FBX", &data );
	if ( h ) {
		do {
			char file[256];
			strcpy ( file, "Output//" );
			strcat ( file, data.cFileName );
			if ( strcmp ( data.cFileName, "World.FBX" ) != 0 ) {
				if ( LoadScene ( pManager, pScene, data.cFileName ) )
					CreateMapData ( file, data.cFileName, pScene, core, 0.3f, 1, 1 );
			} else {
				if ( LoadScene ( pManager, pScene, data.cFileName ) )
					CreateMapData ( file, data.cFileName, pScene, core, 12.0f, 7, 7 );
			}
		} while ( FindNextFileA ( h, &data ) );
	}

	CreateModelData ( core, "Output//models", "Output//assets" );
	CombineMapFiles ( "Output//maps" );

	DestroySdkObjects ( pManager, 0 );

	return true;
}

bool Application::Loop ( float fDeltaTime ) {

	//
	//TODO: Add code that updates the game
	//

	//core->PhysicsDevice->Step ( fDeltaTime );
	return false; //returning true will allow the application to continue
}

void Application::Cleanup ( void ) {
	//
	//TODO: Free any created resources
	//Note that HasX11 Engine will cleanup all hx* classes for you
	//(except hx***Shader that you allocate explicitly). But it's
	//always a good practice to cleanup the mess.
	//There is nothing to free here in this tutorial.
	//
}

//******************************************************************************************
//This function is always needed. Every new HasX11 will be registered by the HasX11 system
//and will be automatically freed. Each new HasX11 class will run on its own thread. This
//can be used for multiuthreading. Note that the HasX11 class is completely generic and can
//run without core->Init and still work, thus you don't need to have 2 windows to use this
//multithreading feature.
//******************************************************************************************
void HasX11Init ( void ) {
	new Application; //The base construction, HasX11(), will take care of the rest.
}



MESHDATA ParseMesh ( FbxMesh* pMesh, float WORLDSCALE )
{
	MESHDATA ret;
	FbxDouble3 d = pMesh->GetNode()->LclTranslation.Get ( );
	ret.pos = hxVector3 ( d[0], d[1], d[2] ) * WORLDSCALE;

	FbxAMatrix FBXmeshMtx = pMesh->GetNode()->EvaluateGlobalTransform ( );
	FbxAMatrix mx1 = FBXmeshMtx;
	hxMatrix meshMtx = hxMatrix (	mx1.Get ( 0, 0 ), mx1.Get ( 0, 1 ), mx1.Get ( 0, 2 ), mx1.Get ( 0, 3 ),
											mx1.Get ( 1, 0 ), mx1.Get ( 1, 1 ), mx1.Get ( 1, 2 ), mx1.Get ( 1, 3 ),
											mx1.Get ( 2, 0 ), mx1.Get ( 2, 1 ), mx1.Get ( 2, 2 ), mx1.Get ( 2, 3 ),
											mx1.Get ( 3, 0 ), mx1.Get ( 3, 1 ), mx1.Get ( 3, 2 ), mx1.Get ( 3, 3 ) );
	for ( int i = 0; i < 12; i++ )
		*((float*)(((float*)&meshMtx)+i)) *= -1;
	FBXmeshMtx.SetRow ( 0, FbxVector4 ( meshMtx._11, meshMtx._21, meshMtx._31, meshMtx._41 ) );
	FBXmeshMtx.SetRow ( 1, FbxVector4 ( meshMtx._12, meshMtx._22, meshMtx._32, meshMtx._42 ) );
	FBXmeshMtx.SetRow ( 2, FbxVector4 ( meshMtx._13, meshMtx._23, meshMtx._33, meshMtx._43 ) );
	FBXmeshMtx.SetRow ( 3, FbxVector4 ( meshMtx._14, meshMtx._24, meshMtx._34, meshMtx._44 ) );

	int lPolygonCount = pMesh->GetPolygonCount ( );
	FbxVector4* lControlPoints = pMesh->GetControlPoints ( );

	FbxVector4* allVerts = new FbxVector4[pMesh->GetControlPointsCount ( )];

	int vbSize = 0;
	for ( int i = 0; i < lPolygonCount; i++ ) {
		int lPolygonSize = pMesh->GetPolygonSize ( i );
		if ( lPolygonSize != 3 ) {
			printf ( "Error: non-triangular polygon found, skipping." );
			continue;
		}
		//for ( int j = lPolygonSize-1; j >= 0; j-- ) {
		for ( int j = 0; j < lPolygonSize; j++ ) {
			int lControlPointIndex = pMesh->GetPolygonVertex(i, j);
			FbxVector4 norm;
			pMesh->GetPolygonVertexNormal ( i, j, norm );
			allVerts[lControlPointIndex] = norm;
			ret.ib.push_back ( lControlPointIndex );
			vbSize = max ( vbSize, lControlPointIndex + 1 );
		}
	}

	for ( int i = 0; i < vbSize; i++ )
		ret.vb.push_back ( VERTEX ( ) );

	hxVector3 maxVert ( -FLT_MAX, -FLT_MAX, -FLT_MAX );
	hxVector3 minVert = -maxVert;
	vector<DWORD> repeatedVerts;
	for ( UINT i = 0; i < ret.ib.size ( ); i++ ) {
		int lControlPointIndex = ret.ib[i];
		bool bRepeated = false;
		for ( UINT k = 0; k < repeatedVerts.size ( ) && !bRepeated; k++ )
			if ( repeatedVerts[k] == lControlPointIndex )
				bRepeated = true;
		if ( bRepeated )
			continue;
		repeatedVerts.push_back ( lControlPointIndex );

		FbxVector4 pos = lControlPoints[lControlPointIndex];
		FbxVector2 uv ( 0, 0 );
		FbxVector4 norm ( 0, 1, 0, 0 );
		FbxVector4 tang ( 1, 0, 0, 0 );

		if ( pMesh->GetElementUVCount ( ) ) {
			FbxGeometryElementUV* leUV = pMesh->GetElementUV( 0 );

			switch (leUV->GetMappingMode())
			{
			case FbxGeometryElement::eByControlPoint:
				switch (leUV->GetReferenceMode())
				{
				case FbxGeometryElement::eDirect:
					uv = leUV->GetDirectArray().GetAt(lControlPointIndex);
					break;
				case FbxGeometryElement::eIndexToDirect:
					{
						int id = leUV->GetIndexArray().GetAt(lControlPointIndex);
						uv = leUV->GetDirectArray().GetAt(id);
					}
					break;
				default:
					break; // other reference modes not shown here!
				}
				break;

			case FbxGeometryElement::eByPolygonVertex:
				{
					int lTextureUVIndex = pMesh->GetTextureUVIndex(i / 3, i % 3);
					switch (leUV->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
					case FbxGeometryElement::eIndexToDirect:
						{
							uv = leUV->GetDirectArray().GetAt(lTextureUVIndex);
						}
						break;
					default:
						break; // other reference modes not shown here!
					}
				}
				break;

			case FbxGeometryElement::eByPolygon: // doesn't make much sense for UVs
			case FbxGeometryElement::eAllSame:   // doesn't make much sense for UVs
			case FbxGeometryElement::eNone:       // doesn't make much sense for UVs
				break;
			}
		}
		/*if ( pMesh->GetElementNormalCount ( ) ) {
			FbxGeometryElementNormal* leNormal = pMesh->GetElementNormal ( 0 );

			if(leNormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
			{
				switch (leNormal->GetReferenceMode())
				{
				case FbxGeometryElement::eDirect:
					norm = leNormal->GetDirectArray().GetAt(lControlPointIndex);
					break;
				case FbxGeometryElement::eIndexToDirect:
					{
						int id = leNormal->GetIndexArray().GetAt(lControlPointIndex);
						norm = leNormal->GetDirectArray().GetAt(id);
					}
					break;
				default:
					break; // other reference modes not shown here!
				}
			}

		}*/
		norm = allVerts[lControlPointIndex];
		if ( pMesh->GetElementTangentCount ( ) ) {
			FbxGeometryElementTangent* leTangent = pMesh->GetElementTangent ( 0 );

			if(leTangent->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
			{
				switch (leTangent->GetReferenceMode())
				{
				case FbxGeometryElement::eDirect:
					tang = leTangent->GetDirectArray().GetAt(lControlPointIndex);
					ret.bTangents = true;
					break;
				case FbxGeometryElement::eIndexToDirect:
					{
						int id = leTangent->GetIndexArray().GetAt(lControlPointIndex);
						tang = leTangent->GetDirectArray().GetAt(id);
						ret.bTangents = true;
					}
					break;
				default:
					break; // other reference modes not shown here!
				}
			}

		}
		
		hxVector3 p = hxVector3 ( pos[0], pos[1], pos[2] );
		hxVector3 n = hxVector3 ( norm[0], norm[1], norm[2] );
		hxVector3 t = hxVector3 ( tang[0], tang[1], tang[2] );
		p = hxVec3TransformCoord ( p, meshMtx ) * WORLDSCALE - ret.pos;
		n = hxVec3TransformNormal ( n, meshMtx );
		t = hxVec3TransformNormal ( t, meshMtx );
		
		maxVert.x = max ( maxVert.x, p.x );
		maxVert.y = max ( maxVert.y, p.y );
		maxVert.z = max ( maxVert.z, p.z );
		minVert.x = min ( minVert.x, p.x );
		minVert.y = min ( minVert.y, p.y );
		minVert.z = min ( minVert.z, p.z );

		VERTEX v;
		v.pos.x = p.x;
		v.pos.y = p.y;
		v.pos.z = p.z;
		v.uv.x = uv[0];
		v.uv.y = 1.0f - uv[1]; //inverse v coordinate
		v.norm.x = n.x;
		v.norm.y = n.y;
		v.norm.z = n.z;
		v.tang.x = t.x;
		v.tang.y = t.y;
		v.tang.z = t.z;
		ret.vb[ret.ib[i]] = v;
	}
	ret.vmin = minVert;
	ret.vmax = maxVert;

	delete[] allVerts;

	//reverse the winding order
	for ( int i = 0; i < ret.ib.size ( ); i += 3 ) {
		DWORD t = ret.ib[i+0];
		ret.ib[i+0] = ret.ib[i+2];
		ret.ib[i+2] = t;
	}

	return ret;
}
void CalculateTangents ( vector<VERTEX>& vb ) {
	for ( UINT i = 0; i < vb.size ( ); i++ ) {
		hxVector3 forw ( 0, 0, 1 );
		hxVector3 up ( 0, 1, 0 );
		hxVector3 c1 = hxVec3Cross ( vb[i].norm, forw );
		hxVector3 c2 = hxVec3Cross ( vb[i].norm, up );
		hxVector3 tang;

		if ( hxVec3LengthSq ( c1 ) > hxVec3LengthSq ( c2 ) )
			 tang = c1;
		else
			 tang = c2;

		vb[i].tang = hxVec3Normalize ( tang );
	}
}
hxMesh* CreateMesh ( hxCore* core, MESHDATA data ) {
	hxMesh* m = new hxMesh ( curMeshID++, core );
	m->SetName ( data.name );
	m->CreateFromBuffers ( (void*)&data.vb[0], (DWORD*)&data.ib[0], data.vb.size ( ), data.ib.size ( ) );
	return m;
}
void InitializeSdkObjects ( FbxManager*& pManager, FbxScene*& pScene )
{
	//The first thing to do is to create the FBX Manager which is the object allocator for almost all the classes in the SDK
	pManager = FbxManager::Create ( );
	if ( !pManager ) {
		printf ( "Error: Unable to create FBX Manager!\n" );
		exit ( 1 );
	}

	//Create an IOSettings object. This object holds all import/export settings.
	FbxIOSettings* ios = FbxIOSettings::Create ( pManager, IOSROOT );
	pManager->SetIOSettings ( ios );

	//Create an FBX scene. This object holds most objects imported/exported from/to files.
	pScene = FbxScene::Create ( pManager, "My Scene" );
	if ( !pScene )
	{
		printf ( "Error: Unable to create FBX scene!\n" );
		exit ( 1 );
	}
}

void DestroySdkObjects ( FbxManager* pManager, bool pExitStatus )
{
	//Delete the FBX Manager.
	//All the objects that have been allocated using the FBX Manager and
	//that haven't been explicitly destroyed are also automatically destroyed.
	if ( pManager ) pManager->Destroy ( );
	if ( pExitStatus ) printf ( "Program Success!\n" );
}

bool LoadScene ( FbxManager* pManager, FbxDocument* pScene, const char* pFilename )
{
    int lFileMajor, lFileMinor, lFileRevision;
    int lSDKMajor,  lSDKMinor,  lSDKRevision;
    //int lFileFormat = -1;
    int i, lAnimStackCount;
    bool lStatus;
    char lPassword[1024];

    // Get the file version number generate by the FBX SDK.
    FbxManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);

    // Create an importer.
    FbxImporter* lImporter = FbxImporter::Create(pManager,"");

    // Initialize the importer by providing a filename.
    const bool lImportStatus = lImporter->Initialize(pFilename, -1, pManager->GetIOSettings());
    lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

    if( !lImportStatus )
    {
        FbxString error = lImporter->GetStatus().GetErrorString();
        FBXSDK_printf("Call to FbxImporter::Initialize() failed.\n");
        FBXSDK_printf("Error returned: %s\n\n", error.Buffer());

        if (lImporter->GetStatus().GetCode() == FbxStatus::eInvalidFileVersion)
        {
            FBXSDK_printf("FBX file format version for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);
            FBXSDK_printf("FBX file format version for file '%s' is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);
        }

        return false;
    }

    //FBXSDK_printf("FBX file format version for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);

    if (lImporter->IsFBX())
    {
        FBXSDK_printf("\nFBX file format version for file '%s' is %d.%d.%d\n", pFilename, lFileMajor, lFileMinor, lFileRevision);

        // From this point, it is possible to access animation stack information without
        // the expense of loading the entire file.

        //FBXSDK_printf("Animation Stack Information\n");

        lAnimStackCount = lImporter->GetAnimStackCount();

        //FBXSDK_printf("    Number of Animation Stacks: %d\n", lAnimStackCount);
        //FBXSDK_printf("    Current Animation Stack: \"%s\"\n", lImporter->GetActiveAnimStackName().Buffer());
        //FBXSDK_printf("\n");

        for(i = 0; i < lAnimStackCount; i++)
        {
            FbxTakeInfo* lTakeInfo = lImporter->GetTakeInfo(i);

            //FBXSDK_printf("    Animation Stack %d\n", i);
            //FBXSDK_printf("         Name: \"%s\"\n", lTakeInfo->mName.Buffer());
            //FBXSDK_printf("         Description: \"%s\"\n", lTakeInfo->mDescription.Buffer());

            // Change the value of the import name if the animation stack should be imported 
            // under a different name.
            //FBXSDK_printf("         Import Name: \"%s\"\n", lTakeInfo->mImportName.Buffer());

            // Set the value of the import state to false if the animation stack should be not
            // be imported. 
            //FBXSDK_printf("         Import State: %s\n", lTakeInfo->mSelect ? "true" : "false");
            //FBXSDK_printf("\n");
        }

        // Set the import states. By default, the import states are always set to 
        // true. The code below shows how to change these states.
        IOS_REF.SetBoolProp(IMP_FBX_MATERIAL,        true);
        IOS_REF.SetBoolProp(IMP_FBX_TEXTURE,         true);
        IOS_REF.SetBoolProp(IMP_FBX_LINK,            true);
        IOS_REF.SetBoolProp(IMP_FBX_SHAPE,           true);
        IOS_REF.SetBoolProp(IMP_FBX_GOBO,            true);
        IOS_REF.SetBoolProp(IMP_FBX_ANIMATION,       true);
        IOS_REF.SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
    }

    // Import the scene.
    lStatus = lImporter->Import(pScene);

    if(lStatus == false && lImporter->GetStatus().GetCode() == FbxStatus::ePasswordError)
    {
        FBXSDK_printf("Please enter password: ");

        lPassword[0] = '\0';

        FBXSDK_CRT_SECURE_NO_WARNING_BEGIN
        scanf("%s", lPassword);
        FBXSDK_CRT_SECURE_NO_WARNING_END

        FbxString lString(lPassword);

        IOS_REF.SetStringProp(IMP_FBX_PASSWORD,      lString);
        IOS_REF.SetBoolProp(IMP_FBX_PASSWORD_ENABLE, true);

        lStatus = lImporter->Import(pScene);

        if(lStatus == false && lImporter->GetStatus().GetCode() == FbxStatus::ePasswordError)
        {
            FBXSDK_printf("\nPassword is wrong, import aborted.\n");
        }
    }

    // Destroy the importer.
    lImporter->Destroy();

    return lStatus;
}