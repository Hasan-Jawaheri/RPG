#ifndef _SPELLS_H_
#define _SPELLS_H_

#include "..//Common.h"
#include "..//Entities//Unit.h"

class Spell;

class SpellManager {
	static vector<Spell*> castedSpells;

public:
	static void LoadSpellResources ( void );
	static void Update ( float fDeltaTime );
	static void Cleanup ( void );

	static bool SpellExists ( Spell* s );
	
	static void StopSpell ( Spell* spell );
	static void CastSpell ( Spell* spell );
	static Spell* CastSpell ( UINT spellID, class Unit* caster, class Unit* target );
};

class Spell {

protected:
	Unit *caster, *target;
	float fCastTime;
	bool bSlow;
	bool bMustFade;

	struct ANIMATIONSTRUCT {
		bool bAnimation;
		bool bForceWholeBody;
		const char* animName;
		ANIMATIONRANGE range;
		float fSpeed; //normal speed
		float fLowSpeed; //lowest slow speed
		float fSlowRange; //range around slow point (1 side)

		UINT param;
		bool bLoop;

		float fSlowTime; //time while in the slow zone
		float fTotalTime; //total time to finish
	} anim;
	bool bLowerBody; //true if this spell is controling lower body animation

public:
	Spell ( class Unit* c, class Unit* t = nullptr );
	~Spell ( void );

	virtual void LoadResources ( void );

	virtual bool OnCast ( void );
	virtual bool Update ( float fDeltaTime );
	virtual void OnFade ( void );

	virtual bool IsInterruptable ( void ) const { return false; }

	float GetTotalCastTime ( void ) const;
	float GetSlowCastTime ( void ) const;
	bool IsSlow ( void ) const;
	void SetTarget ( class Unit* t );
};

class BuffSpell : public Spell {
	class Buff* buff;

public:
	BuffSpell ( class Unit* c, class Buff* b );
	~BuffSpell ( void );
	
	void LoadResources ( void );
	
	bool OnCast ( void );
	bool Update ( float fDeltaTime );
	void OnFade ( void );
};

#endif //_SPELLS_H_
