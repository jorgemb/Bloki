#pragma once
#ifndef GUARD_WINCLASS_H
#define GUARD_WINCLASS_H

/*****************************************************************************************
------------------------------------------------------------------------------------------
* Author: Jorge Luis Martinez
* Introduces some classes for managing fundamental Windows stuff focused on game
programming.  It will only let you create one window, but you could add more manually
either using normal windows or dialogs.
* Version : WIN 6.0
------------------------------------------------------------------------------------------
- NOTES: - In order to use an alternative MsgHandler function you must inherit the class and
	implement the new handler.  However, you must ALSO implement a default destructor
	that calls the Release member, that way the WM_DESTROY message is also covered by
	the new message handler.
- When the window is created the specified dimensions are assured but the given
	position changes slightly.
*****************************************************************************************/

// DEFINES ///////////////////////////////////////////////////////////////////////////////

#ifndef WIN32_LEAN_AND_MEAN		// This is for only including most used Windows functions
	#define WIN32_LEAN_AND_MEAN
#endif

// INCLUDES //////////////////////////////////////////////////////////////////////////////
#include <windows.h>		// Main Windows header file
#include <string>			// For managing strings

#include "Utility.h"
#include "Log.h"
#include "Exception.h"


/*** LUDOS - Game namespace ***/
namespace Ludos{

// DEFINES ///////////////////////////////////////////////////////////////////////////////

#define WINCLASS_VERSION 0x3C			// The version number of the class

/* Default values */
#define WINDOW_DEFAULTWIDTH		640
#define WINDOW_DEFAULTHEIGHT	480
#define WINDOW_DEFAULTPOSX		24
#define WINDOW_DEFAULTPOSY		32

// MACROS & FUNCTIONS ////////////////////////////////////////////////////////////////////

// This macro returns the current state of a given key.
#define KeyState(vk) ( (GetAsyncKeyState(vk) & 0x8000) ? TRUE : FALSE )

std::string LoadResourceString( HINSTANCE hInstance, UINT nStringID );

// CLASSES ///////////////////////////////////////////////////////////////////////////////

/* This is the main class for Windows. It handles creation and destruction of the window*/
class Window{
public:
// CONSTRUCTOR AND DESTRUCTOR ////////////////////////////////////////////////////////////

	Window( const std::string& szName = "Windows" );
	virtual ~Window();

	BOOL Initialize( BOOL bWindowed );
	VOID Kill();

// UTILITIES /////////////////////////////////////////////////////////////////////////////

	// Handles a single message
	BOOL Update();

	// Waits for a message to come, returns when one arrives.
	BOOL Update_Idle();

	// The message handler
	virtual LRESULT MsgHandler( HWND hWindow, UINT Msg, WPARAM wParam, LPARAM lParam );

// STATES ////////////////////////////////////////////////////////////////////////////////

	BOOL isActive() const;


// SET- FUNCTIONS ////////////////////////////////////////////////////////////////////////

	BOOL setIcon( LPCSTR szIconName, BOOL bFromResource );
	BOOL setCursor( LPCSTR szCursorName, BOOL bFromResource );
	BOOL setCaption( const std::string& szText );
	BOOL setPosition( UINT x, UINT y );
	BOOL setDimensions( UINT nWidth, UINT nHeight );
	VOID setCharInputFunction( VOID (*fpCharInput)(TCHAR) );
	
// GET- FUNCTIONS ////////////////////////////////////////////////////////////////////////

	INT getX() const;
	INT getY() const;
	UINT getWidth() const;
	UINT getHeight() const;

	BOOL getFocus() const;
	std::string getCaption() const;
	std::string getVersionString() const;

	// OPERATORS /////////////////////////////////////////////////////////////////////////	
	
	operator HWND();

protected:
// PRIVATE METHODS ///////////////////////////////////////////////////////////////////////
	
	// Creates, fills and registers the WNDCLASSEX to Windows
	BOOL registerClass();
	// Unregisters the WNDCLASSEX from windows
	BOOL unregisterClass();
	// Creates a new window with the default settings
	BOOL createWindow( DWORD dwFlags );
	// Destroys the main window
	BOOL destroyWindow();
	// Releases the class
	VOID Release();

	/// MESSAGE ROUTER ///
	static LRESULT CALLBACK MsgRouter( HWND hWindow, UINT Message, 
												WPARAM wParam, LPARAM lParam );
		
// PRIVATE VARIABLES /////////////////////////////////////////////////////////////////////

	// Main aplication instance handler
	HINSTANCE m_hInstance;
	// Handler to the main window
	HWND m_hWindow;

	BOOL m_bActive;
	BOOL m_bFocus;
	BOOL m_bWindowed;

	DWORD m_dwWindowStyle;
	INT m_nYPOS, m_nXPOS, m_nHeight, m_nWidth;

	// Function pointer, wich contains the function that handles character input
	VOID (*m_fpCharInput)(TCHAR);

	// MESSAGE ROUTER
	/* This is used to let know the message router that if the window handler doesn't
	includes a Window pointer then use the temporary one */
	static Window* m_pTempClassPointer;

	// LOG
	Log m_Log;

};	// Window

} // Ludos namespace

#endif
