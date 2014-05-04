#ifndef _SPELLS_WARRIRORSPELLS_H_
#define _SPELLS_WARRIRORSPELLS_H_

#include "..//Spell.h"


class Spell_Slash_S1 : public Spell {
	bool bDoneDamage;
	EquippedItem* sword;

public:
	Spell_Slash_S1 ( class Unit* c );

	virtual void LoadResources ( void );

	virtual bool OnCast ( void );
	virtual bool Update ( float fDeltaTime );
	virtual void OnFade ( void );
};

class Spell_Slash_S2 : public Spell {
	bool bDoneDamage;

public:
	Spell_Slash_S2 ( class Unit* c );

	virtual void LoadResources ( void );

	virtual bool OnCast ( void );
	virtual bool Update ( float fDeltaTime );
	virtual void OnFade ( void );
};

class Spell_Slash_S3 : public Spell {
	bool bDoneDamage;
	bool bKnocked;

public:
	Spell_Slash_S3 ( class Unit* c );

	virtual void LoadResources ( void );

	virtual bool OnCast ( void );
	virtual bool Update ( float fDeltaTime );
	virtual void OnFade ( void );
};

class Spell_Dash_S1 : public Spell {
	bool bDoneDamage;

public:
	Spell_Dash_S1 ( class Unit* c );

	virtual void LoadResources ( void );

	virtual bool OnCast ( void );
	virtual bool Update ( float fDeltaTime );
	virtual void OnFade ( void );
};

class Spell_DeathGrip : public Spell {
	class DeathGripGlow : public GlowBuff {
		UINT type;

	public:
		DeathGripGlow ( class Unit* u, UINT id = 0 );
		virtual float GetDuration ( void ) const { return 0.0f; }
		virtual void Destroy ( void );
	};
	DeathGripGlow* glow;
	DeathGripGlow* glow2;
	UINT stage;
	float fDamageTimer;

public:
	Spell_DeathGrip ( class Unit* c );

	virtual void LoadResources ( void );

	virtual bool IsInterruptable ( void ) const { return (CURGAMETIME - fCastTime) > 1000.0f; }

	virtual bool OnCast ( void );
	virtual bool Update ( float fDeltaTime );
	virtual void OnFade ( void );
};

class Spell_DarkMeteor : public Spell {
public:
	Spell_DarkMeteor ( class Unit* c );

	virtual void LoadResources ( void );

	virtual bool OnCast ( void );
	virtual bool Update ( float fDeltaTime );
	virtual void OnFade ( void );
};

#endif //_SPELLS_WARRIRORSPELLS_H_