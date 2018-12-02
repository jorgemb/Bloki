#include <string>
#include <objidl.h>
#include <Gdiplus.h>

#include "Ludos.h"
#include "resources.h"

#include "BlokiWindow.h"

#include "Renderer.h"
#include "StateManager.h"
#include "MainState.h"
#include "Baro.h"

// CONSTANTS
CONST INT LangOffset = OFFSET_SPANISH;
CONST BOOL bWindowed = TRUE;

INT WINAPI WinMain( HINSTANCE hInstance,
				    HINSTANCE hPrevInstance,
					LPSTR lpCmdLine,
					INT nShowCmd )
{
	LUDOS_MEM_LEAK_TRACE;

	// INITIALIZE
	/* Log */
	Ludos::LogUtilities::LogFile LogFile( "LOG.txt" );

	/* Windows */
	BlokiWindow Window;
	if( !Window.Initialize( bWindowed ) )
		return -1;
	Window.setIcon( MAKEINTRESOURCE( ICON_MAIN ), TRUE );
	Window.setCaption( Ludos::LoadResourceString( hInstance, ST_CAPTION + LangOffset ) );
	Window.setDimensions( 800, 600 );

	/* Input */
	Ludos::InputDX Input;
	if( !Input.Initialize( Window, FALSE, TRUE ) )
		return -1;

	try{

		/* Renderer */
		Renderer MainRenderer;
		Window.setRenderer( &MainRenderer );

		/* Baro */
		Baro theBaro( &MainRenderer );

		/* State manager */
		StateManager stateManager;
		stateManager.pushState( new MainMenu( &Window, &MainRenderer, &Input ) );

		// MAIN LOOP
		DWORD lastTime = timeGetTime();
		DWORD timeDelta = 0;
		while( Window.isActive() && stateManager.isValid() )
		{
			if( Window.getFocus() )
			{
				// Update Devices
				while( Window.Update() );
				Input.updateInput();

				DWORD thisTime = timeGetTime();
				timeDelta = thisTime - lastTime;
				lastTime = thisTime;

				// Update Baro
				theBaro.Update( timeDelta );

				// Update states
				stateManager.Update( timeDelta );

				// Update window
				InvalidateRect( Window, NULL, FALSE );

				//// Manage input
				//if( Input.KeyDown( DIK_ESCAPE ) )
				//	Window.Kill();
			}
			else
			{
				Window.Update_Idle();
				lastTime = timeGetTime();
			}
		}
	} catch( Ludos::Exception e ) {
		Ludos::Log ErrorLog( e.Place );
		ErrorLog.Error( e.Problem );

		MessageBox( Window, e.Problem.c_str(), e.Place.c_str(), MB_ICONWARNING | MB_OK );
	}

	return 0;
}