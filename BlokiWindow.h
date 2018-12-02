#ifndef BLOKIWINDOW_H
#define BLOKIWINDOW_H

#include "Window.h"
#include <objidl.h>
#include <Gdiplus.h>

#include "Renderer.h"

class BlokiWindow: public Ludos::Window
{
public:
	BlokiWindow();
	~BlokiWindow();

	LRESULT MsgHandler( HWND hWindow, UINT Msg, WPARAM wParam, LPARAM lParam );
	
	VOID	setRenderer( Renderer* pRenderer );
private:
	VOID onPaint( Gdiplus::Graphics* Graphics );

/* VARIABLES */
	Renderer*			m_pRenderer;

	ULONG_PTR			m_gdiToken;

	Gdiplus::Graphics*	m_pBackBuffer;
	Gdiplus::Bitmap*	m_pBackBufferBitmap;
};

#endif