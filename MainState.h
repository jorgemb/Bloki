#ifndef STATES_H
#define STATES_H

#include "StateManager.h"
#include "GUIManager.h"

#include "Window.h"
#include "Input.h"
#include "Renderer.h"

const INT NewGameX = 272;
const INT NewGameY = 300;

const INT HighscoresX = 272;
const INT HighscoresY = 364;

const INT QuitGameX = 272;
const INT QuitGameY = 428;

const INT TitleX = 50;
const INT TitleY = 100;

class NewGame: public GUIObject{
public:
	NewGame( Ludos::Window* pWindow,
		     StateManager* pStateManager,
			 Renderer* pRenderer,
			 Ludos::InputDX* pInput );
	~NewGame();

	BOOL Init();
	BOOL Draw( Gdiplus::Graphics* pGraphics );
	VOID onClic();
	VOID onRollover();
private:
	Ludos::Window*		m_pWindow;
	Gdiplus::Bitmap*	m_pImage;
	Gdiplus::Rect		m_ImageRect;

	BOOL				m_isMouseOver;

	StateManager*		m_pStateManager;
	Renderer*			m_pRenderer;
	Ludos::InputDX*		m_pInput;
};

class ShowHighscores: public GUIObject{
public:
	ShowHighscores( HWND hWindow,
					StateManager* pStateManager,
					Renderer* pRenderer,
					Ludos::InputDX* pInput );
	~ShowHighscores();

	BOOL Init();
	BOOL Draw( Gdiplus::Graphics* pGraphics );
	VOID onClic();
	VOID onRollover();
private:
	Gdiplus::Bitmap*	m_pImage;
	Gdiplus::Rect		m_ImageRect;

	BOOL				m_isMouseOver;

	HWND				m_hWindow;
	StateManager*		m_pStateManager;
	Renderer*			m_pRenderer;
	Ludos::InputDX*		m_pInput;
};

class QuitGame: public GUIObject{
public:
	QuitGame();
	~QuitGame();

	BOOL Init();
	BOOL Draw( Gdiplus::Graphics* pGraphics );
	VOID onClic();
	VOID onRollover();
private:
	Gdiplus::Bitmap*	m_pImage;
	Gdiplus::Rect		m_ImageRect;

	BOOL				m_isMouseOver;
};


class TitleImage: public RenderObj{
public:
	TitleImage( Renderer* pRenderer );
	~TitleImage();

	VOID Submit();
	BOOL Draw( Gdiplus::Graphics* pGraphics );
private:
	Renderer*			m_pRenderer;
	Gdiplus::Bitmap		*m_pTitle, *m_pBaroMessage;
};

////////
// MAIN MENU
////////

class MainMenu: public State
{
public:
	MainMenu( Ludos::Window* pWindow, Renderer* pRenderer, Ludos::InputDX* pInput );
	~MainMenu();

	BOOL onStart();
	BOOL onEnd();
	BOOL onStep( DWORD timeDelta );

private:
	GUIManager	m_GUIManager;
	HWND		m_hWindow;

	Renderer*			m_pRenderer;
	Ludos::InputDXUtilities::KeyboardDeviceRef m_Keyb;

	Ludos::Window*		m_pWindow;
	Ludos::InputDX*		m_pInput;

	TitleImage	m_Title;
};


#endif // STATES_H

