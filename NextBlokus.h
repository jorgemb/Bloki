#ifndef NEXTBLOKUS_H
#define NEXTBLOKUS_H

#include "Blokus.h"
#include "GdiFigures.h"
#include "Renderer.h"
#include "BlokiFactory.h"


class NextBlokus: public RenderObj{
public:
// FUNCTIONS /////////////////////////////////////////////////////////////////////////////
	// CONSTRUCTOR AND DESTRUCTOR ////////////////////////////////////////////////////////

	NextBlokus( BlokiFactory* pBFactory,
				Renderer* pRenderer,
				const Gdiplus::Point& Position );
	~NextBlokus();

	// UTILITIES /////////////////////////////////////////////////////////////////////////

	Blokus* getBlokus();
	VOID Submit();

	BOOL Draw( Gdiplus::Graphics* pGraphics );

	// SET/GET-FUNCTIONS /////////////////////////////////////////////////////////////////

	VOID setPosition( const Gdiplus::Point& Position );
	Gdiplus::Point getPosition();

	// GET-FUNCTIONS /////////////////////////////////////////////////////////////////////
	// LOST DEVICES //////////////////////////////////////////////////////////////////////
private:
// VARIABLES /////////////////////////////////////////////////////////////////////////////

	Renderer*			m_pRenderer;
	Gdiplus::Point		m_Position;
	Blokus*				m_pBlokus;
	BlokiFactory*		m_pBlokiFactory;
	CenteredPath		m_Square;

	Gdiplus::Bitmap*	m_pText;
};


#endif // NEXTBLOKUS_H
