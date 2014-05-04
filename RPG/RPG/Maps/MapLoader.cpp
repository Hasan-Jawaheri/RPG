#include "MapLoader.h"
#include "..//Entities//Unit.h"

FILE* MapLoader::pMapFile = nullptr;
vector<MapLoader::MAPHDR> MapLoader::maps;
UINT MapLoader::curMapID = 0;

MapLoader::pMapHeader MapLoader::pmh;
vector<MapLoader::ActiveChunk> MapLoader::curChunks;
vector<MapLoader::LoadedObject> MapLoader::loadedObjects;
vector<MapLoader::LoadedLight> MapLoader::loadedLights;
vector<UINT> MapLoader::loadedUnits;

void MapLoader::Init ( void ) {
	Cleanup ( );
	fopen_s ( &pMapFile, "Media\\Data\\maps", "rb" );

	UINT numMaps;
	fread ( (char*)&numMaps, 4, 1, pMapFile );
	for ( UINT i = 0; i < numMaps; i++ ) {
		MAPHDR hdr;
		fread ( &hdr, sizeof MAPHDR, 1, pMapFile );
		maps.push_back ( hdr );
	}

	SetMap ( 0 );
}
void MapLoader::SetMap ( UINT mapID ) {
	curMapID = mapID;
	if ( curMapID >= maps.size ( ) )
		curMapID = 0;

	//remove all chunks
	while ( curChunks.size ( ) )
		UnloadChunk ( curChunks[0] );

	fseek ( pMapFile, maps[curMapID].posInFile, SEEK_SET );
	fread_s ( (char*)&pmh, sizeof pMapHeader, 4 * sizeof UINT, 1, pMapFile );
	for ( UINT i = 0; i < pmh.xChunks * pmh.yChunks; i++ ) {
		UINT u;
		fread_s ( (char*)&u, sizeof UINT, sizeof UINT, 1, pMapFile );
		pmh.chunks.push_back ( u );
	}
	fread_s ( (char*)&pmh.meshesPos, sizeof pMapHeader, sizeof UINT, 1, pMapFile );
	fread_s ( (char*)&pmh.lightsPos, sizeof pMapHeader, sizeof UINT, 1, pMapFile );
	fread_s ( (char*)&pmh.unitsPos, sizeof pMapHeader, sizeof UINT, 1, pMapFile );
	fread_s ( (char*)&pmh.spawnPos, sizeof pMapHeader, sizeof hxVector3, 1, pMapFile );
}
void MapLoader::Update ( float fDeltaTime, hxVector3 playerPos ) {
	float fHalfViewRange = 20000.0f;
	float fChunkDecayTimer = 10000.0f; //10 seconds
	hxVector2 pos = hxVector2 ( playerPos.x, playerPos.z );
	pos.x = max ( min ( pos.x, pmh.mapWidth ), 0 );
	pos.y = max ( min ( pos.y, pmh.mapHeight ), 0 );
	
	float xchunksize = pmh.mapWidth / pmh.xChunks;
	float ychunksize = pmh.mapHeight / pmh.yChunks;
	hxVector2 vx = hxVector2 ( pos.x - fHalfViewRange, pos.x + fHalfViewRange );
	hxVector2 vy = hxVector2 ( pos.y - fHalfViewRange, pos.y + fHalfViewRange );
	float X1 = vx.x / xchunksize; //floor it
	float X2 = vx.y / xchunksize; //floor it + 1
	float Y1 = vy.x / ychunksize; //floor it
	float Y2 = vy.y / ychunksize; //floor it + 1
	X1 = max ( min ( X1, pmh.xChunks ), 0 );
	X2 = max ( min ( X2, pmh.xChunks ), 0 );
	Y1 = max ( min ( Y1, pmh.yChunks ), 0 );
	Y2 = max ( min ( Y2, pmh.yChunks ), 0 );
	for ( UINT i = floor ( X1 ); i < floor ( X2 ) + (X2-(int)X2 > 0.001f ? 1 : 0); i++ ) {
		for ( UINT j = floor ( Y1 ); j < floor ( Y2 ) + (Y2-(int)Y2 > 0.001f ? 1 : 0); j++ ) {
			UINT index = j * pmh.xChunks + i;
			bool bExists = false;
			for ( UINT k = 0; k < curChunks.size ( ); k++ ) {
				if ( curChunks[k].id == index ) {
					curChunks[k].fTimer = CURGAMETIME;
					bExists = true;
					break;
				}
			}
			if ( !bExists ) { //newly reached chunk, add it
				LoadChunk ( index );
			}
		}
	}
	//remove chunks out of view for a certain time
	for ( int i = 0; i < curChunks.size ( ); i++ ) {
		if ( curChunks[i].fTimer + fChunkDecayTimer < CURGAMETIME ) {
			UnloadChunk ( curChunks[i] );
			i--;
		}
	}
}
void MapLoader::Cleanup ( void ) {
	//remove all chunks
	while ( curChunks.size ( ) )
		UnloadChunk ( curChunks[0] );

	if ( pMapFile )
		fclose ( pMapFile );
	pMapFile = nullptr;
}
void MapLoader::LoadChunk ( UINT chunkIndex ) {
	ActiveChunk nc;
	nc.id = chunkIndex;
	nc.fTimer = CURGAMETIME;

	UINT chunkPosInFile = pmh.chunks[nc.id] + maps[curMapID].posInFile; //aapply offset from map
	UINT chunkSize = 0;
	fseek ( pMapFile, chunkPosInFile, 0 );
	fread ( (char*)&chunkSize, sizeof UINT, 1, pMapFile );
	if ( chunkSize ) {
		UINT* ids = new UINT[chunkSize / 4];
		fread ( (char*)ids, (chunkSize / 4) * sizeof UINT, 1, pMapFile );
		for ( UINT i = 0; i < chunkSize / 4; i++ ) {
			UINT ID = ids[i] & 0x3FFFFFFF;
			UINT type = (ids[i] & 0xC0000000) >> 30;
			if ( type == 0 ) { //object
				MESH m;
				fseek ( pMapFile, maps[curMapID].posInFile + pmh.meshesPos + ID * sizeof MESH, 0 );
				fread ( (char*)&m, sizeof MESH, 1, pMapFile );

				nc.objects.push_back ( m.modelID );
				if ( !APPHANDLE->core->ObjectManager->GetEntity ( m.modelID ) ) {
					WorldObject* obj = Map::Get()->CreateObject ( m.modelID );
					obj->SetLoadingPosition ( m.pos );
					obj->Load ( );
					LoadedObject o;
					o.numChunks = 1;
					o.obj = obj;
					loadedObjects.push_back ( o );
				} else {
					for ( UINT j = 0; j < loadedObjects.size ( ); j++ ) {
						if ( loadedObjects[j].obj->modelID == m.modelID ) {
							loadedObjects[j].numChunks++;
							break;
						}
					}
				}
			} else if ( type == 1 ) { //light
				nc.lights.push_back ( ID );
				bool bLoaded = false;
				for ( UINT j = 0; j < loadedLights.size ( ) && !bLoaded; j++ )
					if ( loadedLights[j].ID == ID )
						bLoaded = true;
				if ( !bLoaded ) {
					LIGHT l;
					fseek ( pMapFile, maps[curMapID].posInFile + pmh.lightsPos + ID * sizeof LIGHT, 0 );
					fread ( (char*)&l, sizeof LIGHT, 1, pMapFile );
					hxLight* hxl = nullptr;
					switch ( l.type ) {
					case 0: //point
						hxl = new hxPointLight ( 0, APPHANDLE->core );
						((hxPointLight*)hxl)->SetPosition ( l.pos );
						((hxPointLight*)hxl)->SetRange ( l.fRange * 5 );
						((hxPointLight*)hxl)->GetMaterial()->SetColor ( l.col / 4 );
						break;
					case 1: //directional
						break;
					case 2: //spot
						hxl = new hxSpotLight ( 0, APPHANDLE->core );
						((hxSpotLight*)hxl)->SetPosition ( l.pos );
						((hxSpotLight*)hxl)->Point ( l.tar );
						((hxSpotLight*)hxl)->SetRange ( l.fRange * 2 );
						((hxSpotLight*)hxl)->GetMaterial()->SetColor ( l.col );
						break;
					}
					hxl->SetName ( l.name );
					LoadedLight loaded;
					loaded.l = hxl;
					loaded.ID = ID;
					loaded.numChunks = 1;
					loadedLights.push_back ( loaded );
				} else {
					for ( UINT j = 0; j < loadedLights.size ( ); j++ ) {
						if ( loadedLights[j].ID == ID ) {
							loadedLights[j].numChunks++;
							break;
						}
					}
				}
			} else if ( type == 2 ) { //unit
				bool bLoaded = false;
				for ( UINT i = 0; i < loadedUnits.size ( ) && !bLoaded; i++ )
					if ( loadedUnits[i] == ID )
						bLoaded = true;
				if ( !bLoaded ) {
					loadedUnits.push_back ( ID );
					UNIT u;
					fseek ( pMapFile, maps[curMapID].posInFile + pmh.unitsPos + ID * sizeof UNIT, 0 );
					fread ( (char*)&u, sizeof UNIT, 1, pMapFile );
					Unit* unit = UnitManager::LoadUnit ( u.unitID );
					if ( unit ) {
						if ( unit->GetAI ( ) ) {
							unit->GetAI()->SetAngle ( u.fAng );
							unit->GetAI()->SetPosition ( u.pos );
						} else {
							unit->GetOriDev()->Yaw ( u.fAng );
							unit->GetOriDev()->SetPosition ( u.pos );
						}
					}
				}
			}
		}
		delete ids;
	}
	curChunks.push_back ( nc );
}

