// Utils.cpp: implementation of the CStringUtils class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StringUtils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStringUtils::CStringUtils()
{

}

CStringUtils::~CStringUtils()
{

}


void CStringUtils::DebugPrint( const TCHAR* lpszFmt, ...)
{
	#ifdef _DEBUG
	va_list			vargs;
	TCHAR			szMsgeBuf[512] = { NULL, };			

	

	va_start(vargs, lpszFmt);
#ifdef _VS_2017
	_vsnprintf_s( szMsgeBuf, 512, lpszFmt, vargs);
#else
	_vsnprintf( szMsgeBuf, 512, lpszFmt, vargs );
#endif // #ifdef _VS_2017

	va_end(vargs);
#ifdef _VS_2017
	strcat_s( szMsgeBuf, "\n" ) ;
#else
	strcat( szMsgeBuf, "\n") ;
#endif // #ifdef _VS_2017

	OutputDebugString( szMsgeBuf );
	#endif // _DEBUG
}

CString CStringUtils::TabToSpace(int nOffset)
{
	CString strTab = " ";
	nOffset++;
	
	while ( nOffset++ % C_TAB_SIZE )   
	{
		strTab += " ";
	}
	
	return strTab;
}

int			CStringUtils::GetLength				( const CString& strText )
{
	LPSTR	pszData = (LPSTR)(LPCTSTR)strText;
	int i;
	int nCount = 0;
	int nLen = strText.GetLength();

	for( i=0 ; i<nLen ; i++, nCount++ )
	{
		if( pszData[i] & 0x80 )
			i++;
	}

	return nCount;
}

CString		CStringUtils::Left					( const CString& strText, int nLength )
{
	CString strResult = _T("");
	LPSTR	pszData = (LPSTR)(LPCTSTR)strText;
	char szTemp[4];
	int i;
	int nCount = 0;
	int nLen = strText.GetLength();

	for( i=0 ; i<nLen ; i++, nCount++ )
	{
		if( nLength <= nCount )
			break;

		if( pszData[i] & 0x80 )
		{
			memset( szTemp, 0x00, sizeof(szTemp));
			memcpy( szTemp, &pszData[i], 2 );
			szTemp[2] = NULL;
			strResult += szTemp;
			i++;
		}
		else
		{
			strResult += pszData[i];
		}


	}

	return strResult;
}

CString		CStringUtils::Mid					( const CString& strText, int nStart )
{
	CString strResult = _T("");
	LPSTR	pszData = (LPSTR)(LPCTSTR)strText;
	char szTemp[4];
	int i;
	int nCount = 0;
	int nLen = strText.GetLength();

	for( i=0 ; i<nLen ; i++, nCount++ )
	{
		if( nCount < nStart )
		{
			if( pszData[i] & 0x80 )
				i++;
			continue;
		}

		if( pszData[i] & 0x80 )
		{
			memset( szTemp, 0x00, sizeof(szTemp));
			memcpy( szTemp, &pszData[i], 2 );
			szTemp[2] = NULL;
			strResult += szTemp;
			i++;
		}
		else
		{
			strResult += pszData[i];
		}
	}

	return strResult;
}

CString		CStringUtils::Mid					( const CString& strText, int nStart, int nLength )
{
	CString strResult = _T("");
	LPSTR	pszData = (LPSTR)(LPCTSTR)strText;
	char	szTemp[4];
	int i;
	int nCount = 0;
	int nLen = strText.GetLength();

	for( i=0 ; i<nLen ; i++, nCount++ )
	{
		if( nCount < nStart )
		{
			if( pszData[i] & 0x80 )
				i++;
			continue;
		}

		if( nStart + nLength <= nCount )
			break;

		if( pszData[i] & 0x80 )
		{
			memset( szTemp, 0x00, sizeof(szTemp));
			memcpy( szTemp, &pszData[i], 2 );
			szTemp[2] = NULL;
			strResult += szTemp;
			i++;
		}
		else
		{
			strResult += pszData[i];
		}


	}

	return strResult;
}

