#include "Test Map.h"

TestMap::TestMap ( void ) : Map ( ) {
	//load AIs for this map
}
TestMap::~TestMap ( void ) {
}
void TestMap::Update ( float fDeltaTime ) {
	Map::Update ( fDeltaTime );
}
void TestMap::OnLoadObject ( Object* obj ) {
	Map::OnLoadObject ( obj );
}