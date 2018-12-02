#ifndef SCORE_H
#define SCORE_H

#include "Exception.h"
#include "Renderer.h"

const UINT DefaultBase = 1;
const Gdiplus::REAL FontSize = 49;

class Score: public RenderObj{
public:
// FUNCTIONS /////////////////////////////////////////////////////////////////////////////
	// CONSTRUCTOR AND DESTRUCTOR ////////////////////////////////////////////////////////

	Score( Renderer* pRenderer );
	~Score();

	// UTILITIES /////////////////////////////////////////////////////////////////////////

	/* Singleton */
	static Score* getSingletonPtr();
	static Score& getSingleton();


	void addScore();
	void subtractScore( UINT subScore );

	void resetMultiplier();
	void resetScore();

	BOOL Draw( Gdiplus::Graphics* pGraphics );
	VOID Submit();

	// SET-FUNCTIONS /////////////////////////////////////////////////////////////////////

	void setScore( UINT newScore );
	void setMultiplier( UINT newMultiplier );
	void setBase( UINT newBase );
	void setPosition( Gdiplus::PointF& newPos );

	// GET-FUNCTIONS /////////////////////////////////////////////////////////////////////

	UINT getScore() const;
	UINT getMultiplier() const;
	UINT getBase() const;
	Gdiplus::PointF getPosition() const;

private:
// VARIABLES /////////////////////////////////////////////////////////////////////////////
	
	Renderer*			m_pRenderer;
	UINT				m_nScore, m_nMultiplier, m_nBase;
	Gdiplus::PointF		m_Position;
	Gdiplus::Bitmap*	m_pText;

	static Score*	st_pScore;
};

class ScoreObjAlreadyInitialized: public Ludos::Exception
{
public:
	ScoreObjAlreadyInitialized()
		: Exception( "Score object has already been initialized.", "Score" ) {}
};

class InvalidScoreObj: public Ludos::Exception
{
public:
	InvalidScoreObj() 
		: Exception( "Invalid score object. Singleton hasn't been initialized.",
		"Score" ) {}
};

// Make Score look like a Global Variable
#define g_Score Score::getSingleton()

#endif // SCORE_H