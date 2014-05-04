#include "Desert Arena.h"
#include "..//..//Entities//Unit.h"
#include "..//..//Entities//Player.h"

class BalashtiAI : public CreatureAI {
public:
	BalashtiAI ( void ) : CreatureAI ( 5.0f, 2.5f ) {
	}
	~BalashtiAI ( void ) {
	}
	AI* Clone ( void ) const {
		return new BalashtiAI ( );
	}

	virtual void Apply ( Object* obj, float fDeltaTime ) {
		UnitAI::Apply ( obj, fDeltaTime );
	}
};

DesertArena::DesertArena ( void ) : Map ( ) {
	//load AIs for this map
	AIFactory::AddAIEntry ( new BalashtiAI, "balashti" );
}
DesertArena::~DesertArena ( void ) {
}
void DesertArena::Init ( void ) {
	core->Renderer->EnableFog ( );
	core->Renderer->SetFogRange ( 100.0f, 500.0f );
	core->Renderer->SetFogColor ( hxColor ( 40.0f / 255.0f, 40.0f / 255.0f, 40.0f / 255.0f ) / 2.0f );

	hxDirectionalLight* defLight = core->LightManager->GetDefaultLight ( );
	core->LightManager->DisableShadows ( );
	defLight->Show ( );
	defLight->GetMaterial ( )->SetIntensity ( 1.6f );
	defLight->SetPosition ( 1, 2, 0 );
	defLight->Point ( 0, 0, 0 );

	core->Renderer->SetAmbientColor ( hxColor ( 0.4f ) );
}
void DesertArena::Update ( float fDeltaTime ) {
	Map::Update ( fDeltaTime );
}
void DesertArena::OnLoadObject ( Object* obj ) {
	Map::OnLoadObject ( obj );
}