/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_CONTACT_UPDATER_H
#define HK_CONTACT_UPDATER_H

#include <hkbase/hkBase.h>
#include <hkdynamics/collide/hkDynamicsContactMgr.h>

class hkDynamicsContactMgr;
class hkCollidable;
class hkEntity;

	/// This event is fired in a ContactUpdateCallback for each collision pair when you call hkContactUpdater::updateContacts().
	/// It is equivalent to the hkContactProcessEvent, except that instead of a list of collision points (the hkProcessCollisionOutput field
	/// in the hkContactProcessEvent, this event returns an array of contact point ids. From these ids you can get all the
	/// contact points and their associated contact point properties by calling hkDynamicsContactMgr::getContactPoint() and
	/// hkDynamicsContactMgr::getContactPointProperties() on the m_contactMgr.
	/// This event allows you to change the friction, and restitution for the collision points (which are not recalculated by the
	/// system otherwise).
struct hkContactUpdateEvent
{
	hkContactUpdateEvent( hkDynamicsContactMgr& mgr, const hkCollidable& a, const hkCollidable& b )
		:	m_contactMgr(mgr),
			m_collidableA(a),
			m_collidableB(b)
	{
	}

		/// The contact manager.
	hkDynamicsContactMgr& m_contactMgr;
	
		/// The first entity's hkCollidable.
	const hkCollidable& m_collidableA;

		/// The second entity's hkCollidable.
	const hkCollidable& m_collidableB;

		/// A list of contact point ids
	hkInplaceArray<hkContactPointId, 256> m_contactPointIds;

	    ///The entity that fired this callback.
	hkEntity* m_callbackFiredFrom;
};


	/// This is a utility class which allows you to update all the contact points for a given rigid body which has been added
	/// to the world. You typically use it when you want to change the friction of a rigid body, and you want the rigid body's
	/// contacts to be updated immediately. This is in fact the default behavior of hkContactUpdater::updateContacts().
	/// You can override this behavior, (which you may need to do if you are using a friction map etc) by passing in your own 
	/// callback function to handle the hkContactUpdateEvent. The source code for the default callback - 
	/// hkContactUpdater::defaultFrictionUpdateCallback() is public, as an example
class hkContactUpdater
{
	public:

			/// The function prototype for the contact update event
		typedef void (HK_CALL *ContactUpdateCallback) (hkContactUpdateEvent& event);

			/// The default update callback - this just updates the friction of the contact based on the friction values
			/// stored in the material of the two rigid bodies in collision. This is the same as the default behaviour
			/// of the system when new contact points are created.
		static void HK_CALL defaultFrictionUpdateCallback( hkContactUpdateEvent& event );

			/// Call this function to update all the friction on all contact points for a rigid body.
			/// NOTE: if the rigid body is fixed, this function is quite slow, as it has to analyse the entire world
			/// to search for all the dynamic bodies in contact.
		static void HK_CALL updateContacts( hkEntity* entity, ContactUpdateCallback cb = defaultFrictionUpdateCallback );

};

#endif // HK_CONTACT_UPDATER_H

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
