#include "Window.h"

// FUNCTIONS /////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------------------
/*  Function	:	LoadResourceString													*/	
//----------------------------------------------------------------------------------------
std::string Ludos::LoadResourceString( HINSTANCE hInstance, UINT nStringID )
{
	static CHAR szBuffer[1024];

	if( LoadString( hInstance, nStringID, (LPSTR)&szBuffer, sizeof( szBuffer ) ) != 0 )
		return std::string( szBuffer );
	else
		return "";
}

//////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Window :: IMPLEMENTATION /////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////


	/// STATIC INITIALIZATION ///
Ludos::Window* Ludos::Window::m_pTempClassPointer = NULL;

	// DEFAULT CONSTRUCTOR AND DESTRUCTOR //

//----------------------------------------------------------------------------------------
/*  Function	:	Window (Default constructor)										*/	
//----------------------------------------------------------------------------------------
Ludos::Window::Window( const std::string &szName ) :
	// INITIALIZING LIST
	m_Log( szName ),
	m_nYPOS( WINDOW_DEFAULTPOSX ), m_nXPOS( WINDOW_DEFAULTPOSY ),
	m_nHeight( WINDOW_DEFAULTHEIGHT ), m_nWidth( WINDOW_DEFAULTWIDTH ),
	m_hInstance( GetModuleHandle(NULL) ),
	m_dwWindowStyle( NULL ),
		/* Default flags */
	m_bActive( FALSE ),
	m_bFocus( FALSE ),
	m_bWindowed( TRUE ),
		/* NULL pointers */
	m_hWindow( NULL ),
	m_fpCharInput( NULL )
{
	
}

//----------------------------------------------------------------------------------------
/*  Function	:	~Window															*/	
//----------------------------------------------------------------------------------------
Ludos::Window::~Window()
{
	Release();
}

//----------------------------------------------------------------------------------------
/*  Function	:	Initialize															*/	
//----------------------------------------------------------------------------------------
BOOL Ludos::Window::Initialize( BOOL bWindowed )
{
	/* Initialize the window */
	LogUtilities::LogBlock InitializationBlock( "Initializing", m_Log );

	/* Register the WNDCLASSEX structure to Windows */
	if ( !registerClass() )
		return FALSE;

	// Check is going to be windowed or full screen
	if ( bWindowed )
		m_dwWindowStyle = ( WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_BORDER );
	else
		m_dwWindowStyle = ( WS_VISIBLE | WS_POPUP | WS_SYSMENU );
	
	/* Create the new window */
	if ( !createWindow( m_dwWindowStyle ) )
		return FALSE;

	/* Correct the window size */
	setDimensions( m_nWidth, m_nHeight );
	
	m_Log.ProcWrite( "Initialization", TRUE );
	m_bActive = TRUE;

	return TRUE;
}

//----------------------------------------------------------------------------------------
/*  Function	:	Kill																*/	
//----------------------------------------------------------------------------------------
VOID Ludos::Window::Kill()
{
	m_bActive = FALSE;
}

	// MESSAGE HANDLING //

//----------------------------------------------------------------------------------------
/*  Function	:	Update																*/	
//----------------------------------------------------------------------------------------
BOOL Ludos::Window::Update()
{
	MSG Message;
	if ( PeekMessage( &Message, NULL, 0, 0, PM_REMOVE ) )
	{
		if ( Message.message == WM_QUIT )
		{
			m_Log.Write( "QUIT message received. TERMINATING." );
			Kill();
		}

		// Send the message to the message handler
		TranslateMessage( &Message );
		DispatchMessage( &Message );
			
		return TRUE;
	}
	return FALSE;
}


//----------------------------------------------------------------------------------------
/*  Function	:	Update_Idle															*/	
//----------------------------------------------------------------------------------------
BOOL Ludos::Window::Update_Idle()
{
	MSG Message;

	BOOL bRet = GetMessage( &Message, NULL, 0, 0 );
	if (bRet != 0)
	{
		if (bRet == -1)		// Error while processing the messages
		{
			m_Log.Error( "While processing messages with GetMessage. Terminating." );
			Kill();

			return FALSE;
		} else {
			// Send the message to the message handler
			TranslateMessage( &Message );
			DispatchMessage( &Message );
			return TRUE;
		}
	}

	// This means that no more messages are going to be received
	m_Log.Write( "QUIT message received. Terminating." );
	m_bActive = FALSE;
	return FALSE;
}

