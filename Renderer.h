#ifndef RENDERER_H
#define RENDERER_H

#include <vector>

#include <objidl.h>
#include <Gdiplus.h>

class RenderObj
{
public:
	virtual BOOL Draw( Gdiplus::Graphics* pGraphics ) = 0;
};


class Renderer{
public:
// FUNCTIONS /////////////////////////////////////////////////////////////////////////////
	// UTILITIES /////////////////////////////////////////////////////////////////////////

	VOID addRenderObj( RenderObj* pObject );
	BOOL Draw( Gdiplus::Graphics* pGraphics );

private:
// TYPEDEFS //////////////////////////////////////////////////////////////////////////////

	typedef std::vector<RenderObj*>::iterator	iterObj;

// VARIABLES /////////////////////////////////////////////////////////////////////////////

	std::vector<RenderObj*>		m_ObjectsToDraw;
};


#endif // RENDERER_H