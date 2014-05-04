#include "Warrior Spells.h"

//**********************************************************************************************
//SPELL:Slash_Stage1
//**********************************************************************************************
Spell_Slash_S1::Spell_Slash_S1 ( Unit* c ) : Spell ( c, nullptr ) {
	bDoneDamage = false;
	anim.animName = "hit1";
	anim.bAnimation = true;
	anim.bForceWholeBody = false;
	anim.fSpeed = 80.0f;
	anim.fLowSpeed = 4.0f;
	anim.fSlowRange = 4.0f;
	sword = caster->GetEquippedItem ( SLOT_WEAPON1 );
}
void Spell_Slash_S1::LoadResources ( void ) {
	Spell::LoadResources ( );
	//hxParticles* p = new hxParticles ( 0, core );
	sword->GetOriDev ( );
}
bool Spell_Slash_S1::OnCast ( void ) {
	return Spell::OnCast ( );
}
bool Spell_Slash_S1::Update ( float fDeltaTime ) {
	float castTime = CURGAMETIME - fCastTime;

	if ( castTime > 250.0f && !bDoneDamage ) {
		DAMAGE_STRUCT dmg = { 0 };
		dmg.attackDmgMul = 0.3f;
		dmg.fBaseAttackDmg = 10.0f;
		hxOrientationDevice* oriDev = caster->GetOriDev ( );
		UnitManager::DealDamage ( caster, oriDev->GetPosition ( ) + oriDev->GetLVector ( ) * 2, 10.0f, dmg );
		bDoneDamage = true;
	}

	return Spell::Update ( fDeltaTime );
}
void Spell_Slash_S1::OnFade ( void ) {
	return Spell::OnFade ( );
}

//**********************************************************************************************
//SPELL:Slash_Stage2
//**********************************************************************************************
Spell_Slash_S2::Spell_Slash_S2 ( Unit* c ) : Spell ( c, nullptr ) {
	bDoneDamage = false;
	anim.animName = "hit2";
	anim.bAnimation = true;
	anim.bForceWholeBody = false;
	anim.fSpeed = 80.0f;
	anim.fLowSpeed = 4.0f;
	anim.fSlowRange = 4.0f;
}
void Spell_Slash_S2::LoadResources ( void ) {
	Spell::LoadResources ( );
}
bool Spell_Slash_S2::OnCast ( void ) {
	return Spell::OnCast ( );
}
bool Spell_Slash_S2::Update ( float fDeltaTime ) {
	float castTime = CURGAMETIME - fCastTime;

	if ( castTime > 250.0f && !bDoneDamage ) {
		DAMAGE_STRUCT dmg = { 0 };
		dmg.attackDmgMul = 0.3f;
		dmg.fBaseAttackDmg = 30.0f;
		hxOrientationDevice* oriDev = caster->GetOriDev ( );
		UnitManager::DealDamage ( caster, oriDev->GetPosition ( ) + oriDev->GetLVector ( ) * 2, 10.0f, dmg );
		bDoneDamage = true;
	}

	return Spell::Update ( fDeltaTime );
}
void Spell_Slash_S2::OnFade ( void ) {
	return Spell::OnFade ( );
}

//**********************************************************************************************
//SPELL:Slash_Stage3
//**********************************************************************************************
Spell_Slash_S3::Spell_Slash_S3 ( Unit* c ) : Spell ( c, nullptr ) {
	bDoneDamage = false;
	bKnocked = false;
	anim.animName = "hit3";
	anim.bAnimation = true;
	anim.bForceWholeBody = false;
	anim.fSpeed = 60.0f;
	anim.fLowSpeed = 2.0f;
	anim.fSlowRange = 3.0f;
}
void Spell_Slash_S3::LoadResources ( void ) {
	Spell::LoadResources ( );
}
bool Spell_Slash_S3::OnCast ( void ) {
	caster->DisableMotion ( );
	return Spell::OnCast ( );
}
bool Spell_Slash_S3::Update ( float fDeltaTime ) {
	float castTime = CURGAMETIME - fCastTime;

	if ( castTime > 250.0f && !bDoneDamage ) {
		DAMAGE_STRUCT dmg = { 0 };
		dmg.attackDmgMul = 0.3f;
		dmg.fBaseAttackDmg = 70.0f;
		hxOrientationDevice* oriDev = caster->GetOriDev ( );
		UnitManager::DealDamage ( caster, oriDev->GetPosition ( ) + oriDev->GetLVector ( ) * 30, 40.0f, dmg );
		bDoneDamage = true;
	} else if ( bDoneDamage && !bKnocked ) {
		caster->GetAI ( )->SetLinearVelocity ( caster->GetOriDev ( )->GetLVector ( ) * -50.0f, fDeltaTime );
		bKnocked = true;
	}

	return Spell::Update ( fDeltaTime );
}
void Spell_Slash_S3::OnFade ( void ) {
	caster->EnableMotion ( );
	return Spell::OnFade ( );
}

