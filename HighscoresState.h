#ifndef HIGHSCORESSTATE_H
#define HIGHSCORESSTATE_H

#include <string>
#include <fstream>
#include <vector>

#include "Window.h"
#include "Input.h"
#include "Renderer.h"
#include "StateManager.h"
#include "Score.h"


const std::string::size_type	MaxChars = 6;
const std::string				InvalidString = "INVALID";
const std::string				ScoresFile = "Highscores.dat";


// TYPES /////////////////////////////////////////////////////////////////////////////////
struct ScoreObj{
	ScoreObj();
	ScoreObj( const ScoreObj& Other );

	CHAR		Name[6];
	UINT		Score;
};



// CLASSES ///////////////////////////////////////////////////////////////////////////////
class ScoreText: public RenderObj{
public:
	ScoreText( Renderer* pRenderer );
	~ScoreText();

	VOID Submit();
	BOOL Draw( Gdiplus::Graphics* pGraphics );

	VOID setPosition( const Gdiplus::PointF& newPosition );
	VOID setScore( UINT Score );

private:
	const Gdiplus::REAL m_FontSize;

	Renderer*			m_pRenderer;
	Gdiplus::PointF		m_Position;
	UINT				m_Score;

	Gdiplus::Bitmap*	m_pYournameImage;
	Gdiplus::Bitmap*	m_pScoreImage;
	Gdiplus::Bitmap*	m_pTitleImage;
};

class HighscoresState: public State{
public:
// FUNCTIONS /////////////////////////////////////////////////////////////////////////////
	// CONSTRUCTOR AND DESTRUCTOR ////////////////////////////////////////////////////////

	HighscoresState( Ludos::Window* pWindow, Renderer* pRenderer, Ludos::InputDX* pInput );
	~HighscoresState();

	// UTILITIES /////////////////////////////////////////////////////////////////////////

	BOOL onStart();
	BOOL onEnd();
	BOOL onStep( DWORD timeDelta );

	static std::string	s_inputString;

	static bool compare( const ScoreObj& Score1, const ScoreObj& Score2 );
private:

// FUNCTIONS /////////////////////////////////////////////////////////////////////////////

	static VOID charInput( TCHAR character );
	static BOOL			s_endInput;

	VOID readScores( std::ifstream& File );
	VOID writeScores( std::ofstream& File );

// VARIABLES /////////////////////////////////////////////////////////////////////////////

	Ludos::Window*		m_pWindow;
	Ludos::InputDX*		m_pInput;
	Renderer*			m_pRenderer;
	UINT				m_Score;

	std::vector<ScoreObj>	m_ScoreList;
	typedef std::vector<ScoreObj>::iterator	iterScore;

	ScoreText			m_ScoreText;
};

#endif // HIGHSCORESSTATE_H
