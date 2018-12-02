#ifndef BLOKIFACTORY_H
#define BLOKIFACTORY_H

#include <objidl.h>
#include <gdiplus.h>

#include "Exception.h"

#include "Blokus.h"
#include "BlokusMatrix.h"

const UINT BlokusTypes = 7;

class BlokiFactory{
public:
// FUNCTIONS /////////////////////////////////////////////////////////////////////////////
	// CONSTRUCTOR AND DESTRUCTOR ////////////////////////////////////////////////////////

	BlokiFactory( Gdiplus::Image* pTexture, Gdiplus::Point ParentPos );
	~BlokiFactory();

	// UTILITIES /////////////////////////////////////////////////////////////////////////

	Blokus* createBlokus();
	Blokus* createBlokus( UINT Type );

	// SET-FUNCTIONS /////////////////////////////////////////////////////////////////////

	VOID setParentPos( const Gdiplus::Point ParentPos );

private:
// FUNCTIONS /////////////////////////////////////////////////////////////////////////////
	
	BlokusMatrix* createBlokusMatrix( UINT Type );

// VARIABLES /////////////////////////////////////////////////////////////////////////////

	Gdiplus::Image* m_pTexture;
	Gdiplus::Point	m_ParentPos;
};

class InvalidBlokusType: public Ludos::Exception{
public:
	InvalidBlokusType() : Exception( "Invalid blokus type.", "BlokiFactory" ) {}
};


#endif // BLOKIFACTORY_H