//**********************************************************************************************
//SPELL:Dash_Stage1
//**********************************************************************************************
Spell_Dash_S1::Spell_Dash_S1 ( Unit* c ) : Spell ( c, nullptr ) {
	bDoneDamage = false;
	anim.animName = "dash";
	anim.bAnimation = true;
	anim.bForceWholeBody = true;
	anim.fSpeed = 40.0f;
	anim.fLowSpeed = 2.0f;
	anim.fSlowRange = 3.0f;
}
void Spell_Dash_S1::LoadResources ( void ) {
	Spell::LoadResources ( );
}
bool Spell_Dash_S1::OnCast ( void ) {
	if ( !caster->GetAI ( )->IsOnGround ( ) )
		return false;

	caster->DisableMotion ( );
	return Spell::OnCast ( );
}
bool Spell_Dash_S1::Update ( float fDeltaTime ) {
	float castTime = CURGAMETIME - fCastTime;

	hxVector3 up = caster->GetOriDev ( )->GetUVector ( ) * ((500.0f - castTime) / 80.0f);
	if ( castTime < 200.0f )
		up = hxVector3 ( );

	float fMul = 2.0f;
	if ( castTime > 800.0f ) {
		if ( !bDoneDamage && castTime > 1000.0f ) {
			DAMAGE_STRUCT dmg = { 0 };
			dmg.attackDmgMul = 0.3f;
			dmg.fBaseAttackDmg = 70.0f;
			hxOrientationDevice* oriDev = caster->GetOriDev ( );
			UnitManager::DealDamage ( caster, oriDev->GetPosition ( ) + oriDev->GetLVector ( ) * 30, 40.0f, dmg );
			bDoneDamage = true;
		}
		fMul = max ( 0, 2.0f-((castTime-800.0f) / 200.0f) );
	}
	caster->GetAI ( )->SetLinearVelocity (
		caster->GetOriDev ( )->GetLVector ( ) *
		min ( 10.0f, max ( castTime / 40.0f - 1.0f, 0 ) ) * fMul + up * 3, fDeltaTime );

	return Spell::Update ( fDeltaTime );
}
void Spell_Dash_S1::OnFade ( void ) {
	caster->EnableMotion ( );
	return Spell::OnFade ( );
}

