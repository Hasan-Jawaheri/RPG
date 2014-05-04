#include "Arena.h"

Arena::Arena ( void ) : Map ( ) {
	//load AIs for this map
}
Arena::~Arena ( void ) {
}
void Arena::Init ( void ) {
	core->Renderer->EnableFog ( );
	core->Renderer->SetFogRange ( 400.0f, 3000.0f );
	core->Renderer->SetFogColor ( hxColor ( 50.0f / 255.0f, 85.0f / 255.0f, 135.0f / 255.0f ) / 2.0f );

	core->LightManager->DisableShadows ( );
	core->LightManager->GetDefaultLight()->Hide ( );
	core->Renderer->SetAmbientColor ( hxColor ( 0.1f ) );
}
void Arena::Update ( float fDeltaTime ) {
	Map::Update ( fDeltaTime );
}
void Arena::OnLoadObject ( Object* obj ) {
	Map::OnLoadObject ( obj );
}