#include "Input.h"
#include "Utility.h"

//////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////// InputDX : IMPLEMENTATION /////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------------------
/*  Function	:	InputDX (constructor)
*	Description	:	Set's everything to the default value.
*
*	Author		:	Jorge Luis Martínez
*	Created on	:	2006
*/	
//----------------------------------------------------------------------------------------
Ludos::InputDX::InputDX( const std::string& szName ) :
	// INITIALIZING LIST //
	m_Log(szName),
	/* FLAG values */
	m_bKeyEnabled(TRUE),
	m_bMouseEnabled(TRUE),
	/* NULL pointers */
	m_pDInput(NULL),
	m_pDIKeyboard(NULL),
	m_pDIMouse(NULL),
	/* Default values */
	m_pKeyInputNow(m_pKeyInput1),
	m_pKeyInputLast(m_pKeyInput2),
	m_pMouseInputNow(&m_sMouseInput1),
	m_pMouseInputLast(&m_sMouseInput2)
{
	// Zero out everything
	ZeroMemory( m_pKeyInput1, sizeof(BYTE)*256 );
	ZeroMemory( m_pKeyInput2, sizeof(BYTE)*256 );

	ZeroMemory( &m_sMouseInput1, sizeof(DIMOUSESTATE) );
	ZeroMemory( &m_sMouseInput2, sizeof(DIMOUSESTATE) );
}

//----------------------------------------------------------------------------------------
/*  Function	:	~InputDX (destructor)
*
*	Author		:	Jorge Luis Martínez
*	Created on	:	2006
*/	
//----------------------------------------------------------------------------------------
Ludos::InputDX::~InputDX()
{
	Release();
}

//----------------------------------------------------------------------------------------
/*  Function	:	Initialize
*
*	Author		:	Jorge Luis Martínez
*	Created on	:	2006
*/	
//----------------------------------------------------------------------------------------
BOOL Ludos::InputDX::Initialize( HWND in_hWindow,
								  BOOL in_bExclusive,
								  BOOL in_bForeground,
								  HRESULT* out_pResult )
{
	HRESULT result = 0;

	// Exclusive flags
	std::string szCoopLvlString;
	DWORD dCoopLvlFlags;

	if( in_bForeground ){
		dCoopLvlFlags = DISCL_FOREGROUND;
		szCoopLvlString = "FOREGROUND and ";
	} else {
		dCoopLvlFlags = DISCL_BACKGROUND;
		szCoopLvlString = "BACKGROUND and ";
	}

	if ( in_bExclusive )
	{
		dCoopLvlFlags |= DISCL_EXCLUSIVE;
		szCoopLvlString += "EXCLUSIVE";
	} else {
		dCoopLvlFlags |= DISCL_NONEXCLUSIVE;
		szCoopLvlString += "NON-EXCLUSIVE";
	}


	/* Start the block */
	LogUtilities::LogBlock Block( "Initializing", m_Log );

	// Check that the user didn't request a Exclusive and Background device, because
	// that's illegal
	if( in_bExclusive && !in_bForeground )
	{
		m_Log.Error( "Can't create an exclusive device in background mode." );

		fillResultParameter( E_FAIL, out_pResult );
		return FALSE;
	}
	m_Log.Write( "Creating " + szCoopLvlString + " input Devices." );

	// First create the main interface pointer
	result = createInterface();
	if( FAILED(result) )
	{
		Release();

		fillResultParameter( result, out_pResult );
		return FALSE;
	}


	/// KEYBOARD ///
	result = initKeyboard( in_hWindow, dCoopLvlFlags );
	if( FAILED(result) )
	{
		Release();

		fillResultParameter( result, out_pResult );
		return FALSE;
	}

	/// MOUSE ///
	result = initMouse( in_hWindow, dCoopLvlFlags );
	if( FAILED(result) )
	{
		Release();

		fillResultParameter( result, out_pResult );
		return FALSE;
	}
	

	fillResultParameter( S_OK, out_pResult );
	return TRUE;
}

//----------------------------------------------------------------------------------------
/*  Function	:	Release
*
*	Author		:	Jorge Luis Martínez
*	Created on	:	2006
*/	
//----------------------------------------------------------------------------------------
VOID Ludos::InputDX::Release()
{
	// First the mouse
	if (m_pDIMouse != NULL)
	{
		m_pDIMouse->Unacquire();
		m_pDIMouse->Release();
	}
	m_pDIMouse = NULL;

	// Now the keyboard
	if (m_pDIKeyboard != NULL)
	{
		m_pDIKeyboard->Unacquire();
		m_pDIKeyboard->Release();
	}
	m_pDIKeyboard = NULL;

	// Last, the interface
	S_RELEASE( m_pDInput );
}

