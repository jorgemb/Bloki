#ifndef GDIFIGURES_H
#define GDIFIGURES_H

#include <objidl.h>
#include <Gdiplus.h>

#include <vector>

#include "Renderer.h"

enum LinePosition{
	TOP,
	BOTTOM,
	LEFT,
	RIGHT,
	CENTER,
};

class CenteredPath: public RenderObj{
public:
// FUNCTIONS /////////////////////////////////////////////////////////////////////////////
	// CONSTRUCTOR AND DESTRUCTOR ////////////////////////////////////////////////////////
	CenteredPath( Gdiplus::Color Color, Gdiplus::REAL Width );
	~CenteredPath();

	// UTILITIES /////////////////////////////////////////////////////////////////////////

	VOID addLine( const Gdiplus::Point& Point1, const Gdiplus::Point& Point2, LinePosition Pos );
	VOID Clear();

	VOID Submit( Renderer* pRenderer );
	BOOL Draw( Gdiplus::Graphics* pGraphics );

private:
// VARIABLES /////////////////////////////////////////////////////////////////////////////

	Gdiplus::GraphicsPath			m_Path;
	Gdiplus::Pen					m_Pen;
};

class ClipSquare: public RenderObj{
public:
// FUNCTIONS /////////////////////////////////////////////////////////////////////////////
	// CONSTRUCTOR AND DESTRUCTOR ////////////////////////////////////////////////////////

	ClipSquare();
	ClipSquare( INT x1, INT y1, INT x2, INT y2 );
	ClipSquare( const Gdiplus::Point& TopLeft, const Gdiplus::Point& BottomRight );
	ClipSquare( const Gdiplus::Rect& Square );
	~ClipSquare();

	// UTILITIES /////////////////////////////////////////////////////////////////////////

	VOID Submit( Renderer* pRenderer );
	VOID EndClip( Renderer* pRenderer );

	BOOL Draw( Gdiplus::Graphics* pGrahics );

	VOID setSquare( INT x1, INT y1, INT x2, INT y2 );
	VOID setSquare( const Gdiplus::Point& TopLeft, const Gdiplus::Point& BottomRight );
	VOID setSquare( const Gdiplus::Rect& Square );

private:
// VARIABLES /////////////////////////////////////////////////////////////////////////////

	Gdiplus::Rect	m_Square;
	
	BOOL			m_bEndClip;
	BOOL			m_bDrawn;
};


#endif // GDIFIGURES_H
