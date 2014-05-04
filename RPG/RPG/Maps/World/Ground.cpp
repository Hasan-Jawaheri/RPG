#include "Ground.h"

Ground::Ground ( UINT id ) : WorldObject ( id ) {
}
Ground::~Ground ( void ) {
	Destroy ( );
}
void Ground::Load ( void ) {
	WorldObject::Load ( );
	hxPixelShader* ps = new hxPixelShader ( obj->GetCorePtr ( ) );
	ps->LoadFromFile ( "Media\\ground.PS", HX_MAKEPROFILE ( 4, 0 ), "PS" );
	fx = new hxEffect ( 0, obj->GetCorePtr ( ) );
	fx->CopyFrom ( obj->GetGBufferMaterial()->GetEffect ( ) );
	fx->SetPixelShader ( ps );
	obj->GetGBufferMaterial()->SetEffect ( fx );
	ps->RemoveReference ( );
}
void Ground::Destroy ( void ) {
	HX_SAFE_REMOVEREF ( fx );
	WorldObject::Destroy ( );
}
bool Ground::Update ( float fDeltaTime ) {
	bool bParentUpdate = WorldObject::Update ( fDeltaTime );
	return bParentUpdate;
}