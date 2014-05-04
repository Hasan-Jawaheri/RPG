#include "Map.h"
#include "MapLoader.h"
#include "..//Entities//Player.h"

#include "Dungeons//Demon Lord's Lair.h"
#include "Dungeons//Test Map.h"
#include "Dungeons//Arena.h"
#include "Dungeons//Desert Arena.h"
#include "World//World.h"

WorldObject::WorldObject ( UINT id ) : Object ( 0, TYPE_STATIC ) {
	modelID = id;
	loadingPos = hxVector3 ( 0, 0, 0 );
}
WorldObject::~WorldObject ( void ) {
	Destroy ( );
}
void WorldObject::Load ( void ) {
	ALLOCATED_MODEL m = ResourceManager::AllocModel ( modelID );
	obj = m.obj;
	m.rb->SetPosition ( loadingPos );
	Map::Get()->OnLoadObject ( this );
}
void WorldObject::Destroy ( void ) {
	Map::Get()->OnDestroyObject ( this );
	ResourceManager::ReleaseModel ( obj, modelID );
}
bool WorldObject::Update ( float fDeltaTime ) {
	return Object::Update ( fDeltaTime );
}
void WorldObject::SetLoadingPosition ( hxVector3 pos ) {
	loadingPos = pos;
}

Map* Map::curMap = nullptr;

Map::Map ( void ) {
	core = APPHANDLE->core;
}
Map::~Map ( void ) {
}
void Map::Init ( void ) {
	core->Renderer->EnableFog ( );
	core->Renderer->SetFogRange ( 400.0f, 3000.0f );
	core->Renderer->SetFogColor ( hxColor ( 50.0f / 255.0f, 85.0f / 255.0f, 135.0f / 255.0f ) / 2.0f );

	core->LightManager->DisableShadows ( );
	core->LightManager->GetDefaultLight ( )->Hide ( );
	core->Renderer->SetAmbientColor ( hxColor ( 0.1f ) );
}
void Map::Update ( float fDeltaTime ) {
	Player* player = UnitManager::GetPlayer ( );
	if ( player && player->GetOriDev ( ) )
		MapLoader::Update ( fDeltaTime, player->GetOriDev()->GetPosition ( ) );
	for ( int i = 0; i < objects.size ( ); i++ ) {
		if ( !objects[i]->Update ( fDeltaTime ) ) {
			delete objects[i];
			objects.erase ( objects.begin ( ) + i );
			i--;
		}
	}
}
void Map::OnLoadObject ( Object* obj ) {
	objects.push_back ( obj );
}
void Map::OnDestroyObject ( Object* obj ) {
	for ( int i = 0; i < objects.size ( ); i++ ) {
		if ( objects[i] == obj ) {
			objects.erase ( objects.begin ( ) + i );
			return;
		}
	}
}
WorldObject* Map::CreateObject ( UINT modelID ) {
	return new WorldObject ( modelID );
}
void Map::Cleanup ( void ) {
	for ( int i = 0; i < objects.size ( ); i++ )
		delete objects[i];
	objects.clear ( );
}
Map* Map::Get ( void ) {
	return curMap;
}
void Map::Set ( UINT mapID ) {
	if ( curMap ) {
		curMap->Cleanup ( );
		delete curMap;
	}

	//when the map changes all AIs will be deleted since they dont exist in a different map
	//if they do, they will be reloaded without a problem
	AIFactory::ClearAIEntries ( );

	//world loader will keep track of world chunks.
	//it will load/unload them and their components
	MapLoader::SetMap ( mapID );
	char name[64];
	MapLoader::GetMapName ( name, 64 );

	if ( strcmp ( name, "dungeon_DLL" ) == 0 )
		curMap = new DemonLordsLair ( );
	else if ( strcmp ( name, "dungeon_test" ) == 0 )
		curMap = new TestMap ( );
	else if ( strcmp ( name, "arena" ) == 0 )
		curMap = new Arena ( );
	else if ( strcmp ( name, "desert_arena" ) == 0 )
		curMap = new DesertArena ( );
	else
		curMap = new World ( );

	//initialize the current dungeon
	curMap->Init ( );

	//position the player at the spawn point
	Player* player = UnitManager::GetPlayer ( );
	if ( player->GetAI ( ) )
		player->GetAI()->SetPosition ( MapLoader::GetSpawnPoint ( ) );
	else if ( player->GetOriDev ( ) )
		player->GetOriDev()->SetPosition ( MapLoader::GetSpawnPoint ( ) );
}