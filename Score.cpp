#include "Score.h"
#include "Utility.h"

#include "resources.h"
#include "GdiUtility.h"

Score* Score::st_pScore = NULL;

Score::Score( Renderer *pRenderer ) :
m_pRenderer( pRenderer ),

m_nScore( 0 ),
m_nMultiplier( 1 ),
m_nBase( DefaultBase ),

m_Position( 0, 0 ),
m_pText( NULL )
{
	// Validate singleton
	if( st_pScore )
		throw ScoreObjAlreadyInitialized();

	st_pScore = this;

	m_pText = loadGdiBitmap( IMAGE_SCORE, PNG_TYPE );
}

Score::~Score()
{
	st_pScore = NULL;
}

Score* Score::getSingletonPtr()
{
	if( !st_pScore )
		throw InvalidScoreObj();

	return st_pScore;
}

Score& Score::getSingleton()
{
	if( !st_pScore )
		throw InvalidScoreObj();

	return *st_pScore;
}

void Score::addScore()
{
	m_nScore += m_nBase * m_nMultiplier;
	++m_nMultiplier;
}

void Score::subtractScore( UINT subScore )
{
	m_nScore -= subScore;
}

void Score::resetMultiplier()
{
	m_nMultiplier = 1;
}

void Score::resetScore()
{
	m_nScore = 0;
	resetMultiplier();
}

BOOL Score::Draw( Gdiplus::Graphics* pGraphics )
{
	Gdiplus::FontFamily FFamily( L"Arial" );
	Gdiplus::Font Font( &FFamily, FontSize, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel );

	Gdiplus::SolidBrush Brush( Gdiplus::Color( 255, 255, 255 ) );

	LPWSTR szWBuffer = new WCHAR[1024];
	INT length = wsprintfW( szWBuffer, L"%u", m_nScore );
	if( length == 0 )
	{
		S_DELETE( szWBuffer );
		return FALSE;
	}
	
	using Gdiplus::REAL;
	pGraphics->DrawImage( m_pText,
						  m_Position.X,
						  m_Position.Y,
						  (REAL)m_pText->GetWidth(),
						  (REAL)m_pText->GetHeight() );
	pGraphics->DrawString( szWBuffer,
						   length,
						   &Font,
						   Gdiplus::PointF( m_Position.X + m_pText->GetWidth(), m_Position.Y ),
						   &Brush );
	S_DELETE( szWBuffer );

	return TRUE;
}

VOID Score::Submit()
{
	m_pRenderer->addRenderObj( this ); 
}

///
// SET FUNCTIONS
///

void Score::setScore(UINT newScore)
{
	m_nScore = newScore;
}

void Score::setMultiplier( UINT newMultiplier )
{
	m_nMultiplier = newMultiplier;
}

void Score::setBase( UINT newBase )
{
	m_nBase = newBase;
}

void Score::setPosition( Gdiplus::PointF &newPos )
{
	m_Position = newPos;
}

///
// GET FUNTIONS
///

UINT Score::getScore() const
{
	return m_nScore;
}

UINT Score::getMultiplier() const
{
	return m_nMultiplier;
}

UINT Score::getBase() const
{
	return m_nBase;
}

Gdiplus::PointF Score::getPosition() const
{
	return m_Position;
}
