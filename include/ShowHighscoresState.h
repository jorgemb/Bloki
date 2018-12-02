#ifndef SHOWHIGHSCORESSTATE_H
#define SHOWHIGHSCORESSTATE_H

#include "Input.h"
#include "Renderer.h"
#include "StateManager.h"
#include "HighscoresState.h"

class Highscores: public RenderObj{
public:
	Highscores( HWND hWindow, Renderer* pRenderer );
	~Highscores();

	VOID Submit();
	BOOL Draw( Gdiplus::Graphics* pGraphics );

private:
	HWND		m_hWindow;
	Renderer*	m_pRenderer;

	std::vector<ScoreObj>	m_Scores;
	Gdiplus::Bitmap*		m_pTitle;
};

class ShowHighscoresState: public State{
public:
	ShowHighscoresState( HWND hWindow, Renderer* pRenderer, Ludos::InputDXUtilities::KeyboardDeviceRef Keyb );
	~ShowHighscoresState();

	BOOL onStart();
	BOOL onEnd();
	BOOL onStep( DWORD timeDelta );

private:
	HWND		m_hWindow;
	Renderer*	m_pRenderer;
	Ludos::InputDXUtilities::KeyboardDeviceRef	m_Keyb;
	Gdiplus::Point	m_LastBaroPos;

	Highscores	m_Highscores;
};



#endif // SHOWHIGHSCORESSTATE_H
