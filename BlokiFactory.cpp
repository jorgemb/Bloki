#include "BlokiFactory.h"
#include "Utility.h"

BlokiFactory::BlokiFactory( Gdiplus::Image *pTexture, Gdiplus::Point ParentPos ) :
m_pTexture( pTexture ),
m_ParentPos( ParentPos )
{
}

BlokiFactory::~BlokiFactory()
{
}


Blokus* BlokiFactory::createBlokus()
{
	return createBlokus( Ludos::Random( BlokusTypes ) );
}

Blokus* BlokiFactory::createBlokus(UINT Type)
{
	// Validate
	if( Type >= BlokusTypes )
		return NULL;

	BlokusMatrix* pBlokusData = createBlokusMatrix( Type );
	Blokus* retBlokus = new Blokus( m_pTexture, m_ParentPos, pBlokusData );

	return retBlokus;
}

VOID BlokiFactory::setParentPos( const Gdiplus::Point ParentPos )
{
	m_ParentPos = ParentPos;
}

BlokusMatrix* BlokiFactory::createBlokusMatrix( UINT Type )
{
	// Create the matrix
	BlokusMatrix* newMatrix = NULL;

	switch( Type )
	{
	case 0: // LINE
		{
			newMatrix = new BlokusMatrix4X;

			Matrix4X Data = {
			{ 0, 1, 0, 0 },
			{ 0, 1, 0, 0 },
			{ 0, 1, 0, 0 },
			{ 0, 1, 0, 0 } };
			*newMatrix = Data;

			break;
		}
	case 1: // BLOCK
		{
			newMatrix = new BlokusMatrix4X;

			Matrix4X Data = {
			{ 0, 0, 0, 0 },
			{ 0, 1, 1, 0 },
			{ 0, 1, 1, 0 },
			{ 0, 0, 0, 0 } };
			*newMatrix = Data;

			break;
		}
	case 2: // LEFT L
		{
			newMatrix = new BlokusMatrix3X;

			Matrix4X Data = {
			{ 1, 0, 0, 0 },
			{ 1, 1, 1, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 } };
			*newMatrix = Data;

			break;
		}
	case 3: // RIGHT L
		{
			newMatrix = new BlokusMatrix3X;

			Matrix4X Data = {
			{ 0, 0, 1, 0 },
			{ 1, 1, 1, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 } };
			*newMatrix = Data;

			break;
		}
	case 4: // REV ZETA
		{
			newMatrix = new BlokusMatrix3X;

			Matrix4X Data = {
			{ 0, 1, 1, 0 },
			{ 1, 1, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 } };
			*newMatrix = Data;

			break;
		}
	case 5: // BLOKUS T
		{
			newMatrix = new BlokusMatrix3X;

			Matrix4X Data = {
			{ 0, 1, 0, 0 },
			{ 1, 1, 1, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 } };
			*newMatrix = Data;

			break;
		}
	case 6: // ZETA
		{
			newMatrix = new BlokusMatrix3X;

			Matrix4X Data = {
			{ 1, 1, 0, 0 },
			{ 0, 1, 1, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 } };
			*newMatrix = Data;

			break;
		}
	default:
		throw InvalidBlokusType();
	}

	return newMatrix;
}
