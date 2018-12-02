#ifndef BLOKUS_H
#define BLOKUS_H

#include <objidl.h>
#include <gdiplus.h>

#include "Renderer.h"
#include "BlokusMatrix.h"

const UINT BlockSize	= 32;
const UINT MaxColors	= 8;

class Blokus: public RenderObj{
public:
// FUNCTIONS /////////////////////////////////////////////////////////////////////////////
	// CONSTRUCTOR AND DESTRUCTOR ////////////////////////////////////////////////////////

	Blokus( Gdiplus::Image* pTexture, Gdiplus::Point ParentPos, BlokusMatrix* pBlokusData );
	~Blokus();

	// UTILITIES /////////////////////////////////////////////////////////////////////////

	BOOL Draw( Gdiplus::Graphics* pGraphics );

	VOID rotateCW();
	VOID rotateCCW();

	BOOL collisionWithCanvas( INT Rows, INT Cols, BOOL checkTopBorder = FALSE );
	BOOL collisionWithBlokus( const Blokus* other );
	BOOL collisionWithPoint( const Gdiplus::Point& RelativePoint, BOOL deleteBlock = FALSE );

	BOOL hasValidBlocks() const;
	BOOL hasValidBlocks();

	Blokus* splitBlokus();

	// SET-FUNCTIONS /////////////////////////////////////////////////////////////////////

	VOID setRelativePosition( const Gdiplus::Point& RelPosition );
	VOID setColor( UINT Color );
	VOID setParentPosition( const Gdiplus::Point& ParentPos );

	// GET-FUNCTIONS /////////////////////////////////////////////////////////////////////

	Gdiplus::Point getRelativePosition();
	Gdiplus::Point getRelativePosition() const;
	UINT getColor();
	UINT getColor() const;

	BOOL getBit( UINT Row, UINT Col ) const;

private:
// FUNCTIONS /////////////////////////////////////////////////////////////////////////////

	VOID calculateColorRect();
	VOID drawSingleBlock( const Gdiplus::Point& RelativePos, Gdiplus::Graphics* pGraphics );

// VARIABLES /////////////////////////////////////////////////////////////////////////////

	Gdiplus::Image*		m_pTexture;
	UINT				m_Color;
	Gdiplus::Rect		m_ColorRect;

	Gdiplus::Point		m_ParentPos;
	Gdiplus::Point		m_RelativePos;

	BlokusMatrix*		m_pBlokusData;
	BOOL				m_hasValidBlocks;
};

#endif // BLOKUS_H