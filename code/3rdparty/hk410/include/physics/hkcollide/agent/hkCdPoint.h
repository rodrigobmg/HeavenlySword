/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLLIDE2_CD_POINT
#define HK_COLLIDE2_CD_POINT

#include <hkmath/basetypes/hkContactPoint.h>

class hkCdBody;

	/// A simple structure used for a contact point between two hkCdBody objects
	/// IMPORTANT NOTE: The distance value in the contact point is a fraction (between 0 and 1) when this struct is 
	/// used with linear cast
struct hkCdPoint
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AGENT, hkCdPoint);

		/// Constructor takes two hkCdBody objects
	hkCdPoint( const hkCdBody& a, const hkCdBody& b ) : m_cdBodyA(a), m_cdBodyB(b) {}

		/// The contact point
	hkContactPoint m_contact;


		/// body A
	const hkCdBody& m_cdBodyA;

		/// body B
	const hkCdBody& m_cdBodyB;
};

#endif // HK_COLLIDE2_CD_POINT

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
