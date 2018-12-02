#include "Blokus.h"
#include "Exception.h"

#include "Utility.h"

Blokus::Blokus( Gdiplus::Image *pTexture, Gdiplus::Point ParentPos, BlokusMatrix* pBlokusData ):
m_pTexture( pTexture ),
m_Color( 0 ),
m_ColorRect( 0, 0, BlockSize, BlockSize ),
m_ParentPos( ParentPos ),
m_RelativePos( 0, 0 ),
m_pBlokusData( pBlokusData ),
m_hasValidBlocks( TRUE )
{
	if( !m_pTexture )
		throw Ludos::Exception( "Invalid texture pointer.", "Blokus::Blokus()" );

	calculateColorRect();
}

Blokus::~Blokus()
{
	S_DELETE( m_pBlokusData );
}

VOID Blokus::rotateCW()
{
	m_pBlokusData->rotateCW();
}

VOID Blokus::rotateCCW()
{
	m_pBlokusData->rotateCCW();
}

BOOL Blokus::collisionWithCanvas( INT Rows, INT Cols, BOOL checkTopBorder )
{
	// The only part that doesn't collide with the blokus is the top side
	
	// Discard if we aren't near of the sides
	if( m_RelativePos.X > 0 
		&& m_RelativePos.Y > 0
		&& m_RelativePos.X <= (Cols - (INT)ArraySize) 
		&& m_RelativePos.Y <= (Rows - (INT)ArraySize) )
		return FALSE;

	// Check if any given block is outside
	for( INT col = 0; col < ArraySize; ++col )
	{
		for( INT row = 0; row < ArraySize; ++row )
		{
			if( m_pBlokusData->getBit( row, col ) )
			{
				Gdiplus::Point BlockPos( m_RelativePos.X + col, m_RelativePos.Y + row );

				if( BlockPos.X < 0 || BlockPos.X >= Cols ||
					BlockPos.Y >= Rows || ( checkTopBorder && BlockPos.Y < 0 ) )
					return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL Blokus::collisionWithBlokus( const Blokus* other )
{
	if( !other || other == this )
		return FALSE;

	// Discard if the blokus are too far away
	Gdiplus::Point otherPos = other->getRelativePosition();
	if( abs( m_RelativePos.X - otherPos.X ) >= ArraySize 
		|| abs( m_RelativePos.Y - otherPos.Y ) >= ArraySize )
	{
		return FALSE;
	}

	// Check for blocks that may collide
	Gdiplus::Point difference = m_RelativePos - other->getRelativePosition();

	for( INT row = 0; row < ArraySize; ++row )
	{
		for( INT col = 0; col < ArraySize; ++col )
		{
			INT diffX = difference.X + col;
			INT diffY = difference.Y + row;
			if( diffX < 0 || diffX >= ArraySize 
				|| diffY < 0 || diffY >= ArraySize )
				continue;

			if( m_pBlokusData->getBit( row, col ) && other->m_pBlokusData->getBit( diffY, diffX ) )
				return TRUE;
		}
	}

	return FALSE;
}

BOOL Blokus::collisionWithPoint( const Gdiplus::Point& RelativePoint, BOOL deleteBlock )
{
	// Discard if the blokus is too far away
	Gdiplus::Point diffPoint = RelativePoint - m_RelativePos;

	if( diffPoint.X < 0 || diffPoint.X >= ArraySize
		|| diffPoint.Y < 0 || diffPoint.Y >= ArraySize )
		return FALSE;

	// Check the point for collision
	if( m_pBlokusData->getBit( diffPoint.Y, diffPoint.X ) )
	{
		if( deleteBlock )
		{
			m_pBlokusData->setBit( diffPoint.Y, diffPoint.X, false );

			// TODO: Check if there are some remaining blocks

			// TODO: Check if the block was splitted.
		}
		return TRUE;
	}
	else
		return FALSE;
}

BOOL Blokus::hasValidBlocks() const
{
	return m_hasValidBlocks;
}

BOOL Blokus::hasValidBlocks()
{
	return m_hasValidBlocks;
}

Blokus* Blokus::splitBlokus()
{
	// Validate the splitting
	if( !hasValidBlocks() )
		return NULL;

	BlokusMatrix* newBlokusData = m_pBlokusData->splitMatrix();

	if( newBlokusData )
	{
		Blokus* ret = new Blokus( m_pTexture, m_ParentPos, newBlokusData );

		ret->setRelativePosition( m_RelativePos );
		ret->setColor( m_Color );
		return ret;
	}
	else
		return NULL;
}

VOID Blokus::setRelativePosition(const Gdiplus::Point &RelPosition)
{
	m_RelativePos = RelPosition;
}

Gdiplus::Point Blokus::getRelativePosition()
{
	return m_RelativePos;
}

Gdiplus::Point Blokus::getRelativePosition() const
{
	return m_RelativePos;
}

VOID Blokus::setColor( UINT Color )
{
	if( Color < MaxColors )
	{
		m_Color = Color;
		calculateColorRect();
	}
	else
		return;
}

VOID Blokus::setParentPosition( const Gdiplus::Point &ParentPos )
{
	m_ParentPos = ParentPos;
}

UINT Blokus::getColor()
{
	return m_Color;
}

UINT Blokus::getColor() const
{
	return m_Color;
}

BOOL Blokus::getBit( UINT Row, UINT Col ) const
{
	if( Row >= ArraySize || Col >= ArraySize )
		return FALSE;

	return m_pBlokusData->getBit( Row, Col ) ? TRUE : FALSE;
}

VOID Blokus::calculateColorRect()
{
	INT y = m_Color / 4;
	INT x = m_Color % 4;

	m_ColorRect.X		= x * BlockSize;
	m_ColorRect.Y		= y * BlockSize;
	m_ColorRect.Width	= BlockSize;
	m_ColorRect.Height	= BlockSize;
}

BOOL Blokus::Draw( Gdiplus::Graphics *pGraphics )
{
	BOOL hasValidBlocks = FALSE;

	// Draw the blocks
	for( unsigned int row = 0; row != ArraySize; ++row )
	{
		for( unsigned int col = 0; col != ArraySize; ++col )
		{
			if( m_pBlokusData->getBit( row, col ) )
			{
				hasValidBlocks = TRUE;
				Gdiplus::Point blockPosition( m_RelativePos.X + col, m_RelativePos.Y + row );

				// Check if the block is above the canvas, if so, don't draw.
				if( blockPosition.Y >= 0 )
					drawSingleBlock( blockPosition, pGraphics );
			}
		}
	}

	if( !hasValidBlocks )
		m_hasValidBlocks = FALSE;

	return TRUE;
}

VOID Blokus::drawSingleBlock( const Gdiplus::Point &RelativePos, Gdiplus::Graphics *pGraphics )
{
	// Calculate draw rect
	Gdiplus::Rect destRect( m_ParentPos.X + RelativePos.X * BlockSize,
							m_ParentPos.Y + RelativePos.Y * BlockSize,
							BlockSize, BlockSize );
	
	// Draw the block
	pGraphics->DrawImage( m_pTexture,
						  destRect,
						  m_ColorRect.X,
						  m_ColorRect.Y,
						  m_ColorRect.Width,
						  m_ColorRect.Height,
						  Gdiplus::UnitPixel,
						  NULL, NULL, NULL );

}