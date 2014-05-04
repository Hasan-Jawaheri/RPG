#include "Demon Lord's Lair.h"
#include "..//..//AI//AI.h"

//AI and not UnitAI. this boss will not behave like a normal unit.
class DemonLordAI : public AI {
public:
	DemonLordAI ( void ) : AI ( ) {
	}
	~DemonLordAI ( void ) {
	}
	
	virtual void Apply ( Object* obj, float fDeltaTime ) {
	}
	virtual AI* Clone ( void ) const {
		return new DemonLordAI ( );
	}
};

DemonLordsLair::DemonLordsLair ( void ) : Map ( ) {
	//load AIs for this map
	AIFactory::AddAIEntry ( new DemonLordAI ( ), "demon_lord" );
}
DemonLordsLair::~DemonLordsLair ( void ) {
}
void DemonLordsLair::Update ( float fDeltaTime ) {
	Map::Update ( fDeltaTime );
}
void DemonLordsLair::OnLoadObject ( Object* obj ) {
	Map::OnLoadObject ( obj );
}