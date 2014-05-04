#include "Health Bar.h"

const int barWidth = 227;
const int barHeight = 70;

HealthBar::HealthBar ( bool bRev ) : PivotMenu ( bRev? -1 : 1, 0, barWidth, barHeight, 0, 0, -15.0f, nullptr )
{
	bReversed = bRev;
	fOffset = 0.0f;
}
HealthBar::~HealthBar ( void )
{
	shader = pivotFX->SetPixelShader ( shader );
}
void HealthBar::Load ( hxCore* const core )
{
	PivotMenu::Load ( core );

	/*hxImage* asd = new hxImage ( 0, core );
	asd->Load ( "Data\\Health Foreground.png" );
	asd->SaveToHXI ( "Data\\Health Foreground.HXI" );
	asd->Load ( "Data\\Health Motion.png" );
	asd->SaveToHXI ( "Data\\Health Motion.HXI" );*/

	shader = new hxPixelShader ( core );
	shader->LoadFromFile ( "Data\\HealthBarShader.HLSL", HX_MAKEPROFILE ( 4, 0 ), "PS" );

	LPCSTR files[3] = {
		"Data\\Health Background.png",
		"Data\\Health Foreground.png",
		"Data\\Health Motion.png"
	};
	bar = new hxImage ( 0, core );
	bar->Load ( files, 3 );

	hxPixelShader* ps = shader;
	shader = pivotFX->SetPixelShader ( ps );
	pivot->GetMaterial()->SetEffect ( pivotFX );

	pivot->GetMaterial()->SetConstantBufferVariableFloat ( "gAlpha", 1.0f );
	pivot->GetMaterial()->SetConstantBufferVariableInt ( "gReversed", (int)bReversed );
	fAlpha = 255.0f;
	int size[2] = { barWidth, barHeight };
	pivot->GetMaterial()->SetConstantBufferVariableIntArray ( "gBoxSize", 2, size );

	pivot->GetMaterial()->SetConstantBufferVariableFloat ( "gOffset", fOffset );

	pivot->GetMaterial()->SetTexture ( 0, bar );
	
	SetHealth ( 100.0f );
	SetPower ( 100.0f );
	SetPowerColor ( 0.0f, 0.0f, 1.0f );
}
bool HealthBar::Update ( float fDeltaTime )
{
	PivotMenu::Update ( fDeltaTime );
	fOffset -= fDeltaTime * 0.2f;
	pivot->GetMaterial()->SetConstantBufferVariableFloat ( "gOffset", fOffset );

	return true;
}
void HealthBar::SetHealth ( float fPercentage )
{
	pivot->GetMaterial()->SetConstantBufferVariableFloat ( "gHealth", fPercentage / 100.0f );
}
void HealthBar::SetPower ( float fPercentage )
{
	pivot->GetMaterial()->SetConstantBufferVariableFloat ( "gPower", fPercentage / 100.0f );
}
void HealthBar::SetPowerColor ( float pwrR, float pwrG, float pwrB )
{
	float powerColor[3];
	powerColor[0] = pwrR;
	powerColor[1] = pwrG;
	powerColor[2] = pwrB;
	pivot->GetMaterial()->SetConstantBufferVariableFloatArray ( "gPowerCol", 3, powerColor );
}
void HealthBar::SetPosition ( int x, int y )
{
	float fXOffset = bReversed ? barWidth : 0.0f;
	float width = pivot->GetMaterial()->GetCorePtr()->WindowsDevice->GetWindowWidth ( );
	float height = pivot->GetMaterial()->GetCorePtr()->WindowsDevice->GetWindowHeight ( );
	//pivot->SetPosition ( (float)(x+fXOffset)*2.0f/width - 1.0f, -((float)(y+(barHeight/2.0f))*2.0f/height - 1.0f), 0 );
	pivot->SetPosition ( x - width / 2 + fXOffset, -y + height / 2, 1.0f );
}
int HealthBar::GetPositionX ( void ) const
{
	float fXOffset = bReversed ? barWidth : 0.0f;
	float width = pivot->GetMaterial()->GetCorePtr()->WindowsDevice->GetWindowWidth ( );
	return pivot->GetPositionX ( ) + width / 2 - fXOffset;//(pivot->GetPositionX ( ) + 1)*(width/2.0f)-fXOffset;
}
int HealthBar::GetPositionY ( void ) const
{
	float height = pivot->GetMaterial()->GetCorePtr()->WindowsDevice->GetWindowHeight ( );
	return -pivot->GetPositionY ( ) + height / 2;//(-pivot->GetPositionY ( ) + 1.0f)*(height/2.0f) - barHeight/2.0f;
}
int HealthBar::GetSizeX ( void ) const
{
	return barWidth;
}
int HealthBar::GetSizeY ( void ) const
{
	return barHeight;
}