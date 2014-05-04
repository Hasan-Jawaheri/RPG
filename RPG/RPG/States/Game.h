#ifndef _GAME_H_
#define _GAME_H_

#include "..//Common.h"
#include "..//UI//UI.h"
#include "..//UI//Game Controls//Chat.h"

class Game;

class GameInputHandler : public UIElement {
	class Game* game;
public:
	GameInputHandler ( class Game* g );
	virtual void OnMouseButton ( int mx, int my, bool bDown );
	virtual void OnMouseButton2 ( int mx, int my, bool bDown );
	virtual bool OnFocus ( void ) { return false; }
	virtual bool OnInput ( void ) { return true; }
	virtual bool OnEnter ( void );
	virtual void OnKeydown ( short key );
	virtual void OnKeyup ( short key );
	virtual bool OnEscape ( void );
	virtual int GetPosZ ( void ) const { return 0; }
};

class Game : public hxGameState {

	friend class GameInputHandler;

	class Player* player;
	class Unit* target;
	
	GameInputHandler* input;
	struct USER_INTERFACE {
		class CastBar* castBar;
		class HealthBar* playerHealth;
		class HealthBar* targetHealth;
		class StanceBar* stanceBar;
		class TargetCursor* targetCursor;
		class ChatEditBox* chatEdit;
	} ui;

public:
	Game ( HasX11* a );
	~Game ( void );

	void Load ( void );
	void Update ( float fDeltaTime );
	void Cleanup ( void );
};

#endif //_GAME_H_