//----------------------------------------------------------------------------------------
/*  Function	:	isActive															*/	
//----------------------------------------------------------------------------------------
BOOL Ludos::Window::isActive() const
{
	return m_bActive;
}


// SET- FUNCTIONS ////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------------------
/*  Function	:	setIcon																*/	
//----------------------------------------------------------------------------------------
BOOL Ludos::Window::setIcon( LPCSTR szIconName, BOOL bFromResource )
{
	if ( !bFromResource && strcmp( szIconName, "" ) == 0 )
		return FALSE;

	
	HICON hIcon		= NULL;
	HICON hIconSM	= NULL;
	
	// Load both icons
	if( bFromResource )
	{
		hIcon = LoadIcon( m_hInstance, szIconName );
		hIconSM = LoadIcon( m_hInstance, szIconName );
	}
	else
	{
		hIcon		= (HICON)LoadImage( NULL, szIconName, 
									IMAGE_ICON, 32, 32, LR_LOADFROMFILE );
		hIconSM		= (HICON)LoadImage( NULL, szIconName, 
									IMAGE_ICON, 16, 16, LR_LOADFROMFILE );
	}
		
	// Check for failure
	if ( !hIcon || !hIconSM )
	{
		if( bFromResource )
			m_Log.ProcWrite( "Load resource icon.", FALSE );
		else
			m_Log.ProcWrite( "Load icon: " + toStr( szIconName ), FALSE);
		return FALSE;
	}

	// Everything went ok, so we now set the icons
	SendMessage( m_hWindow, WM_SETICON, ICON_BIG, (LPARAM)hIcon );
	SendMessage( m_hWindow, WM_SETICON, ICON_SMALL, (LPARAM)hIconSM );

	if( bFromResource )
		m_Log.ProcWrite( "Load resource icon.", TRUE );
	else
		m_Log.ProcWrite( "Loaded icon: " + toStr( szIconName ), TRUE );

	return TRUE;
}

//----------------------------------------------------------------------------------------
/*  Function	:	setCursor															*/
/*	Notes		:	For some reason, this function will only fail if an animated cursor	*/
/*					(.ani) is loaded from a resource and the build is in release mode.	*/
//----------------------------------------------------------------------------------------
BOOL Ludos::Window::setCursor( LPCSTR szCursorName, BOOL bFromResource )
{
	if( !bFromResource && strcmp( szCursorName, "" ) == 0 )
		return FALSE;

	// Load cursor
	HCURSOR hCursor = NULL;
	if( bFromResource )
	{
		hCursor = LoadCursor( m_hInstance, szCursorName );
		if( !hCursor )
		{
			m_Log.ProcWrite( "Load resource cursor. Error code: " 
				+ toStr( GetLastError() ), FALSE );
			return FALSE;
		}
	}
	else
	{
		hCursor = LoadCursorFromFile( szCursorName );
		if( !hCursor )
		{
			m_Log.ProcWrite( "Load cursor: " + toStr( szCursorName )
				+ ". Error code: " + toStr( GetLastError() ), FALSE );
			return FALSE;
		}
	}

	// Set cursor
	SetClassLong( m_hWindow, GCL_HCURSOR, (LONG)hCursor );

	if( bFromResource )
		m_Log.ProcWrite( "Set resource cursor.", TRUE );
	else
		m_Log.ProcWrite( "Set cursor: " + toStr( szCursorName ), TRUE );

	return TRUE;
}

//----------------------------------------------------------------------------------------
/*  Function	:	setCaption															*/	
//----------------------------------------------------------------------------------------
BOOL Ludos::Window::setCaption( const std::string &szText )
{
	return SetWindowText( m_hWindow, szText.c_str() );
}

