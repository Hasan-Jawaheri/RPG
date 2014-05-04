#ifndef _GAME_COMMON_H_
#define _GAME_COMMON_H_

#include <HasX11.h>
#include "Main.h"
#include "Entities//Object.h"
#include "Resource Manager//Resource Manager.h"

#define APPNAME "RPG"

#define FONT_DEFAULT 0
#define FONT_CONSOLAS_16 1

extern class Application* APPHANDLE;
#define CURGAMETIME (APPHANDLE->fCurGameTime)

struct STATS_STRUCT {
	//passive stats
	float moveSpeed;

	//abilities
	float health;
	float maxHealth;
	float mana;
	float maxMana;
	float energy;
	float maxEnergy;

	//offensive stats
	float attackDmg;
	float magicDmg;
	float armorPen;
	float magicPen;

	//defensive stats
	float fireResist;
	float frostResist;
	float natureResist;
	float armor;
	float shield;

	STATS_STRUCT ( void ) { ZeroMemory ( this, sizeof STATS_STRUCT ); }
};

struct DAMAGE_STRUCT {
	float fBaseAttackDmg;
	float fBaseFireDmg;
	float fBaseFrostDmg;
	float fBaseNatureDmg;

	float attackDmgMul;
	float fireDmgMul;
	float frostDmgMul;
	float natureDmgMul;
};

struct ANIMATIONRANGE {
	char name[16];
	UINT params[3];
};

#endif //_GAME_COMMON_H_
