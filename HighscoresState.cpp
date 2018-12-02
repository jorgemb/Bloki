#include "HighscoresState.h"
#include <algorithm>
#include <string>

#include "resources.h"
#include "ResourceUtility.h"
#include "GdiUtility.h"
#include "Utility.h"
#include "Baro.h"

std::string HighscoresState::s_inputString = "";
BOOL HighscoresState::s_endInput = FALSE;


HighscoresState::HighscoresState( Ludos::Window *pWindow, Renderer *pRenderer, Ludos::InputDX *pInput ):
m_pWindow( pWindow ),
m_pRenderer( pRenderer ),
m_pInput( pInput ),
m_Score( 0 ),
m_ScoreText( pRenderer )
{
}

HighscoresState::~HighscoresState()
{
}

BOOL HighscoresState::onStart()
{
	m_ScoreList.clear();

	m_Score = g_Score.getScore();
	m_ScoreText.setScore( m_Score );

	// Open file
	std::ifstream File( ScoresFile.c_str(), std::ios::in | std::ios::binary );
	if( File.is_open() )
		readScores( File );

	// Check if the new score is actually a Highscore
	BOOL isHighscore = FALSE;

	if( m_Score != 0 )
	{
		for( iterScore pIter = m_ScoreList.begin(); pIter != m_ScoreList.end(); ++pIter )
		{
			if( pIter->Score < m_Score )
			{
				isHighscore = TRUE;
				break;
			}
		}

		if( m_ScoreList.size() < 10 )
			isHighscore = TRUE;
	}

	return isHighscore;
}

BOOL HighscoresState::onEnd()
{
	// Sort and write the scores to the file
	std::sort( m_ScoreList.begin(), m_ScoreList.end(), compare );
	
	std::ofstream File( ScoresFile.c_str(), std::ios::out | std::ios::trunc | std::ios::binary );
	if( File.is_open() )
		writeScores( File );

	// Return static flags to normal
	s_inputString = "";
	s_endInput = FALSE;
	m_pInput->enableDevice( Ludos::IN_KEYBOARD );
	m_pWindow->setCharInputFunction( NULL );

	return TRUE;
}

BOOL HighscoresState::onStep( DWORD timeDelta )
{
	if( KeyState( VK_ESCAPE ) )
	{
		m_pParent->popState();
		return TRUE;
	}

	m_pInput->disableDevice( Ludos::IN_KEYBOARD );
	m_pWindow->setCharInputFunction( charInput );

	m_ScoreText.Submit();

	// Check input end
	if( s_endInput )
	{
		ScoreObj newScore;
		newScore.Score = m_Score;
		strcpy_s( newScore.Name, sizeof( newScore.Name ), s_inputString.c_str() );

		m_ScoreList.push_back( newScore );
		m_pParent->popState();
	}

	return TRUE;
}

VOID HighscoresState::readScores( std::ifstream &File )
{
	while( !File.eof() )
	{
		ScoreObj newScore;
		if( File.read( (char*)&newScore, sizeof( ScoreObj ) ) )
			m_ScoreList.push_back( newScore );
	}
}

VOID HighscoresState::writeScores( std::ofstream &File )
{
	for( iterScore pIter = m_ScoreList.begin(); pIter != m_ScoreList.end(); ++pIter )
	{
		ScoreObj& newScore = *pIter;
		File.write( (char*)&newScore, sizeof( ScoreObj ) );
	}
}

bool HighscoresState::compare( const ScoreObj &Score1,
							   const ScoreObj &Score2 )
{
	return Score1.Score < Score2.Score;
}



ScoreObj::ScoreObj() :
Score( 0 )
{
	ZeroMemory( Name, sizeof( CHAR[6] ) );
}


ScoreObj::ScoreObj(const ScoreObj &Other)
{
	Score = Other.Score;
	memcpy( Name, Other.Name, sizeof( CHAR[6] ) );
}