//----------------------------------------------------------------------------------------
/*  Function	:	setPosition															*/	
//----------------------------------------------------------------------------------------
BOOL Ludos::Window::setPosition( UINT x, UINT y )
{
	if ( !isActive() || !m_bWindowed )
		return FALSE;

	return SetWindowPos( m_hWindow, NULL, x, y, 0, 0, SWP_NOSIZE );
}

//----------------------------------------------------------------------------------------
/*  Function	:	setDimensions														*/	
//----------------------------------------------------------------------------------------
BOOL Ludos::Window::setDimensions( UINT nWidth, UINT nHeight )
{
	if( !isActive() || !m_bWindowed )
		return FALSE;

	BOOL result = SetWindowPos( m_hWindow, NULL, 0, 0, nWidth, nHeight, SWP_NOMOVE );
	if( !result )
	{
		m_Log.ProcWrite( "Make first resize of the window. Error code: " 
			+ toStr( GetLastError() ), FALSE );
		return FALSE;
	}

	RECT ClientArea;
	GetClientRect( m_hWindow, &ClientArea );
	
	UINT newWidth = nWidth + ( nWidth - ClientArea.right );
	UINT newHeight = nHeight + ( nHeight - ClientArea.bottom );

	return SetWindowPos( m_hWindow, NULL, 0, 0, newWidth, newHeight, SWP_NOMOVE );
}

//----------------------------------------------------------------------------------------
/*  Function	:	setCharInputFunction												*/	
//----------------------------------------------------------------------------------------
VOID Ludos::Window::setCharInputFunction( VOID (*fpCharInput)(TCHAR) )
{
	m_fpCharInput = fpCharInput;
}


// GET- FUNCTIONS ////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------------------
/*  Function	:	getX																*/	
//----------------------------------------------------------------------------------------
INT Ludos::Window::getX() const
{
	return m_nXPOS;
}

//----------------------------------------------------------------------------------------
/*  Function	:	getY																*/	
//----------------------------------------------------------------------------------------
INT Ludos::Window::getY() const
{
	return m_nYPOS;
}

//----------------------------------------------------------------------------------------
/*  Function	:	getWidth															*/	
//----------------------------------------------------------------------------------------
UINT Ludos::Window::getWidth() const
{
	return m_nWidth;
}

//----------------------------------------------------------------------------------------
/*  Function	:	getHeight															*/	
//----------------------------------------------------------------------------------------
UINT Ludos::Window::getHeight() const
{
	return m_nHeight;
}

//----------------------------------------------------------------------------------------
/*  Function	:	getFocus															*/	
//----------------------------------------------------------------------------------------		
BOOL Ludos::Window::getFocus() const
{
	return m_bFocus;
}

//----------------------------------------------------------------------------------------
/*  Function	:	getCaption															*/	
//----------------------------------------------------------------------------------------
std::string Ludos::Window::getCaption() const
{
	CONST INT nBufferSize = 256;
	LPSTR szBuffer = new CHAR[nBufferSize];
	GetWindowText( m_hWindow, szBuffer, nBufferSize );

	std::string szCaption = szBuffer;
	S_DELETE( szBuffer );
	
	return szCaption;
}

//----------------------------------------------------------------------------------------
/*  Function	:	getVersionString													*/	
//----------------------------------------------------------------------------------------
std::string Ludos::Window::getVersionString() const
{
	const int nBufferSize = 256;
	char* szBuffer = new char[nBufferSize];

	sprintf_s( szBuffer, 
		nBufferSize, 
		"Window version : %.1f", 
		WINCLASS_VERSION/10.0f );

	std::string szVersion = szBuffer;
	S_DELETE( szBuffer );

	return szVersion;
}

// OPERATORS /////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------------------
/*  Function	:	operator<HWND>														*/	
//----------------------------------------------------------------------------------------
Ludos::Window::operator HWND()
{
	return m_hWindow;
}

