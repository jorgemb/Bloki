#include "ShowHighscoresState.h"

#include <fstream>
#include <algorithm>
#include <string>

#include "resources.h"
#include "ResourceUtility.h"
#include "GdiUtility.h"
#include "Utility.h"
#include "Baro.h"

ShowHighscoresState::ShowHighscoresState( HWND hWindow, Renderer *pRenderer, Ludos::InputDXUtilities::KeyboardDeviceRef Keyb ):
m_hWindow( hWindow ),
m_pRenderer( pRenderer ),
m_Keyb( Keyb ),

m_Highscores( hWindow, pRenderer )
{
}

ShowHighscoresState::~ShowHighscoresState()
{
}

BOOL ShowHighscoresState::onStart()
{
	// Hide Baro
	m_LastBaroPos = g_Baro.getPosition();

	return TRUE;
}

BOOL ShowHighscoresState::onEnd()
{
	// Show Baro
	g_Baro.setPosition( m_LastBaroPos );

	return TRUE;
}

BOOL ShowHighscoresState::onStep( DWORD timeDelta )
{
	g_Baro.setPosition( Gdiplus::Point( -1000, 0 ) );
	m_Highscores.Submit();

	if( m_Keyb.KeyPress( DIK_ESCAPE ) )
		m_pParent->popState();

	return TRUE;
}





Highscores::Highscores( HWND hWindow, Renderer *pRenderer ):
m_hWindow( hWindow ),
m_pRenderer( pRenderer ),
m_pTitle( NULL )
{
	// Read scores from file
	std::ifstream File( ScoresFile.c_str(), std::ios::in | std::ios::binary );
	if( File.is_open() )
	{
		ScoreObj newScore;
		while( File.read( (char*)&newScore, sizeof( ScoreObj ) ) )
			m_Scores.push_back( newScore );
	}
	std::sort( m_Scores.rbegin(), m_Scores.rend(), HighscoresState::compare );

	// Load the title image
	m_pTitle = loadGdiBitmap( IMAGE_HIGHSCORES, PNG_TYPE );
}

Highscores::~Highscores()
{
	S_DELETE( m_pTitle );
}

VOID Highscores::Submit()
{
	m_pRenderer->addRenderObj( this );
}

BOOL Highscores::Draw( Gdiplus::Graphics *pGraphics )
{
	using Gdiplus::REAL;
	RECT ClientRect;
	GetClientRect( m_hWindow, &ClientRect );

	UINT ScreenWidth = ClientRect.right;
	UINT ScreenHeight = ClientRect.bottom;


	// Draw title
	pGraphics->DrawImage( m_pTitle,
						  ( ScreenWidth - m_pTitle->GetWidth() ) / 2.0f, 10.0f,
						  (REAL)m_pTitle->GetWidth(), (REAL)m_pTitle->GetHeight() );

	// Draw scores
	Gdiplus::REAL FontSize = 32.0f;
	Gdiplus::FontFamily FFamily( L"Arial" );
	Gdiplus::Font Font( &FFamily, FontSize, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel );
	Gdiplus::SolidBrush Brush( Gdiplus::Color( 255, 255, 255 ) );

	Gdiplus::REAL OffsetY = FontSize + 16.0f;
	Gdiplus::PointF CurrentPos( ( ScreenWidth - m_pTitle->GetWidth() ) / 2.0f,
								m_pTitle->GetHeight() + 10.0f );

	/* NOTE: Write only the first 9 scores */
	const int ScoresToWrite = 9;
	int WrittenScores = 0;

	for( std::vector<ScoreObj>::iterator pIter = m_Scores.begin();
		 pIter != m_Scores.end() && WrittenScores != ScoresToWrite; ++pIter, ++WrittenScores )
	{
		std::string Name = pIter->Name;
		std::string Score = Ludos::toStr( pIter->Score );

		std::wstring ToWrite;
		std::copy( Name.begin(), Name.end(), std::back_inserter( ToWrite ) );
		pGraphics->DrawString( ToWrite.c_str(), ToWrite.size(), &Font, CurrentPos, &Brush );

		ToWrite.clear();
		std::copy( Score.begin(), Score.end(), std::back_inserter( ToWrite ) );
		pGraphics->DrawString( ToWrite.c_str(),
							   ToWrite.size(),
							   &Font,
							   Gdiplus::PointF(  ScreenWidth * 3.0f/4.0f, CurrentPos.Y ),
							   &Brush );

		CurrentPos.Y += OffsetY;
	}

	return TRUE;
}