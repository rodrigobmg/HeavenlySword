//------------------------------------------------------------------------------------------
//!
//!	\file CamCool_kingBossFightv2.cpp
//!
//------------------------------------------------------------------------------------------

#include "camera/coolcam_kingbossfightv2.h"
#include "game/entity.h"
#include "game/entity.inl"
#include "objectdatabase/dataobject.h"
#include "game/entityboss.h"
#include "game/entitymanager.h"
#include "camera/camutils.h"
#include "core/visualdebugger.h"
#include "gfx/graphing.h"
#include "camera/camview.h"

START_CHUNKED_INTERFACE( CoolCam_KingBossFightv2Def, Mem::MC_CAMERA)
	DEFINE_INTERFACE_INHERITANCE(CoolCam_RootBossDef)
	COPY_INTERFACE_FROM(CoolCam_RootBossDef)

	PUBLISH_VAR_WITH_DEFAULT_AS( m_fBossHeightThreshold, 1.8f, BossHeightThreshold )
	PUBLISH_VAR_WITH_DEFAULT_AS( m_fBossHeightCamDistanceModifier, 1.2f, BossHeightCamDistanceModifier )
END_STD_INTERFACE

CoolCam_KingBossFightv2Def::CoolCam_KingBossFightv2Def( void )
: CoolCam_RootBossDef(),
  m_fBossHeightThreshold(1.8f),
  m_fBossHeightCamDistanceModifier(1.2f)
{
}

CoolCam_KingBossFightv2* CoolCam_KingBossFightv2Def::Create( const CamView& view )
{
	return NT_NEW_CHUNK( Mem::MC_CAMERA ) CoolCam_KingBossFightv2( view, this );
}

CoolCam_KingBossFightv2::CoolCam_KingBossFightv2( const CamView& view, const CoolCam_KingBossFightv2Def* pCamDef )
: CoolCam_RootBoss( view, pCamDef ),
  m_pCamDef( pCamDef ),
  m_fSeperationDistanceModifier(1.0f),
  m_obFocusToCameraDir( CONSTRUCT_CLEAR ),
  m_obCamVerticalOffset( CONSTRUCT_CLEAR ),
  m_fCameraPullbackMultiplier(1.0f),
  m_obFocusVerticalOffset( CONSTRUCT_CLEAR ),
  m_bReverseModeActivated(false),
  m_obCachedPosition( CONSTRUCT_CLEAR )
{
	m_pobLinearSpeedSampleSet = m_obLinearSpeed.AddSampleSet( "SPEED", 750, DC_GREEN );
	m_pobFocusSpeedSampleSet = m_obFocusSpeed.AddSampleSet( "SPEED", 750, DC_GREEN );

	m_obLinearSpeed.SetYAxis( 0.0f, m_pCamDef->GetCameraMaxSpeed() , m_pCamDef->GetCameraMaxSpeed() / 100.0f );
	m_obFocusSpeed.SetYAxis( 0.0f, m_pCamDef->GetFocusPointMaxSpeed(), m_pCamDef->GetFocusPointMaxSpeed() / 100.0f );

	CPoint obPlayerPosition( CONSTRUCT_CLEAR );
	CEntity* pPlayer = CEntityManager::Get().GetPlayer();
	if( pPlayer && pPlayer->GetSceneElement() )
	{
		obPlayerPosition = pPlayer->GetSceneElement()->GetPosition();
	}
	else if( pPlayer ) 
	{
		obPlayerPosition = pPlayer->GetPosition();
	}

	CPoint obBossPosition( CONSTRUCT_CLEAR );
	if( m_pCamDef->GetBossEntity() && m_pCamDef->GetBossEntity()->GetSceneElement() )
	{
		obBossPosition = m_pCamDef->GetBossEntity()->GetSceneElement()->GetPosition();
	}
	else if( m_pCamDef->GetBossEntity() )
	{
		obBossPosition = m_pCamDef->GetBossEntity()->GetPosition();
	}
	CPoint obFocusPoint = (obPlayerPosition + obBossPosition) * 0.5f;

	CDirection obPlayerDir = pPlayer->GetMatrix().GetZAxis();

	CPoint obNewCameraPos = obPlayerPosition - (obPlayerDir * m_pCamDef->GetCamPlayerDistance());

	m_obNewCamPos = obNewCameraPos;
	//m_obVelBasedPos = m_obTransform.GetTranslation();
	m_obVelBasedPos = obNewCameraPos;
	m_obNewCamLookAt = obFocusPoint;

	CCamUtil::CreateFromPoints(m_obTransform, m_obNewCamPos, m_obNewCamLookAt);
}

