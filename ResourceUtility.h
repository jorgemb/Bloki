#ifndef RESOURCEUTILITY_H
#define RESOURCEUTILITY_H

#include <WinDef.h>
#include <WinBase.h>
#include <string>

struct CustomResource{
	HGLOBAL		hResource;
	DWORD		nSize;
	LPVOID		pData;
};

inline BOOL LoadCustomResource( INT ResID, const std::string& Type, CustomResource& Resource )
{
	HRSRC hResInfo = NULL;
	HGLOBAL hResource = NULL;
	HINSTANCE hModule = GetModuleHandle(NULL);

	// Find the resource
	hResInfo = FindResource( hModule, MAKEINTRESOURCE( ResID ), Type.c_str() );
	if( !hResInfo )
		return FALSE;

	// Get resource size
	DWORD nResSize = 0;
	nResSize = SizeofResource( hModule, hResInfo );
	if( nResSize == 0 )
		return FALSE;

	// Load resource
	hResource = LoadResource( hModule, hResInfo );
	if( !hResource )
		return FALSE;

	// Lock resource
	LPVOID pResData = NULL;
	pResData = LockResource( hResource );
	if( !pResData )
		return FALSE;

	// Fill custom resource
	ZeroMemory( &Resource, sizeof( CustomResource ) );
	Resource.hResource	= hResource;
	Resource.nSize		= nResSize;
	Resource.pData		= pResData;

	return TRUE;
}

#endif // RESOURCEUTILITY_H
