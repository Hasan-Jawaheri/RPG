#include "Combo System.h"
#include "SpellIDs.h"

ComboSystem::ComboSystem ( void ) {
	for ( UINT i = 0; i < 6; i++ )
		combos[i] = nullptr;


	combos[0] = new ComboNode;
	ZeroMemory ( combos[0], sizeof ComboNode );
	combos[0]->spellID = SPELLID_DEATHGRIP;
	combos[0]->treeID = 1;
	combos[0]->manaCost = 40;
	combos[0]->nextStage[0] = new ComboNode;
	combos[0]->nextStage[0]->spellID = SPELLID_DARKMETEOR;
	combos[0]->nextStage[0]->treeID = 1;
	combos[0]->nextStage[0]->manaCost = 50;

	combos[1] = new ComboNode;
	ZeroMemory ( combos[1], sizeof ComboNode );
	combos[1]->spellID = SPELLID_DASHS1;
	combos[1]->treeID = 0;
	combos[1]->manaCost = 10;
	combos[1]->nextStage[1] = new ComboNode;
	ZeroMemory ( combos[1]->nextStage[1], sizeof ComboNode );
	combos[1]->nextStage[1]->manaCost = 20;
	combos[1]->nextStage[1]->energyCost = 20;
	combos[1]->nextStage[1]->spellID = 2;
	combos[1]->nextStage[1]->treeID = 0;
	combos[1]->nextStage[1]->nextStage[0] = new ComboNode ( );
	ZeroMemory ( combos[1]->nextStage[1]->nextStage[0], sizeof ComboNode );
	combos[1]->nextStage[1]->nextStage[0]->manaCost = 30;
	combos[1]->nextStage[1]->nextStage[0]->energyCost = 30;
	combos[1]->nextStage[1]->nextStage[0]->spellID = 3;
	combos[1]->nextStage[1]->nextStage[0]->treeID = 0;

	curSpell = nullptr;
	curCombo = nullptr;
}

ComboSystem::~ComboSystem ( void ) {
	for ( UINT i = 0; i < 6; i++ )
		delete combos[i];
}

ComboNode* ComboSystem::AdvanceCombo ( UINT stance, int btn, Unit* caster, Unit* target ) {
	if ( stance > 2 || btn > 2 )
		return nullptr;

	if ( curSpell ) {
		if ( !SpellManager::SpellExists ( curSpell ) ) {
			curSpell = nullptr;
			curCombo = nullptr;
		}
	}

	if ( !curCombo && !curSpell ) {
		if ( btn == 2 )
			curCombo = combos[stance*2]; //WRONG******************************
		else
			curCombo = combos[stance*2+btn];
		if ( curCombo ) {
			curSpell = SpellManager::CastSpell ( curCombo->spellID, caster, target );
			if ( !curSpell )
				curCombo = nullptr;
		}
		return curCombo;
	} else if ( curCombo ) {
		if ( curSpell->IsSlow ( ) || curSpell->IsInterruptable ( ) ) {
			curCombo = curCombo->nextStage[stance*2+btn];
			if ( curCombo ) {
				SpellManager::StopSpell ( curSpell );
				curSpell = SpellManager::CastSpell ( curCombo->spellID, caster, target );
				if ( curSpell ) {
					return curCombo;
				}
				//failed
				curCombo = nullptr;
			}
		}
		//getting to this point means penalty
		//penalty: casted combo while another is in progress
		//curCombo = nullptr; <--- THIS MAKES IT VERY DIFFICULT.
	}

	return nullptr;
}
Spell* ComboSystem::GetCurrentSpell ( void ) const {
	return curSpell;
}