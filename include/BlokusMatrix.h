#ifndef BLOKUSMATRIX_H
#define BLOKUSMATRIX_H

const unsigned int ArraySize = 4;
typedef bool Matrix4X[ArraySize][ArraySize];

class BlokusMatrix{
public:
// FUNCTIONS /////////////////////////////////////////////////////////////////////////////
	// CONSTRUCTOR AND DESTRUCTOR ////////////////////////////////////////////////////////

	BlokusMatrix();
	BlokusMatrix( const BlokusMatrix& other );
	~BlokusMatrix();

	// UTILITIES /////////////////////////////////////////////////////////////////////////

	BlokusMatrix& operator=( const BlokusMatrix& other );
	BlokusMatrix& operator=( const Matrix4X& otherMatrix );

	void setBit( unsigned int Row, unsigned int Col, bool Value );
	bool getBit( unsigned int Row, unsigned int Col );

	void Clear();

	BlokusMatrix* splitMatrix();

	virtual void rotateCW() = 0;
	virtual void rotateCCW() = 0;
protected:
// FUNCTIONS /////////////////////////////////////////////////////////////////////////////

	virtual BlokusMatrix* createMatrixCopy() = 0;

// VARIABLES /////////////////////////////////////////////////////////////////////////////

	Matrix4X m_Data;
};

class BlokusMatrix4X: public BlokusMatrix{
public:
// FUNCTIONS /////////////////////////////////////////////////////////////////////////////
	// UTILITIES /////////////////////////////////////////////////////////////////////////

	BlokusMatrix* createMatrixCopy();
	void rotateCW();
	void rotateCCW();
};

class BlokusMatrix3X: public BlokusMatrix{
public:
// FUNCTIONS /////////////////////////////////////////////////////////////////////////////
	// UTILITIES /////////////////////////////////////////////////////////////////////////

	BlokusMatrix* createMatrixCopy();
	void rotateCW();
	void rotateCCW();
};


#endif // BLOKUSMATRIX_H
