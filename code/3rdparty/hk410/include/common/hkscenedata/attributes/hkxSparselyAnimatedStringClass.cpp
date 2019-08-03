/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

// WARNING: THIS FILE IS GENERATED. EDITS WILL BE LOST.
// Generated from 'hkscenedata/attributes/hkxSparselyAnimatedString.h'

#include <hkscenedata/hkSceneData.h>
#include <hkbase/class/hkClass.h>
#include <hkbase/class/hkInternalClassMember.h>
#include <hkscenedata/attributes/hkxSparselyAnimatedString.h>


// External pointer and enum types
extern const hkClass hkxSparselyAnimatedStringStringTypeClass;

//
// Class hkxSparselyAnimatedString::StringType
//
static const hkInternalClassMember hkxSparselyAnimatedString_StringTypeClass_Members[] =
{
	{ "string", HK_NULL, HK_NULL, hkClassMember::TYPE_CSTRING, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkxSparselyAnimatedString::StringType,m_string) }
};
const hkClass hkxSparselyAnimatedStringStringTypeClass(
	"hkxSparselyAnimatedStringStringType",
	HK_NULL, // parent
	sizeof(hkxSparselyAnimatedString::StringType),
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkxSparselyAnimatedString_StringTypeClass_Members),
	int(sizeof(hkxSparselyAnimatedString_StringTypeClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL // defaults
	);

//
// Class hkxSparselyAnimatedString
//
static const hkInternalClassMember hkxSparselyAnimatedStringClass_Members[] =
{
	{ "strings", &hkxSparselyAnimatedStringStringTypeClass, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_STRUCT, 0, 0, HK_OFFSET_OF(hkxSparselyAnimatedString,m_strings) },
	{ "times", HK_NULL, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_REAL, 0, 0, HK_OFFSET_OF(hkxSparselyAnimatedString,m_times) }
};
extern const hkClass hkxSparselyAnimatedStringClass;
const hkClass hkxSparselyAnimatedStringClass(
	"hkxSparselyAnimatedString",
	HK_NULL, // parent
	sizeof(hkxSparselyAnimatedString),
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkxSparselyAnimatedStringClass_Members),
	int(sizeof(hkxSparselyAnimatedStringClass_Members)/sizeof(hkInternalClassMember)),
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