//----------------------------------------------------------------------------------------
/*  Function	:	updateInput
*	Description	:	It will adquire the device each time is called.
*
*	Author		:	Jorge Luis Martínez
*	Created on	:	2006
*/	
//----------------------------------------------------------------------------------------
VOID Ludos::InputDX::updateInput()
{
	// Check for a valid interface
	if ( m_pDInput == NULL )
		return;

	HRESULT result = 0;
	

	/// KEYBOARD ///
	// Check that the keyboard is enabled
	if ( m_bKeyEnabled == TRUE ) {
		// Acquire
		result = m_pDIKeyboard->Acquire();
		if ( FAILED(result) )
		{
			// In case of failure, clear all the input
			ZeroMemory( m_pKeyInput1, 256 );
			ZeroMemory( m_pKeyInput2, 256 );
		} else {
			Key_swapPointers();

			result = m_pDIKeyboard->GetDeviceState( 256, m_pKeyInputNow );
			if ( FAILED(result) )
				// On failure, asume no input
				ZeroMemory(m_pKeyInputNow, 256);
		}
	}

	/// MOUSE ///
	// Check that the mouse is enabled
	if ( m_bMouseEnabled == TRUE ){
		// Acquire
		result = m_pDIMouse->Acquire();
		if ( FAILED(result) )
		{
			// On failure, clear the entire input
			ZeroMemory( &m_sMouseInput1, sizeof(DIMOUSESTATE) );
			ZeroMemory( &m_sMouseInput2, sizeof(DIMOUSESTATE) );
		} else {
			Mouse_swapPointers();

			result = m_pDIMouse->GetDeviceState( sizeof(DIMOUSESTATE), 
						(LPVOID)m_pMouseInputNow );
			if ( FAILED(result) )
				// On failure, asume no input
				ZeroMemory( m_pMouseInputNow, sizeof(DIMOUSESTATE) );
		}
	}

	return;
}

// ENABLED && DISABLED ///////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------------------
/*  Function	:	enableDevice
*
*	Author		:	Jorge Luis Martínez
*	Created on	:	2006
*/	
//----------------------------------------------------------------------------------------
VOID Ludos::InputDX::enableDevice( INPUT_ENABLE Device )
{
	switch ( Device ){
		case IN_ALL :
			m_bKeyEnabled = m_bMouseEnabled = TRUE;
			break;
		case IN_KEYBOARD :
			m_bKeyEnabled = TRUE;
			break;
		case IN_MOUSE :
			m_bMouseEnabled = TRUE;
			break;
	}
}

//----------------------------------------------------------------------------------------
/*  Function	:	disableDevice
*
*	Author		:	Jorge Luis Martínez
*	Created on	:	2006
*/	
//----------------------------------------------------------------------------------------
VOID Ludos::InputDX::disableDevice( INPUT_ENABLE Device )
{
	switch ( Device ){
		case IN_ALL :
			m_bKeyEnabled = m_bMouseEnabled = FALSE;
			break;
		case IN_KEYBOARD :
			m_bKeyEnabled = FALSE;
			break;
		case IN_MOUSE :
			m_bMouseEnabled = FALSE;
			break;
	}

	// Clear the input
	// Keyboard
	if ( m_bKeyEnabled == FALSE )
	{
		ZeroMemory(m_pKeyInput1, 256);
		ZeroMemory(m_pKeyInput2, 256);
		// Unacquire
		if (m_pDIKeyboard != NULL)
			m_pDIKeyboard->Unacquire();
	}
	// Mouse
	if ( m_bMouseEnabled == FALSE )
	{
		// Clear the input
		ZeroMemory(&m_sMouseInput1, sizeof(DIMOUSESTATE));
		ZeroMemory(&m_sMouseInput2, sizeof(DIMOUSESTATE));
		// Unacquire
		if (m_pDIMouse != NULL)
			m_pDIMouse->Unacquire();
	}
}

