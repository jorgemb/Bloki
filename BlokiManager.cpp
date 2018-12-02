#include "BlokiManager.h"

#include <set>

#include "Window.h"
#include "resources.h"
#include "GdiUtility.h"
#include "Baro.h"

const Gdiplus::Color CanvasColor( 0, 104, 139 );

BlokiManager::BlokiManager( Renderer *pRenderer ) :
m_Position( 0, 0 ),
m_pTexture( NULL ),
m_pRenderer( pRenderer ),
m_pFactory( NULL ),
m_pNextBlokus( NULL ),
m_pCurrentBlokus( NULL ),

m_Time( 0 ),
m_TimeForStep( StartingDifficulty ),
m_DifficultyChangeTime( 0 ),
m_bPendingBlokiFalling( FALSE ),
m_bLostGame( FALSE ),

m_CanvasFrame( CanvasColor, 5.0f )
{
	// Validate
	if( !m_pRenderer )
		throw InvalidRenderer();
	
	loadTexture();

	m_pFactory = new BlokiFactory( m_pTexture, m_Position );
	Ludos::Randomize();

	Gdiplus::Point NextBlokusPos;
	NextBlokusPos.X = m_Position.X + ( BlockSize * CanvasCols ) + 64;
	NextBlokusPos.Y = m_Position.Y + 128;
	m_pNextBlokus = new NextBlokus( m_pFactory, m_pRenderer, NextBlokusPos );

	makeCanvasFrame();
	ZeroMemory( (void*)m_CollisionData, sizeof(m_CollisionData) );
}

BlokiManager::~BlokiManager()
{
	S_DELETE( m_pCurrentBlokus );
	for( iterBlokus iter = m_listBloki.begin(); iter != m_listBloki.end(); ++iter )
	{
		Blokus* pBlokus = *iter;
		S_DELETE( pBlokus );
	}

	S_DELETE( m_pNextBlokus );
	S_DELETE( m_pFactory );
	S_DELETE( m_pTexture );
}

VOID BlokiManager::setPosition(const Gdiplus::Point &Pos)
{
	m_Position = Pos;
	m_pFactory->setParentPos( Pos );

	if( m_pCurrentBlokus )
		m_pCurrentBlokus->setParentPosition( Pos );

	for( iterBlokus iter = m_listBloki.begin(); iter != m_listBloki.end(); ++iter )
	{
		Blokus* pBlokus = *iter;
		pBlokus->setParentPosition( Pos );
	}

	makeCanvasFrame();

	Gdiplus::Point NextBlokusPos;
	NextBlokusPos.X = m_Position.X + ( BlockSize * CanvasCols ) + 64;
	NextBlokusPos.Y = m_Position.Y + 128;
	m_pNextBlokus->setPosition( NextBlokusPos );
}

VOID BlokiManager::setPosition(INT x, INT y)
{
	setPosition( Gdiplus::Point( x, y ) );
}

Gdiplus::Point BlokiManager::getPosition()
{
	return m_Position;
}

VOID BlokiManager::update( DWORD timeDelta, Ludos::InputDXUtilities::KeyboardDeviceRef Keyb )
{
	if( !m_bLostGame )
	{
		if( !m_bPendingBlokiFalling )
			NormalBlokusState( timeDelta, Keyb );
		else
			FallBlokusState( timeDelta );
	}

	// Draw
	if( m_pCurrentBlokus )
		m_pRenderer->addRenderObj( m_pCurrentBlokus );

	iterBlokus iter = m_listBloki.begin();
	while( iter != m_listBloki.end() )
	{
		Blokus* pBlokus = *iter;

		if( pBlokus->hasValidBlocks() )
		{
			m_pRenderer->addRenderObj( pBlokus );
			++iter;
		}
		else
		{
			S_DELETE( pBlokus );
			iter = m_listBloki.erase( iter );
		}
	}

	m_CanvasFrame.Submit( m_pRenderer );
	g_Score.Submit();
	m_pNextBlokus->Submit();
}

VOID BlokiManager::Clear()
{
	if( m_pCurrentBlokus )
		S_DELETE( m_pCurrentBlokus );
	
	for( iterBlokus pIter = m_listBloki.begin(); pIter != m_listBloki.end(); ++pIter )
	{
		Blokus* pBlokus = *pIter;
		S_DELETE( pBlokus );
	}
	m_listBloki.clear();

	m_bLostGame = FALSE;
	m_TimeForStep = StartingDifficulty;
}

