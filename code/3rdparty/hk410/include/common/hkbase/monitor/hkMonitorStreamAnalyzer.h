/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HKBASE_MONITOR_STREAM_ANALYZER_H
#define HKBASE_MONITOR_STREAM_ANALYZER_H

#include <hkbase/monitor/hkMonitorStream.h>
#include <hkbase/htl/hkPointerMap.h>

extern const hkClass hkMonitorStreamStringMapClass;
struct hkMonitorStreamStringMap
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_MONITOR, hkMonitorStreamStringMap);
	HK_DECLARE_REFLECTION();

	struct StringMap
	{
		HK_DECLARE_REFLECTION();

		HK_ALIGN(hkUint64 m_id,8); // no ulong in serialization yet, and void* will be zero 
		const char* m_string;
	};

	hkArray<struct StringMap> m_map;
};


/// Information about how to read the data from the monitors captured over a
/// given frame. You need to fill this structure in per frame captured.
/// The timing monitors are setup to capture 2 floats per call. However,
/// there are 4 "slots" in the corresponding monitor capture structure here.
/// This allows you for example to alternate between i-cache and d-cache misses
/// per frame and send them to different slost in the capture.
extern const hkClass hkMonitorStreamFrameInfoClass;
struct hkMonitorStreamFrameInfo
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_MONITOR, hkMonitorStreamFrameInfo ); 
	HK_DECLARE_REFLECTION();

	/// The heading for the frame (which will be printed out in the detailed timings file)
	/// Typically you should not forget to record what timers are being used for, and what units you
	/// are measuring times in (e.g. usec).
	const char* m_heading;

	/// The slot in our montior capture structure, where the timer 0 should be stored
	/// If this is set to -1, timer 0 will not get stored. By default this is 0.
	hkInt32 m_indexOfTimer0;

	/// The slot in our montior capture structure, where the timer 1 should be stored
	/// If this is set to -1, timer 1 will not get stored. By default this is 1.
	hkInt32 m_indexOfTimer1;

	/// The index of the absolute time. This should be 0 if timer O is used to
	/// store the absolute time, 1 if timer 1 is used to store the absolute time
	/// and -1 if neither timer 0 or timer 1 are used to store absolute time.
	/// This value is used to draw multiple timer threads to an image.
	/// By default this is ABSOLUTE_TIME_TIMER_0.

	enum AbsoluteTimeCounter
	{
		ABSOLUTE_TIME_TIMER_0 = 0,
		ABSOLUTE_TIME_TIMER_1 = 1,
		ABSOLUTE_TIME_NOT_TIMED = 0xffffffff
	};

	/// Which timer if either is used to measure absolute time
	hkEnum<AbsoluteTimeCounter,hkUint32> m_absoluteTimeCounter;

	/// This value is used to multiply the raw data collected by the monitors and written into slot 0 of the
	/// monitor capture structure.  The value should be set depending on what information the monitors are 
	/// collecting for this slot. (e.g. timings, cache misses). For example, if you wish to display timings
	/// in microseconds you would set this value to (1 / number of clock cycles per microsecond).  Using hkBase
	/// this would be 1e6f / float(hkStopwatch::getTicksPerSecond());
	/// By default this is 1.f
	hkReal m_timerFactor0;

	/// This value is used to multiply the raw data collected by the monitors and written into slot 1 of the
	/// monitor capture structure.  See comments for m_timerFactor0 for more info. 
	/// By default this is 1.f
	hkReal m_timerFactor1;

	/// Constructor. Initializes values to defaults as documented.
	hkMonitorStreamFrameInfo();
};


