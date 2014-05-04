#include "Menu.h"
#include "Game.h"

Menu::Menu ( HasX11* a ) : hxGameState ( a ) {
}

Menu::~Menu ( void ) {
}

void Menu::Load ( void ) {
	app->SwitchState ( new Game ( app ) );
}

void Menu::Update ( float fDeltaTime ) {
}

void Menu::Cleanup ( void ) {
}