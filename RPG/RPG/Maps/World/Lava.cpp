#include "Lava.h"

Lava::Lava ( UINT id ) : WorldObject ( id ) {
}
Lava::~Lava ( void ) {
	Destroy ( );
}
void Lava::Load ( void ) {
	WorldObject::Load ( );
	hxPixelShader* ps = new hxPixelShader ( obj->GetCorePtr ( ) );
	ps->LoadFromFile ( "Media\\lava.PS", HX_MAKEPROFILE ( 4, 0 ), "PS" );
	fx = new hxEffect ( 0, obj->GetCorePtr ( ) );
	fx->CopyFrom ( obj->GetGBufferMaterial()->GetEffect ( ) );
	fx->SetPixelShader ( ps );
	obj->GetGBufferMaterial()->SetEffect ( fx );
	ps->RemoveReference ( );
}
void Lava::Destroy ( void ) {
	HX_SAFE_REMOVEREF ( fx );
	WorldObject::Destroy ( );
}
bool Lava::Update ( float fDeltaTime ) {
	bool bParentUpdate = WorldObject::Update ( fDeltaTime );
	return bParentUpdate;
}