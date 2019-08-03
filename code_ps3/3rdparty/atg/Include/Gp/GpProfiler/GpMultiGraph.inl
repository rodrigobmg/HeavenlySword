//--------------------------------------------------------------------------------------------------
/**
	@file		GpMultiGraph.h

	@brief		Graph drawing helper.

	@note		(c) Copyright Sony Computer Entertainment 2006. All Rights Reserved.	
**/
//--------------------------------------------------------------------------------------------------

#ifndef GP_MULTI_GRAPH_INL
#define GP_MULTI_GRAPH_INL

//--------------------------------------------------------------------------------------------------
//  INLINE FUNCTION DEFINITIONS
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
/**
	@brief			
**/
//--------------------------------------------------------------------------------------------------
	
inline void GpMultiGraph::Destroy()
{
	for (uint i = 0; i < m_numGraphs; ++i)
		m_pGraphs[i].Destroy();
	
	FW_DELETE_ARRAY(m_pGraphs);
}
	
//--------------------------------------------------------------------------------------------------
/**
	@brief			
**/
//--------------------------------------------------------------------------------------------------

inline void GpMultiGraph::ResetSamples()
{
	for (uint i = 0; i < m_numGraphs; ++i)
		m_pGraphs[i].ResetSamples();
}

//--------------------------------------------------------------------------------------------------
/**
	@brief			
**/
//--------------------------------------------------------------------------------------------------
	
inline void GpMultiGraph::AddSample(uint plotIndex, float sample)
{
	FW_ASSERT(plotIndex < m_numGraphs);
	
	m_pGraphs[plotIndex].AddSample(sample);
}

//--------------------------------------------------------------------------------------------------
/**
	@brief			
**/
//--------------------------------------------------------------------------------------------------

inline void GpMultiGraph::Draw(GcContext& context)
{
	if (!m_isVisible)
		return;
		
	for (uint i = 0; i < m_numGraphs; ++i)
		m_pGraphs[i].Draw(context);
}

//--------------------------------------------------------------------------------------------------
/**
	@brief			
**/
//--------------------------------------------------------------------------------------------------
	
inline GpGraph& GpMultiGraph::GetGraph(uint plotIndex)
{
	FW_ASSERT(plotIndex < m_numGraphs);

	return m_pGraphs[plotIndex];
}
	
//--------------------------------------------------------------------------------------------------
/**
	@brief			
**/
//--------------------------------------------------------------------------------------------------

inline void GpMultiGraph::SetVisibility(bool isVisible)
{
	m_isVisible = isVisible;
}

//--------------------------------------------------------------------------------------------------
/**
	@brief			
**/
//--------------------------------------------------------------------------------------------------

inline bool GpMultiGraph::GetVisibility() const
{
	return m_isVisible;
}

//--------------------------------------------------------------------------------------------------
/**
	@brief			
**/
//--------------------------------------------------------------------------------------------------

inline void GpMultiGraph::SetDrawOrigin(float x, float y)
{
	for (uint i = 0; i < m_numGraphs; ++i)
		m_pGraphs[i].SetDrawOrigin(x, y);
}

//--------------------------------------------------------------------------------------------------
/**
	@brief			
**/
//--------------------------------------------------------------------------------------------------

inline void GpMultiGraph::GetDrawOrigin(float* pX, float* pY) const
{
	m_pGraphs[0].GetDrawOrigin(pX, pY);
}

//--------------------------------------------------------------------------------------------------
/**
	@brief			
**/
//--------------------------------------------------------------------------------------------------

inline void GpMultiGraph::SetDrawDimensions(float width, float height)
{
	for (uint i = 0; i < m_numGraphs; ++i)
		m_pGraphs[i].SetDrawDimensions(width, height);
}

//--------------------------------------------------------------------------------------------------
/**
	@brief			
**/
//--------------------------------------------------------------------------------------------------

inline void GpMultiGraph::GetDrawDimensions(float* pWidth, float* pHeight) const
{
	m_pGraphs[0].GetDrawDimensions(pWidth, pHeight);
}

//--------------------------------------------------------------------------------------------------
/**
	@brief			
**/
//--------------------------------------------------------------------------------------------------

inline void GpMultiGraph::SetTransparency(float alpha)
{
	for (uint i = 0; i < m_numGraphs; ++i)
		m_pGraphs[i].SetTransparency(alpha);
}

//--------------------------------------------------------------------------------------------------
/**
	@brief			
**/
//--------------------------------------------------------------------------------------------------

inline float GpMultiGraph::GetTransparency() const
{
	return m_pGraphs[0].GetTransparency();
}

//--------------------------------------------------------------------------------------------------
/**
	@brief			
**/
//--------------------------------------------------------------------------------------------------

inline void GpMultiGraph::SetClampFactor(float clampFactor)
{
	for (uint i = 0; i < m_numGraphs; ++i)
		m_pGraphs[i].SetClampFactor(clampFactor);
}

//--------------------------------------------------------------------------------------------------
/**
	@brief			
**/
//--------------------------------------------------------------------------------------------------

inline float GpMultiGraph::GetClampFactor() const
{
	return m_pGraphs[0].GetClampFactor();
}

//--------------------------------------------------------------------------------------------------

#endif // GP_MULTI_GRAPH_INL

