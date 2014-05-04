#include "Pivot Menu.h"

PivotMenu::PivotMenu ( int dirX, int dirY, int sX, int sY, int offsetX, int offsetY, float ang, LPCSTR tabFilename )
{
	dx = dirX;
	dy = dirY;
	sx = sX;
	sy = sY;
	ox = offsetX;
	oy = offsetY;
	bOpen = false;
	fMinAngle = 0.0f;
	tabFile = tabFilename;
	fAng = ang;
}
PivotMenu::~PivotMenu ( void )
{
	HX_SAFE_REMOVEREF ( pivot );
	HX_SAFE_REMOVEREF ( pivotMesh );
	HX_SAFE_REMOVEREF ( pivotFX );
	if ( tabTex )
		HX_SAFE_REMOVEREF ( tabTex );
}
void PivotMenu::Load ( hxCore* const core )
{
	pivotFX = new hxEffect ( 0, core );
	pivotFX->LoadFromHXE ( "Data\\PivotMenuShader.HLSL", false );

	pivotMesh = new hxMesh ( 0, core );
	if ( tabFile )
	{
		_hxVertex verts[8];
		DWORD indices[12];
		verts[0] = _hxVertex ( 0, 0.9, 0, 0, 0, 0, 0, 0, 0, 1-((float)dx/2.0f+0.5f), 0 );
		verts[1] = _hxVertex ( 0, -0.9, 0, 0, 0, 0, 0, 0, 0, 1-((float)dx/2.0f+0.5f), 1 );
		verts[2] = _hxVertex ( 0, 0.9, 0.8, 0, 0, 0, 0, 0, 0, ((float)dx/2.0f+0.5f), 0 );
		verts[3] = _hxVertex ( 0, -0.9, 0.8, 0, 0, 0, 0, 0, 0, ((float)dx/2.0f+0.5f), 1 );
		verts[4] = _hxVertex ( 0, 0.3, 0, 0, 0, 0, 0, 0, 0, 1-((float)dx/2.0f+0.5f), 0 );
		verts[5] = _hxVertex ( 0, -0.3, 0, 0, 0, 0, 0, 0, 0, 1-((float)dx/2.0f+0.5f), 1 );
		verts[6] = _hxVertex ( 0.1 * dx, 0.3, 0, 0, 0, 0, 0, 0, 0, ((float)dx/2.0f+0.5f), 0 );
		verts[7] = _hxVertex ( 0.1 * dx, -0.3, 0, 0, 0, 0, 0, 0, 0, ((float)dx/2.0f+0.5f), 1 );
		verts[0].pos = hxVector3 ( 0, sy*0.5f, 0 );
		verts[1].pos = hxVector3 ( 0, -sy*0.5f, 0 );
		verts[2].pos = hxVector3 ( 0, sy*0.5f, sx );
		verts[3].pos = hxVector3 ( 0, -sy*0.5f, sx );
		verts[4].pos = hxVector3 ( 0, (float)tabTex->GetHeight()*0.5f, 0 );
		verts[5].pos = hxVector3 ( 0, -(float)tabTex->GetHeight(), 0 );
		verts[6].pos = hxVector3 ( tabTex->GetWidth() * dx, 50, 0 );
		verts[7].pos = hxVector3 ( tabTex->GetWidth() * dx, -50, 0 );
		indices[0] = 0; indices[1] = 1; indices[2] = 2;
		indices[3] = 1; indices[4] = 3; indices[5] = 2;
		indices[6] = 4; indices[7] = 5; indices[8] = 6;
		indices[9] = 5; indices[10] = 7; indices[11] = 6;
		pivotMesh->CreateFromBuffers ( verts, indices, 8, 12 );

		tabTex = new hxImage ( 0, core );
		tabTex->LoadFromHXI ( tabFile );
	}
	else
	{
		_hxVertex verts[4];
		DWORD indices[6];
		if ( dx )
		{
			verts[0] = _hxVertex ( 0, 0.9, 0, 0, 0, 0, 0, 0, 0, 1-((float)dx/2.0f+0.5f), 0 );
			verts[1] = _hxVertex ( 0, -0.9, 0, 0, 0, 0, 0, 0, 0, 1-((float)dx/2.0f+0.5f), 1 );
			verts[2] = _hxVertex ( 0, 0.9, 0.8, 0, 0, 0, 0, 0, 0, ((float)dx/2.0f+0.5f), 0 );
			verts[3] = _hxVertex ( 0, -0.9, 0.8, 0, 0, 0, 0, 0, 0, ((float)dx/2.0f+0.5f), 1 );
			verts[0].pos = hxVector3 ( 0, sy*0.5f, 0 );
			verts[1].pos = hxVector3 ( 0, -sy*0.5f, 0 );
			verts[2].pos = hxVector3 ( 0, sy*0.5f, sx );
			verts[3].pos = hxVector3 ( 0, -sy*0.5f, sx );
		}
		else if ( dy )
		{
			verts[0].pos = hxVector3 ( -sx*0.5f, 0, 0 );
			verts[0].texC = hxVector2 ( 0, 0.0f + (1-dy)/2.0f );
			verts[1].pos = hxVector3 ( sx*0.5f, 0, 0 );
			verts[1].texC = hxVector2 ( 1, 0.0f + (1-dy)/2.0f );
			verts[2].pos = hxVector3 ( -sx*0.5f, 0, sy );
			verts[2].texC = hxVector2 ( 0, 1.0f - (1-dy)/2.0f );
			verts[3].pos = hxVector3 ( sx*0.5f, 0, sy );
			verts[3].texC = hxVector2 ( 1, 1.0f - (1-dy)/2.0f );
		}
		indices[0] = 0; indices[1] = 1; indices[2] = 2;
		indices[3] = 1; indices[4] = 3; indices[5] = 2;
		pivotMesh->CreateFromBuffers ( verts, indices, 4, 6 );

		tabTex = nullptr;
	}

	pivot = new hxObject ( 0, core );
	pivot->SetMaterial ( new hxMaterial ( 0, core ) );
	pivot->GetMaterial()->SetEffect ( pivotFX );
	pivot->GetMaterial()->SetConstantBufferVariableFloat ( "gAlpha", 1.0f );
	pivot->SetMesh ( pivotMesh );
	if ( tabTex )
		pivot->GetMaterial()->SetTexture ( 0, tabTex );
	pivot->SetGBufferMaterial ( nullptr );
	pivot->GetMaterial()->SetDepthMode ( DEPTH_DISABLED );
	pivot->GetMaterial()->SetCullingMode ( CULL_NONE );
	pivot->GetMaterial()->SetBlendingMode ( BLEND_ALPHA );
	pivot->DisableFrustumCulling ( );

	fCurAngle = 0.0f;

	OnResize ( core->WindowsDevice->GetWindowWidth ( ), core->WindowsDevice->GetWindowHeight ( ) );
}
void PivotMenu::SetFade ( float fFade )
{
	pivot->GetMaterial()->SetConstantBufferVariableFloat ( "gAlpha", fFade );
}
void PivotMenu::OnResize ( UINT width, UINT height )
{
	//pivot->SetPosition ( ox, oy, 1 );//-dx + (float)ox*2.0f/(float)1, (float)oy*2.0f/(float)1, 1.0f );
	
	/*if ( !dy )
	{
		posX = (float)(width - (dx*width)) / 2.0f; posX += posX? -sx : sx;
		posY = (float)height / 2.0f - (float)sy / 2.0f;
		if ( tabTex )
		{
			posXsm = width - tabTex->GetWidth ( );
			posYsm = (float)height / 2.0f - (float)tabTex->GetHeight ( ) / 2.0f;
		}
	}
	else
	{
		posX = (float)width / 2.0f - (float)sx / 2.0f;
		posY = (float)(height - (dy*height)) / 2.0f - sy; posY += posY? -sy : sy;
		if ( tabTex )
		{
			posXsm = (float)width / 2.0f - (float)tabTex->GetWidth ( ) / 2.0f;
			posYsm = height - tabTex->GetHeight ( );
		}
	}*/
}
bool PivotMenu::Update ( float fDeltaTime )
{
	if ( !dy )
	{
		float fTargetAngle = fMinAngle;
		if ( bOpen )
			fTargetAngle = 90.0f * dx;

		float fDeltaAngle = fTargetAngle - fCurAngle;
		float fMultiplier = 5.0f;
		if ( abs ( fDeltaAngle ) < 5.0f )
			fMultiplier = 1.0f / fDeltaTime;
		fCurAngle += fmod ( fDeltaAngle * fMultiplier * fDeltaTime, 360.0f );
		pivot->SetAngle ( 0, 0, 0 );
		pivot->Yaw ( fCurAngle );
		pivot->Pitch ( fAng );
	}
	else
	{
		float fTargetAngle = fMinAngle;
		if ( bOpen )
			fTargetAngle = 90.0f * dy;
		
		float fDeltaAngle = fTargetAngle - fCurAngle;
		float fMultiplier = 5.0f;
		if ( abs ( fDeltaAngle ) < 5.0f )
			fMultiplier = 1.0f / fDeltaTime;
		fCurAngle += fmod ( fDeltaAngle * fMultiplier * fDeltaTime, 360.0f );
		pivot->SetAngle ( 0, 0, 0 );
		pivot->Pitch ( fCurAngle );
		pivot->Yaw ( fAng );
	}
	
	UINT w = pivot->GetCorePtr()->WindowsDevice->GetWindowWidth ( );
	UINT h = pivot->GetCorePtr()->WindowsDevice->GetWindowHeight ( );
	hxMatrix mtx =  hxOrthogonalProjMatrix ( w, h, 0.01f, 10.0f );
	pivot->GetMaterial()->SetConstantBufferVariableMatrix ( "gProj", mtx );

	return true;
}
void PivotMenu::SetMinAngle ( float fAngle )
{
	fMinAngle = fAngle;
}
void PivotMenu::OnMouseButton ( int mx, int my, bool bDown )
{
	if ( !bDown )
		bOpen = !bOpen;
}
int PivotMenu::GetPositionX ( void ) const
{
	return pivot->GetPositionX ( );
	//return posXsm;
}
int PivotMenu::GetPositionY ( void ) const
{
	return pivot->GetPositionX ( );
	//return posYsm;
}
int PivotMenu::GetSizeX ( void ) const
{
	if ( !tabTex )
		return 0;

	return tabTex->GetWidth ( );
}
int PivotMenu::GetSizeY ( void ) const
{
	if ( !tabTex )
		return 0;

	return tabTex->GetHeight ( );
}