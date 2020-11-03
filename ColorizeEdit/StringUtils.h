// StringUtils.h: interface for the CStringUtils class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STRINGUTILS_H__80D1170C_0F8D_4C1F_A310_D1A893D9BA97__INCLUDED_)
#define AFX_STRINGUTILS_H__80D1170C_0F8D_4C1F_A310_D1A893D9BA97__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define C_TAB_SIZE		4
class CStringUtils
{
public:
	CStringUtils();
	virtual ~CStringUtils();

public:
	static void			DebugPrint				( const TCHAR* lpszFmt, ...);
	static CString		TabToSpace				( int nOffset);
	static int			GetLength				( const CString& strText );
	static CString		Left					( const CString& strText, int nLength );
	static CString		Mid						( const CString& strText, int nStart );
	static CString		Mid						( const CString& strText, int nStart, int nLength );
	static CString		Insert					( const CString& strText, int nStart, const CString& strChar );
	static CString		Delete					( const CString& strText, int nStart, BOOL bBackword );
	static CString		Replace					( const CString& strText, int nStart, const CString& strChar );
	static BOOL			PutToClipboard			(HWND hWnd, LPCTSTR pszText);
	static CString		GetFromClipboard		(HWND hWnd );
	static BOOL			IsAlphabet				( const char	cChar );

};

#endif // !defined(AFX_STRINGUTILS_H__80D1170C_0F8D_4C1F_A310_D1A893D9BA97__INCLUDED_)

