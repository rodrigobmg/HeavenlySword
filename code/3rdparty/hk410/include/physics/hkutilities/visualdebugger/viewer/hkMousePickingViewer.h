/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_MOUSE_PICKING_VIEWER
#define HK_MOUSE_PICKING_VIEWER

#include <hkutilities/visualdebugger/viewer/hkWorldViewerBase.h>
class hkWorld;
class hkVector4;
class hkMouseSpringAction;

/// Server side mouse picking implementation.
class hkMousePickingViewer : public hkWorldViewerBase
{
	public:

		enum Command // for serialization
		{
			HK_PICK_OBJECT = 0xB0,
			HK_DRAG_OBJECT,
			HK_RELEASE_OBJECT
		};

		static hkProcess* HK_CALL create(const hkArray<hkProcessContext*>& contexts);

			/// Registers the hkDebugDisplayProcess with the hkProcessFactory.
		static void HK_CALL registerViewer();

			/// Gets the tag associated with this viewer type
		virtual int getProcessTag() { return m_tag; }

		static inline const char* HK_CALL getName() { return "MousePicking"; }


		//
		// Process interface
		//

		virtual void getConsumableCommands( hkUint8*& commands, int& numCommands );

			/// Given a command (that has already been parsed from
			/// the stream), consume the rest of the data.
		virtual void consumeCommand( hkUint8 command );

		virtual void worldRemovedCallback( hkWorld* world );


		//
		// Own interface
		//

			/// Attaches a hkMouseSpringAction to an object in the world
		hkBool pickObject( hkUint64 displayObject, const hkVector4& worldPosition );

			/// Move the fixed point of the hkMouseSpringAction
		void dragObject( const hkVector4& newWorldSpacePoint );

			/// Removes the hkMouseSpringAction from the world freeing the attached object.
		void releaseObject();


	protected:

		/// Constructor.
		hkMousePickingViewer(const hkArray<hkProcessContext*>& contexts);

		/// Destructor.
		~hkMousePickingViewer();

		hkWorld* m_currentWorld; 
		hkMouseSpringAction* m_mouseSpring;
		hkReal m_mouseSpringMaxRelativeForce;
		static int m_tag;
};

#endif // HK_MOUSE_PICKING_HANDLER

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