CString		CStringUtils::Insert					( const CString& strText, int nStart, const CString& strChar )
{
	int nLength = CStringUtils::GetLength( strText );
	if( nStart < 0 )
		nStart = 0;
	else if( nLength < nStart )
		nStart = nLength;

	CString strResult = _T("");
	CString strLeft = CStringUtils::Left( strText, nStart );
	CString strRight = CStringUtils::Mid( strText, nStart, nLength - nStart );
	strResult = strLeft;
	strResult += strChar;
	strResult += strRight;
	return strResult;
}

CString		CStringUtils::Delete					( const CString& strText, int nStart, BOOL bBackword )
{
	int nLength = CStringUtils::GetLength( strText );
	if( bBackword == TRUE )
		nStart--;

	if( nStart < 0 )
		nStart = 0;
	else if( nLength < nStart )
		nStart = nLength;

	CString strResult = _T("");
	CString strLeft = CStringUtils::Left( strText, nStart-1 );
	CString strRight = CStringUtils::Mid( strText, nStart );
	strResult = strLeft;
	strResult += strRight;
	return strResult;
}

CString		CStringUtils::Replace					( const CString& strText, int nStart, const CString& strChar )
{
	int nLength = CStringUtils::GetLength( strText );


	if( nStart < 0 )
		nStart = 0;
	else if( nLength < nStart )
		nStart = nLength;

	CString strResult = _T("");
	CString strLeft = CStringUtils::Left( strText, nStart-1 );
	CString strRight = CStringUtils::Mid( strText, nStart );
	strResult = strLeft;
	strResult += strChar;
	strResult += strRight;
	return strResult;
}


BOOL CStringUtils::PutToClipboard(HWND hWnd, LPCTSTR pszText)
{
	BOOL bOK = FALSE;

	int nLength = lstrlen(pszText);

	if (pszText == NULL || nLength == 0)
		return FALSE;

	CWaitCursor wc;
	
	if (OpenClipboard( hWnd ))
	{
		EmptyClipboard();
		HGLOBAL hData = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, nLength + 1);
		if (hData != NULL)
		{
			LPSTR pszData = (LPSTR) ::GlobalLock(hData);
			memset(pszData, 0x00, nLength + 1);
#ifdef _UNICODE
			char* pszTemp = new char[nLength+1];
			WideCharToMultiByte(CP_ACP, 0, pszText, nLength, pszTemp, nLength, NULL, NULL);
			pszTemp[nLength] = NULL;
			//strcpy_s(pszData, nLength, pszTemp);
			memcpy(pszData, pszTemp, nLength);
			delete[]pszTemp;
#else
	#ifdef _VS_2017
			strcpy_s(pszData, nLength + 1, pszText);
	#else
			strcpy( pszData, pszText );
	#endif // _VS_2017
#endif // _UNICODE

			GlobalUnlock(hData);
			bOK = SetClipboardData(CF_TEXT, hData) != NULL;
		}
		CloseClipboard();
	}
	return bOK;
}

CString		CStringUtils::GetFromClipboard		(HWND hWnd )
{
	CString strResult = _T("");
	// Try opening the clipboard
	if (!OpenClipboard( hWnd ))
		return _T("");
		
	// Get handle of clipboard object for ANSI text
	HANDLE hData = GetClipboardData(CF_TEXT);
	if ( hData == NULL )
		return _T("");
		
	// Lock the handle to get the actual text pointer
	char * pszText = static_cast<char*>( GlobalLock(hData) );
	if ( pszText == NULL )
		goto e;
		
	strResult = pszText;

e:	
	// Release the lock
	GlobalUnlock( hData );
	
	// Release the clipboard
	CloseClipboard();

	return strResult;

}

BOOL	CStringUtils::IsAlphabet				( const char	cChar )
{
	if( '0' <= cChar && cChar <= '9' )
		return TRUE;
	if( 'a' <= cChar && cChar <= 'z' )
		return TRUE;
	if( 'A' <= cChar && cChar <= 'Z' )
		return TRUE;

	return FALSE;

}