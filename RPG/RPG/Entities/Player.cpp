#include "Player.h"
#include "..//AI//AI.h"

Character::Character ( UINT ID ) : Unit ( ID, ID ) {
}

Character::~Character ( void ) {
}

Player::Player ( void ) : Unit ( 0, 0 ) {
	__ID = 0;
}

Player::~Player ( void ) {
	Destroy ( );
}

void Player::Load ( void ) {
	//for now, just load it as default
	Unit::Load ( );

	//load an equipment as a test
	Equip ( SLOT_WEAPON1, 1 );
	//Equip ( SLOT_HEAD, 2 );
	Equip ( SLOT_PANTS, 3 );
	Equip ( SLOT_SHOULDERS, 4 );
	Equip ( SLOT_BELT, 5 );
	Equip ( SLOT_BOOTS, 6 );
}

void Player::Destroy ( void ) {
	Unit::Destroy ( );
}

bool Player::Update ( float fDeltaTime ) {
	Unit::Update ( fDeltaTime );

	return true; //never delete the player
}

void Player::Equip ( ITEMSLOT slot, UINT itemID ) {
	Unit::Equip ( slot, itemID );
}

void Player::Unequip ( ITEMSLOT slot ) {
	Unit::Unequip ( slot );
}