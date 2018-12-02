#ifndef GDIUTILITY_H
#define GDIUTILITY_H

#include <string>
#include <objidl.h>
#include <Gdiplus.h>
#include <Ole2.h>

#include "ResourceUtility.h"

inline Gdiplus::Bitmap* loadGdiBitmap( INT ResID, const std::string& ResType )
{
	/* Load resource */
	CustomResource Resource;
	if( !LoadCustomResource( ResID, ResType, Resource ) )
		return NULL;

	/* Allocate resource data */
	HGLOBAL hBufferHandle = GlobalAlloc( GMEM_MOVEABLE, Resource.nSize );
	if( !hBufferHandle ) return NULL;

	void* pBuffer = GlobalLock( hBufferHandle );
	if( !pBuffer ) return NULL;

	CopyMemory( pBuffer, Resource.pData, Resource.nSize );

	/* Make IStream object */
	IStream* pStream = NULL;
	CreateStreamOnHGlobal( hBufferHandle, FALSE, &pStream );

	Gdiplus::Bitmap* pBitmap = Gdiplus::Bitmap::FromStream( pStream );
	pStream->Release();
	GlobalUnlock( hBufferHandle );
	GlobalFree( hBufferHandle );

	return pBitmap;
}


#endif // GDIUTILITY_H
