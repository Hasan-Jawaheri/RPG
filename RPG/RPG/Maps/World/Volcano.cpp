#include "Volcano.h"

Volcano::Volcano ( UINT id ) : WorldObject ( id ) {
}
Volcano::~Volcano ( void ) {
	Destroy ( );
}
void Volcano::Load ( void ) {
	WorldObject::Load ( );
	hxPixelShader* ps = new hxPixelShader ( obj->GetCorePtr ( ) );
	ps->LoadFromFile ( "Media\\volcano.PS", HX_MAKEPROFILE ( 4, 0 ), "PS" );
	fx = new hxEffect ( 0, obj->GetCorePtr ( ) );
	fx->CopyFrom ( obj->GetGBufferMaterial()->GetEffect ( ) );
	fx->SetPixelShader ( ps );
	obj->GetGBufferMaterial()->SetEffect ( fx );
	ps->RemoveReference ( );
}
void Volcano::Destroy ( void ) {
	HX_SAFE_REMOVEREF ( fx );
	WorldObject::Destroy ( );
}
bool Volcano::Update ( float fDeltaTime ) {
	bool bParentUpdate = WorldObject::Update ( fDeltaTime );
	return bParentUpdate;
}