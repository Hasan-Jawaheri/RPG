#ifndef _SPELLS_COMBO_H_
#define _SPELLS_COMBO_H_

#include "..//Main.h"
#include "Spell.h"
#include "..//Entities//Unit.h"

struct ComboNode {
	ComboNode* nextStage[6];
	//fire/fost/shock are 3 trees, a subclass will change a stance but the new stance is a different tree
	UINT treeID;
	UINT spellID;
	float manaCost;
	float energyCost;
};

class ComboSystem {

	ComboNode* combos[6];

	class Spell* curSpell;
	ComboNode* curCombo;

public:
	ComboSystem ( void );
	~ComboSystem ( void );
	
	//btn: 0 = left, 1 = right, 2 = both
	ComboNode* AdvanceCombo ( UINT stance, int btn, Unit* caster, Unit* target );
	Spell* GetCurrentSpell ( void ) const;
};

#endif //_SPELLS_COMBO_H_