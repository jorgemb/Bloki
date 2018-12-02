#ifndef BARO_H
#define BARO_H

#include <vector>

#include "Exception.h"
#include "Renderer.h"

#define g_Baro Baro::getSingleton()

enum MOOD{
	MOOD_NORMAL,
	MOOD_HAPPY,
	MOOD_WORRIED,
	MOOD_SAD,
	MOOD_MAD,
};

enum ANIMATION{
	ANIMATION_NONE = 0,
	ANIMATION_WAVE,
	ANIMATION_WAVE_LEFT,
	ANIMATION_WAVE_RIGHT,
};

class Baro: public RenderObj{
public:
// FUNCTIONS /////////////////////////////////////////////////////////////////////////////
	// CONSTRUCTOR AND DESTRUCTOR ////////////////////////////////////////////////////////
	Baro( Renderer* pRenderer );
	~Baro();

	static Baro* getSingletonPtr();
	static Baro& getSingleton();

	// UTILITIES /////////////////////////////////////////////////////////////////////////

	VOID Update( DWORD timeDelta );
	BOOL Draw( Gdiplus::Graphics* pGraphics );

	// GET-/SET- FUNCTIONS ///////////////////////////////////////////////////////////////

	VOID setMood( MOOD Mood );
	MOOD getMood();

	VOID setAnimation( ANIMATION Animation, DWORD TimeMS );
	ANIMATION getAnimation();

	VOID setPosition( const Gdiplus::Point& Pos );
	Gdiplus::Point getPosition();

private:
// FUNCTIONS /////////////////////////////////////////////////////////////////////////////

	UINT ResourceToIndex( UINT Resource );
	UINT IndexToResource( UINT Index );

	VOID drawImage( Gdiplus::Graphics* pGraphics, UINT Resource, INT RelativePos = 0 );

// VARIABLES /////////////////////////////////////////////////////////////////////////////
	static Baro*	s_pBaro;

	Renderer*		m_pRenderer;
	MOOD			m_Mood;
	ANIMATION		m_Animation;
	Gdiplus::Point	m_Position;

	DOUBLE			m_LeftHandPos, m_RightHandPos;	// These mark the delta to move the arms
													// starting from Baro's position
	INT				m_CurrentAnimationTime;

	typedef std::vector<Gdiplus::Bitmap*>	ImageArray;
	typedef ImageArray::iterator			iterImage;
	ImageArray		m_Images;
};

class BaroObjAlreadyInitialized: public Ludos::Exception{
public:
	BaroObjAlreadyInitialized() : 
		Exception( "Baro objects has already been initialized.", "Baro" ) {}
};

class InvalidBaroObj: public Ludos::Exception{
public:
	InvalidBaroObj() : Exception( "Invalid Baro object. Singleton hasn't been initialized", "Baro" ) {}
};

#endif // BARO_H
