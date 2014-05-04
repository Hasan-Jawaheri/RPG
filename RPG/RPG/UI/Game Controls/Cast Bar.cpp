#include "Cast Bar.h"
#include "Action Bar.h"

CastBar::CastBar ( void )
{
	fCast = 0.0f;
	//spell = nullptr;
	image = nullptr;
	bOver = false;
}
CastBar::~CastBar ( void )
{
	HX_SAFE_REMOVEREF ( coverImg );
}
void CastBar::Load ( hxCore* const core )
{
	shader = new hxPixelShader ( core );
	shader->LoadFromFile ( "Data\\CastBarShader.HLSL", HX_MAKEPROFILE ( 4, 0 ), "PS" );

	sprite = new hxSprite ( 0, core );
	sprite->GetMaterial()->SetImage ( nullptr );
	sprite->GetMaterial()->SetPixelShader ( shader );
	sprite->GetMaterial()->SetConstantBufferVariableFloat ( "gAlpha", 1.0f );
	sprite->SetPriority ( 0 );
	fAlpha = 255.0f;

	coverImg = new hxImage ( 0, core );
	coverImg->Load ( "Data\\Cast Cover.png" );
	sprite->GetMaterial()->SetTexture ( 0, coverImg );

	SetSize ( 100, 100 );
}
bool CastBar::Update ( float fDeltaTime )
{
	if ( image )
	{
		fCast += fDeltaTime;
		if ( fCast >= fSpellTime )
		{
			sprite->GetMaterial()->SetTexture ( 0, coverImg );
		}
		if ( fCast >= fSpellTime + 0.5f ) //0.5 = time for animation after cast
		{
			fCast = 0.0f;
			image = nullptr;
			sprite->GetMaterial()->SetTexture ( 1, coverImg );
			return true;
		}

		float fRealCast = fCast / fSpellTime;
		sprite->GetMaterial()->SetConstantBufferVariableFloat ( "gCast", min ( fRealCast, 1.5f ) );

		if ( fRealCast <= 1.0f )
		{
			float fCosAng;
			if ( fRealCast < 0.5f )
				fCosAng = 2 - (cosf ( HX_PI * fRealCast * 2.0f ) + 1.0f);
			else
				fCosAng = 2 - (cosf ( HX_PI * (fRealCast-0.5f) * 2.0f ) + 1) + 10.0f;

			sprite->GetMaterial()->SetConstantBufferVariableFloat ( "gCosAngle", fCosAng );
		}
		else
			fCast += fDeltaTime;
	}
	else
		sprite->GetMaterial()->SetConstantBufferVariableFloat ( "gCast", 1.5f );

	return true;
}
void CastBar::Cast ( hxImage* img, float fTime ) {
	hxColor color1 ( 0.1f, 0.0f, 0.5f );
	hxColor color2 ( 0.05f, 0.0f, 0.2f );
	sprite->GetMaterial()->SetConstantBufferVariableFloatArray ( "color1", 3, color1 );
	sprite->GetMaterial()->SetConstantBufferVariableFloatArray ( "color2", 3, color2 );
	sprite->GetMaterial()->SetTexture ( 1, img );

	fCast = 0.0f;
	bOver = false;
	fSpellTime = fTime;
	image = img;
}
float CastBar::GetCurTime ( void ) const {
	return fCast;
}
/*bool CastBar::Cast ( float fTime, Action* sp, UINT targetGUID )
{
	if ( sp == nullptr ) //stop/finish casting
	{
		fCast = spell->GetCastTime ( );
		if ( fTime > 0.5f ) spell->Begin ( targetGUID ); //finish casting
		bOver = true;
		return false;
	}

	if ( fCast > 0.001f )
		if ( fCast / spell->GetCastTime ( ) < 1.0f )
			return false;
	
	if ( sp->GetTargetDependency ( ) && targetGUID == 0 )
		return false;

	float color1[3];
	float color2[3];
	for ( UINT i = 0; i < 3; i++ )
	{
		color1[i] = sp->GetCastColor ( i );
		color2[i] = sp->GetCastColor ( i + 3 );
	}
	sprite->GetMaterial()->SetConstantBufferVariableFloatArray ( "color1", 3, color1 );
	sprite->GetMaterial()->SetConstantBufferVariableFloatArray ( "color2", 3, color2 );
	sprite->GetMaterial()->SetTexture ( 1, sp->GetImage ( ) );
	fCast = 0.0f;
	spell = sp;
	bOver = false;
	return true;
}
Action* CastBar::GetCastingSpell ( void ) const
{
	return spell;
}*/