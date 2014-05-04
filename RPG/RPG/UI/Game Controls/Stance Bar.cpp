#include "Stance Bar.h"

StanceBar::StanceBar ( void ) : PivotMenu ( 0, 1, 50, 120, 0, 0, 0, nullptr )
{
	curStance = 0;
}
StanceBar::~StanceBar ( void )
{
}
void StanceBar::Load ( hxCore* const core )
{
	PivotMenu::Load ( core );

	bar = new hxImage ( 0, core );
	bar->Load ( "Data\\Stance Bar.png" );

	pivot->GetMaterial()->SetConstantBufferVariableFloat ( "gAlpha", 1.0f );
	pivot->GetMaterial()->SetConstantBufferVariableInt ( "gReversed", 0 );
	fAlpha = 255.0f;
	int size[2] = { 50, 120 };
	pivot->GetMaterial()->SetConstantBufferVariableIntArray ( "gBoxSize", 2, size );
	
	pivot->GetMaterial()->SetTexture ( 0, bar );
	pivot->GetMaterial()->SetTexture ( 1, bar );
	
	const char files[3][MAX_PATH] = {
		"Media\\warrior_1.png",
		"Media\\warrior_2.png",
		"Media\\warrior_3.png"
	};
	for ( UINT i = 0; i < 3; i++ ) {
		stances[i] = new hxImage ( 0, core );
		stances[i]->Load ( files[i] );
		stanceSprs[i] = new hxSprite ( 0, core );
		stanceSprs[i]->GetMaterial()->SetImage ( stances[i] );
		stanceSprs[i]->SetSize ( 32, 32 );
	}
}
bool StanceBar::Update ( float fDeltaTime )
{
	PivotMenu::Update ( fDeltaTime );
	for ( UINT i = 0; i < 3; i++ ) {
		if ( curStance - 1 == i )
			stanceSprs[i]->GetMaterial()->SetAlpha ( 255 );
		else
			stanceSprs[i]->GetMaterial()->SetAlpha ( 100 );
	}

	return true;
}
void StanceBar::SetStance ( UINT stance ) {
	curStance = min ( max ( stance, 0 ), 3 );
}
UINT StanceBar::GetCurrentStance ( void ) const {
	return curStance;
}
hxImage* StanceBar::GetCurrentImage ( void ) const {
	if ( !curStance )
		return nullptr;
	return stances[curStance-1];
}
void StanceBar::SetPosition ( int x, int y )
{
	float width = pivot->GetMaterial()->GetCorePtr()->WindowsDevice->GetWindowWidth ( );
	float height = pivot->GetMaterial()->GetCorePtr()->WindowsDevice->GetWindowHeight ( );
	pivot->SetPosition ( x - width / 2, -y + height / 2, 1.0f );
	for ( int i = 0; i < 3; i++ ) {
		stanceSprs[i]->SetPosition ( x - 16, y + 6 + 32 * i );
	}
}
int StanceBar::GetPositionX ( void ) const
{
	float width = pivot->GetMaterial()->GetCorePtr()->WindowsDevice->GetWindowWidth ( );
	return pivot->GetPositionX ( ) + width / 2;
}
int StanceBar::GetPositionY ( void ) const
{
	float height = pivot->GetMaterial()->GetCorePtr()->WindowsDevice->GetWindowHeight ( );
	return -pivot->GetPositionY ( ) + height / 2;
}