//----------------------------------------------------------------------------------------
/*  Function	:	isDeviceEnabled
*
*	Author		:	Jorge Luis Martínez
*	Created on	:	2006
*/	
//----------------------------------------------------------------------------------------
BOOL Ludos::InputDX::isDeviceEnabled( INPUT_ENABLE Device ) const
{
	switch ( Device ){
		case IN_ALL :
			if (m_bKeyEnabled == TRUE && m_bMouseEnabled == TRUE)
				return TRUE;
			break;
		case IN_KEYBOARD :
			if (m_bKeyEnabled == TRUE)
				return TRUE;
			break;
		case IN_MOUSE :
			if (m_bMouseEnabled == TRUE)
				return TRUE;
			break;
	}

	return FALSE;
}

//----------------------------------------------------------------------------------------
/*  Function	:	isValid																*/	
//----------------------------------------------------------------------------------------
BOOL Ludos::InputDX::isValid() const
{
	if( m_pDInput && m_pDIKeyboard && m_pDIMouse )
		return TRUE;
	else
		return FALSE;
}

// KEYBOARD INPUT ////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------------------
/*  Function	:	KeyDown
*	Description	:	It can be used as "KeyUp" by just negating the return value.
*
*	Author		:	Jorge Luis Martínez
*	Created on	:	2006
*/	
//----------------------------------------------------------------------------------------
BOOL Ludos::InputDX::KeyDown( BYTE nKey ) const
{
	if ( m_pKeyInputNow[nKey] & 0x80 )
		return TRUE;
	else
		return FALSE;
}

//----------------------------------------------------------------------------------------
/*  Function	:	KeyPress
*	Description	:	Return TRUE if the given key is being pressed on this only frame. Will
*	return FALSE otherwise.
*
*	Author		:	Jorge Luis Martínez
*	Created on	:	2006
*/	
//----------------------------------------------------------------------------------------
BOOL Ludos::InputDX::KeyPress (BYTE nKey ) const
{
	// In order to check this, the key must be true in the "now" buffer and 
	// must be false in "last" buffer

	if ( ( m_pKeyInputNow[nKey] & 0x80 ) && !( m_pKeyInputLast[nKey] & 0x80 ) )
		return TRUE;
	else
		return FALSE;
}

//----------------------------------------------------------------------------------------
/*  Function	:	KeyRelease
*	Description	:	Returns TRUE if a given key is being released on this only frame.
*
*	Author		:	Jorge Luis Martínez
*	Created on	:	2006
*/	
//----------------------------------------------------------------------------------------
BOOL Ludos::InputDX::KeyRelease( BYTE nKey ) const
{
	// In order to check this, the key must be false in the "now" buffer and
	// must be true in the "last" buffer

	if ( !( m_pKeyInputNow[nKey] & 0x80 ) && ( m_pKeyInputLast[nKey] & 0x80 ) )
		return TRUE;
	else
		return FALSE;
}

