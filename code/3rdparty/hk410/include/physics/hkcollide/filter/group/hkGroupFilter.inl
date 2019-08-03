/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


inline int	hkGroupFilter::getNewSystemGroup()
{
	return ++m_nextFreeSystemGroup;
}

inline hkUint32 HK_CALL hkGroupFilter::calcFilterInfo( int layer, int systemGroup, int subSystemId, int subSystemDontCollideWith)
{
	HK_ASSERT(0x1902b596,  layer >=0 && layer < 32 );
	HK_ASSERT(0x1902b597,  subSystemId  >=0 && subSystemId  < 32 );
	HK_ASSERT(0x1902b598,  subSystemDontCollideWith  >=0 && subSystemDontCollideWith  < 32 );
	HK_ASSERT(0x5ae6770c,  systemGroup>=0 && systemGroup < 0x10000);

	return hkUint32( (subSystemId<<5) | ( subSystemDontCollideWith<<10) | (systemGroup<<16) | layer);
}

int hkGroupFilter::getLayerFromFilterInfo( hkUint32 filterInfo )
{
	return filterInfo & 0x1f;
}

int hkGroupFilter::getSystemGroupFromFilterInfo( hkUint32 filterInfo )
{
	return filterInfo>>16;
}

int hkGroupFilter::getSubSystemIdFromFilterInfo( hkUint32 filterInfo )
{
	return (filterInfo >> 5) & 0x1f;
}

int hkGroupFilter::getSubSystemDontCollideWithFromFilterInfo( hkUint32 filterInfo )
{
	return (filterInfo >> 10) & 0x1f;
}

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
