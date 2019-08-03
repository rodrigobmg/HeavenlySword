/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_CONSTRAINT_CHAIN_INSTANCE_H
#define HK_DYNAMICS2_CONSTRAINT_CHAIN_INSTANCE_H

#include <hkdynamics/constraint/hkConstraintInstance.h>
#include <hkdynamics/constraint/chain/hkConstraintChainData.h>


class hkEntity;
class hkConstraintChainInstanceAction;
	
	/// hkConstraintChainInstance allows you to create chains of constraints which are solved with a full-matrix solver (as opposed to Havok's iterative solver).
	/// This approach allows for creating much more stable setups. It is useful for creating long ropes, linking objects with high relative mass ratio, and creating
	/// more sturdy rag-doll setups.
	///
	/// Solving of the constraint chains is based on the solver-in-a-solver approach. The entire constraint-chain is treated by the main Havok's solver as a single
	/// constraint. When the chain is processed it performs calculations on all its bodies at once using its own custom information. This means that in itself
	/// the constraint chain is perfectly stiff, however if single elements of the chain are colliding with other objects, separation can occur.
	/// 
	/// If you have a more complicated constraint system, like a ragdoll, you can use several constraint chain instances at the same time. 
	/// Example, you can create a chain going from the left foot to the right hand and another one going from the right foot to the left hand.
	/// Ideally the body/bodies which are shared by the two chains should be as heavy as possible to reduce stretching artefacts.
	///
	/// Note: Constraints chains must use PRIORITY_PSI. Constraint chains do not support TOI-event solving.
	///
	/// hkConstraintChainInstance implements hkConstraintInstance's interface and is treated by hkdynamics just like a normal constraint. However the hkConstraintInstance 
	/// is designed to only connect two rigid bodies. hkConstraintChainInstance stores internally the full list of bodies it links. That information is then passed 
	/// to the constraint constraint solver with hkConstraintChainInstance's custom code.
	///
	/// In order to properly react to e.g. bodies being removed from the hkWorld, a hkConstraintChainInstance is always coupled with a hkConstraintChainInstanceAction
	/// which is automatically added to the world with the constraint chain. This action makes sure that the constraint-chain is removed from the world when some of the bodies
	/// that it references are missing.
	///
	/// Data organization:
	///  - hkConstraintChainInstance stores the list of bodies linked. 
	///  - The constraint properties are stored in its hkConstraintChainData. 
	///  - The current state of those constraints is stored in the chain's Runtime.
	///
class hkConstraintChainInstance : public hkConstraintInstance
{
	public:
		HK_DECLARE_REFLECTION();

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_CONSTRAINT );

			/// Constructor. 
		hkConstraintChainInstance(hkConstraintChainData* data);

			/// Destructor. Deletes the hkConstraintChainInstanceAction belonging to the chain.
		~hkConstraintChainInstance();

			/// Get the hkConstraintChainData object
		inline hkConstraintChainData* getData() const { return static_cast<hkConstraintChainData*>(m_data); }

			/// Interface implementation. Also removes the chain's hkConstraintChainInstanceAction from the world. 
		virtual void entityRemovedCallback(hkEntity* entity);

			/// Returns constraint type (normal/chain).
		virtual InstanceType getType() const { return TYPE_CHAIN; }

			/// Adds entity to the constraint-chain. The number of entities cannot exceed (getData()->getNumConstraintInfos() + 1) but may be less than that. In such
			/// case the constraint infos at the end are ignored when solving the constraint.
		void addEntity(hkEntity* entity);

			/// Adds entity to the constraint chain at the beginning of the list, and shifts all previous constraints. 
			/// Note that this causes a reorganization in the entity-to-constraintInfo matching.
		inline void insertEntityAtFront(hkEntity* entity) 
		{
			HK_ASSERT2(0xad6d5d44, m_owner == HK_NULL, "Cannot add entities when constraint chain is added to the world");
			HK_ASSERT2(0xad7877dd, m_chainedEntities.getSize() <= getData()->getNumConstraintInfos(), "Cannot add anymore entities to the chain -- pivots not specified.");
			m_chainedEntities.insertAt( 0, entity );
			entity->addReference();
		}

			/// Returns the number of constraints that will be solved in the chain.
		inline int getNumConstraints() { return hkMath::max2( 0, m_chainedEntities.getSize() - 1 ); }

			///	List of entieties linked by the chain. This array is also referenced by the hkConstraintChainInstanceAction.
		hkArray<hkEntity*> m_chainedEntities;

			/// Action guarding the chain constraint. When an entity belonging to the chain is removed, that action receives a callback and removes the constraint.
			/// We use this workaround because constraints can only hold references to two bodies.
		hkConstraintChainInstanceAction* m_action;

			/// Serialization constructor
		hkConstraintChainInstance( hkFinishLoadedObjectFlag f ) : hkConstraintInstance(f), m_chainedEntities(f) {}
};


#endif // HK_DYNAMICS2_CONSTRAINT_CHAIN_INSTANCE_H




/*
* Havok SDK - PUBLIC RELEASE, BUILD(#20060902)
*
* Confidential Information of Havok.  (C) Copyright 1999-2006 
* Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
* Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
* rights, and intellectual property rights in the Havok software remain in
* Havok and/or its suppliers.
*
* Use of this software for evaluation purposes is subject to and indicates 
* acceptance of the End User licence Agreement for this product. A copy of 
* the license is included with this software and is also available from salesteam@havok.com.
*
*/