/// Allows you to capture several frames (generated by hkMonitorStream)
/// and convert the data stream into a stream that can be moved across the network.
/// Also this class allows you to pretty print the results to a file.
class hkMonitorStreamAnalyzer
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_MONITOR, hkMonitorStreamAnalyzer);

		///////////////////////////////////////////////////////////////////////
		//
		// Constructor, monitor capture, and reset
		//
		///////////////////////////////////////////////////////////////////////

			/// Create an analyzer given a maximum memory size. This will allocate a block of data
			/// with this size, and use it to copy monitor information when captureFrameDetails is called.
		hkMonitorStreamAnalyzer( int maxMemorySize );
		
			/// This function must be called every frame. It copies the data between monitorStreamBegin and
			/// monitorStreamEnd, and stores the frame info structure.
		void captureFrameDetails( const char* monitorStreamBegin, const char* monitorStreamEnd, hkMonitorStreamFrameInfo& info );

			/// This resets the analyzer. It clears all the arrays, but does not deallocate any data.
		void reset();

			/// Get the start of the data stream.
			/// If the steam is empty, HK_NULL will be returned.
		char* getStreamBegin();		


		///////////////////////////////////////////////////////////////////////
		//
		// Utilities to write .txt statistics files
		//
		///////////////////////////////////////////////////////////////////////

		enum
		{
			REPORT_ALL = 0xFF,
			REPORT_SUMMARIES = 0x07,
			REPORT_TOTAL = 0x01,
			REPORT_PERFRAME_TIME = 0x02,
			REPORT_PERFRAME_SUMMARY = 0x04,
			REPORT_PERFRAME_DETAIL = 0x08
		};

			/// Create a user readable statistics file of the raw data.
		void writeStatistics( hkOstream& outstream, int reportLevel = REPORT_ALL );

			/// Create a user readable statistics file of the raw data. This utility function should be called
			/// when, in a multithreaded environment, you have multiple hkMonitorStreamAnalyzer objects (1 per thread).
		static void HK_CALL writeMultiThreadedStatistics( const hkArray<hkMonitorStreamAnalyzer*>& streamCaptures,  hkOstream& outStream, int reportLevel = REPORT_ALL, const char* nodeIdForFrameOverview = "");


		///////////////////////////////////////////////////////////////////////
		//
		// Utilities to draw statistics to an image
		//
		///////////////////////////////////////////////////////////////////////

			/// The input structure for the drawThreadsToTga function. Each thread is drawn as
			/// a colored bar, with different colors indicating different timers.
		struct ColorTable
		{	
			/// A mapping of timer names to colors
			struct ColorPair
			{
				const char* m_colorName;
				int m_color;

				ColorPair( const char* name, int color ): m_colorName( name), m_color(color){}
			};

			hkArray< struct ColorPair > m_colorPairs;
			hkInt32 m_defaultColor;

			hkInt32 findColor( const char* color );
		};
		
		struct ThreadDrawInput
		{
			hkArray<hkMonitorStreamAnalyzer*> m_streamCaptures;

				/// The frame to start drawing from (1 by default)
			hkInt32 m_frameStart;

				/// The number of frames to draw (1 by default)
			hkInt32 m_numFrames;

				/// Layout info
			hkInt32 m_heightPerThread;
				/// Layout info
			hkInt32 m_gapBetweenThreads;

			hkInt32 m_gapBetweenFrames;

				/// Number of samples to take per frame
			hkInt32 m_outputPixelWidth;
				/// How long a frame lasts for
			hkReal m_frameTime;
			
			hkBool m_warnAboutMissingTimers;


				/// A mapping of timer names to colors
			struct ColorTable* m_colorTable;
		};

			/// Draw the threads to a .tga file
		static void HK_CALL drawThreadsToTga( const ThreadDrawInput& input, hkOstream& outStream );


		///////////////////////////////////////////////////////////////////////
		//
		// Utilities to print timers to a runtime display
		//
		///////////////////////////////////////////////////////////////////////


		struct Node; 

			/// You can use this function in conjunction with mergeTreesForRuntimeDisplay, navigateMonitors and printMonitorsToRuntimeDisplay 
			/// to display user navigatable timers on screen. You need to be able to output text to the screen, and capture
			/// cursor key input to do this. 
			/// This call will create a tree of nodes given a monitor stream start and end. It returns the root node of the tree.
			/// For multithreaded applications, you will have a monitor stream per thread. You call this on each monitor stream,
			/// and merge the results together using mergeTreesForRuntimeDisplay. You can then use navigateMonitors and 
			/// printMonitorsToRuntimeDisplay to view the combined trees.
		static Node* HK_CALL makeStatisticsTree( const char* frameStart, const char* frameEnd, const hkMonitorStreamFrameInfo& info, const char* rootNodeName = "/", hkBool reuseNodesIfPossible = true );

			/// Given two trees, this function takes all the nodes in the subTree and merges them into the main tree. The sourceTimerId
			/// describes which slot in the subTree nodes the time is stored. This corresponds with your monitor setup, but is usually 0.
			/// The destTimerId describes which slot in the main tree nodes you wish to write. There are 4 slots, so this value must be
			/// between 0 and 3. If a node exists in the subTree but not in mainTree it is created. The smoothingFactor describes
			/// how the value is smoothed as it changes over time. Values closer to 1 smooth the timers more. 0 is unsmoothed.
		static void HK_CALL mergeTreesForRuntimeDisplay( Node* mainTree, Node* subTree, int destTimerId, int sourceTimerId = 0, hkReal smoothingFactor = .93f );


			/// Set these values from your inupt device. This is used as an input to navigateMonitors
		struct CursorKeys
		{
				/// up
			hkBool m_upPressed;
				/// down
			hkBool m_downPressed;
				/// left
			hkBool m_leftPressed;
				/// right
			hkBool m_rightPressed;
		};

			/// This function returns the new active node, given the current active node and the cursor keys.
		static Node* HK_CALL navigateMonitors( const CursorKeys& keys, Node* activeNodeIn );

			/// This function prints the monitor tree to an hkOstream. You can create an hkOstream from an array of char,
			/// then print this array to the screen. The rootNode is the root of the tree, and the active node is the
			/// current selected node, returned by navigateMonitors. The numThreads is the number of fields (from 1 to 4)
			/// to print. The unexpanded and expanded variables allow you to insert your own graphics for these markers.
		static void HK_CALL printMonitorsToRuntimeDisplay( hkOstream& os, Node* rootNode, Node* activeNode, hkObjectArray<hkString>& names, int numThreads, char unexpanded = '>', char expanded = '^');

	public:

		//
		// Static internal utility methods and data used for the above functions
		//
