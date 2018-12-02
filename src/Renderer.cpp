#include "Renderer.h"

VOID Renderer::addRenderObj( RenderObj *pObject )
{
	if( pObject != NULL )
		m_ObjectsToDraw.push_back( pObject );
}

BOOL Renderer::Draw( Gdiplus::Graphics *pGraphics ) 
{
	// Validate
	if( !pGraphics )
		return FALSE;

	if( m_ObjectsToDraw.empty() )
		return TRUE;


	// Draw all the objects
	BOOL Ret = TRUE;
	for( iterObj pIter = m_ObjectsToDraw.begin(); pIter != m_ObjectsToDraw.end(); ++pIter )
	{
		RenderObj* thisObject = (*pIter);
		
		if( !thisObject->Draw( pGraphics ) )
			Ret = FALSE;
	}

	// Clear the render objects
	m_ObjectsToDraw.clear();

	return Ret;
}