BOOL BlokiManager::lostGame()
{
	return m_bLostGame;
}

VOID BlokiManager::NormalBlokusState( DWORD timeDelta, 
									  Ludos::InputDXUtilities::KeyboardDeviceRef Keyb )
{
	// Reset Score
	g_Score.resetMultiplier();

	// Make a new blokus
	if( m_pCurrentBlokus == NULL )
	{
		m_pCurrentBlokus = m_pNextBlokus->getBlokus();
		m_pCurrentBlokus->setParentPosition( m_Position );

		Gdiplus::Point BlokusPos( Ludos::Random( CanvasCols - ArraySize ), -(INT)ArraySize );
		m_pCurrentBlokus->setRelativePosition( BlokusPos );
	}

	// Move the blokus
	if( Keyb.KeyPress( DIK_LEFT ) )
	{
		Gdiplus::Point oldPos = m_pCurrentBlokus->getRelativePosition();
		m_pCurrentBlokus->setRelativePosition( Gdiplus::Point( oldPos.X - 1, oldPos.Y ) );
		if( checkBlokusCollision( m_pCurrentBlokus ) )
			m_pCurrentBlokus->setRelativePosition( oldPos );
	}
	if( Keyb.KeyPress( DIK_RIGHT ) )
	{
		Gdiplus::Point oldPos = m_pCurrentBlokus->getRelativePosition();
		m_pCurrentBlokus->setRelativePosition( Gdiplus::Point( oldPos.X + 1, oldPos.Y ) );
		if( checkBlokusCollision( m_pCurrentBlokus ) )
			m_pCurrentBlokus->setRelativePosition( oldPos );
	}

	// Rotate the blokus
	if( Keyb.KeyPress( DIK_UP ) )
	{
		m_pCurrentBlokus->rotateCW();
		if( checkBlokusCollision( m_pCurrentBlokus ) )
			m_pCurrentBlokus->rotateCCW();
	}

	// Fast fall
	BOOL bFastFall = FALSE;
	if( Keyb.KeyDown( DIK_DOWN ) )
		bFastFall = TRUE;

	// Fall the blokus
	m_Time += timeDelta;
	if( ( bFastFall && m_Time >= FastFallTime ) || m_Time >= m_TimeForStep )
	{
		m_Time = 0;
		if( !fallBlokus( m_pCurrentBlokus ) )
		{
			m_listBloki.push_back( m_pCurrentBlokus );
			m_pCurrentBlokus = NULL;
			bFastFall = FALSE;

			checkRowFormations();
		}
	}

	// Update difficulty
	m_DifficultyChangeTime += timeDelta;
	if( m_DifficultyChangeTime >= DifficultyTime )
	{
		if( m_TimeForStep >= MaxDifficulty )
			m_TimeForStep -= DifficultyStep;
		m_DifficultyChangeTime = 0;
	}
}

VOID BlokiManager::FallBlokusState( DWORD timeDelta )
{
	const DWORD TimeForFallStep = 200;

	// Make a fall blokus test for every blokus
	m_Time += timeDelta;

	if( m_Time >= TimeForFallStep )
	{
		BOOL bBlokusFalled = FALSE;
		m_Time = 0;

		for( iterBlokus iter = m_listBloki.begin(); iter != m_listBloki.end(); ++iter )
		{
			Blokus* pBlokus = (*iter);

			if( fallBlokus( pBlokus ) )
				bBlokusFalled = TRUE;
		}

		if( !bBlokusFalled )
		{
			m_bPendingBlokiFalling = FALSE;
			checkRowFormations();
		}
	}
}

VOID BlokiManager::loadTexture()
{
	m_pTexture = loadGdiBitmap( IMAGE_TILES, PNG_TYPE );
	if( !m_pTexture )
		throw InvalidTexture();
}

VOID BlokiManager::makeCanvasFrame()
{
	using Gdiplus::Point;
	
	Point P1( m_Position );
	Point P2( P1.X, P1.Y + (CanvasRows * BlockSize) );
	Point P3( P2.X + (CanvasCols * BlockSize), P2.Y );
	Point P4( P3.X, P1.Y );

	m_CanvasFrame.Clear();
	
	m_CanvasFrame.addLine( P1, P2, LEFT );
	m_CanvasFrame.addLine( P2, P3, BOTTOM );
	m_CanvasFrame.addLine( P3, P4, RIGHT );
}