#if defined(HK_PLATFORM_PS3)
		// 2 ppu and 6 spu threads
		enum { NUM_VALUES = 8};
#else
		enum { NUM_VALUES = 6};
#endif

		/// Data to store the parsed results of a capture
		struct Node
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_MONITOR, Node );

			enum NodeType
			{
				NODE_TYPE_TIMER,
				NODE_TYPE_DIRECTORY,
				NODE_TYPE_SINGLE,
			};

			hkReal			m_value[NUM_VALUES];
			hkInt32			m_count[NUM_VALUES];
			hkArray<struct Node*> m_children;
			hkReal			m_absoluteStartTime;
			Node*			m_parent;
			const char*		m_name;	
			hkUint32		m_userFlags;
			hkEnum<NodeType,hkUint32> m_type;

			Node(Node* parent, const char* name, NodeType type );

			void setTimers( const hkMonitorStreamFrameInfo& frameInfo, const hkMonitorStream::TimerCommand& start, const hkMonitorStream::TimerCommand& end);

			~Node();
		};


		static Node* HK_CALL makeStatisticsTreeForMultipleFrames(	const char* streamStart, 
																	const hkArray<hkMonitorStreamFrameInfo>& frameInfos,
																	const hkArray<int>& frameStartLocations,
																	hkBool reuseNodesIfPossible = false);


		static void HK_CALL writeStatisticsDetails( hkOstream& outstream, hkArray<Node*>& nodes, int reportLevel, const char* nodeIdForFrameOverview = "" );

		static void HK_CALL printMonitorsToRuntimeDisplay( hkOstream& os, Node* node, int recursionDepth, Node* activeNode, int numFields, char rightArrow, char downArrow);

		static void HK_CALL extractStringMap( const char* frameStart, const char* frameEnd, hkPointerMap<void*, char*>& map );

		static void HK_CALL applyStringMap( const char* frameStart, const char* frameEnd, const hkPointerMap<void*, char*>& map, hkBool endianSwap );

	protected:

		//
		// Data for multiple frames
		//

		hkArray<struct hkMonitorStreamFrameInfo> m_frameInfos;
		hkArray<hkInt32> m_frameStartLocations;
		hkArray<char> m_data;

	public:

		const char* m_nodeIdForFrameOverview;
};


#endif // HKBASE_MONITOR_STREAM_ANALYZER_H

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
