#include "GameState.h"

#include "Resources.h"
#include "GdiUtility.h"
#include "Baro.h"

GameState::GameState( Ludos::Window* pWindow, Renderer* pRenderer, Ludos::InputDX* pInput ) :
m_BlokiManager( pRenderer ),
m_Score( pRenderer ),
m_Keyb( *pInput ),
m_GameOver( *pWindow, pRenderer ),
m_HighscoresState( pWindow, pRenderer, pInput )
{
}

GameState::~GameState()
{
}

BOOL GameState::onStart()
{
	m_BlokiManager.setPosition( 50, 50 );
	m_Score.setPosition( Gdiplus::PointF( 400, 50 ) );

	g_Baro.setAnimation( ANIMATION_NONE, 0 );

	return TRUE;
}

BOOL GameState::onEnd()
{
	return TRUE;
}

BOOL GameState::onStep( DWORD timeDelta )
{
	// Check for input
	if( m_BlokiManager.lostGame() )
	{
		g_Baro.setMood( MOOD_SAD );
		m_GameOver.Show();

		if( m_Keyb.KeyPress( DIK_RETURN ) )
		{
			g_Baro.setMood( MOOD_NORMAL );
			g_Baro.setAnimation( ANIMATION_WAVE, 3000 );

			m_BlokiManager.Clear();
			m_GameOver.Hide();

			m_pParent->pushState( &m_HighscoresState, FALSE );
			m_Score.setScore( 0 );
		}
	}
	if( m_Keyb.KeyPress( DIK_ESCAPE ) )
		m_pParent->popState();

	m_BlokiManager.update( timeDelta, m_Keyb );
	m_GameOver.Submit();

	g_Baro.setPosition( Gdiplus::Point( 800 - 256, 600 - 256 ) );

	return TRUE;
}



///
// GameOverText
///

GameOverText::GameOverText( HWND hWindow, Renderer *pRenderer ):
m_bShow( FALSE ),
m_hWindow( hWindow ),
m_pRenderer( pRenderer ),
m_pText( NULL ),
m_Position( 0, 0 )
{
	m_pText = loadGdiBitmap( IMAGE_GAMEOVER, PNG_TYPE );
	
	// Calculate position
	RECT ClientRect;
	GetClientRect( hWindow, &ClientRect );

	UINT Width = m_pText->GetWidth();
	UINT Height = m_pText->GetHeight();

	m_Position.X = ( ClientRect.right - Width ) / 2;
	m_Position.Y = ( ClientRect.bottom - Height ) / 2;
}

GameOverText::~GameOverText()
{
	S_DELETE( m_pText );
}

VOID GameOverText::Show()
{
	m_bShow = TRUE;
}

VOID GameOverText::Hide()
{
	m_bShow = FALSE;
}

VOID GameOverText::Submit()
{
	if( m_pRenderer && m_bShow )
		m_pRenderer->addRenderObj( this );
}

BOOL GameOverText::Draw( Gdiplus::Graphics *pGraphics )
{
	pGraphics->DrawImage( m_pText, m_Position );

	return TRUE;
}