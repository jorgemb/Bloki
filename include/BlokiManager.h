#ifndef BLOKIMANAGER_H
#define BLOKIMANAGER_H

#include <string>
#include <list>

#include "Window.h"
#include "Input.h"

#include "Renderer.h"
#include "BlokiFactory.h"
#include "NextBlokus.h"
#include "Blokus.h"
#include "GdiFigures.h"
#include "Score.h"

#include "Exception.h"

const UINT CanvasRows			= 15;
const UINT CanvasCols			= 10;

const DWORD FastFallTime		= 20;

const UINT StartingDifficulty	= 500;
const UINT DifficultyTime		= 2000;
const UINT DifficultyStep		= 1;
const UINT MaxDifficulty		= 50;

class BlokiManager{
public:
// FUNCTIONS /////////////////////////////////////////////////////////////////////////////
	// CONSTRUCTOR AND DESTRUCTOR ////////////////////////////////////////////////////////

	BlokiManager( Renderer* pRenderer );
	~BlokiManager();

	// UTILITIES /////////////////////////////////////////////////////////////////////////

	VOID update( DWORD timeDelta, Ludos::InputDXUtilities::KeyboardDeviceRef Keyb );
	VOID Clear();

	BOOL lostGame();

	// SET-FUNCTIONS /////////////////////////////////////////////////////////////////////

	VOID setPosition( const Gdiplus::Point& Pos );
	VOID setPosition( INT x, INT y );

	// GET-FUNCTIONS /////////////////////////////////////////////////////////////////////

	Gdiplus::Point getPosition();

private:
// FUNCTIONS /////////////////////////////////////////////////////////////////////////////

	VOID NormalBlokusState( DWORD timeDelta,
							Ludos::InputDXUtilities::KeyboardDeviceRef Keyb );
	VOID FallBlokusState( DWORD timeDelta );

	VOID loadTexture();
	VOID makeCanvasFrame();

	BOOL checkBlokusCollision( Blokus* pBlokus );
	BOOL fallBlokus( Blokus* pBlokus );

	VOID addBlokusCollisionData( Blokus* pBlokus );
	VOID checkRowFormations();

// VARIABLES /////////////////////////////////////////////////////////////////////////////

	Gdiplus::Point		m_Position;
	Gdiplus::Image*		m_pTexture;

	Renderer*			m_pRenderer;
	BlokiFactory*		m_pFactory;
	NextBlokus*			m_pNextBlokus;

	typedef std::list<Blokus*>::iterator	iterBlokus;
	std::list<Blokus*>	m_listBloki;
	Blokus*				m_pCurrentBlokus;

	DWORD				m_Time, m_TimeForStep;
	DWORD				m_DifficultyChangeTime;
	CenteredPath		m_CanvasFrame;
	BOOL				m_bPendingBlokiFalling;
	BOOL				m_bLostGame;

	struct CollisionData{
		Blokus*			m_pAssociatedBlokus;			
	};
	CollisionData		m_CollisionData[CanvasRows][CanvasCols];
};

class InvalidRenderer: public Ludos::Exception
{
public:
	InvalidRenderer() : Exception( "Invalid renderer pointer.", "BlokiManager" ) {}
};

class InvalidTexture: public Ludos::Exception
{
public:
	InvalidTexture() : Exception( "Couldn't load tiles texture.", "BlokiManager" ) {}
};

#endif // BLOKIMANAGER_H
