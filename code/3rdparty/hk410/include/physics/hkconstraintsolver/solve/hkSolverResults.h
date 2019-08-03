/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_CONSTRAINTSOLVER2_SOLVER_RESULTS_H
#define HK_CONSTRAINTSOLVER2_SOLVER_RESULTS_H

#include <hkmath/hkMath.h>

/// Constraint solver results. This class is used internally by the system, however it may be used to query the results of the constraint
/// solver for this constraint on the last solve() call.
class hkSolverResults
{
	public:
		
			/// Get the impulse applied by the solver on the last timestep. This structure may be returned prior to the application of the
			/// constraint solver. For the case of a contact constraint it is initialised to a very crude guess at the expected initial impulse.
		inline hkReal getImpulseApplied() const;

	public:
			// internal use only
		inline hkSolverResults();

			// internal use only
		inline void init();

	public:
			// internal use only
		float m_impulseApplied;

			// internal use only
		float m_internalSolverData;
};


#include <hkconstraintsolver/solve/hkSolverResults.inl>


#endif // HK_CONSTRAINTSOLVER2_SOLVER_RESULTS_H

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
