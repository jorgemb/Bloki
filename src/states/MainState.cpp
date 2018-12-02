#include "MainState.h"
#include "resources.h"
#include "GdiUtility.h"

#include "GameState.h"
#include "ShowHighscoresState.h"
#include "Baro.h"

#include <ObjIdl.h>
#include <GdiPlus.h>

MainMenu::MainMenu( Ludos::Window* pWindow, Renderer* pRenderer, Ludos::InputDX* pInput ):
m_GUIManager( pRenderer, *pInput ),
m_hWindow( *pWindow ),
m_Keyb( *pInput ),
m_pRenderer( pRenderer ),
m_pWindow( pWindow ),
m_pInput( pInput ),
m_Title( pRenderer )
{
}

MainMenu::~MainMenu()
{
}

BOOL MainMenu::onStart()
{
	NewGame* NGButton = new NewGame( m_pWindow, m_pParent, m_pRenderer, m_pInput );
	ShowHighscores* SHButton = new ShowHighscores( *m_pWindow, m_pParent, m_pRenderer, m_pInput );
	QuitGame* QButton = new QuitGame;
	if( !NGButton->Init() || !SHButton->Init() || !QButton->Init() ){
		S_DELETE( NGButton );
		S_DELETE( SHButton );
		S_DELETE( QButton );
		return FALSE;
	}

	m_GUIManager.addObject( NGButton );
	m_GUIManager.addObject( SHButton );
	m_GUIManager.addObject( QButton );

	return TRUE;
}

BOOL MainMenu::onEnd()
{
	return TRUE;
}

BOOL MainMenu::onStep( DWORD timeDelta )
{
	if( m_Keyb.KeyPress( DIK_ESCAPE ) )
		m_pParent->popState();

	if( g_Baro.getMood() == MOOD_SAD )
		g_Baro.setMood( MOOD_HAPPY );

	m_GUIManager.Update( m_hWindow );
	m_Title.Submit();

	return TRUE;
}



///
// BUTTONS
///

NewGame::NewGame( Ludos::Window* pWindow,
				  StateManager* pStateManager,
				  Renderer* pRenderer,
				  Ludos::InputDX* pInput ) :
m_pWindow( pWindow ),
m_pImage( NULL ),
m_ImageRect( 0, 0, 0, 0 ),
m_isMouseOver( FALSE ),
m_pStateManager( pStateManager ),
m_pRenderer( pRenderer ),
m_pInput( pInput )
{
}

NewGame::~NewGame()
{
	S_DELETE( m_pImage );
}

BOOL NewGame::Init()
{
	// Load Image
	m_pImage = loadGdiBitmap( GUI_NEWGAME, PNG_TYPE );
	if( !m_pImage )
		return FALSE;

	// Make drawing rect
	m_ImageRect.Width = m_pImage->GetWidth();
	m_ImageRect.Height = m_pImage->GetHeight() / 2;

	// Set the position
	m_Hotspot.X = NewGameX;
	m_Hotspot.Y = NewGameY;
	m_Hotspot.Width = m_ImageRect.Width;
	m_Hotspot.Height = m_ImageRect.Height;

	return TRUE;
}

VOID NewGame::onRollover(){
	m_isMouseOver = TRUE;
}

VOID NewGame::onClic()
{
	// Push the game state
	m_pStateManager->pushState( new GameState( m_pWindow, m_pRenderer, m_pInput ) );
}

BOOL NewGame::Draw(Gdiplus::Graphics *pGraphics)
{
	if( m_pImage ){
		if( m_isMouseOver ){
			pGraphics->DrawImage( m_pImage,
								  m_Hotspot,
								  m_ImageRect.X,
								  m_ImageRect.Y + m_ImageRect.Height,
								  m_ImageRect.Width,
								  m_ImageRect.Height,
								  Gdiplus::UnitPixel,
								  NULL, NULL, NULL );
		}
		else {
			pGraphics->DrawImage( m_pImage,
								  m_Hotspot,
								  m_ImageRect.X,
								  m_ImageRect.Y,
								  m_ImageRect.Width,
								  m_ImageRect.Height,
								  Gdiplus::UnitPixel,
								  NULL, NULL, NULL );
		}
	}
	m_isMouseOver = FALSE;

	return TRUE;
}




ShowHighscores::ShowHighscores( HWND hWindow,
							    StateManager *pStateManager,
								Renderer *pRenderer,
								Ludos::InputDX *pInput ):
m_pImage( NULL ),
m_ImageRect( 0, 0, 0, 0 ),
m_isMouseOver( FALSE ),

m_hWindow( hWindow ),
m_pStateManager( pStateManager ),
m_pRenderer( pRenderer ),
m_pInput( pInput )
{
}

ShowHighscores::~ShowHighscores()
{
	S_DELETE( m_pImage );
}

