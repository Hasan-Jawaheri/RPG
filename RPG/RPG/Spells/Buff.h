#ifndef _BUFF_H_
#define _BUFF_H_

#include "..//Common.h"
#include "..//Entities//Unit.h"

struct ANIMDATA {
	bool bAnim;
	UINT start, end;
	float fSpeed;
};

class Buff;

class BuffManager {
	struct ACTIVEBUFF {
		class Buff* buff;
		class Unit* unit;
	};
	static vector<ACTIVEBUFF> activeBuffs;

public:
	static void LoadBuffResources ( void );
	static void Update ( float fDeltaTime );
	static void Cleanup ( void );
	
	static void AddBuff ( class Unit* caster, Buff* buff );
	static void RemoveBuff ( class Unit* caster, Buff* buff );
};

class Buff {
protected:
	Unit* owner;
	float fStartTime;
	STATS_STRUCT stats;
	mutable bool bAnim;

	virtual ANIMDATA _GetAnimationData ( void ) const;

public:
	Buff ( Unit* u );
	~Buff ( void );

	virtual bool Update ( float fDeltaTime );
	virtual void Destroy ( void );

	virtual float GetDuration ( void ) const = 0;
	ANIMDATA GetAnimationData ( void ) const;

	STATS_STRUCT GetStats ( void ) const;
};

class GlowBuff : public Buff {
protected:
	hxParticles* particles;

public:
	GlowBuff ( Unit* u );
	~GlowBuff ( void );

	virtual bool Update ( float fDeltaTime );
	virtual void Destroy ( void );
	virtual float GetDuration ( void ) const = 0;
};

#endif //_BUFF_H_
