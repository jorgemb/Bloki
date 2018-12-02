#include "BlokiWindow.h"

using namespace Gdiplus;

BlokiWindow::BlokiWindow():
	m_pRenderer( NULL ),
	m_gdiToken( NULL ),

	m_pBackBuffer( NULL ),
	m_pBackBufferBitmap( NULL )
{
	GdiplusStartupInput gdiplusInput;
	GdiplusStartup( &m_gdiToken, &gdiplusInput, NULL );

	// Back buffer
	m_pBackBufferBitmap = new Gdiplus::Bitmap( m_nWidth, m_nHeight );
	m_pBackBuffer = Gdiplus::Graphics::FromImage( m_pBackBufferBitmap );

}

BlokiWindow::~BlokiWindow()
{
	S_DELETE( m_pBackBuffer );
	S_DELETE( m_pBackBufferBitmap );

	GdiplusShutdown( m_gdiToken );
}

LRESULT BlokiWindow::MsgHandler( HWND hWindow, UINT Msg, WPARAM wParam, LPARAM lParam )
{
	switch( Msg )
	{
	case( WM_PAINT ):
		{
			PAINTSTRUCT Paint;
			HDC hDC = BeginPaint( hWindow, &Paint );

			Gdiplus::Graphics GraphicsObj( hDC );

			onPaint( m_pBackBuffer );
			GraphicsObj.DrawImage( m_pBackBufferBitmap, 0, 0 );

			EndPaint( hWindow, &Paint );
			break;
		}
	case( WM_SIZE ):
		{
			Window::MsgHandler( hWindow, Msg, wParam, lParam );

			S_DELETE( m_pBackBuffer );
			S_DELETE( m_pBackBufferBitmap );

			m_pBackBufferBitmap = new Gdiplus::Bitmap( m_nWidth, m_nHeight );
			m_pBackBuffer = Gdiplus::Graphics::FromImage( m_pBackBufferBitmap );
			break;
		}
	default:
		return Window::MsgHandler( hWindow, Msg, wParam, lParam );
	}
	return 0;
}

VOID BlokiWindow::setRenderer( Renderer* pRenderer )
{
	m_pRenderer = pRenderer;
}

VOID BlokiWindow::onPaint( Gdiplus::Graphics *Graphics )
{
	RECT ClientRect;
	GetClientRect( m_hWindow, &ClientRect );
	
	Gdiplus::SolidBrush blackBrush( Gdiplus::Color( 0, 0, 0 ) );
	Graphics->FillRectangle( &blackBrush, ClientRect.left, ClientRect.top, ClientRect.right, ClientRect.bottom );

	/* Render */
	if( m_pRenderer )
		m_pRenderer->Draw( Graphics );
}