BOOL ShowHighscores::Init()
{
	// Load Image
	m_pImage = loadGdiBitmap( GUI_HIGHSCORES, PNG_TYPE );
	if( !m_pImage )
		return FALSE;

	// Make drawing rect
	m_ImageRect.Width = m_pImage->GetWidth();
	m_ImageRect.Height = m_pImage->GetHeight() / 2;

	// Set the position
	m_Hotspot.X = HighscoresX;
	m_Hotspot.Y = HighscoresY;
	m_Hotspot.Width = m_ImageRect.Width;
	m_Hotspot.Height = m_ImageRect.Height;

	return TRUE;
}

VOID ShowHighscores::onRollover()
{
	m_isMouseOver = TRUE;
}

VOID ShowHighscores::onClic()
{
	m_pStateManager->pushState( new ShowHighscoresState( m_hWindow, m_pRenderer, *m_pInput ) );
}

BOOL ShowHighscores::Draw( Gdiplus::Graphics *pGraphics )
{
	if( m_pImage ){
		if( m_isMouseOver ){
			pGraphics->DrawImage( m_pImage,
								  m_Hotspot,
								  m_ImageRect.X,
								  m_ImageRect.Y + m_ImageRect.Height,
								  m_ImageRect.Width,
								  m_ImageRect.Height,
								  Gdiplus::UnitPixel,
								  NULL, NULL, NULL );
		}
		else {
			pGraphics->DrawImage( m_pImage,
								  m_Hotspot,
								  m_ImageRect.X,
								  m_ImageRect.Y,
								  m_ImageRect.Width,
								  m_ImageRect.Height,
								  Gdiplus::UnitPixel,
								  NULL, NULL, NULL );
		}
	}
	m_isMouseOver = FALSE;

	return TRUE;
}






QuitGame::QuitGame() :
m_pImage( NULL ),
m_ImageRect( 0, 0, 0, 0 ),
m_isMouseOver( FALSE )
{
}

QuitGame::~QuitGame()
{
	S_DELETE( m_pImage );
}

BOOL QuitGame::Init()
{
	// Load Image
	m_pImage = loadGdiBitmap( GUI_QUIT, PNG_TYPE );
	if( !m_pImage )
		return FALSE;

	// Make drawing rect
	m_ImageRect.Width = m_pImage->GetWidth();
	m_ImageRect.Height = m_pImage->GetHeight() / 2;

	// Set the position
	m_Hotspot.X = QuitGameX;
	m_Hotspot.Y = QuitGameY;
	m_Hotspot.Width = m_ImageRect.Width;
	m_Hotspot.Height = m_ImageRect.Height;

	return TRUE;
}

BOOL QuitGame::Draw(Gdiplus::Graphics *pGraphics)
{
	if( m_pImage ){
		if( m_isMouseOver ){
			pGraphics->DrawImage( m_pImage,
								  m_Hotspot,
								  m_ImageRect.X,
								  m_ImageRect.Y + m_ImageRect.Height,
								  m_ImageRect.Width,
								  m_ImageRect.Height,
								  Gdiplus::UnitPixel,
								  NULL, NULL, NULL );
		}
		else {
			pGraphics->DrawImage( m_pImage,
								  m_Hotspot,
								  m_ImageRect.X,
								  m_ImageRect.Y,
								  m_ImageRect.Width,
								  m_ImageRect.Height,
								  Gdiplus::UnitPixel,
								  NULL, NULL, NULL );
		}
	}
	m_isMouseOver = FALSE;

	return TRUE;
}

VOID QuitGame::onClic()
{
	PostQuitMessage( 0 );
}

VOID QuitGame::onRollover()
{
	m_isMouseOver = TRUE;
}


TitleImage::TitleImage( Renderer* pRenderer ):
m_pRenderer( pRenderer ),
m_pTitle( NULL ),
m_pBaroMessage( NULL )
{
	m_pTitle = loadGdiBitmap( IMAGE_TITLE, PNG_TYPE );
	m_pBaroMessage = loadGdiBitmap( BARO_MESSAGE, PNG_TYPE );

	g_Baro.setMood( MOOD_HAPPY );
	g_Baro.setAnimation( ANIMATION_WAVE_RIGHT, 5000 );
}

TitleImage::~TitleImage()
{
	S_DELETE( m_pTitle );
	S_DELETE( m_pBaroMessage );
}

VOID TitleImage::Submit()
{
	m_pRenderer->addRenderObj( this );
}

BOOL TitleImage::Draw( Gdiplus::Graphics *pGraphics )
{
	Gdiplus::Point Position( TitleX, TitleY );
	pGraphics->DrawImage( m_pTitle, Position.X, Position.Y, 
		m_pTitle->GetWidth(), m_pTitle->GetHeight() );

	Gdiplus::Point MessagePos( Position.X + m_pTitle->GetWidth() + 50, 10 );
	pGraphics->DrawImage( m_pBaroMessage, MessagePos.X, MessagePos.Y,
		(INT)m_pBaroMessage->GetWidth(), (INT)m_pBaroMessage->GetHeight() );
	
	g_Baro.setPosition( Gdiplus::Point( MessagePos.X, MessagePos.Y + m_pBaroMessage->GetHeight() / 2 ) );


	return TRUE;
}