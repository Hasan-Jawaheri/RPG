#include "Buff.h"
#include "SpellIDs.h"

vector<BuffManager::ACTIVEBUFF> BuffManager::activeBuffs;

void BuffManager::LoadBuffResources ( void ) {
}

void BuffManager::Update ( float fDeltaTime ) {
	for ( UINT i = 0; i < activeBuffs.size ( ); i++ ) {
		if ( !activeBuffs[i].buff->Update ( fDeltaTime ) ) {
			activeBuffs[i].unit->RemoveBuff ( activeBuffs[i].buff );
			delete activeBuffs[i].buff;
			activeBuffs.erase ( activeBuffs.begin ( ) + i );
			i--;
		} else {
			ANIMDATA a = activeBuffs[i].buff->GetAnimationData ( );
			//if ( a.bAnim )
			//	activeBuffs[i].unit->StartSpellAnimation ( a.start, a.end, a.fSpeed );
			activeBuffs[i].unit->ApplyPerFrameStats ( activeBuffs[i].buff->GetStats ( ) );
		}
	}
}

void BuffManager::Cleanup ( void ) {
}

void BuffManager::AddBuff ( Unit* caster, Buff* buff ) {
	if ( !buff )
		return;

	caster->ApplyBuff ( buff );
	ACTIVEBUFF b;
	b.buff = buff;
	b.unit = caster;
	activeBuffs.push_back ( b );
}

void BuffManager::RemoveBuff ( Unit* caster, Buff* buff ) {
	if ( !buff )
		return;
	
	for ( UINT i = 0; i < activeBuffs.size ( ); i++ ) {
		if ( activeBuffs[i].buff == buff && activeBuffs[i].unit == caster ) {
			activeBuffs[i].unit->RemoveBuff ( activeBuffs[i].buff );
			activeBuffs.erase ( activeBuffs.begin ( ) + i );
			break;
		}
	}
	buff->Destroy ( );
	delete buff;
}

Buff::Buff ( Unit* u ) {
	owner = u;
	fStartTime = CURGAMETIME;
	stats = STATS_STRUCT ( );
	bAnim = false;
}

Buff::~Buff ( void ) {
}

bool Buff::Update ( float fDeltaTime ) {
	if ( GetDuration ( ) < 0.001f )
		return true; //infinite duration
	return (CURGAMETIME - fStartTime) < GetDuration ( );
}
void Buff::Destroy ( void ) {
}

STATS_STRUCT Buff::GetStats ( void ) const {
	return stats;
}

ANIMDATA Buff::_GetAnimationData ( void ) const {
	ANIMDATA a;
	ZeroMemory ( &a, sizeof ANIMDATA );
	return a;
}

ANIMDATA Buff::GetAnimationData ( void ) const {
	if ( !bAnim ) {
		bAnim = true;
		return _GetAnimationData ( );
	} else {
		return Buff::_GetAnimationData ( );
	}
}


GlowBuff::GlowBuff ( Unit* u ) : Buff ( u ) {
	particles = nullptr;
}
GlowBuff::~GlowBuff ( void ) {
}

bool GlowBuff::Update ( float fDeltaTime ) {
	if ( particles && owner ) {
		hxOrientationDevice* obj = owner->GetOriDev ( );
		hxVector3 up = obj->GetUVector ( );
		hxVector3 look = obj->GetLVector ( );
		hxVector3 right = obj->GetRVector ( );

		//
		//the world matrix is the view matrix's inverse
		//so we build a normal view matrix and inverse it
		//

		//build world matrix
		hxMatrix WorldM;
		(WorldM)( 0, 0 ) = right.x; (WorldM)( 0, 1 ) = up.x; (WorldM)( 0, 2 ) = look.x; (WorldM)( 0, 3 ) = 0.0f;
		(WorldM)( 1, 0 ) = right.y; (WorldM)( 1, 1 ) = up.y; (WorldM)( 1, 2 ) = look.y; (WorldM)( 1, 3 ) = 0.0f;
		(WorldM)( 2, 0 ) = right.z; (WorldM)( 2, 1 ) = up.z; (WorldM)( 2, 2 ) = look.z; (WorldM)( 2, 3 ) = 0.0f;
		(WorldM)( 3, 0 ) = 0;       (WorldM)( 3, 1 ) = 0;    (WorldM)( 3, 2 ) = 0;      (WorldM)( 3, 3 ) = 1.0f;
		WorldM = hxMatrixInverse ( WorldM );
		hxVector3 pos = hxVec3TransformCoord ( hxVector3 ( 0, 0, 0 ), particles->GetBindingMatrix ( ) * WorldM );
		particles->SetPosition ( pos + obj->GetPosition ( ) );
	}

	return Buff::Update ( fDeltaTime );
}

void GlowBuff::Destroy ( void ) {
	HX_SAFE_REMOVEREF ( particles );
	Buff::Destroy ( );
}