//----------------------------------------------------------------------------------------
/*  Function	:	KeyToStr															*/	
//----------------------------------------------------------------------------------------
std::string Ludos::InputDX::KeyToStr( BYTE key )
{
	switch ( key ){
		case DIK_0:
			return "0";
		case DIK_1:
			return "1";
		case DIK_2:
			return "2";
		case DIK_3:
			return "3";
		case DIK_4:
			return "4";
		case DIK_5:
			return "5";
		case DIK_6:
			return "6";
		case DIK_7:
			return "7";
		case DIK_8:
			return "8";
		case DIK_9:
			return "9";
		case DIK_A:
			return "A";
		case DIK_ABNT_C1:
			return "ABNT_C1";
		case DIK_ABNT_C2:
			return "ABNT_C2";
		case DIK_ADD:
			return "ADD";
		case DIK_APOSTROPHE:
			return "APOSTROPHE";
		case DIK_APPS:
			return "APPS";
		case DIK_AT:
			return "AT";
		case DIK_AX:
			return "AX";
		case DIK_B:
			return "B";
		case DIK_BACK:
			return "BACK";
		case DIK_BACKSLASH:
			return "BACKSLASH";
		case DIK_C:
			return "C";
		case DIK_CALCULATOR:
			return "CALCULATOR";
		case DIK_CAPITAL:
			return "CAPSLOCK";
		case DIK_COLON:
			return "COLON";
		case DIK_COMMA:
			return "COMMA";
		case DIK_CONVERT:
			return "CONVERT";
		case DIK_D:
			return "D";
		case DIK_DECIMAL:
			return "DECIMAL";
		case DIK_DELETE:
			return "DELETE";
		case DIK_DIVIDE:
			return "DIVIDE";
		case DIK_DOWN:
			return "DOWN";
		case DIK_E:
			return "E";
		case DIK_END:
			return "END";
		case DIK_EQUALS:
			return "EQUALS";
		case DIK_ESCAPE:
			return "ESCAPE";
		case DIK_F:
			return "F";
		case DIK_F1:
			return "F1";
		case DIK_F2:
			return "F2";
		case DIK_F3:
			return "F3";
		case DIK_F4:
			return "F4";
		case DIK_F5:
			return "F5";
		case DIK_F6:
			return "F6";
		case DIK_F7:
			return "F7";
		case DIK_F8:
			return "F8";
		case DIK_F9:
			return "F9";
		case DIK_F10:
			return "F10";
		case DIK_F11:
			return "F11";
		case DIK_F12:
			return "F12";
		case DIK_F13:
			return "F13";
		case DIK_F14:
			return "F14";
		case DIK_F15:
			return "F15";
		case DIK_G:
			return "G";
		case DIK_GRAVE:
			return "GRAVE";
		case DIK_H:
			return "H";
		case DIK_HOME:
			return "HOME";
		case DIK_I:
			return "I";
		case DIK_INSERT:
			return "INSERT";
		case DIK_J:
			return "J";
		case DIK_K:
			return "K";
		case DIK_KANA:
			return "KANA";
		case DIK_KANJI:
			return "KANJI";
		case DIK_L:
			return "L";
		case DIK_LBRACKET:
			return "LBRACKET";
		case DIK_LCONTROL:
			return "LCONTROL";
		case DIK_LEFT:
			return "LEFT";
		case DIK_LMENU:
			return "LMENU";
		case DIK_LSHIFT:
			return "LSHIFT";
		case DIK_LWIN:
			return "LWIN";
		case DIK_M:
			return "M";
		case DIK_MAIL:
			return "MAIL";
		case DIK_MEDIASELECT:
			return "MEDIASELECT";
		case DIK_MEDIASTOP:
			return "MEDIASTOP";
		case DIK_MINUS:
			return "MINUS";
		case DIK_MULTIPLY:
			return "MULTIPLY";
		case DIK_MUTE:
			return "MUTE";
		case DIK_MYCOMPUTER:
			return "MYCOMPUTER";
		case DIK_N:
			return "N";
		case DIK_NEXT:
			return "NEXT";
		case DIK_NEXTTRACK:
			return "NEXTTRACK";
		case DIK_NOCONVERT:
			return "NOCONVERT";
		case DIK_NUMLOCK:
			return "NUMLOCK";
		case DIK_NUMPAD0:
			return "NUMPAD0";
		case DIK_NUMPAD1:
			return "NUMPAD1";
		case DIK_NUMPAD2:
			return "NUMPAD2";
		case DIK_NUMPAD3:
			return "NUMPAD3";
		case DIK_NUMPAD4:
			return "NUMPAD4";
		case DIK_NUMPAD5:
			return "NUMPAD5";
		case DIK_NUMPAD6:
			return "NUMPAD6";
		case DIK_NUMPAD7:
			return "NUMPAD7";
		case DIK_NUMPAD8:
			return "NUMPAD8";
		case DIK_NUMPAD9:
			return "NUMPAD9";
		case DIK_NUMPADCOMMA:
			return "NUMPADCOMMA";
		case DIK_NUMPADENTER:
			return "NUMPADENTER";
		case DIK_NUMPADEQUALS:
			return "NUMPADEQUALS";
		case DIK_O:
			return "O";
		case DIK_OEM_102:
			return "OEM_102";
		case DIK_P:
			return "P";
		case DIK_PAUSE:
			return "PAUSE";
		case DIK_PERIOD:
			return "PERIOD";
		case DIK_PLAYPAUSE:
			return "PLAYPAUSE";
		case DIK_POWER:
			return "POWER";
		case DIK_PREVTRACK:
			return "PREVTRACK";
		case DIK_PRIOR:
			return "PRIOR";
		case DIK_Q:
			return "Q";
		case DIK_R:
			return "R";
		case DIK_RBRACKET:
			return "RBRACKET";
		case DIK_RCONTROL:
			return "RCONTROL";
		case DIK_RETURN:
			return "RETURN";
		case DIK_RIGHT:
			return "RIGHT";
		case DIK_RMENU:
			return "RMENU";
		case DIK_RSHIFT:
			return "RSHIFT";
		case DIK_RWIN:
			return "RWIN";
		case DIK_S:
			return "S";
		case DIK_SCROLL:
			return "SCROLL";
		case DIK_SEMICOLON:
			return "SEMICOLON";
		case DIK_SLASH:
			return "SLASH";
		case DIK_SLEEP:
			return "SLEEP";
		case DIK_SPACE:
			return "SPACE";
		case DIK_STOP:
			return "STOP";
		case DIK_SUBTRACT:
			return "SUBTRACT";
		case DIK_SYSRQ:
			return "SYSQR";
		case DIK_T:
			return "T";
		case DIK_TAB:
			return "TAB";
		case DIK_U:
			return "U";
		case DIK_UNDERLINE:
			return "UNDERLINE";
		case DIK_UNLABELED:
			return "UNLABELED";
		case DIK_UP:
			return "UP";
		case DIK_V:
			return "V";
		case DIK_VOLUMEDOWN:
			return "VOLUMEDOWN";
		case DIK_VOLUMEUP:
			return "VOLUMEUP";
		case DIK_W:
			return "W";
		case DIK_WAKE:
			return "WAKE";
		case DIK_WEBBACK:
			return "WEBBACK";
		case DIK_WEBFAVORITES:
			return "WEBFAVORITES";
		case DIK_WEBFORWARD:
			return "WEBFORWARD";
		case DIK_WEBHOME:
			return "WEBHOME";
		case DIK_WEBREFRESH:
			return "WEBREFRESH";
		case DIK_WEBSEARCH:
			return "WEBSEARCH";
		case DIK_WEBSTOP:
			return "WEBSTOP";
		case DIK_X:
			return "X";
		case DIK_Y:
			return "Y";
		case DIK_YEN:
			return "YEN";
		case DIK_Z:
			return "Z";
		default:
			return "UNKNOWN";
	}
}

