//---------------------------------------------------------------------------------------------------------
//!
//!	\file physics/system.h
//!	
//!	DYNAMICS COMPONENT:
//!		The system encapsulate the physical representation of an entity.
//!
//!	Author: Mustapha Bismi (mustapha.bismi@ninjatheory.com)
//! Created: 2005.07.11
//!
//---------------------------------------------------------------------------------------------------------

#ifndef _DYNAMICS_SYSTEMS_INC
#define _DYNAMICS_SYSTEMS_INC

#include "config.h"
#include "logicgroup.h"
#include "collisionbitfield.h"
#include "hair/forcefieldresult.h"
#include <hkcollide/shape/capsule/hkCapsuleShape.h>
#include <hkcollide/shape/sphere/hkSphereShape.h>
#include <hkcollide/shape/box/hkBoxShape.h>
#include <hkcollide/shape/cylinder/hkCylinderShape.h>
#include <hkcollide/shape/heightfield/hkHeightFieldShape.h>
#include <hkcollide/shape/list/hkListShape.h>
#include <hkcollide/shape/mesh/hkMeshShape.h>
#include <hkcollide/shape/transform/hkTransformShape.h>
#include <hkcollide/shape/convextransform/hkConvexTransformShape.h>
#include <hkcollide/shape/convexvertices/hkConvexVerticesShape.h>
#include <hkcollide/shape/convexlist/hkConvexListShape.h>
#include <hkdynamics/entity/hkRigidBody.h>
#include "meshshape.h"
#include "lua/ninjalua.h"


//#define JUST_COLLISION_RENDER

class CEntity;
class hkShapePhantom;
class CAttackData;
class AftertouchControlParameters;
class CollisionEffectHandler;
struct hkContactPointAddedEvent;

namespace Physics
{
	// Class for explosion parameters
	class CExplosionParams
	{
	public:
		CPoint		m_obPosition;
		float		m_fPush;
		float		m_fPushDropoff;
		float		m_fRadius;
		CEntity*	m_pobOriginator;
	};

	class CCollisionListener;
	//class AdvancedCharacterController;
	//class CAttackData;

	// ---------------------------------------------------------------
	//	Class for handling combat strikes generated by collisions.
	//	It's more of a game mechanism, so its a temporary solution for integrating
	//  the combat system with the physics.
	// ---------------------------------------------------------------
	class CollisionStrikeHandler
	{
	public:

		enum STRIKE_TARGET
		{
			PLAYER		= 1,
			ENEMY		= 2,
			FRIENDLY	= 4,
		};

		CollisionStrikeHandler(CEntity* pobParentEntity);

		void Update ();
		void SetStrikeFilter (int iFilter) { m_iStrikeFilter=iFilter; }

		void Enable (int iMaxStrikes=10,float fTimeOut=0.0f);
		void Disable ();

		bool IsEnabled () { return m_bEnabled; }
		int GetStrikeCount () { return m_iStrikeCount; }
		bool GenerateStrike (CEntity* pobTarget,const CAttackData* pobAttackData);

		static const int MAX_COLLIDEES_PER_FRAME = 16;
		CEntity*		m_pCollidees[ MAX_COLLIDEES_PER_FRAME ];
		float			m_fProjVel[ MAX_COLLIDEES_PER_FRAME ];
		int32_t		m_iNumCollidee;

		void SetStruckByKO() { m_bStruckByKO = true; }; // Gets reset each time the handler is reset
		bool GetStruckByKO() { return m_bStruckByKO; };
	private:
		CEntity*		m_pobParentEntity;

		int				m_iMaxStrikes;
		float			m_fTimeOut;

		int				m_iStrikeCount;

		int				m_iStrikeFilter;

		bool			m_bEnabled;

		bool			m_bStruckByKO;
	};

	// ---------------------------------------------------------------
	//	Class for attaching LUA message callbacks to collisions
	// ---------------------------------------------------------------
	class CollisionCallbackHandler
	{
	public:

		CollisionCallbackHandler (CEntity* pobParentEntity);

		void SetCollisionCallback (float fMinProjVel);

		void Process (const hkContactPointAddedEvent& event,CEntity* pobCollidee);

		void Update ();

		static const int MAX_COLLIDEES_PER_FRAME = 16;
		CEntity*		m_pCollidees[ MAX_COLLIDEES_PER_FRAME ];
		float			m_fProjVel[ MAX_COLLIDEES_PER_FRAME ];
		int32_t				m_iNumCollidee;