BOOL BlokiManager::checkBlokusCollision(Blokus *pBlokus)
{
	// Validate
	if( !pBlokus )
		return FALSE;

	// Check collision with canvas
	if( pBlokus->collisionWithCanvas( CanvasRows, CanvasCols ) )
		return TRUE;

	// Check collision with other blocks
	for( iterBlokus iter = m_listBloki.begin(); iter != m_listBloki.end(); ++iter )
	{
		Blokus* otherBlokus = *iter;
		if( pBlokus->collisionWithBlokus( otherBlokus ) )
			return TRUE;
	}

	return FALSE;
}

BOOL BlokiManager::fallBlokus( Blokus *pBlokus )
{
	// Validate
	if( !pBlokus )
		return FALSE;

	// Move the blokus one position down
	Gdiplus::Point oldPos = pBlokus->getRelativePosition();
	
	pBlokus->setRelativePosition( Gdiplus::Point( oldPos.X, oldPos.Y + 1 ) );
	if( checkBlokusCollision( pBlokus ) )
	{
		pBlokus->setRelativePosition( oldPos );

		// Update Baro
		if( oldPos.Y < 5 )
			g_Baro.setMood( MOOD_WORRIED );
		else
		{
			if( g_Baro.getAnimation() == ANIMATION_NONE )
				g_Baro.setMood( MOOD_NORMAL );
		}

		return FALSE;
	}
	else
		return TRUE;
}


VOID BlokiManager::addBlokusCollisionData( Blokus *pBlokus )
{
	// Validate
	if( !pBlokus )
		return;

	Gdiplus::Point BlokusPos = pBlokus->getRelativePosition();

	for( INT row = 0; row < ArraySize; ++row )
	{
		for( INT col = 0; col < ArraySize; ++col )
		{
			INT posX = BlokusPos.X + col;
			INT posY = BlokusPos.Y + row;

			if( posX < 0 || posX >= CanvasCols ||
				posY < 0 || posY >= CanvasRows )
				continue;
			else
			{
				if( pBlokus->getBit( row, col ) )
					m_CollisionData[posY][posX].m_pAssociatedBlokus = pBlokus;
			}
		}
	}
}

VOID BlokiManager::checkRowFormations()
{
	// Add all blokus data to the CollisionData object
	ZeroMemory( (void*)m_CollisionData, sizeof(m_CollisionData) );

	for( iterBlokus iter = m_listBloki.begin(); iter != m_listBloki.end(); ++iter )
		addBlokusCollisionData( (*iter) );

	// Check row formations
	for( unsigned int row = 0; row < CanvasRows; ++row )
	{
		BOOL hasBlankSpots = FALSE;
		for( unsigned int col = 0; col < CanvasCols; ++col )
		{
			if( m_CollisionData[row][col].m_pAssociatedBlokus == NULL )
				hasBlankSpots = TRUE;
		}

		// A row has formed
		if( !hasBlankSpots )
		{
			std::set<Blokus*> processedBloki;

			// Handle line deletion
			for( int currentCol = 0; currentCol < CanvasCols; ++currentCol )
			{
				Blokus* pBlokus = m_CollisionData[row][currentCol].m_pAssociatedBlokus;
				pBlokus->collisionWithPoint( Gdiplus::Point( currentCol, row ), TRUE );
				
				processedBloki.insert( pBlokus );
			}
			m_bPendingBlokiFalling = TRUE;

			// Check if we can split some bloki
			for( std::set<Blokus*>::iterator iter = processedBloki.begin();
				 iter != processedBloki.end(); ++iter )
			{
				Blokus* oldBlokus = (*iter);
				Blokus* newBlokus = NULL;

				newBlokus = oldBlokus->splitBlokus();
				if( newBlokus )
					m_listBloki.push_back( newBlokus );


			}

			// Add score
			g_Score.addScore();

			// Update Baro
			g_Baro.setAnimation( ANIMATION_WAVE, 2000 );
			g_Baro.setMood( MOOD_HAPPY );

			break;
		}
	}

	// Check lose condition
	// If any block is outside the canvas at this point, the game is lost
	if( !m_bPendingBlokiFalling )
	{
		for( iterBlokus pIter = m_listBloki.begin(); pIter != m_listBloki.end(); ++pIter )
		{
			Blokus* pBlokus = *pIter;
			if( pBlokus->collisionWithCanvas( CanvasRows, CanvasCols, TRUE ) )
			{
				m_bLostGame = TRUE;
				break;
			}
		}
	}
}