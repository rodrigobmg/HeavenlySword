/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLLIDE2_LEAF_AGENT_H
#define HK_COLLIDE2_LEAF_AGENT_H

#include <hkmath/hkMath.h>

#include <hkcollide/agent/hkCollisionAgent.h>


/// This agent adds some linear casting functionality shared by terminal/leaf hkCollisionAgents.
class hkIterativeLinearCastAgent : public hkCollisionAgent
{
	public:
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AGENT);

		hkIterativeLinearCastAgent(hkContactMgr* contactMgr): hkCollisionAgent(contactMgr){}

			//implementation of the hkCollisionAgent
		virtual void linearCast( const hkCdBody& bodyA, const hkCdBody& bodyB, const hkLinearCastCollisionInput& input, hkCdPointCollector& collector, hkCdPointCollector* startCollector );

			//implementation of the hkCollisionAgent
		static void HK_CALL staticLinearCast( const hkCdBody& bodyA, const hkCdBody& bodyB, const hkLinearCastCollisionInput& input, hkCdPointCollector& collector, hkCdPointCollector* startCollector );
	
		HK_FORCE_INLINE ~hkIterativeLinearCastAgent(){}
};

#endif // HK_COLLIDE2_LEAF_AGENT_H

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
