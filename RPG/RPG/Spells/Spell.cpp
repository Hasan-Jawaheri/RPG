#include "Spell.h"
#include "SpellIDs.h"
#include "Buff.h"

#include "Warrior Spells//Warrior Spells.h"

float ATANH ( float x ) {
	return (logf(1.0+x) - log(1.0-x))/2.0;
}

vector<Spell*> SpellManager::castedSpells;

void SpellManager::LoadSpellResources ( void ) {
}

void SpellManager::Update ( float fDeltaTime ) {
	for ( UINT i = 0; i < castedSpells.size ( ); i++ ) {
		if ( !castedSpells[i]->Update ( fDeltaTime ) ) {
			castedSpells[i]->OnFade ( );
			delete castedSpells[i];
			castedSpells.erase ( castedSpells.begin ( ) + i );
			i--;
		}
	}
}

void SpellManager::Cleanup ( void ) {
	for ( UINT i = 0; i < castedSpells.size ( ); i++ ) {
		castedSpells[i]->OnFade ( );
		delete castedSpells[i];
	}
	castedSpells.clear ( );
}

bool SpellManager::SpellExists ( Spell* s ) {
	for ( UINT i = 0; i < castedSpells.size ( ); i++ )
		if ( castedSpells[i] == s )
			return true;
	return false;
}

void SpellManager::StopSpell ( Spell* spell ) {
	spell->OnFade ( );
	for ( UINT i = 0; i < castedSpells.size ( ); i++ ) {
		if ( castedSpells[i] == spell ) {
			castedSpells.erase ( castedSpells.begin ( ) + i );
			break;
		}
	}
	delete spell;
}

void SpellManager::CastSpell ( Spell* spell ) {
	if ( !spell )
		return;

	if ( !spell->OnCast ( ) )
		delete spell;
	else
		castedSpells.push_back ( spell );
}

Spell* SpellManager::CastSpell ( UINT spellID, Unit* caster, Unit* target ) {
	Spell* s = nullptr;
	switch ( spellID ) {
	case SPELLID_SLASHS1:
		s = new Spell_Slash_S1 ( caster );
		break;
	case SPELLID_SLASHS2:
		s = new Spell_Slash_S2 ( caster );
		break;
	case SPELLID_SLASHS3:
		s = new Spell_Slash_S3 ( caster );
		break;
	case SPELLID_DASHS1:
		s = new Spell_Dash_S1 ( caster );
		break;
	case SPELLID_DEATHGRIP:
		s = new Spell_DeathGrip ( caster );
		break;
	case SPELLID_DARKMETEOR:
		s = new Spell_DarkMeteor ( caster );
		break;
	}
	if ( s ) {
		s->SetTarget ( target );
		s->LoadResources ( );
		if ( !s->OnCast ( ) ) {
			s->OnFade ( );
			HX_SAFE_DELETE ( s );
		} else {
			castedSpells.push_back ( s );
		}
	}
	return s;
}

Spell::Spell ( Unit* c, Unit* t ) {
	caster = c;
	target = t;
	ZeroMemory ( &anim, sizeof ANIMATIONSTRUCT );
	bSlow = false;
	bMustFade = false;
	bLowerBody = false;
}
Spell::~Spell ( void ) {
}
void Spell::LoadResources ( void ) {
}
bool Spell::OnCast ( void ) {
	fCastTime = CURGAMETIME;
	if ( anim.bAnimation && caster ) {
		ANIMATIONRANGE r;
		bLowerBody = caster->StartSpellAnimation ( anim.animName, anim.fSpeed,
			anim.bForceWholeBody, &r, anim.param, (anim.param + 1)%3, anim.bLoop );
		anim.range = r;
		
		float S = anim.fSpeed - anim.fLowSpeed;
		float K = anim.fLowSpeed;// + (S/2.0f);
		float R = anim.fSlowRange;
		float x = R * 2;
		anim.fSlowTime = (2.0f*x)/S + x*(1.0f/anim.fSpeed) + (2*R*sinf((x*HX_PI)/(R)))/(HX_PI*S); // this is (1/spd)*dst = time
		float fAnimRange = anim.range.params[1] - anim.range.params[0];
		anim.fTotalTime = anim.fSlowTime + (fAnimRange-anim.fSlowRange*2)/anim.fSpeed; //time = dst / spd

		anim.fTotalTime = fAnimRange / anim.fSpeed;
	}
	return true;
}
bool Spell::Update ( float fDeltaTime ) {
	float fTime;
	if ( caster && caster->GetAnimation ( ) )
		fTime = caster->GetAnimation()->GetTime ( 0 ) - (float)anim.range.params[0];
	else
		fTime = ( CURGAMETIME - fCastTime ) / 1000.0f;

	/*float fSlowPoint = anim.range.params[2] - anim.range.params[0];
	float fSpeed = anim.fSpeed;
	if ( abs ( fTime - fSlowPoint ) < anim.fSlowRange ) {
		fSpeed = (cosf ( (fTime - anim.fSlowRange) *
			HX_PI/(anim.fSlowRange) ) * 0.5f + 0.5f) *
			(anim.fSpeed-anim.fLowSpeed) + anim.fLowSpeed;
		bSlow = true;//fTime > anim.fSlowPoint; //<--HARD MODE
	} else
		bSlow = false;

	if ( anim.bAnimation && caster && caster->GetAnimation ( ) ) {
		caster->GetAnimation()->SetPlaySpeed ( fSpeed, 1 );
		if ( bLowerBody )
			caster->GetAnimation()->SetPlaySpeed ( fSpeed, 0 );
	}*/
	caster->GetAnimation ( )->SetPlaySpeed ( anim.fSpeed, 0 );

	return (CURGAMETIME - fCastTime <= anim.fTotalTime * 1000.0f) && !bMustFade;
}
void Spell::OnFade ( void ) {
	if ( !bMustFade ) {
		if ( caster && UnitManager::UnitExists ( caster ) )
			caster->StopSpellAnimation ( );
		bMustFade = true;
	}
}
float Spell::GetTotalCastTime ( void ) const {
	return anim.fTotalTime;
}
bool Spell::IsSlow ( void ) const {
	return bSlow;
}
float Spell::GetSlowCastTime ( void ) const {
	return anim.fSlowTime;
}
void Spell::SetTarget ( class Unit* t ) {
	target = t;
}

BuffSpell::BuffSpell ( Unit* c, Buff* b ) : Spell ( c ) {
	buff = b;
}

BuffSpell::~BuffSpell ( void ) {
}

void BuffSpell::LoadResources ( void ) {
}

bool BuffSpell::OnCast ( void ) {
	BuffManager::AddBuff ( caster, buff );
	return false;
}

bool BuffSpell::Update ( float fDeltaTime ) {
	return false;
}

void BuffSpell::OnFade ( void ) {
}