//----------------------------------------------------------------------------------------
/*  Function	:	Key_swapPointers
*	Description	:	Swaps the Keyboard input buffers.
*
*	Author		:	Jorge Luis Martínez
*	Created on	:	2006
*/	
//----------------------------------------------------------------------------------------
VOID Ludos::InputDX::Key_swapPointers()
{
	// Swap the "now" and "last" buffer pointers
	BYTE *pAux = m_pKeyInputNow;
	m_pKeyInputNow = m_pKeyInputLast;
	m_pKeyInputLast = pAux;
}

// MOUSE INPUT ///////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------------------
/*  Function	:	MouseButtonDown
*
*	Author		:	Jorge Luis Martínez
*	Created on	:	2006
*/	
//----------------------------------------------------------------------------------------
BOOL Ludos::InputDX::MouseButtonDown(BYTE nButton) const
{
	// Check that the button number is not out of range
	if ( nButton > 3 )
		return FALSE;

	// Get the data from the current buffer
	if ( m_pMouseInputNow->rgbButtons[nButton] & 0x80 )
		return TRUE;
	else
		return FALSE;
}

//----------------------------------------------------------------------------------------
/*  Function	:	MouseButtonPress
*	Description	:	Same as KeyPress
*
*	Author		:	Jorge Luis Martínez
*	Created on	:	2006
*/	
//----------------------------------------------------------------------------------------
BOOL Ludos::InputDX::MouseButtonPress( BYTE nButton ) const
{
	// Check that the button number is not out of range
	if (nButton > 3)
		return FALSE;

	// In order to check this, the button must be true in the "now" buffer and 
	// must be false in "last" buffer
	if ( ( m_pMouseInputNow->rgbButtons[nButton] & 0x80 ) &&
		!( m_pMouseInputLast->rgbButtons[nButton] & 0x80 ) )
		return TRUE;
	else
		return FALSE;
}

