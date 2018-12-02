#include "BlokusMatrix.h"
#include <memory>

BlokusMatrix::BlokusMatrix()
{
	Clear();
}

BlokusMatrix::BlokusMatrix( const BlokusMatrix& other )
{
	operator=( other );
}

BlokusMatrix::~BlokusMatrix()
{
}

BlokusMatrix& BlokusMatrix::operator =( const BlokusMatrix &other )
{
	if( this == &other )
		return *this;

	memcpy( (void*)m_Data, (void*)other.m_Data, sizeof(m_Data) );
	return *this;
}

BlokusMatrix& BlokusMatrix::operator =( const Matrix4X& otherMatrix )
{
	memcpy( (void*)m_Data, (void*)otherMatrix, sizeof(m_Data) );
	return *this;
}

void BlokusMatrix::setBit( unsigned int Row, unsigned int Col, bool Value )
{
	m_Data[Row][Col] = Value;
}

bool BlokusMatrix::getBit( unsigned int Row, unsigned int Col )
{
	return m_Data[Row][Col];
}

BlokusMatrix* BlokusMatrix::splitMatrix()
{
	/* For matrix splitting two conditions must be met: */
	/* First, the matrix must have specifically 2 or 3 blocks */
	/* Second, 1 or 2 blocks must have one or more rows in between the other(s) */

	unsigned int BlockCount = 0;
	bool RowContainsBlocks[ArraySize]; 
	memset( (void*)RowContainsBlocks, false, sizeof( RowContainsBlocks ) );

	for( unsigned int row = 0; row < ArraySize; ++row )
	{
		for( unsigned int col = 0; col < ArraySize; ++col )
		{
			if( m_Data[row][col] )
			{
				++BlockCount;
				RowContainsBlocks[row] = true;
			}
		}
	}

	// Check the conditions
	if( BlockCount == 0 || BlockCount > 3 )
		return false;

	bool bSplit = false;
	if( !RowContainsBlocks[1] )
	{
		if( RowContainsBlocks[0]
		&& ( RowContainsBlocks[2] || RowContainsBlocks[3] ) )
			bSplit = true;
	}

	if( !RowContainsBlocks[2] )
	{
		if( RowContainsBlocks[3]
		&& ( RowContainsBlocks[0] || RowContainsBlocks[1] ) )
			bSplit = true;
	}

	// Split if necesary
	if( bSplit )
	{
		/* If we split, one matrix gets the first two rows and the other the second two rows */
		BlokusMatrix* ret = createMatrixCopy();
		
		memset( (void*)(ret->m_Data), false, sizeof(bool) * ArraySize * 2 );
		memset( (void*)( &m_Data[2][0] ), false, sizeof(bool) * ArraySize * 2 );
		return ret;
	}
	else
		return NULL;
}

void BlokusMatrix::Clear()
{
	memset( (void*)m_Data, 0, sizeof( m_Data ) );
}

////
// 4X
////

BlokusMatrix* BlokusMatrix4X::createMatrixCopy()
{
	return new BlokusMatrix4X( *this );
}

void BlokusMatrix4X::rotateCW()
{
	BlokusMatrix4X tempArray;
	tempArray.Clear();

	unsigned int TopLimit = ArraySize - 1;

	for( unsigned int col = 0; col != ArraySize; ++col )
	{
		for( unsigned int row = 0; row != ArraySize; ++row )
		{
			tempArray.setBit(col, TopLimit - row, getBit( row, col ));
		}
	}

	BlokusMatrix::operator =( tempArray );
}

void BlokusMatrix4X::rotateCCW()
{
	BlokusMatrix4X tempArray;
	tempArray.Clear();

	unsigned int TopLimit = ArraySize - 1;

	for( unsigned int row = 0; row != ArraySize; ++row )
	{
		for( unsigned int col = 0; col != ArraySize; ++col )
		{
			tempArray.setBit( row, col, getBit( col, TopLimit - row ) );;
		}
	}

	BlokusMatrix::operator =( tempArray );
}

///
// 3X
///

BlokusMatrix* BlokusMatrix3X::createMatrixCopy()
{
	return new BlokusMatrix3X( *this );
}

void BlokusMatrix3X::rotateCW()
{
	BlokusMatrix3X tempArray;
	tempArray.Clear();

	unsigned int TopLimit = ArraySize - 2;

	for( unsigned int col = 0; col != ArraySize-1; ++col )
	{
		for( unsigned int row = 0; row != ArraySize-1; ++row )
		{
			tempArray.setBit( col, TopLimit - row, getBit( row, col ) );
		}
	}

	BlokusMatrix::operator =( tempArray );
}

void BlokusMatrix3X::rotateCCW()
{
	BlokusMatrix3X tempArray;
	tempArray.Clear();

	unsigned int TopLimit = ArraySize - 2;

	for( unsigned int row = 0; row != ArraySize-1; ++row )
	{
		for( unsigned int col = 0; col != ArraySize-1; ++col )
		{
			tempArray.setBit( row, col, getBit( col, TopLimit - row ) );
		}
	}

	BlokusMatrix::operator =( tempArray );
}