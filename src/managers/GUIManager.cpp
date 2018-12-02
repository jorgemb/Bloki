#include "GUIManager.h"


///
// GUIObject
///

GUIObject::GUIObject() :
m_pRenderer( NULL ),
m_Hotspot( 0, 0, 0, 0 )
{
}

GUIObject::~GUIObject()
{
}

VOID GUIObject::Submit()
{
	// Submit self to the renderer
	if( m_pRenderer )
		m_pRenderer->addRenderObj( this );
}

VOID GUIObject::onClic()
{
	return;
}

VOID GUIObject::onRollover()
{
	return;
}

VOID GUIObject::setHotspot( const Gdiplus::Rect &newHotspot )
{
	m_Hotspot = newHotspot;
}

Gdiplus::Rect GUIObject::getHotspot()
{
	return m_Hotspot;
}

///
// GUIManager
///

GUIManager::GUIManager( Renderer *pRenderer, Ludos::InputDXUtilities::MouseDeviceRef Mouse ):
m_pRenderer( pRenderer ),
m_Mouse( Mouse )
{
	if( !pRenderer )
		throw Ludos::Exception( "Invalid renderer.", "GUIManager" );
}

GUIManager::~GUIManager()
{
	for( iterObjects pIter = m_Objects.begin(); pIter != m_Objects.end(); ++pIter )
	{
		GUIObject* currentObj = *pIter;
		S_DELETE( currentObj );
	}
}

BOOL GUIManager::addObject( GUIObject *newObject )
{
	// Validate
	if( !newObject )
		return FALSE;

	newObject->m_pRenderer = m_pRenderer;
	m_Objects.push_back( newObject );

	return TRUE;
}

BOOL GUIManager::Update( HWND hWindow )
{
	if( m_Objects.empty() )
		return TRUE;

	// Retrieve Client area and mouse position
	GetClientRect( hWindow, &m_ClientRect );
	GetCursorPos( &m_MousePos );
	ScreenToClient( hWindow, &m_MousePos );

	// Check mouse position on every GUI object
	for( iterObjects pIter = m_Objects.begin(); pIter != m_Objects.end(); ++pIter )
	{
		GUIObject* currentObject = *pIter;
		if( mouseOnObject( currentObject ) )
		{
			if( m_Mouse.ButtonPress( Ludos::MOUSE_LEFT ) )
				currentObject->onClic();
			else
				currentObject->onRollover();
		}

		// Submit buttons to the renderer
		currentObject->Submit();
	}

	return TRUE;
}

BOOL GUIManager::mouseOnObject( GUIObject *Object )
{
	// Validate
	if( !Object )
		return FALSE;

	// Check if the mouse is inside the hotspot of the object
	Gdiplus::Rect Hotspot = Object->getHotspot();

	if( ( ( m_MousePos.x >= Hotspot.X ) && ( m_MousePos.x < ( Hotspot.X + Hotspot.Width ) ) ) &&
		( ( m_MousePos.y >= Hotspot.Y ) && ( m_MousePos.y < ( Hotspot.Y + Hotspot.Height ) ) ) ) {
		return TRUE;
	}
	else
		return FALSE;
}
