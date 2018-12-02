#include "NextBlokus.h"
#include "Utility.h"

#include "resources.h"
#include "GdiUtility.h"

const Gdiplus::REAL SquareWidth = 5.0f;
const Gdiplus::Color SquareColor( 0, 104, 139 );

NextBlokus::NextBlokus( BlokiFactory* pBFactory,
					    Renderer *pRenderer,
						const Gdiplus::Point& Position ):
m_pRenderer( pRenderer ),
m_Square( SquareColor, SquareWidth ),
m_pBlokus( NULL ),
m_pBlokiFactory( pBFactory ),
m_pText( NULL )
{
	setPosition( Position );

	m_pBlokus = m_pBlokiFactory->createBlokus();
	m_pText = loadGdiBitmap( IMAGE_NEXTBLOKUS, PNG_TYPE );
}

NextBlokus::~NextBlokus()
{
	S_DELETE( m_pBlokus );
}


Blokus* NextBlokus::getBlokus()
{
	Blokus* ret = m_pBlokus;
	m_pBlokus = m_pBlokiFactory->createBlokus();
	m_pBlokus->setParentPosition( Gdiplus::Point( m_Position.X, m_Position.Y + 64 ) );
	m_pBlokus->setColor( Ludos::Random( MaxColors ) );

	return ret;
}

VOID NextBlokus::Submit()
{
	m_pRenderer->addRenderObj( m_pBlokus );
	m_pRenderer->addRenderObj( &m_Square );
	m_pRenderer->addRenderObj( this );
}

BOOL NextBlokus::Draw( Gdiplus::Graphics* pGraphics )
{
	pGraphics->DrawImage( m_pText,
		m_Position.X,
		m_Position.Y,
		m_pText->GetWidth(),
		m_pText->GetHeight() );

	return TRUE;
}

VOID NextBlokus::setPosition( const Gdiplus::Point &Position )
{
	m_Position = Position;

	using Gdiplus::Point;
	INT BlokusDimension = BlockSize * 4;
	Point P1( Position.X, Position.Y + 64 );
	Point P2( P1.X, P1.Y + BlokusDimension );
	Point P3( P2.X + BlokusDimension, P2.Y );
	Point P4( P3.X, P1.Y );

	m_Square.Clear();
	m_Square.addLine( P1, P2, LEFT );
	m_Square.addLine( P2, P3, BOTTOM );
	m_Square.addLine( P3, P4, RIGHT );
	m_Square.addLine( P4, P1, TOP );
}

Gdiplus::Point NextBlokus::getPosition()
{
	return m_Position;
}