Spell_DeathGrip::DeathGripGlow::DeathGripGlow ( Unit* u, UINT id ) : GlowBuff ( u ) {
	type = id;
	if ( u ) {
		hxCore* core = APPHANDLE->core;
		hxSkeleton* anim = u->GetAnimation ( );
		if ( anim ) {
			if ( id == 0 ) {
				hxImage* img = new hxImage ( 0, core );
				img->Load ( "Media\\flame.png" );
				particles = new hxParticles ( 0, core );
				particles->Create ( );
				particles->GetSOMaterial ( )->SetEmittingPositionRandomness ( 0.1f );
				particles->GetSOMaterial ( )->SetEmittingTimer ( 0.001f );
				particles->GetSOMaterial ( )->SetParticleSize ( 3.0f );
				particles->GetSOMaterial ( )->SetParticleAge ( 0.5f );
				particles->GetMaterial ( )->SetParticleAge ( 0.5f );
				particles->GetMaterial ( )->SetGravity ( 0, 0, 0 );
				particles->GetMaterial ( )->SetBlendingMode ( BLEND_SUBTRACT );
				particles->GetMaterial ( )->Texture ( img );
				anim->BindToBone ( particles, anim->GetBone ( 0, "Bip001 L Hand" )->GetIndex ( ) );
				img->RemoveReference ( );
			} else {
				hxImage* img = new hxImage ( 0, core );
				img->Load ( "Media\\flame.png" );
				particles = new hxParticles ( 0, core );
				particles->Create ( );
				particles->GetSOMaterial ( )->SetEmittingPositionRandomness ( 5.0f );
				particles->GetSOMaterial ( )->SetEmittingTimer ( 0.001f );
				particles->GetSOMaterial ( )->SetParticleSize ( 10.0f );
				particles->GetSOMaterial ( )->SetParticleAge ( 2.5f );
				particles->GetMaterial ( )->SetParticleAge ( 2.5f );
				particles->GetMaterial ( )->SetGravity ( 0, 15, 0 );
				//particles->GetMaterial ( )->SetSizeChangeType ( SIZECHANGE_SHRINK );
				particles->GetMaterial ( )->SetBlendingMode ( BLEND_ALPHA );
				particles->GetMaterial ( )->Texture ( img );
				img->RemoveReference ( );
			}
		}
	}
}
void Spell_DeathGrip::DeathGripGlow::Destroy ( void ) {
	if ( owner ) {
		hxSkeleton* anim = owner->GetAnimation ( );
		if ( anim && type == 0 )
			anim->UnbindFromBone ( particles, anim->GetBone ( 0, "Bip001 L Hand" )->GetIndex ( ) );
	}
	GlowBuff::Destroy ( );
}
Spell_DeathGrip::Spell_DeathGrip ( class Unit* c ) : Spell ( c ) {
	stage = 0;
}
void Spell_DeathGrip::LoadResources ( void ) {
	glow = new DeathGripGlow ( caster );
	glow2 = new DeathGripGlow ( target, 1 );
	Spell::LoadResources ( );
}
bool Spell_DeathGrip::OnCast ( void ) {
	if ( !target || !UnitManager::UnitExists ( target ) )
		return false;

	caster->DisableMotion ( );
	BuffManager::AddBuff ( caster, glow );
	BuffManager::AddBuff ( target, glow2 );

	anim.animName = "summon";
	anim.bAnimation = true;
	anim.bForceWholeBody = true;
	anim.fSpeed = 15.0f;
	anim.fLowSpeed = 2.0f;
	anim.fSlowRange = 3.0f;

	Spell::OnCast ( );
	anim.fTotalTime = 4.0f;

	return true;
}
bool Spell_DeathGrip::Update ( float fDeltaTime ) {
	if ( !target || !UnitManager::UnitExists ( target ) )
		return false;

	float castTime = CURGAMETIME - fCastTime;

	switch ( stage ) {
		case 0: //lift target up
			if ( castTime >= (anim.range.params[1] - anim.range.params[0]) * 1000.0f / anim.fSpeed ) {
				stage = 1;
				fDamageTimer = castTime;
				anim.param = 1;
				anim.bLoop = true;
				Spell::OnCast ( );
				anim.fTotalTime = 4.0f;
			} else {
				target->GetAI ( )->SetLinearVelocity ( hxVector3 ( 0, castTime * 0.01f, 0 ), fDeltaTime );
			}
			break;
		case 1: //air torment
			while ( castTime - fDamageTimer > 100.0f ) {
				fDamageTimer += 100.0f;
				DAMAGE_STRUCT dmg;
				ZeroMemory ( &dmg, sizeof DAMAGE_STRUCT );
				dmg.fBaseFireDmg = 4;
				dmg.fireDmgMul = 0.005f;
				UnitManager::DealDamage ( caster, target, dmg );
			}
			target->GetAI ( )->SetLinearVelocity ( hxVector3 ( 0, 0.5f, 0 ), fDeltaTime );
			break;
	}

	return castTime < 3000.0f;
}
void Spell_DeathGrip::OnFade ( void ) {
	BuffManager::RemoveBuff ( caster, glow );
	BuffManager::RemoveBuff ( target, glow2 );
	caster->EnableMotion ( );
	Spell::OnFade ( );
}

Spell_DarkMeteor::Spell_DarkMeteor ( Unit* c ) : Spell ( c ) {
	anim.animName = "drop";
	anim.bAnimation = true;
	anim.bForceWholeBody = true;
	anim.fSpeed = 50.0f;
	anim.fLowSpeed = 2.0f;
	anim.fSlowRange = 3.0f;
}
void Spell_DarkMeteor::LoadResources ( void ) {

}
bool Spell_DarkMeteor::OnCast ( void ) {
	if ( !target )
		return false;
	if ( target->GetAI ( )->IsOnGround ( ) )
		return false;

	caster->DisableMotion ( );

	return Spell::OnCast ( );
}
bool Spell_DarkMeteor::Update ( float fDeltaTime ) {
	if ( !target || !UnitManager::UnitExists ( target ) )
		return false;

	float castTime = CURGAMETIME - fCastTime;

	if ( !target->GetAI ( )->IsOnGround ( ) )
		target->GetAI ( )->SetLinearVelocity ( hxVector3 ( 0, -castTime / 5.0f, 0 ), fDeltaTime );
	else {
		DAMAGE_STRUCT dmg;
		ZeroMemory ( &dmg, sizeof DAMAGE_STRUCT );
		dmg.attackDmgMul = 0.1f;
		dmg.fBaseAttackDmg = 100.0f;
		UnitManager::DealDamage ( caster, target, dmg );
		dmg.fBaseAttackDmg = 100.0f;
		UnitManager::DealDamage ( target, 50.0f, dmg );
		return false;
	}

	return Spell::Update ( fDeltaTime );
}
void Spell_DarkMeteor::OnFade ( void ) {
	caster->EnableMotion ( );
	Spell::OnFade ( );
}