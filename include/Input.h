#pragma once
#ifndef GUARD_INPUT_H
#define GUARD_INPUT_H

/*****************************************************************************************
------------------------------------------------------------------------------------------
* Author: Jorge Luis Martinez
* Provides a simple interface for managing input through DirectInput.  It only grants
* access to the Keyboard and the mouse.
* Version : IN 1.6
------------------------------------------------------------------------------------------
- NOTES: A device can't be created exclusive and background, so expect an exception
in that case.
*****************************************************************************************/

// DEFINES ///////////////////////////////////////////////////////////////////////////////

#define DIRECTINPUT_VERSION 0x0800	// The version to use with Direct Input

// INCLUDES //////////////////////////////////////////////////////////////////////////////
#include <string>

#include <dinput.h>		// Direct Input header file
#include "Exception.h"
#include "Log.h"

/*** LUDOS - Game namespace ***/
namespace Ludos{

// CONSTANTS /////////////////////////////////////////////////////////////////////////////
#define InputDX_VERSION 0x10		// The class version

// These define the mouse buttons
CONST BYTE MOUSE_LEFT	=	0x00;
CONST BYTE MOUSE_RIGHT	=	0x01;
CONST BYTE MOUSE_MIDDLE	=	0x02;
CONST BYTE MOUSE_OTHER	=	0x03;

// ENUM //////////////////////////////////////////////////////////////////////////////////

enum INPUT_ENABLE{
	IN_ALL = 0,		// Enable or disable all
	IN_KEYBOARD,	// Disables or enables keyboard, only
	IN_MOUSE,		// Disables or enables the mouse, only.
};

// LIBRARIES /////////////////////////////////////////////////////////////////////////////


#ifndef LUDOS_NOLIB
	#pragma comment (lib, "dinput8.lib")
	#pragma comment (lib, "dxguid.lib")
#endif

// CLASSES ///////////////////////////////////////////////////////////////////////////////

/* This is for input using DirectX interfaces */
class InputDX{
public:
// FUNCTIONS /////////////////////////////////////////////////////////////////////////////
	// CONSTRUCTOR AND DESTRUCTOR ////////////////////////////////////////////////////////

	explicit InputDX( const std::string &szName = "InputDX" );
	~InputDX();

	// UTILITIES /////////////////////////////////////////////////////////////////////////

	BOOL Initialize( HWND in_hWindow,
					 BOOL in_bExclusive = TRUE,
					 BOOL in_bForeground = TRUE,
					 HRESULT* out_pResult = NULL );
	VOID updateInput();
	VOID enableDevice( INPUT_ENABLE Device );
	VOID disableDevice( INPUT_ENABLE Device );
	BOOL isDeviceEnabled( INPUT_ENABLE Device ) const;

	BOOL isValid() const;

	// KEYBOARD //////////////////////////////////////////////////////////////////////////

	BOOL KeyDown(BYTE nKey) const;
	BOOL KeyPress(BYTE nKey) const;
	BOOL KeyRelease(BYTE nKey) const;

	static std::string KeyToStr( BYTE key );

	// MOUSE /////////////////////////////////////////////////////////////////////////////

	BOOL MouseButtonDown(BYTE nButton) const;
	BOOL MouseButtonPress(BYTE nButton) const;
	BOOL MouseButtonRelease(BYTE nButton) const;

	LONG MouseDeltaX() const;
	LONG MouseDeltaY() const;
	LONG MouseDeltaZ() const;
	
	std::string getVersionString() const;

private:
// FUNCTIONS /////////////////////////////////////////////////////////////////////////////

	inline void fillResultParameter( HRESULT result, HRESULT* pResult )
	{
		if( pResult )
			*pResult = result;
	}

	HRESULT createInterface();
	HRESULT initKeyboard( HWND hWindow, DWORD Flags );
	HRESULT initMouse( HWND hWindow, DWORD Flags );

	VOID Key_swapPointers();
	VOID Mouse_swapPointers();

