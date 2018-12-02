#ifndef GUARD_HPLAINUTILITY_H
#define GUARD_HPLAINUTILITY_H

/*****************************************************************************************
------------------------------------------------------------------------------------------
* Author: Jorge Luis Martinez
* Provides some useful functions, macros and classes to use on any application.
------------------------------------------------------------------------------------------
*****************************************************************************************/

// INCLUDES //////////////////////////////////////////////////////////////////////////////

// STD //
#include <string>			// String handling
#include <sstream>			// Streams handling
#include <cstdlib>			// Different operations
#include <ctime>			// Time handling
#include <cctype>			// Char handling
#include <math.h>			// Mathematical operations

/*** LUDOS - Game namespace ***/
namespace Ludos{

// MACROS ////////////////////////////////////////////////////////////////////////////////

/* This is used for interfaces that provide a release method. So when deleting it the
release method is called instead */
#define S_RELEASE(ptr) { if (ptr != NULL) ptr->Release(); ptr = NULL; }

/* This is used for any pointer at the moment of deleting it. So is safer to delete it */
#define S_DELETE(ptr) { if (ptr != NULL) delete ptr; ptr = NULL; }

/* This is used for safely deleting a pointer to an array */
#define S_DELETE_ARRAY(ptr) { if (ptr != NULL) delete [] ptr; ptr = NULL; }

/* This is used for safely RELEASING and DELETING a pointer of a class */
#define S_RDELETE(ptr) { if (ptr != NULL)				\
							{							\
								ptr->Release();			\
								delete ptr;				\
							}							\
							ptr = NULL;					\
						}

/* This is used for enabling memory debugging and leak tracing */
#if defined( DEBUG ) | defined ( _DEBUG )
	#ifndef LUDOS_MEM_LEAK_TRACE
	#define LUDOS_MEM_LEAK_TRACE							\
	{												\
		_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF |		\
						_CRTDBG_LEAK_CHECK_DF );	\
	}
	#endif
#else
	#ifndef LUDOS_MEM_LEAK_TRACE
	#define LUDOS_MEM_LEAK_TRACE
	#endif
#endif


// FUNCTIONS /////////////////////////////////////////////////////////////////////////////

/* Converts a given value to a string */
template <class T>
inline std::string toStr( const T& Value )
{
	// Use a string stream to convert the value
	std::ostringstream Stream;
	Stream << Value;

	// Return
	return Stream.str();
}

/* Rounds a given number the amount of places specified */
inline float Round( float fNumber, unsigned int nPlaces )
{
	// This variable is for shifting the number of spaces used
	const float fShift = powf( 10.0f, (float)nPlaces );

	// Shift the number multiplying it with the shifter
	fNumber *= fShift;
	// Add 0.5 to the number so it will "round" and not "floor"
	fNumber = floorf( fNumber + 0.5f );
	// Shift the number again, but this time do it backwards
	fNumber /= fShift;

	return fNumber;
}

/* Makes a new seed for the random numbers */
inline void Randomize()
{
	srand( (unsigned int)time(0) );
}

/* This returs a random int number in [0, nMax) */
inline int Random( int nMax )
{
	if( nMax == 0 )
		return 0;
	return rand()%nMax;
}

/* This returns a random float number in [0, nMax) */
inline float Randomf( int nMax )
{
	// Check the value
	if (nMax == 0)
		return 0;

	// This gets the random integer part
	int IntegerPart = Random(nMax);
	// This gets the decimal part
	int DecimalPart = rand();
	float Number = (float)DecimalPart / (float)RAND_MAX;

	// Now get everything together
	Number += (float)IntegerPart;
	return Number;
}

/* Converts a string's characters to uppercase */
inline std::string toUpper( const std::string& szString )
{
	std::string RetString;
	RetString.reserve( szString.size() );

	for( std::string::const_iterator pitIter = szString.begin();
		 pitIter != szString.end(); ++pitIter )
		 RetString.push_back( std::toupper( (int)*pitIter ) );

	return RetString;
}

/* Converts a string's characters to lowercase */
inline std::string toLower( const std::string& szString )
{
	std::string RetString;
	RetString.reserve( szString.size() );

	for( std::string::const_iterator pitIter = szString.begin();
		 pitIter != szString.end(); ++pitIter )
		RetString.push_back(  std::tolower( (int)*pitIter ) );

	return RetString;
}

} // Ludos namespace

#endif