void MapLoader::UnloadChunk ( ActiveChunk chunk ) {
	for ( UINT i = 0; i < chunk.objects.size ( ); i++ ) {
		for ( UINT j = 0; j < loadedObjects.size ( ); j++ ) {
			if ( chunk.objects[i] == loadedObjects[j].obj->modelID ) {
				loadedObjects[j].numChunks--;
				if ( loadedObjects[j].numChunks == 0 ) {
					HX_SAFE_DELETE ( loadedObjects[j].obj );
					loadedObjects.erase ( loadedObjects.begin ( ) + j );
				}
				break;
			}
		}
	}
	for ( UINT i = 0; i < chunk.lights.size ( ); i++ ) {
		for ( UINT j = 0; j < loadedLights.size ( ); j++ ) {
			if ( chunk.lights[i] == loadedLights[j].ID ) {
				loadedLights[j].numChunks--;
				if ( loadedLights[j].numChunks == 0 ) {
					HX_SAFE_DELETE ( loadedLights[j].l );
					loadedLights.erase ( loadedLights.begin ( ) + j );
				}
				break;
			}
		}
	}

	for ( UINT i = 0; i < curChunks.size ( ); i++ ) {
		if ( curChunks[i].id == chunk.id ) {
			curChunks.erase ( curChunks.begin ( ) + i );
			break;
		}
	}
}

hxVector3 MapLoader::GetSpawnPoint ( void ) {
	return pmh.spawnPos;
}
void MapLoader::GetMapName ( LPSTR name, size_t ccMax ) {
	strcpy_s ( name, ccMax, maps[curMapID].name );
}