// PRIVATE FUNCTIONS /////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------------------
/*  Function	:	MsgHandler															*/	
//----------------------------------------------------------------------------------------
LRESULT Ludos::Window::MsgHandler( HWND hWindow, UINT Msg, WPARAM wParam, LPARAM lParam )
{
	switch ( Msg )
	{
	case WM_ACTIVATE:		// When the window gains or looses focus
			if( LOWORD( wParam ) == WA_INACTIVE )
				m_bFocus = FALSE;
			else
				m_bFocus = TRUE;
			break;
	case WM_MOVE:		// When the window changes it's position

			if( isActive() )
			{
				RECT WindowRect; ZeroMemory( &WindowRect, sizeof(RECT) );
				GetWindowRect( m_hWindow, &WindowRect );

				m_nXPOS = WindowRect.left;
				m_nYPOS = WindowRect.top;
			}
			//if( m_bWindowed )
			//{
			//	/* Use this in windowed mode because the Y part of the position
			//	doesn't take into account the Caption Bar size */
			//	INT CaptionBarSize = GetSystemMetrics( SM_CYCAPTION );

			//	m_nXPOS = LOWORD( lParam );
			//	m_nYPOS = HIWORD( lParam ) - CaptionBarSize;
			//}
			//else
			//{
			//	m_nXPOS = LOWORD( lParam );
			//	m_nYPOS = HIWORD( lParam );
			//}
			break;
	case WM_SIZE:		// When the window changes it's dimensions

			if( isActive() )
			{
				RECT WindowRect; ZeroMemory( &WindowRect, sizeof(RECT) );
				GetWindowRect( m_hWindow, &WindowRect );

				m_nWidth	= WindowRect.right;
				m_nHeight	= WindowRect.bottom;
			}
			//m_nWidth	= LOWORD( lParam );
			//m_nHeight	= HIWORD( lParam );
			break;
	case WM_CHAR:		// Character input
			if ( m_fpCharInput != NULL )
				m_fpCharInput( (TCHAR)wParam );
			break;
	case WM_CLOSE:		// When the user want's to close the aplication
			PostQuitMessage( 0 );
			break;
	case WM_DESTROY:	// The main window was destroyed
			PostQuitMessage( 0 );
			break;
	case WM_QUIT:		// The program has to exit
			m_bActive = FALSE;
			break;
	default:	// Other messages
			return DefWindowProc( hWindow, Msg, wParam, lParam );
	}
	
	return 0;	// Message handled succesful
}


//----------------------------------------------------------------------------------------
/*  Function	:	registerClass														*/	
//----------------------------------------------------------------------------------------
BOOL Ludos::Window::registerClass()
{	

	// Load default resources
	HICON hIcon		=	LoadIcon( NULL, IDI_WINLOGO );
	HICON hIconSM	=	LoadIcon( NULL, IDI_WINLOGO );
	HCURSOR hCursor	=	LoadCursor( NULL, IDC_ARROW );
	std::string szClassName = m_Log.GetName();

	// Fill in the WNDCLASSEX struct
	WNDCLASSEX MainClass;
		ZeroMemory(&MainClass, sizeof(WNDCLASSEX));

	MainClass.cbSize			=	sizeof(WNDCLASSEX);			// Size of the structure
	MainClass.style				=	CS_DBLCLKS | CS_OWNDC |		// Style flags
									CS_HREDRAW | CS_VREDRAW;
	MainClass.lpfnWndProc		=	MsgRouter;					// Function pointer to 
																// the MsgRouter
	MainClass.cbClsExtra		=	0;							// Not used...
	MainClass.cbWndExtra		=	0;							// Not used...
	MainClass.hInstance			=	m_hInstance;				// Handle to the app
																// instance
	MainClass.hIcon				=	hIcon;						// The loaded icon
	MainClass.hIconSm			=	hIconSM;					// The loaded small icon
	MainClass.hCursor			=	hCursor;					// The loaded cursor
	MainClass.hbrBackground		=	(HBRUSH)GetStockObject(BLACK_BRUSH);	// Black brush
	MainClass.lpszMenuName		=	NULL;						// No menus
	MainClass.lpszClassName		=	szClassName.c_str();		// Name of the class

	if ( !RegisterClassEx( &MainClass ) )
	{
		m_Log.ProcWrite( "Register WNDCLASSEX. Error code: " + toStr( GetLastError ), FALSE );
		return FALSE;	// Failed to register
	}
	
	return TRUE;
}