	private:

		CEntity*			m_pobParentEntity;

		bool				m_bGenerateCallback;
		float				m_fProjVelThreshold;
	};

	// ---------------------------------------------------------------
	//	The system is the main dynamics component. This is the central 
	//	hub, and the only access to the overall physics from 
	//	external components. A system is mainly a list of Elements 
	//	and LogicGroups.
	// ---------------------------------------------------------------
	class System : public LogicGroup
	{
	public:
												System( CEntity* p_entity, const ntstd::String& p_name );	//!< Constructor
		virtual									~System( );													//!< Destructor.									

		// LogicGroup Interface --------------------------------------------------------------------
		virtual const LogicGroup::GROUP_TYPE	GetType( ) const;											//!< Return the system logic group type.
		virtual void							Update( const float p_timestep );
		virtual void							Activate( bool activateInHavok = false );					//!< Activate everything in the system.
		virtual void							Deactivate( );												//!< Deactivate everything in the system.
		virtual void							PausePresenceInHavokWorld(bool add);					
		virtual void							UpdateCollisionFilter( );									//!< Update the collision filters
		virtual void			                SetCollisionFilterInfo(uint32_t info);						//!< Change the collision filter info. It NOT updates collision filter.
		virtual uint32_t						GetCollisionFilterInfo() const;								//!< Return the collisoin filter info of the first element.

		enum CollisionFilterInfoField
		{
			I_AM = 0,
			I_COLLIDE_WITH = 1,
			RAGDOLL_MATERIAL = 2,
			KO_STATES = 3,
			RAYCAST_MATERIAL = 4
		};

		// Just for testing. Will be removed.
		virtual void			                SetCollisionFilterInfoEx(int field, uint32_t info);//!< Change the collision filter info. It NOT updates collision filter.
		

		virtual void							EntityRootTransformHasChanged();
		virtual void							Pause( bool bPause );

		virtual CDirection						GetLinearVelocity( );
		virtual CDirection						GetAngularVelocity( );
		virtual void							SetLinearVelocity( const CDirection& p_linearVelocity );	
		virtual void							SetAngularVelocity( const CDirection& p_angularVelocity );
		virtual void							ApplyLinearImpulse( const CDirection& obForce );
		virtual void							ApplyAngularImpulse( const CDirection& obForce );
		virtual void							ApplyLocalisedLinearImpulse( const CDirection& p_vel, const CVector& p_point );


		// System specifics ------------------------------------------------------------------------	
		CCollisionListener*						GetCollisionListener();										//!< Return the collision listener

		// - Group management	--------------------------------------------------------------------
		LogicGroup*								GetGroupByName( const ntstd::String& p_name );				//!< We should be able to access to the groups by name.
		LogicGroup*								GetFirstGroupByType( const LogicGroup::GROUP_TYPE p_type );	//!< Return the first group by it's type.
		const LogicGroup*						GetFirstGroupByType( const LogicGroup::GROUP_TYPE p_type ) const;
		ntstd::List<LogicGroup*>&				GetGroupList( );											//!< Return the group list.
		void									AddGroup( LogicGroup* p_groupe );							//!< Add a group to the system.

		// - Legacy code	------------------------------------------------------------------------
		static System*							ConstructSystemFromClump( CEntity* p_entity );				//!< Tries to construct a sensible system from a clump

		ForceFieldResult&						GetForceResult() {return m_forceFieldResult;}
		const ForceFieldResult&					GetForceResult() const {return m_forceFieldResult;}


		// - Collision event handlers --------------------------------------------------------------		

		CollisionStrikeHandler*					GetCollisionStrikeHandler() { return m_pobCollisionStrikeHandler; }

		CollisionCallbackHandler*				GetCollisionCallbackHandler () { return m_pobCollisionCallbackHandler; }

		//! Retrieves collision effect handler.
		CollisionEffectHandler*					GetCollisionEffectHandler() {return m_pobCollisionEffectHandler; }

		void									RegisterCollisionEffectFilterDef(CHashedString pcName);




		/*AdvancedCharacterController*		GetCharacter();
		void								SetCharacter( AdvancedCharacterController* );*/