VOID HighscoresState::charInput( TCHAR character )
{
	if( ( character >= 'A' && character <= 'Z' ) ||
		( character >= 'a' && character <= 'z' ) ||
		( character == ' ' ) )
	{
		if( s_inputString.size() < 5 )
			s_inputString += character;
	}
	else if( character == VK_RETURN )
		s_endInput = TRUE;
	else if( character == VK_BACK && s_inputString.size() > 0 )
		s_inputString.resize( s_inputString.size() - 1 );
}


///
// ScoreText
///

ScoreText::ScoreText( Renderer *pRenderer ):
m_pRenderer( pRenderer ),
m_Position( 150, 300 ),
m_Score( 0 ),
m_FontSize( 64 ),
m_pYournameImage( NULL ),
m_pScoreImage( NULL ),
m_pTitleImage( NULL )
{
	m_pYournameImage = loadGdiBitmap( IMAGE_YOURNAME, PNG_TYPE );
	m_pScoreImage = loadGdiBitmap( IMAGE_SCORE, PNG_TYPE );
	m_pTitleImage = loadGdiBitmap( IMAGE_ENTERHIGHSCORE, PNG_TYPE );

	if( !m_pYournameImage || !m_pScoreImage || !m_pTitleImage )
		throw Ludos::Exception( "Invalid image pointers.", "ScoreText" );
}

ScoreText::~ScoreText()
{
	S_DELETE( m_pYournameImage );
	S_DELETE( m_pScoreImage );
	S_DELETE( m_pTitleImage );
}

VOID ScoreText::Submit()
{
	m_pRenderer->addRenderObj( this );
}

BOOL ScoreText::Draw( Gdiplus::Graphics *pGraphics )
{
	using Gdiplus::REAL;

	// Draw images
	pGraphics->DrawImage( m_pYournameImage, m_Position.X, m_Position.Y,
		(REAL)m_pYournameImage->GetWidth(), (REAL)m_pYournameImage->GetHeight() );
	pGraphics->DrawImage( m_pScoreImage, m_Position.X, m_Position.Y + m_pYournameImage->GetHeight(),
		(REAL)m_pScoreImage->GetWidth(), (REAL)m_pScoreImage->GetHeight() );
	
	UINT TImageWidth = m_pTitleImage->GetWidth();
	UINT TImageHeight = m_pTitleImage->GetHeight();
	UINT ScreenWidth = 800;
	UINT ScreenHeight = 600;
	pGraphics->DrawImage( m_pTitleImage,
						  ( ScreenWidth - TImageWidth ) / 2.0f,
						  ( ScreenHeight / 2.0f - TImageHeight ) / 2.0f,
						  (REAL)TImageWidth, (REAL)TImageHeight );

	// Draw strings
	Gdiplus::PointF NamePos( m_Position.X + m_pYournameImage->GetWidth(), m_Position.Y );
	Gdiplus::PointF ScorePos( m_Position.X + m_pScoreImage->GetWidth(),
							  m_Position.Y + m_pYournameImage->GetHeight() );

	Gdiplus::FontFamily FFamily( L"Arial" );
	Gdiplus::Font Font( &FFamily, m_FontSize, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel );
	Gdiplus::SolidBrush Brush( Gdiplus::Color( 255, 255, 255 ) );

	/* ... name */
	std::wstring ToWrite( HighscoresState::s_inputString.size(), L' ' );
	std::copy( HighscoresState::s_inputString.begin(), HighscoresState::s_inputString.end(),
			   ToWrite.begin() );

	pGraphics->DrawString( ToWrite.c_str(), ToWrite.size(), &Font, NamePos, &Brush );

	/* ... score */
	std::string ScoreText = Ludos::toStr( m_Score );
	ToWrite.resize( ScoreText.size() );
	std::copy( ScoreText.begin(), ScoreText.end(), ToWrite.begin() );

	pGraphics->DrawString( ToWrite.c_str(), ToWrite.size(), &Font, ScorePos, &Brush );


	return TRUE;
}

VOID ScoreText::setPosition( const Gdiplus::PointF& newPosition )
{
	m_Position = newPosition;
}

VOID ScoreText::setScore( UINT Score )
{
	m_Score = Score;
}
