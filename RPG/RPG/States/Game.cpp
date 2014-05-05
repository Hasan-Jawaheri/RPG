#include "Game.h"
#include "..//Maps//Map.h"
#include "..//Entities//Player.h"
#include "..//Spells//Warrior Spells//Warrior Spells.h"
#include "..//UI//Game Controls//Cast Bar.h"
#include "..//UI//Game Controls//Health Bar.h"
#include "..//UI//Game Controls//Stance Bar.h"
#include "..//UI//Game Controls//Target Cursor.h"
#include "..//Quests//Quests.h"

Game::Game ( HasX11* a ) : hxGameState ( a ) {
	player = nullptr;
}

Game::~Game ( void ) {
}

void Game::Load ( void ) {
	//
	// Load the worlds essential resources and initialize the unit manager
	//
	AIFactory::Init ( );
	UnitManager::Initialize ( );

	//
	// Initialize spells and buffs
	//
	SpellManager::LoadSpellResources ( );
	BuffManager::LoadBuffResources ( );

	//
	// Load the player
	//
	player = new Player ( );
	UnitManager::LoadUnit ( 0, player );
	UnitManager::SetPlayer ( player );
	player->SetAI ( new PlayerAI ( 8.0f, 2.5f ) );
	target = nullptr;

	//
	// Load the quest system and progress
	//
	QuestManager::Initialize ( );
	
	//
	// Setup and load the user interface
	//
	UserInterface::AddUIElement ( input = new GameInputHandler ( this ), nullptr );
	UserInterface::AddUIElement ( ui.castBar = new CastBar ( ), input );
	UserInterface::AddUIElement ( ui.playerHealth = new HealthBar ( true ), input );
	UserInterface::AddUIElement ( ui.targetHealth = new HealthBar ( false ), input );
	UserInterface::AddUIElement ( ui.stanceBar = new StanceBar ( ), input );
	UserInterface::AddUIElement ( ui.targetCursor = new TargetCursor ( ), input );
	UserInterface::AddUIElement ( ui.chatEdit = new ChatEditBox ( ), input );
	UserInterface::Load ( core );

	ui.playerHealth->Open ( );
	ui.targetHealth->Open ( );
	ui.stanceBar->Open ( );
	ui.chatEdit->SetFade ( 1.0f );
	ui.chatEdit->SetSize ( 400, 20 );

	//
	// Load the map
	//
	Map::Set ( 1 );
}

void Game::Update ( float fDeltaTime ) {
	int offset = 220;
	float sw = core->WindowsDevice->GetWindowWidth ( );
	float sh = core->WindowsDevice->GetWindowHeight ( );
	ui.castBar->SetPosition ( sw / 2 - ui.castBar->GetSizeX ( ) / 2, sh - offset );
	int cbx = ui.playerHealth->GetSizeX ( );
	int cby = ui.castBar->GetPositionY ( );
	ui.targetHealth->SetPosition ( sw / 2 + 38, cby + 64 - ( ui.playerHealth->GetSizeY ( ) / 2 ) + 14 );
	ui.playerHealth->SetPosition ( sw / 2 - 38 - cbx, cby + 64 - ( ui.playerHealth->GetSizeY ( ) / 2 ) + 14 );
	ui.stanceBar->SetPosition ( sw / 2, cby + 100 - 3 );
	ui.targetCursor->SetPosition ( sw / 2 - 128, sh / 2 - 128 - 50 );
	ui.chatEdit->SetPosition ( 10, sh - 30 );

	if ( player ) {
		ui.playerHealth->SetHealth ( player->curStats.health * 100.0f / player->curStats.maxHealth );
		ui.playerHealth->SetPower ( player->curStats.mana * 100.0f / player->curStats.maxMana );

		/****************************************************************/
		hxOrientationDevice* dev = player->GetOriDev ( );
		if ( dev ) {
			char text[256];
			hxVector3 pos = dev->GetPosition ( );
			hxVector3 ang = dev->GetAngle ( );
			sprintf_s ( text, 256, "(%.2f,%.2f,%.2f) - (%.2f,%.2f,%.2f)",
				pos.x, pos.y, pos.z, ang.x, ang.y, ang.z );
			core->Renderer->RenderText ( text, 5, 100, 1, hxColor ( 0.4, 0.4, 1 ) );
		}
		/****************************************************************/
	}
	if ( target && UnitManager::UnitExists ( target ) ) {
		ui.targetHealth->SetHealth ( target->curStats.health * 100.0f / target->curStats.maxHealth );
		ui.targetHealth->SetPower ( target->curStats.mana * 100.0f / target->curStats.maxMana );
		ui.targetHealth->Open ( );
	} else {
		ui.targetHealth->Close ( );
		target = nullptr;
	}

	Map::Get()->Update ( fDeltaTime );
	UnitManager::Update ( fDeltaTime );
	if ( !UnitManager::UnitExists ( target ) )
		target = nullptr; //if target dies, make it null
	BuffManager::Update ( fDeltaTime );
	SpellManager::Update ( fDeltaTime );
}

