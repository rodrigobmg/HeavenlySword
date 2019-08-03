/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLLIDE2_MOPP_BV_TREE_STREAM_AGENT_H
#define HK_COLLIDE2_MOPP_BV_TREE_STREAM_AGENT_H

#include <hkcollide/agent/bvtreestream/hkBvTreeStreamAgent.h>

class hkCollisionDispatcher;


	/// This agent deals with collisions between hkMoppShapes and other shapes.
	/// This agent only differs in two points from its parent hkBvTreeAgent:
	///  - it implements a specialized staticLinearCast, which is doing an optimized
	///    static linear cast in Mopps.
	///  - When two mopp collide, it checks for each mopps size and collides all the 
	///    triangles of the smaller mopp against the bigger mopp.
class hkMoppBvTreeStreamAgent 
{
	public:
			/// Registers this agent with the collision dispatcher.
		static void HK_CALL registerAgent(hkCollisionDispatcher* dispatcher);
};

#endif // HK_COLLIDE2_MOPP_BV_TREE_STREAM_AGENT_H

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
