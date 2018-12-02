#pragma once
#ifndef GUARD_EXCEPTION_H
#define GUARD_EXCEPTION_H

/*****************************************************************************************
------------------------------------------------------------------------------------------
* Author: Jorge Luis Martinez
* Defines a custom exception class which should be used on exception handling.  Provides
* useful information at the moment of tracking bugs.
* Version :		CEX 2.2
------------------------------------------------------------------------------------------
*****************************************************************************************/

// DEFINES ///////////////////////////////////////////////////////////////////////////////
#define EXCEPTION_VERSION 0x16	// Class version

// INCLUDES //////////////////////////////////////////////////////////////////////////////

/// STD ///
#include <string>	// String handling

/*** LUDOS - Game namespace ***/
namespace Ludos{

// CLASS /////////////////////////////////////////////////////////////////////////////////

class Exception{
public:
// FUNCTIONS /////////////////////////////////////////////////////////////////////////////
	// DEFAULT CONSTRUCTOR AND DESTRUCTOR ////////////////////////////////////////////////

	Exception(	const std::string &szProblem, const std::string &szPlace = "UNKNOWN" ) :
			Problem(szProblem),
			Place(szPlace)
	{}

// VARIABLES /////////////////////////////////////////////////////////////////////////////

	const std::string Problem;
	const std::string Place;
};


}  // Ludos namespace

#endif