//----------------------------------------------------------------------------------------
/*  Function	:	unregisterClass														*/	
//----------------------------------------------------------------------------------------
BOOL Ludos::Window::unregisterClass()
{	
	std::string szClassName = m_Log.GetName();
	toUpper( szClassName );

	return UnregisterClass( szClassName.c_str(), m_hInstance );
}

//----------------------------------------------------------------------------------------
/*  Function	:	createWindow														*/	
//----------------------------------------------------------------------------------------
BOOL Ludos::Window::createWindow( DWORD dwFlags )
{	
	// Use the temporary pointer
	m_pTempClassPointer = this;
	std::string szClassName = m_Log.GetName();

	// Create the extended styles window
	m_hWindow	= CreateWindowEx(	NULL,     //WS_EX_TOPMOST,
									szClassName.c_str(),
									szClassName.c_str(),
									dwFlags,
									m_nXPOS, m_nYPOS,
									m_nWidth, m_nHeight,
									NULL, 
									NULL,
									m_hInstance, 
									NULL );
	
	if ( !m_hWindow )	// ON FAILURE
	{	
		m_Log.ProcWrite( "Create an extended window. Trying with a normal one. Error code: "
			+ toStr( GetLastError() ), FALSE );

		// DEBUG WINDOW, TRY TO CREATE A NON-EXTENDED STYLES WINDOW //////////////////////
		m_hWindow	= CreateWindow( szClassName.c_str(),
									szClassName.c_str(),
									dwFlags,
									m_nXPOS, m_nYPOS,
									m_nWidth, m_nHeight,
									NULL,
									NULL,
									m_hInstance,
									NULL );

		if ( !m_hWindow )		// Failed completely. TERMINATE
		{
			m_Log.ProcWrite( "Create a normal window. Error code: " 
				+ toStr( GetLastError() ), FALSE );

			m_pTempClassPointer = NULL;
			return FALSE;
		}
	}

	// Register the class' pointer to the window
	m_pTempClassPointer = NULL;
	SetWindowLongPtr( m_hWindow, GWLP_USERDATA, (LONG_PTR)this );
	Update();	// Handle the WM_CREATE msg

	m_Log.ProcWrite( "Create window.", TRUE );

	return TRUE;
}

//----------------------------------------------------------------------------------------
/*  Function	:	destroyWindow														*/	
//----------------------------------------------------------------------------------------
BOOL Ludos::Window::destroyWindow()
{
	return DestroyWindow( m_hWindow );
}

//----------------------------------------------------------------------------------------
/*  Function	:	Release																*/	
//----------------------------------------------------------------------------------------
VOID Ludos::Window::Release()
{
	if ( !destroyWindow() )
		m_Log.Error( "Window not destroyed properly" );

	if ( !unregisterClass() )
		m_Log.Error( "WNDCLASSEX not unregistered from Windows" );
}



//////////////////////////////////////////////////////////////////////////////////////////
// STATIC FUNCTIONS //////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////


//----------------------------------------------------------------------------------------
/*  Function	:	MsgRouter															*/	
//----------------------------------------------------------------------------------------
LRESULT CALLBACK Ludos::Window::MsgRouter( HWND hWindow, UINT Message, 
											 WPARAM wParam, LPARAM lParam )
{
	// Check if the window handle has it's class registered
	Window* WinClass = NULL;

	WinClass = (Window*)::GetWindowLongPtr( hWindow, GWLP_USERDATA );
	if( WinClass == NULL ){
		// Use the temporary pointer
		WinClass = Window::m_pTempClassPointer;
		
		if( WinClass == NULL )
			// We can't do anything...
			return -1;
	}

	// Call the class message handler
	return WinClass->MsgHandler( hWindow, Message, wParam, lParam );
}