	VOID Release();

// VARIABLES /////////////////////////////////////////////////////////////////////////////

	LPDIRECTINPUT8 m_pDInput;

	BOOL m_bKeyEnabled;
	BOOL m_bMouseEnabled;

	// KEYBOARD //
	LPDIRECTINPUTDEVICE8 m_pDIKeyboard;
	BYTE m_pKeyInput1[256], m_pKeyInput2[256];
	BYTE *m_pKeyInputNow, *m_pKeyInputLast;

	// MOUSE //
	LPDIRECTINPUTDEVICE8 m_pDIMouse;
	DIMOUSESTATE m_sMouseInput1, m_sMouseInput2;
	LPDIMOUSESTATE m_pMouseInputNow, m_pMouseInputLast;

	Log m_Log;
};

namespace InputDXUtilities{

	/* Class for retrieving keyboard data */
	class KeyboardDeviceRef{
	public:
	// FUNCTIONS /////////////////////////////////////////////////////////////////////////////
		// CONSTRUCTOR AND DESTRUCTOR ////////////////////////////////////////////////////////

		KeyboardDeviceRef();
		KeyboardDeviceRef( const InputDX& InputDevice );
		KeyboardDeviceRef( const KeyboardDeviceRef& Other );
		~KeyboardDeviceRef();

		// UTILITIES /////////////////////////////////////////////////////////////////////////

		BOOL isValid() const;
		BOOL isDeviceEnabled() const;

		// GET-FUNCTIONS /////////////////////////////////////////////////////////////////////

		BOOL KeyDown( BYTE nKey ) const;
		BOOL KeyPress( BYTE nKey ) const;
		BOOL KeyRelease( BYTE nKey ) const;

	private:
	// FUNCTIONS /////////////////////////////////////////////////////////////////////////////
		
		KeyboardDeviceRef& operator= ( KeyboardDeviceRef& Other ) {}

	// VARIABLES /////////////////////////////////////////////////////////////////////////////

		const InputDX* const	m_pInputDevice;
		Log						m_Log;

	};

	/* Class for retrieving mouse data */
	class MouseDeviceRef{
	public:
	// FUNCTIONS /////////////////////////////////////////////////////////////////////////////
		// CONSTRUCTOR AND DESTRUCTOR ////////////////////////////////////////////////////////

		MouseDeviceRef();
		MouseDeviceRef( const InputDX& InputDevice );
		MouseDeviceRef( const MouseDeviceRef& Other );
		~MouseDeviceRef();

		// UTILITIES /////////////////////////////////////////////////////////////////////////

		BOOL isValid() const;
		BOOL isDeviceEnabled() const;

		// GET-FUNCTIONS /////////////////////////////////////////////////////////////////////

		BOOL ButtonDown(BYTE nButton) const;
		BOOL ButtonPress(BYTE nButton) const;
		BOOL ButtonRelease(BYTE nButton) const;

		LONG DeltaX() const;
		LONG DeltaY() const;
		LONG DeltaZ() const;

	private:
	// FUNCTIONS /////////////////////////////////////////////////////////////////////////////
	
		MouseDeviceRef& operator=( const MouseDeviceRef& Other ) {}

	// VARIABLES /////////////////////////////////////////////////////////////////////////////

		const InputDX* const	m_pInputDevice;
		Log						m_Log;
	};

	///// EXCEPTIONS /////
	class InvalidKeyboardDevice: public Ludos::Exception
	{
	public:
		InvalidKeyboardDevice( const std::string& szPlace )
		: Exception( "Tried to access data from an invalid keyboard device.", szPlace )
		{}
	};
	class InvalidMouseDevice: public Ludos::Exception
	{
	public:
		InvalidMouseDevice( const std::string& szPlace )
		: Exception( "Tried to access data from an invalid mouse device.", szPlace )
		{}
	};

}	// InputDXUtilities namespace

}  // Ludos namespace

#endif
