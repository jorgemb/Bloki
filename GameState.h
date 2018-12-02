#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "StateManager.h"
#include "BlokiManager.h"
#include "HighscoresState.h"
#include "Score.h"
#include "Renderer.h"

#include "Window.h"
#include "Input.h"

class GameOverText: public RenderObj{
public:
	GameOverText( HWND hWindow, Renderer* pRenderer );
	~GameOverText();

	VOID Show();
	VOID Hide();

	VOID Submit();
	BOOL Draw( Gdiplus::Graphics* pGraphics );
private:
	BOOL				m_bShow;
	HWND				m_hWindow;
	Renderer*			m_pRenderer;

	Gdiplus::Bitmap*	m_pText;
	Gdiplus::Point		m_Position;
};

class GameState: public State {
public:
	GameState( Ludos::Window* pWindow, Renderer* pRenderer, Ludos::InputDX* pInput );
	~GameState();

	BOOL onStart();
	BOOL onEnd();
	BOOL onStep( DWORD timeDelta );
private:
	BlokiManager	m_BlokiManager;
	Score			m_Score;
	HighscoresState	m_HighscoresState;

	Ludos::InputDXUtilities::KeyboardDeviceRef m_Keyb;
	GameOverText	m_GameOver;
};


#endif // GAMESTATE_H