CoolCam_KingBossFightv2::~CoolCam_KingBossFightv2( void )
{
}

CPoint CoolCam_KingBossFightv2::CalcDesiredCameraPosition( CEntity& obPlayer, CPoint& obBossPosition )
{
	if( m_pCamDef->IsReverseAngleActive()==true )
	{
		// calculate position for caching
		const CEntity* pBoss = m_pCamDef->GetBossEntity();
		ntError_p( pBoss!=0, ("Failed to get boss entity for reverse angle setup") );
		if( pBoss )
		{
			const CMatrix obBossTrans = pBoss->GetMatrix();

			if( pBoss->GetSceneElement() )
			{
				obBossPosition = pBoss->GetSceneElement()->GetPosition();
			}
			else
			{
				obBossPosition = pBoss->GetPosition();
			}

			CDirection obVertical = obBossTrans.GetYAxis();
			CDirection obForward = obBossTrans.GetZAxis();
			CDirection obRight = obBossTrans.GetXAxis();

			CDirection obReverseOffset = m_pCamDef->GetReverseAngleOffset();
			CDirection obOffset = (obRight * obReverseOffset.X()) + (obVertical * obReverseOffset.Y()) + (obForward * obReverseOffset.Z());

			m_obCachedPosition = obBossPosition + obOffset;
			m_bReverseModeActivated = true;
		}
		return m_obCachedPosition;
	}
	else
	{
		m_bReverseModeActivated = false;
		CMatrix matCamTrans = GetTransform();

		CDirection obVertical = matCamTrans.GetYAxis();
		CDirection obViewDir = matCamTrans.GetZAxis();
		CDirection obRightDir = matCamTrans.GetXAxis();

		CPoint obPlayerPosition = obPlayer.GetPosition();

	#ifdef BOSS_CAM_DBGREND
		if( m_pCamDef->GetRenderDebug() )
		{
			g_VisualDebug->Printf2D( 885, 20, DC_GREEN, 0, "Cam vert: %f, %f, %f", obVertical.X(), obVertical.Y(), obVertical.Z() );
			g_VisualDebug->Printf2D( 885, 32, DC_GREEN, 0, "Cam right: %f, %f, %f", obRightDir.X(), obRightDir.Y(), obRightDir.Z() );
			g_VisualDebug->Printf2D( 885, 44, DC_GREEN, 0, "Cam dir: %f, %f, %f", obViewDir.X(), obViewDir.Y(), obViewDir.Z() );
		}
	#endif

		CPoint rectPointsPlayer[4];
		rectPointsPlayer[0] = obPlayerPosition + (obRightDir * 0.65f); // bottom left
		rectPointsPlayer[1] = obPlayerPosition - (obRightDir * 0.65f); // bottom right

		rectPointsPlayer[2] = obPlayerPosition + (obVertical * 1.7f) + (obRightDir * 0.65f); // top left
		rectPointsPlayer[3] = obPlayerPosition + (obVertical * 1.7f) - (obRightDir * 0.65f); // top right

		CPoint obBossPositionRoot = m_pCamDef->GetBossEntity()->GetPosition();
		CPoint rectPointsBoss[4];
		rectPointsBoss[0] = obBossPositionRoot + (obRightDir * 0.65f); 
		rectPointsBoss[1] = obBossPositionRoot - (obRightDir * 0.65f);
		
		rectPointsBoss[2] = obBossPositionRoot + (obVertical * 2.4f) + (obRightDir * 0.65f);
		rectPointsBoss[3] = obBossPositionRoot + (obVertical * 2.4f) - (obRightDir * 0.65f);

	#ifdef BOSS_CAM_DBGREND
		if( m_pCamDef->GetRenderDebug() )
		{
			g_VisualDebug->RenderSphere( CQuat( CONSTRUCT_IDENTITY ), rectPointsPlayer[0], 0.25, DC_RED );
			g_VisualDebug->RenderLine( rectPointsPlayer[0], rectPointsPlayer[1], DC_YELLOW );
			g_VisualDebug->RenderLine( rectPointsPlayer[1], rectPointsPlayer[3], DC_YELLOW );
			g_VisualDebug->RenderLine( rectPointsPlayer[3], rectPointsPlayer[2], DC_YELLOW );
			g_VisualDebug->RenderLine( rectPointsPlayer[2], rectPointsPlayer[0], DC_YELLOW );
			g_VisualDebug->RenderSphere( CQuat( CONSTRUCT_IDENTITY ), rectPointsPlayer[3], 0.25, DC_GREEN );

			g_VisualDebug->RenderSphere( CQuat( CONSTRUCT_IDENTITY ), rectPointsBoss[0], 0.25, DC_BLUE );
			g_VisualDebug->RenderLine( rectPointsBoss[0], rectPointsBoss[1], DC_YELLOW );
			g_VisualDebug->RenderLine( rectPointsBoss[1], rectPointsBoss[3], DC_YELLOW );
			g_VisualDebug->RenderLine( rectPointsBoss[3], rectPointsBoss[2], DC_YELLOW );
			g_VisualDebug->RenderLine( rectPointsBoss[2], rectPointsBoss[0], DC_YELLOW );
			g_VisualDebug->RenderSphere( CQuat( CONSTRUCT_IDENTITY ), rectPointsBoss[3], 0.25, DC_CYAN );
		}
	#endif

		CMatrix matProj;
		matProj.Perspective( m_fFOV, GetView().GetAspectRatio(), GetView().GetZNear(), GetView().GetZFar() );

		CMatrix affineInverse = matCamTrans.GetAffineInverse();

		CVector projPointsPlayer[4];
		CVector projPointsBoss[4];
#ifdef BOSS_CAM_DBGREND
		if( m_pCamDef->GetRenderDebug() )
		{
			g_VisualDebug->Printf2D( 885, 550, DC_GREEN, 0, "Player:" );
		}
#endif
		for( int count=0; count<4; count++ )
		{
			// Get Cam Space
			projPointsPlayer[count] = CVector( rectPointsPlayer[count] * affineInverse );

			// Get Projection Space
			projPointsPlayer[count].W() = 0.f;
			projPointsPlayer[count] = projPointsPlayer[count] * matProj;

			// Deal with points at infinity
			if(projPointsPlayer[count].W() < EPSILON)
			{
				projPointsPlayer[count] = CVector( 0.f,0.f,-1.f,1.f );
			}
			else
			{
				projPointsPlayer[count] /= fabsf( projPointsPlayer[count].W() );
			}
			//projPointsPlayer[count].X() *= -1.0f;
	#ifdef BOSS_CAM_DBGREND
			if( m_pCamDef->GetRenderDebug() )
			{
				g_VisualDebug->Printf2D( 885.0f, 562.0f + (count * 12.0f), DC_GREEN, 0, "point %d: (%.3f, %.3f, %.3f)", count, projPointsPlayer[count].X(), projPointsPlayer[count].Y(), projPointsPlayer[count].Z() );
			}
	#endif
			projPointsPlayer[count] += CVector( 1.0f, 1.0f, 0.0f, 0.0f );
			//projPointsPlayer[count] *= 300.0f;
		}

#ifdef BOSS_CAM_DBGREND
		if( m_pCamDef->GetRenderDebug() )
		{
			g_VisualDebug->Printf2D( 885, 610, DC_GREEN, 0, "Boss:" );
		}
#endif
		for( int count=0; count<4; count++ )
		{
			// Get Cam Space
			projPointsBoss[count] = CVector( rectPointsBoss[count] * affineInverse );

			// Get Projection Space
			projPointsBoss[count].W() = 0.f;
			projPointsBoss[count] = projPointsBoss[count] * matProj;

			// Deal with points at infinity
			if(projPointsBoss[count].W() < EPSILON)
			{
				projPointsBoss[count] = CVector( 0.f,0.f,-1.f,1.f );
			}
			else
			{
				projPointsBoss[count] /= fabsf( projPointsBoss[count].W() );
			}
			//projPointsBoss[count].X() *= -1.0f;
	#ifdef BOSS_CAM_DBGREND
			if( m_pCamDef->GetRenderDebug() )
			{
				g_VisualDebug->Printf2D( 885.0f, 622.0f + (count * 12.0f), DC_GREEN, 0, "point %d: (%.3f, %.3f, %.3f)", count, projPointsBoss[count].X(), projPointsBoss[count].Y(), projPointsBoss[count].Z() );
			}
	#endif
			projPointsBoss[count] += CVector( 1.0f, 1.0f, 0.0f, 0.0f );
			//projPointsBoss[count] *= 300.0f;
		}

		bool bHorizTest1 = ( (projPointsPlayer[0].X() < projPointsBoss[0].X()) && (projPointsPlayer[0].X() > projPointsBoss[3].X()) );
		bool bHorizTest2 = ( (projPointsPlayer[3].X() < projPointsBoss[0].X()) && (projPointsPlayer[3].X() > projPointsBoss[3].X()) );

		bool bVertTest1 = ( (projPointsPlayer[0].Y() < projPointsBoss[0].Y()) && (projPointsPlayer[0].Y() > projPointsBoss[3].Y()) );
		bool bVertTest2 = ( (projPointsPlayer[3].Y() < projPointsBoss[0].Y()) && (projPointsPlayer[3].Y() > projPointsBoss[3].Y()) );


		bool bHorizTest3 = ( (projPointsBoss[0].X() < projPointsPlayer[0].X()) && (projPointsBoss[0].X() > projPointsPlayer[3].X()) );
		bool bHorizTest4 = ( (projPointsBoss[3].X() < projPointsPlayer[0].X()) && (projPointsBoss[3].X() > projPointsPlayer[3].X()) );

		bool bVertTest3 = ( (projPointsBoss[0].Y() < projPointsPlayer[0].Y()) && (projPointsBoss[0].Y() > projPointsPlayer[3].Y()) );
		bool bVertTest4 = ( (projPointsBoss[3].Y() < projPointsPlayer[0].Y()) && (projPointsBoss[3].Y() > projPointsPlayer[3].Y()) );


		bool bOverlap = (bHorizTest1 && bVertTest1) || (bHorizTest1 && bVertTest2) || (bHorizTest2 && bVertTest2) || (bHorizTest2 && bVertTest1) ||
						(bHorizTest3 && bVertTest3) || (bHorizTest3 && bVertTest4) || (bHorizTest4 && bVertTest4) || (bHorizTest4 && bVertTest3);
		if( bOverlap )
		{
	#ifdef BOSS_CAM_DBGREND
			if( m_pCamDef->GetRenderDebug() )
			{
				g_VisualDebug->Printf2D( 550.0f, 380.0f, DC_RED, 0, "OVERLAP OVERLAP OVERLAP OVERLAP OVERLAP" );
			}
	#endif
			m_bCameraDragActive = true;
		}

		CPoint obPlayerPositionProper( CONSTRUCT_CLEAR );
		if( obPlayer.GetSceneElement() )
		{
			obPlayerPositionProper = obPlayer.GetSceneElement()->GetPosition();
		}
		else 
		{
			obPlayerPositionProper = obPlayer.GetPosition();
		}

		CDirection obPlayerToBoss(obPlayerPosition - obBossPosition);
		float fPlayerBossSeperation = obPlayerToBoss.Length();
		float fPlayerToBossVerticalDiff = fabsf( obPlayerToBoss.Y() );
		obPlayerToBoss.Normalise();

		CPoint newCamPos( CONSTRUCT_CLEAR );
		CPoint obFocusPoint = ((obPlayerPositionProper * m_pCamDef->GetFocusSplitLambda()) + (obBossPosition * (1.0f - m_pCamDef->GetFocusSplitLambda()))); // * 0.5f;

		if( fPlayerBossSeperation<m_pCamDef->GetCameraModeSwitchDistance() )
		{

			CPoint obCameraPosition( m_obNewCamPos );
			//CDirection obCamToPlayer( obCameraPosition - obPlayerPosition );
			CDirection obFocusToPlayer( obPlayerPositionProper - obFocusPoint );
			float fPlayerFocusDist = obFocusToPlayer.Length();
			obFocusToPlayer.Normalise();
			obFocusToPlayer.Y() = 0.0f;

			// rotate camera to player direction by offset angle to get camera position
			CQuat offsetRot( CDirection( 0.0f, 1.0f, 0.0f), -m_pCamDef->GetCameraOffsetAngle() * DEG_TO_RAD_VALUE );
			CMatrix offsetMat( offsetRot );

			CDirection camPosDir = obFocusToPlayer * offsetMat;

	#ifdef BOSS_CAM_DBGREND
			if( m_pCamDef->GetRenderDebug() )
			{
				g_VisualDebug->RenderSphere( CQuat( CONSTRUCT_IDENTITY ), obFocusPoint, 0.4f, DC_GREEN );
				g_VisualDebug->RenderLine( obFocusPoint, obFocusPoint + (obFocusToPlayer * 4.0f), DC_CYAN );			
				g_VisualDebug->RenderLine( obFocusPoint, obFocusPoint + (camPosDir * 4.0f), DC_BLACK );			
			}
	#endif
		
			if( fPlayerToBossVerticalDiff<m_pCamDef->GetBossHeightThreshold() )
			{
				fPlayerToBossVerticalDiff = 0.0f;
			}
			float fDistanceModifier = fPlayerToBossVerticalDiff * m_pCamDef->GetBossHeightCamDistanceModifier();

			//CDirection obVerticalOffset( 0.0f, m_pCamDef->GetVerticalOffset(), 0.0f );
			CDirection obProxOffset = m_pCamDef->GetProximityOffset();
			CDirection obOffset = CDirection( 0.0f, obProxOffset.Y(), 0.0f ) + (obRightDir * obProxOffset.X()) + (obViewDir * obProxOffset.Z());
			newCamPos = obFocusPoint + (camPosDir * (m_pCamDef->GetCameraPullbackBase() + fDistanceModifier + (fPlayerFocusDist * m_pCamDef->GetCameraPullbackMultiplier()))) + obOffset;
		}
		else
		{
			m_obFocusToCameraDir = obPlayerToBoss.Cross( CDirection( 0.0f, 1.0f, 0.0f ) );
			CQuat obAngleModify( CDirection( 0.0f, 1.0f, 0.0f ), m_pCamDef->GetCameraAngleModifierDeg() * DEG_TO_RAD_VALUE );
			CMatrix obAngleModifyMat( obAngleModify );
			m_obFocusToCameraDir = m_obFocusToCameraDir * obAngleModifyMat;

			m_obFocusToCameraDir.Y() = 0.0f;
			CDirection obDistOffset = m_pCamDef->GetDistanceOffset();
			CDirection obOffset = CDirection( 0.0f, obDistOffset.Y(), 0.0f ) + (obRightDir * obDistOffset.X()) + (obViewDir * obDistOffset.Z());
			newCamPos = obPlayerPositionProper + (m_obFocusToCameraDir * m_pCamDef->GetCamPlayerDistance()) + obOffset;
		}

	#ifdef BOSS_CAM_DBGREND
		if( m_pCamDef->GetRenderDebug() )
		{
			if( m_pCamDef->GetDebugVolumeRender() )
			{
				g_VisualDebug->RenderSphere( CQuat( CONSTRUCT_IDENTITY ), m_obTransform.GetTranslation(), 1.0f, 0x00ff0000 );
			}
			g_VisualDebug->RenderLine( m_obTransform.GetTranslation(), obFocusPoint, 0x00ff0000 );

			CDirection obFrustumCentre( obFocusPoint - m_obTransform.GetTranslation() );
			
			CQuat obQuatRot( CDirection( 0.0f, 1.0f, 0.0f ), m_fFOV * DEG_TO_RAD_VALUE );
			CMatrix obRot( obQuatRot );
			CDirection obFrustumRight = obFrustumCentre * obRot;
			g_VisualDebug->RenderLine( m_obTransform.GetTranslation(), m_obTransform.GetTranslation() + obFrustumRight, 0x00ff0000 );

			obQuatRot = CQuat( CDirection( 0.0f, 1.0f, 0.0f ), -m_fFOV * DEG_TO_RAD_VALUE );
			obRot = CMatrix( obQuatRot );
			CDirection obFrustumLeft = obFrustumCentre * obRot;
			g_VisualDebug->RenderLine( m_obTransform.GetTranslation(), m_obTransform.GetTranslation() + obFrustumLeft, 0x00ff0000 );

			g_VisualDebug->Printf2D( 35, 55, DC_GREEN, 0, "FOV: %f\n", m_fFOV );
			g_VisualDebug->Printf2D( 35, 67, DC_PURPLE, 0, "Player->Boss Dist: %f\n", fPlayerBossSeperation );
		}

	#endif
		return newCamPos;
	}
}

CPoint CoolCam_KingBossFightv2::CalcDesiredCameraLookatPoint( CEntity& obPlayer, CPoint& obBossPosition )
{
	CPoint obPlayerPosition( CONSTRUCT_CLEAR );
	if( obPlayer.GetSceneElement() )
	{
		obPlayerPosition = obPlayer.GetSceneElement()->GetPosition();
	}
	else 
	{
		obPlayerPosition = obPlayer.GetPosition();
	}

	CPoint obFocusPoint = ((obPlayerPosition * m_pCamDef->GetFocusSplitLambda()) + (obBossPosition * (1.0f - m_pCamDef->GetFocusSplitLambda()))); // * 0.5f;

	return obFocusPoint;
}