void Game::Cleanup ( void ) {
	BuffManager::Cleanup ( );
	SpellManager::Cleanup ( );
	QuestManager::Cleanup ( );
	HX_SAFE_DELETE ( player );
	UnitManager::SetPlayer ( nullptr );
	UnitManager::Cleanup ( );
	AIFactory::Cleanup ( );
}

GameInputHandler::GameInputHandler ( class Game* g ) {
	game = g;
}
bool GameInputHandler::OnEnter ( void ) {
	//no chat (yet)
	if ( UserInterface::GetFocus ( ) != (UIElement*)game->ui.chatEdit ) {
		game->ui.chatEdit->OnFocus ( );
		UserInterface::SetFocus ( game->ui.chatEdit );
	} else {
		UserInterface::SetFocus ( this );
		char str[512];
		game->ui.chatEdit->GetText ( str, 512 );
		if ( strlen ( str ) ) {
			Unit* u = UnitManager::LoadUnit ( 1 );
			if ( u ) {
				u->GetAI ( )->SetPosition ( game->player->GetOriDev ( )->GetPosition ( ) );
			}
		}
		game->ui.chatEdit->SetText ( "" );
	}
	return true;
}
void GameInputHandler::OnKeydown ( short key ) {
	switch ( key ) {
		case 'W':
			game->player->GetAI ( )->Forward ( true );
			break;
		case 'S':
			game->player->GetAI ( )->Backward ( true );
			break;
		case 'A':
			game->player->GetAI ( )->Left ( true );
			break;
		case 'D':
			game->player->GetAI ( )->Right ( true );
			break;
		case ' ':
			game->player->GetAI ( )->Jump ( true );
			break;
		case '1':
			game->ui.stanceBar->SetStance ( 1 );
			break;
		case '2':
			game->ui.stanceBar->SetStance ( 2 );
			break;
		case '3':
			game->ui.stanceBar->SetStance ( 3 );
			break;
		case '	':
			game->target = UnitManager::FindTarget ( hxVector2 ( game->ui.targetCursor->GetPositionX ( ) + 128,
				game->ui.targetCursor->GetPositionY ( ) + 128 ), 200 );
			break;
	}
}
void GameInputHandler::OnKeyup ( short key ) {
	char actionKeys[10] = { '1', '2', '3', '4', '5', '6', '7', '8', '9', '0' };
	switch ( key ) {
		case 'W':
			game->player->GetAI ( )->Forward ( false );
			break;
		case 'S':
			game->player->GetAI ( )->Backward ( false );
			break;
		case 'A':
			game->player->GetAI ( )->Left ( false );
			break;
		case 'D':
			game->player->GetAI ( )->Right ( false );
			break;
		case ' ':
			game->player->GetAI ( )->Jump ( false );
			break;
		case '1':
			if ( game->ui.stanceBar->GetCurrentStance ( ) == 1 )
				game->ui.stanceBar->SetStance ( 0 );
			break;
		case '2':
			if ( game->ui.stanceBar->GetCurrentStance ( ) == 2 )
				game->ui.stanceBar->SetStance ( 0 );
			break;
		case '3':
			if ( game->ui.stanceBar->GetCurrentStance ( ) == 3 )
				game->ui.stanceBar->SetStance ( 0 );
			break;
		default:
			break;
			/*for ( UINT i = 0; i < 10; i++ )
				if ( key == actionKeys[i] )
				ab->DoAction ( i );*/
	}
}
void GameInputHandler::OnMouseButton ( int mx, int my, bool bDown ) {
	if ( bDown ) {
		if ( hxImage* img = game->ui.stanceBar->GetCurrentImage ( ) ) {
			if ( ComboNode* n = game->player->GetComboSystem()->AdvanceCombo
				( game->ui.stanceBar->GetCurrentStance ( ) - 1, 0, game->player, game->target ) ) {
					game->player->curStats.mana -= n->manaCost;
					game->player->curStats.energy -= n->energyCost;
					game->ui.castBar->Cast (
						img, game->player->GetComboSystem()->GetCurrentSpell()->GetTotalCastTime ( ) );
			}
		}
	}
	//Camera::MouseLeftClick ( bDown );
}
void GameInputHandler::OnMouseButton2 ( int mx, int my, bool bDown ) {
	if ( bDown ) {
		if ( hxImage* img = game->ui.stanceBar->GetCurrentImage ( ) ) {
			if ( ComboNode* n = game->player->GetComboSystem()->AdvanceCombo
				( game->ui.stanceBar->GetCurrentStance ( ) - 1, 1, game->player, game->target ) ) {
					game->player->curStats.mana -= n->manaCost;
					game->player->curStats.energy -= n->energyCost;
					game->ui.castBar->Cast (
						img, game->player->GetComboSystem()->GetCurrentSpell()->GetTotalCastTime ( ) );
			}
		}
	}
	//Camera::MouseRightClick ( bDown );
}
bool GameInputHandler::OnEscape ( void ) {
	APPHANDLE->SwitchState ( nullptr );
	return false;
}