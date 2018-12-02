#include "GdiFigures.h"

///
// CenteredPath
///

CenteredPath::CenteredPath( Gdiplus::Color Color, Gdiplus::REAL Width ) :
m_Pen( Color, Width )
{
}

CenteredPath::~CenteredPath()
{
}

VOID CenteredPath::addLine( const Gdiplus::Point &Point1, const Gdiplus::Point &Point2, LinePosition Pos )
{
	using Gdiplus::REAL;
	REAL PenWidth = m_Pen.GetWidth();
	REAL HalfWidth = PenWidth / 2.0f;

	Gdiplus::PointF P1( (REAL)Point1.X, (REAL)Point1.Y ), P2( (REAL)Point2.X, (REAL)Point2.Y );

	switch( Pos )
	{
	case TOP:
		P1.Y -= HalfWidth;
		P2.Y -= HalfWidth;
		break;
	case BOTTOM:
		P1.Y += HalfWidth;
		P2.Y += HalfWidth;
		break;
	case RIGHT:
		P1.X += HalfWidth;
		P2.X += HalfWidth;
		break;
	case LEFT:
		P1.X -= HalfWidth;
		P2.X -= HalfWidth;
		break;
	default:
		break;
	}

	m_Path.AddLine( P1, P2 );
}

VOID CenteredPath::Clear()
{
	m_Path.Reset();
}

VOID CenteredPath::Submit( Renderer *pRenderer )
{
	if( !pRenderer )
		return;

	pRenderer->addRenderObj( this );
}

BOOL CenteredPath::Draw( Gdiplus::Graphics *pGraphics )
{
	if( !pGraphics )
		return FALSE;

	if( pGraphics->DrawPath( &m_Pen, &m_Path ) == Gdiplus::Ok )
		return TRUE;
	else
		return FALSE;
}


///
// ClipSquare
///

ClipSquare::ClipSquare():
m_bEndClip( FALSE ),
m_bDrawn( FALSE )
{
}

ClipSquare::ClipSquare(INT x1, INT y1, INT x2, INT y2):
m_bEndClip( FALSE ),
m_bDrawn( FALSE )
{
	setSquare( x1, y1, x2, y2 );
}

ClipSquare::ClipSquare( const Gdiplus::Point &TopLeft, const Gdiplus::Point &BottomRight ):
m_bEndClip( FALSE ),
m_bDrawn( FALSE )
{
	setSquare( TopLeft, BottomRight );
}

ClipSquare::ClipSquare( const Gdiplus::Rect &Square ):
m_bEndClip( FALSE ),
m_bDrawn( FALSE )
{
	setSquare( Square );
}

ClipSquare::~ClipSquare()
{
}

VOID ClipSquare::Submit( Renderer* pRenderer )
{
	if( !pRenderer )
		return;

	m_bDrawn = FALSE;
	m_bEndClip = FALSE;
	pRenderer->addRenderObj( this );
}

VOID ClipSquare::EndClip( Renderer *pRenderer )
{
	if( !pRenderer )
		return;

	m_bEndClip = TRUE;
	pRenderer->addRenderObj( this );
}

BOOL ClipSquare::Draw( Gdiplus::Graphics *pGraphics )
{
	if( !pGraphics )
		return FALSE;

	if( !m_bDrawn )
	{
		pGraphics->SetClip( m_Square );
		m_bDrawn = TRUE;

		return TRUE;
	}

	if( m_bDrawn && m_bEndClip )
	{
		pGraphics->ResetClip();

		m_bEndClip = FALSE;
		return TRUE;
	}

	return FALSE;
}

VOID ClipSquare::setSquare(INT x1, INT y1, INT x2, INT y2)
{
	m_Square = Gdiplus::Rect( x1, y1, x2 - x1, y2 - y1 );
}

VOID ClipSquare::setSquare( const Gdiplus::Point &TopLeft, const Gdiplus::Point &BottomRight )
{
	m_Square = Gdiplus::Rect( TopLeft.X, TopLeft.Y, BottomRight.X - TopLeft.X, BottomRight.Y - TopLeft.Y );
}

VOID ClipSquare::setSquare( const Gdiplus::Rect &Square )
{
	m_Square = Square;
}