		// This convenient interface is to provide an easy access to the character functionality.
		// Using these functions on an object have no effects whatsoever.
		enum CHARACTER_MODE
		{
			CHARACTER_CONTROLLER,
			RAGDOLL_DEAD,
			RAGDOLL_ANIMATED_PHYSICALLY,
			RAGDOLL_ANIMATED_ABSOLUTELY,
			RAGDOLL_TRANSFORM_TRACKING_PHYSICALLY,
			RAGDOLL_TRANSFORM_TRACKING_ABSOLUTELY
		};
 
		enum COLLISION_MODE
		{
			CHARACTER_CONTROLLER_COLLIDE_WITH_EVERYTHING = 0,
            CHARACTER_CONTROLLER_COLLIDE_WITH_STATIC_ONLY,
            CHARACTER_CONTROLLER_DO_NOT_COLLIDE,
            RAGDOLL_DEAD_ON_CONTACT,
            RAGDOLL_IGNORE_CONTACT
		};

		void SetCharacterCollisionRepresentation( CHARACTER_MODE eChar, COLLISION_MODE eCol);
		void UnfixRagdoll();

		void SetKOState( KO_States state );

		// Debugging stuff

		void									Debug_RenderAllCollisionInfo ();

		// Send a message when a collision is made, only works if there is a contactPointConfirmedCallback
		void SetMsgOnCollision(bool bValue) { m_bMsgOnCollision = bValue; }
		bool GetMsgOnCollision(void) const { return m_bMsgOnCollision; }
		void SetSendCollisionMessageNextUpdate() { m_bSendCollisionMessageNextUpdate = true; };
	private:

		bool m_bSendCollisionMessageNextUpdate;

		System();											//!< Private constructor
		System( const System& );							//!< Private constructor

		ntstd::List<LogicGroup*>	m_groupList;			//!< The current list of instance
		CCollisionListener*			m_collisionListener;	//!< Collision listener mainly used for sound and various events. [Mem::MC_HAVOK]

		ForceFieldResult			m_forceFieldResult;

		CollisionStrikeHandler*		m_pobCollisionStrikeHandler;	//!< [Mem::MC_HAVOK]
		CollisionCallbackHandler*	m_pobCollisionCallbackHandler;	//!< [Mem::MC_HAVOK]

		CollisionEffectHandler*		m_pobCollisionEffectHandler;	//!< Maintains collision effects (bounce/crash/roll/slide sounds etc.)

		CEntity*                    m_pobParentEntity;

		/*
		bool						m_hurtPlayer;
		bool						m_doHurt;
		float						m_hurtTiming;
		bool						m_immuneToRC;
		*/

		// Allow this object to send messages to iteslf when it has collided
		bool						m_bMsgOnCollision;

		//AdvancedCharacterController* m_character;			//!< Facultative character controller.

		// Bring in the dynamics bindings
		public:
			#include "game/dynamics_lua.h"
	};

	class DebugCollisionTools
	{
	public:

		static void RenderCylinderShape (const CMatrix& obWorldMatrix,const hkCylinderShape* pobShape);
		static void RenderCapsuleShape (const CMatrix& obWorldMatrix,const hkCapsuleShape* pobShape);
		static void RenderBoxShape (const CMatrix& obWorldMatrix,const hkBoxShape* pobShape);
		static void RenderSphereShape (const CMatrix& obWorldMatrix,const hkSphereShape* pobShape);
		static void RenderMeshShape(const CMatrix& obWorldMatrix,const hsMeshShape * pobShape);
		static void RenderConvexVerticesShape(const CMatrix& obWorldMatrix,const hkConvexVerticesShape * pobShape);
		static void RenderTransformShape (const CMatrix& obWorldMatrix,const hkTransformShape* pobShape);
		static void RenderConvexTransformShape (const CMatrix& obWorldMatrix,const hkConvexTransformShape* pobShape);
		static void RenderListShape (const CMatrix& obWorldMatrix,const hkListShape* pobShape);

		static void RenderShape (const CMatrix& obWorldMatrix,const hkShape* pobShape);
		static void RenderConstraint (const hkConstraintInstance& constraint);


		static void GetMotionType (const hkRigidBody* pobRigidBody,char* pcOutput);

		static void RenderCollisionFlags (const CPoint& obPosition,const Physics::EntityCollisionFlag& obFlag,const Physics::FilterExceptionFlag& obException);
		static void RenderCollisionFlags (const hkRigidBody* pobRigidBody);
		static void RenderCollisionFlags (const hkShapePhantom* pobShapePhantom);
	};

} // Physics

LV_DECLARE_USERDATA2(Physics::System, System);
 
#endif // _DYNAMICS_SYSTEMS_INC