//----------------------------------------------------------------------------------------
/*  Function	:	MouseButtonRelease
*	Description	:	Same as KeyRelease
*
*	Author		:	Jorge Luis Martínez
*	Created on	:	2006
*/	
//----------------------------------------------------------------------------------------
BOOL Ludos::InputDX::MouseButtonRelease( BYTE nButton ) const
{
	// Check that the button number is not out of range
	if (nButton > 3)
		return FALSE;

	// In order to check this, the button must be false in the "now" buffer and
	// must be true in the "last" buffer
	if ( !( m_pMouseInputNow->rgbButtons[nButton] & 0x80 ) &&
		( m_pMouseInputLast->rgbButtons[nButton] & 0x80 ) )
		return TRUE;
	else
		return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////////////
// These functions return the delta of the mouse position on a given axis.				//
//////////////////////////////////////////////////////////////////////////////////////////
LONG Ludos::InputDX::MouseDeltaX() const
{
	return m_pMouseInputNow->lX;
}
LONG Ludos::InputDX::MouseDeltaY() const
{
	return m_pMouseInputNow->lY;
}
LONG Ludos::InputDX::MouseDeltaZ() const
{
	return m_pMouseInputNow->lZ;
}


//----------------------------------------------------------------------------------------
/*  Function	:	createInterface														*/	
//----------------------------------------------------------------------------------------
HRESULT Ludos::InputDX::createInterface()
{
	HRESULT result = S_OK;
	result = DirectInput8Create (	GetModuleHandle(NULL),	// The HINSTANCE of the program
									DIRECTINPUT_VERSION,	// DInput version
									IID_IDirectInput8,		// REFIID
									(VOID**)&m_pDInput,		// DirectInput interface
									NULL);

	m_Log.ProcWrite( "Create DirectInput 8 interface.", SUCCEEDED(result) );

	return result;
}

//----------------------------------------------------------------------------------------
/*  Function	:	initKeyboard														*/	
//----------------------------------------------------------------------------------------
HRESULT Ludos::InputDX::initKeyboard( HWND hWindow, DWORD Flags )
{
	LogUtilities::LogBlock KeyBlock( "KEYBOARD Device initialization", m_Log );
	
	// Create device
	HRESULT result = S_OK;
	result = m_pDInput->CreateDevice(	GUID_SysKeyboard,	// The GUID of the keyboard
										&m_pDIKeyboard,		// The pointer to the key interfa
										NULL);
	if ( FAILED(result) )
	{
		m_Log.ProcWrite("Create the device. Error code: " + toStr( result ), FALSE);
		Release();

		return result;
	}
	m_Log.ProcWrite("Create the device", TRUE);

	// Data format
	result = m_pDIKeyboard->SetDataFormat( &c_dfDIKeyboard );
	if ( FAILED(result) )
	{
		m_Log.ProcWrite("Set the data format. Error code: " + toStr( result ), FALSE);
		return result;
	}
	m_Log.ProcWrite("Set the data format.", TRUE);

	// Cooperative level, check if exclusive
	result = m_pDIKeyboard->SetCooperativeLevel( hWindow, Flags );
	if ( FAILED(result) )
	{
		m_Log.ProcWrite("Set the cooperative level. Error code: " + toStr( result ), FALSE);
		return result;
	}
	m_Log.ProcWrite("Set the cooperative level.", TRUE);

	return result;
}

//----------------------------------------------------------------------------------------
/*  Function	:	initMouse															*/	
//----------------------------------------------------------------------------------------
HRESULT Ludos::InputDX::initMouse( HWND hWindow, DWORD Flags )
{
	LogUtilities::LogBlock MouseBlock( "MOUSE Device initialization", m_Log );

	// Now go on with the mouse device interface
	HRESULT result = S_OK;
	result = m_pDInput->CreateDevice(	GUID_SysMouse,
										&m_pDIMouse,
										NULL);
	if ( FAILED(result) )
	{
		m_Log.ProcWrite("Create the Device. Error code: " + toStr( result ), FALSE);
		return result;
	}
	m_Log.ProcWrite("Create the Device.", TRUE);

	// Data format
	result = m_pDIMouse->SetDataFormat( &c_dfDIMouse );
	if ( FAILED(result) )
	{
		m_Log.ProcWrite("Set the data format. Error code: " + toStr( result ), FALSE);
		return result;
	}
	m_Log.ProcWrite("Set the data format.", TRUE);

	// Cooperative level
	result = m_pDIMouse->SetCooperativeLevel( hWindow, Flags );
	if ( FAILED(result) )
	{
		m_Log.ProcWrite("Set the cooperative level. Error code: " + toStr( result ), FALSE);
		return result;
	}
	m_Log.ProcWrite("Set the cooperative level.", TRUE);

	return result;
}

//----------------------------------------------------------------------------------------
/*  Function	:	Mouse_swapPointers
*	Description	:	Swaps the Mouse input buffers.
*
*	Author		:	Jorge Luis Martínez
*	Created on	:	2006
*/	
//----------------------------------------------------------------------------------------
VOID Ludos::InputDX::Mouse_swapPointers()
{
	// Swap the "now" and "last" input buffers
	LPDIMOUSESTATE pAux = m_pMouseInputNow;
	m_pMouseInputNow = m_pMouseInputLast;
	m_pMouseInputLast = pAux;
}

//----------------------------------------------------------------------------------------
/*  Function	:	getVersionString
*	Description	:	Returns the version string.
*
*	Author		:	Jorge Luis Martínez
*	Created on	:	2006
*/	
//----------------------------------------------------------------------------------------
std::string Ludos::InputDX::getVersionString() const
{
	const int nBufferSize = 256;
	char* szBuffer = new CHAR[nBufferSize];

	sprintf_s( szBuffer, nBufferSize, "InputDX Version : %.1f", InputDX_VERSION / 10.0f );
	
	std::string szVersion = szBuffer;
	S_DELETE( szBuffer );

	return szVersion;
}


//////////////////////////////////////////////////////////////////////////////////////////
////////////////////// KeyboardDeviceRef :: IMPLEMENTATION ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------------------
/*  Function	:	KeyboardDeviceRef (default constructor)								*/	
//----------------------------------------------------------------------------------------
Ludos::InputDXUtilities::KeyboardDeviceRef::KeyboardDeviceRef()
: m_pInputDevice( NULL ), m_Log( "KeyboardDeviceRef" )
{
}

//----------------------------------------------------------------------------------------
/*  Function	:	KeyboardDeviceRef (init constructor)								*/	
//----------------------------------------------------------------------------------------
Ludos::InputDXUtilities::KeyboardDeviceRef::KeyboardDeviceRef( const Ludos::InputDX &InputDevice )
: m_pInputDevice( &InputDevice ), m_Log( "KeyboardDeviceRef" )
{
}

//----------------------------------------------------------------------------------------
/*  Function	:	KeyboardDeviceRef (copy constructor)								*/	
//----------------------------------------------------------------------------------------
Ludos::InputDXUtilities::KeyboardDeviceRef::KeyboardDeviceRef( 
	const Ludos::InputDXUtilities::KeyboardDeviceRef& Other )
	: m_pInputDevice( Other.m_pInputDevice ), m_Log( "KeyboardDeviceRef" )
{
}

//----------------------------------------------------------------------------------------
/*  Function	:	~KeyboardDeviceRef													*/	
//----------------------------------------------------------------------------------------
Ludos::InputDXUtilities::KeyboardDeviceRef::~KeyboardDeviceRef()
{
}

//----------------------------------------------------------------------------------------
/*  Function	:	isValid																*/	
//----------------------------------------------------------------------------------------
BOOL Ludos::InputDXUtilities::KeyboardDeviceRef::isValid() const
{
	if( m_pInputDevice != NULL && m_pInputDevice->isValid() )
		return TRUE;
	else
		return FALSE;
}

//----------------------------------------------------------------------------------------
/*  Function	:	isDeviceEnabled														*/	
//----------------------------------------------------------------------------------------
BOOL Ludos::InputDXUtilities::KeyboardDeviceRef::isDeviceEnabled() const
{
	if( isValid() )
		return m_pInputDevice->isDeviceEnabled( Ludos::IN_KEYBOARD );
	else
		return FALSE;
}

//----------------------------------------------------------------------------------------
/*  Function	:	KeyDown																*/	
//----------------------------------------------------------------------------------------
BOOL Ludos::InputDXUtilities::KeyboardDeviceRef::KeyDown( BYTE nKey ) const
{
	if( isValid() )
		return m_pInputDevice->KeyDown( nKey );
	else
		throw InvalidKeyboardDevice( m_Log.GetName() );
}

//----------------------------------------------------------------------------------------
/*  Function	:	KeyPress															*/	
//----------------------------------------------------------------------------------------
BOOL Ludos::InputDXUtilities::KeyboardDeviceRef::KeyPress( BYTE nKey ) const
{
	if( isValid() )
		return m_pInputDevice->KeyPress( nKey );
	else
		throw InvalidKeyboardDevice( m_Log.GetName() );
}

//----------------------------------------------------------------------------------------
/*  Function	:	KeyRelease															*/	
//----------------------------------------------------------------------------------------
BOOL Ludos::InputDXUtilities::KeyboardDeviceRef::KeyRelease( BYTE nKey ) const
{
	if( isValid() )
		return m_pInputDevice->KeyRelease( nKey );
	else
		throw InvalidKeyboardDevice( m_Log.GetName() );
}


//////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////MouseDeviceRef :: IMPLEMENTATION ////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------------------
/*  Function	:	MouseDeviceRef (default constructor)								*/	
//----------------------------------------------------------------------------------------
Ludos::InputDXUtilities::MouseDeviceRef::MouseDeviceRef()
: m_pInputDevice( NULL ), m_Log( "MouseDeviceRef" )
{
}

//----------------------------------------------------------------------------------------
/*  Function	:	MouseDeviceRef (init constructor)									*/	
//----------------------------------------------------------------------------------------
Ludos::InputDXUtilities::MouseDeviceRef::MouseDeviceRef( const InputDX& InputDevice )
: m_pInputDevice( &InputDevice ), m_Log( "MouseDeviceRef" )
{
}

//----------------------------------------------------------------------------------------
/*  Function	:	MouseDeviceRef (copy constructor)									*/	
//----------------------------------------------------------------------------------------
Ludos::InputDXUtilities::MouseDeviceRef::MouseDeviceRef(
	const Ludos::InputDXUtilities::MouseDeviceRef &Other )
	: m_pInputDevice( Other.m_pInputDevice ), m_Log( "MouseDeviceRef" )
{
}

//----------------------------------------------------------------------------------------
/*  Function	:	~MouseDeviceRef														*/	
//----------------------------------------------------------------------------------------
Ludos::InputDXUtilities::MouseDeviceRef::~MouseDeviceRef()
{
}

//----------------------------------------------------------------------------------------
/*  Function	:	isValid																*/	
//----------------------------------------------------------------------------------------
BOOL Ludos::InputDXUtilities::MouseDeviceRef::isValid() const
{
	if( m_pInputDevice != NULL && m_pInputDevice->isValid() )
		return TRUE;
	else
		return FALSE;
}

//----------------------------------------------------------------------------------------
/*  Function	:	isDeviceEnabled														*/	
//----------------------------------------------------------------------------------------
BOOL Ludos::InputDXUtilities::MouseDeviceRef::isDeviceEnabled() const
{
	if( isValid() )
		return m_pInputDevice->isDeviceEnabled( Ludos::IN_MOUSE );
	else
		return FALSE;
}

//----------------------------------------------------------------------------------------
/*  Function	:	ButtonDown															*/	
//----------------------------------------------------------------------------------------
BOOL Ludos::InputDXUtilities::MouseDeviceRef::ButtonDown( BYTE nButton ) const
{
	if( isValid() )
		return m_pInputDevice->MouseButtonDown( nButton );
	else
		throw InvalidMouseDevice( m_Log.GetName() );
}

//----------------------------------------------------------------------------------------
/*  Function	:	ButtonPress															*/	
//----------------------------------------------------------------------------------------
BOOL Ludos::InputDXUtilities::MouseDeviceRef::ButtonPress( BYTE nButton ) const
{
	if( isValid() )
		return m_pInputDevice->MouseButtonPress( nButton );
	else
		throw InvalidMouseDevice( m_Log.GetName() );
}

//----------------------------------------------------------------------------------------
/*  Function	:	ButtonRelease														*/	
//----------------------------------------------------------------------------------------
BOOL Ludos::InputDXUtilities::MouseDeviceRef::ButtonRelease( BYTE nButton ) const
{
	if( isValid() )
		return m_pInputDevice->MouseButtonRelease( nButton );
	else
		throw InvalidMouseDevice( m_Log.GetName() );
}

//----------------------------------------------------------------------------------------
/*  Function	:	DeltaX																*/	
//----------------------------------------------------------------------------------------
LONG Ludos::InputDXUtilities::MouseDeviceRef::DeltaX() const
{
	if( isValid() )
		return m_pInputDevice->MouseDeltaX();
	else
		throw InvalidMouseDevice( m_Log.GetName() );
}

//----------------------------------------------------------------------------------------
/*  Function	:	DeltaY																*/	
//----------------------------------------------------------------------------------------
LONG Ludos::InputDXUtilities::MouseDeviceRef::DeltaY() const
{
	if( isValid() )
		return m_pInputDevice->MouseDeltaY();
	else
		throw InvalidMouseDevice( m_Log.GetName() );
}

//----------------------------------------------------------------------------------------
/*  Function	:	DeltaZ																*/	
//----------------------------------------------------------------------------------------
LONG Ludos::InputDXUtilities::MouseDeviceRef::DeltaZ() const
{
	if( isValid() )
		return m_pInputDevice->MouseDeltaZ();
	else
		throw InvalidMouseDevice( m_Log.GetName() );
}

