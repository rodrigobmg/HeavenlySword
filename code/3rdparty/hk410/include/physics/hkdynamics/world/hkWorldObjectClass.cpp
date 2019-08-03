/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

// WARNING: THIS FILE IS GENERATED. EDITS WILL BE LOST.
// Generated from 'hkdynamics/world/hkWorldObject.h'

#include <hkdynamics/hkDynamics.h>
#include <hkbase/class/hkClass.h>
#include <hkbase/class/hkInternalClassMember.h>
#include <hkdynamics/world/hkWorldObject.h>


// External pointer and enum types
extern const hkClass hkLinkedCollidableClass;
extern const hkClass hkMultiThreadLockClass;
extern const hkClass hkPropertyClass;
extern const hkClass hkWorldClass;

//
// Enum hkWorldObject::BroadPhaseType
//
static const hkInternalClassEnumItem hkWorldObjectBroadPhaseTypeEnumItems[] =
{
	{0, "BROAD_PHASE_INVALID"},
	{1, "BROAD_PHASE_ENTITY"},
	{2, "BROAD_PHASE_PHANTOM"},
	{3, "BROAD_PHASE_BORDER"},
	{4, "BROAD_PHASE_MAX_ID"},
};
static const hkInternalClassEnum hkWorldObjectEnums[] = {
	{"BroadPhaseType", hkWorldObjectBroadPhaseTypeEnumItems, 5 }
};
extern const hkClassEnum* hkWorldObjectBroadPhaseTypeEnum = reinterpret_cast<const hkClassEnum*>(&hkWorldObjectEnums[0]);

//
// Class hkWorldObject
//
const hkInternalClassMember hkWorldObject::Members[] =
{
	{ "world", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, HK_OFFSET_OF(hkWorldObject,m_world) },
	{ "userData", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, HK_OFFSET_OF(hkWorldObject,m_userData) },
	{ "name", HK_NULL, HK_NULL, hkClassMember::TYPE_CSTRING, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkWorldObject,m_name) },
	{ "multithreadLock", &hkMultiThreadLockClass, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkWorldObject,m_multithreadLock) },
	{ "collidable", &hkLinkedCollidableClass, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkWorldObject,m_collidable) },
	{ "properties", &hkPropertyClass, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, HK_OFFSET_OF(hkWorldObject,m_properties) }
};
extern const hkClass hkReferencedObjectClass;

extern const hkClass hkWorldObjectClass;
const hkClass hkWorldObjectClass(
	"hkWorldObject",
	&hkReferencedObjectClass, // parent
	sizeof(hkWorldObject),
	HK_NULL,
	0, // interfaces
	reinterpret_cast<const hkClassEnum*>(hkWorldObjectEnums),
	1, // enums
	reinterpret_cast<const hkClassMember*>(hkWorldObject::Members),
	int(sizeof(hkWorldObject::Members)/sizeof(hkInternalClassMember)),
	HK_NULL // defaults
	);

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
