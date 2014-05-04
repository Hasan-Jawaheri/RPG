#ifndef _WORLD_WORLDLOADER_H_
#define _WORLD_WORLDLOADER_H_

#include "Map.h"

class MapLoader {
	struct MAPHDR {
		UINT posInFile;
		char name[64];
	};
	struct pMapHeader {
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
	struct pMapChunk {
		UINT size;
		vector<UINT> objectIDs;
	};
	struct ActiveChunk {
		UINT id;
		float fTimer;
		vector<UINT> objects;
		vector<UINT> lights;
	};
	struct LoadedObject {
		class WorldObject* obj;
		int numChunks;
	};
	struct LoadedLight {
		hxLight* l;
		UINT ID;
		int numChunks;
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
	static FILE* pMapFile;
	static vector<MAPHDR> maps;
	static UINT curMapID;

	static pMapHeader pmh;
	static vector<ActiveChunk> curChunks;
	static vector<LoadedObject> loadedObjects;
	static vector<LoadedLight> loadedLights;
	static vector<UINT> loadedUnits;

public:

	static void Init ( void );
	static void SetMap ( UINT mapID );
	static void Update ( float fDeltaTime, hxVector3 playerPos );
	static void Cleanup ( void );

	static void LoadChunk ( UINT chunkIndex );
	static void UnloadChunk ( ActiveChunk chunk );

	static hxVector3 GetSpawnPoint ( void );
	static void GetMapName ( LPSTR name, size_t ccMax );
};

#endif //_WORLD_WORLDLOADER_H_