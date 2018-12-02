#ifndef GUIMANAGER_H
#define GUIMANAGER_H

#include <vector>

#include "Window.h"
#include "Input.h"
#include "Exception.h"
#include "Renderer.h"

class GUIObject: public RenderObj{
public:
// FUNCTIONS /////////////////////////////////////////////////////////////////////////////
	// CONSTRUCTOR AND DESTRUCTOR ////////////////////////////////////////////////////////
	GUIObject();
	virtual ~GUIObject();

	// UTILITIES /////////////////////////////////////////////////////////////////////////

	virtual BOOL Draw( Gdiplus::Graphics* pGraphics )= 0;
	VOID Submit();

	virtual VOID onClic();
	virtual VOID onRollover();

	// SET-FUNCTIONS /////////////////////////////////////////////////////////////////////

	VOID setHotspot( const Gdiplus::Rect& newHotspot );

	// GET-FUNCTIONS /////////////////////////////////////////////////////////////////////

	Gdiplus::Rect getHotspot();

protected:
// VARIABLES /////////////////////////////////////////////////////////////////////////////
	Gdiplus::Rect m_Hotspot;

private:
// VARIABLES /////////////////////////////////////////////////////////////////////////////
	friend class	GUIManager;
	Renderer*		m_pRenderer;
};

class GUIManager{
public:
// FUNCTIONS /////////////////////////////////////////////////////////////////////////////
	// CONSTRUCTOR AND DESTRUCTOR ////////////////////////////////////////////////////////

	GUIManager( Renderer* pRenderer, Ludos::InputDXUtilities::MouseDeviceRef inMouse );
	~GUIManager();

	// UTILITIES /////////////////////////////////////////////////////////////////////////

	BOOL addObject( GUIObject* newObject );
	BOOL Update( HWND hWindow );

private:
// FUNCTION //////////////////////////////////////////////////////////////////////////////

	BOOL mouseOnObject( GUIObject* Object );

// VARIABLES /////////////////////////////////////////////////////////////////////////////

	typedef std::vector<GUIObject*>			ObjectsArray;
	typedef ObjectsArray::iterator			iterObjects;
	typedef ObjectsArray::const_iterator	citerObjects;
	typedef ObjectsArray::size_type			ObjectsSize;

	Renderer*									m_pRenderer;
	ObjectsArray								m_Objects;

	Ludos::InputDXUtilities::MouseDeviceRef		m_Mouse;
	RECT										m_ClientRect;
	POINT										m_MousePos;
};


#endif // GUIMANAGER_H
