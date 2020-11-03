// ScriptTextWord.cpp: implementation of the CScriptTextWord class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ScriptTextWord.h"
#include "StringUtils.h"
#include <ctype.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScriptTextWord::CScriptTextWord()
{
	m_strWord = _T("");
	memset( &m_rcWord, 0x00, sizeof(m_rcWord));
	m_nSelStart						= 0;
	m_nSelLength					= 0;
	m_dwType						= C_WORD_TYPE_UNKNOWN;
}

CScriptTextWord::~CScriptTextWord()
{
	RemoveAll();
}

CScriptTextWord::CScriptTextWord( const CScriptTextWord& st )
{
	m_strWord	= st.m_strWord;
	S_CHAR* pCharNew = NULL;

	int i;
	int nCount = st.m_aryChar.GetSize();

	RemoveAll();

	for( i=0 ; i<nCount ; i++ )
	{
		pCharNew = new S_CHAR( *st.m_aryChar.GetAt(i) );
		m_aryChar.Add( pCharNew );
	}
}

CScriptTextWord& CScriptTextWord::operator = (const CScriptTextWord& st )
{
	m_strWord	= st.m_strWord;
	S_CHAR* pCharNew = NULL;

	int i;
	int nCount = st.m_aryChar.GetSize();

	RemoveAll();

	for( i=0 ; i<nCount ; i++ )
	{
		pCharNew = new S_CHAR( *st.m_aryChar.GetAt(i) );
		m_aryChar.Add( pCharNew );
	}

	return *this;
}

void		CScriptTextWord::SetWord			( const CString& strValue ) 
{
	
	m_strWord = strValue;

	m_aryChar.RemoveAll();
	BOOL bDigit = TRUE;
	char szTemp[3] = { NULL, };
	int i;
	int nLength = m_strWord.GetLength();
	LPCTSTR pszText = (LPCTSTR)m_strWord;

	for( i=0 ; i<nLength ; i++ )
	{
		memset( szTemp, 0x00, sizeof(szTemp));
		if( ( pszText[i] & 0x80 ) & 0x80 )
		{
			S_CHAR* pChar = new S_CHAR;
#ifdef _VS_2017
			strncpy_s( szTemp, &pszText[i], 2 );
#else
			strncpy( szTemp, &pszText[i], 2 );
#endif // _VS_2017
			pChar->strValue = szTemp;
			m_aryChar.Add( pChar );
			i++;
		}
		else
		{
			S_CHAR* pChar = new S_CHAR;
#ifdef _VS_2017
			strncpy_s( szTemp, &pszText[i], 1 );
#else
			strncpy( szTemp, &pszText[i], 1 );
#endif // 
			pChar->strValue = szTemp;
			m_aryChar.Add( pChar );
		}
		if( bDigit == TRUE && !( '0' <= pszText[i] && pszText[i] <= '9') )
			bDigit = FALSE;
	}
	if( m_strWord.Left(2) == "//" )
		m_dwType = C_WORD_TYPE_COMMENT;
	else if( m_strWord.Left(1) == "\"" && m_strWord.Right(1) == "\"" )
		m_dwType = C_WORD_TYPE_STRING;
	else if( bDigit == TRUE )
		m_dwType = C_WORD_TYPE_NUMBER;

}

void		CScriptTextWord::RemoveAll				()
{
	S_CHAR* pChar = NULL;
	int i;
	int nCount = m_aryChar.GetSize();
	for( i=0 ; i<nCount ; i++ )
	{
		pChar = m_aryChar.GetAt( i );
		delete pChar;
	}
	m_aryChar.RemoveAll();
}

BOOL		CScriptTextWord::HitTest				( CDC* pDC, int x, int& nLeftHalfWidth, int& nStartPos )
{
	BOOL bRet = FALSE;
	S_CHAR* pChar = NULL;
	RECT rcChar;
	CString strValue;
	int i;
	int nHalfWidth = 0;
	int nCount = m_aryChar.GetSize();

	if( nCount == 0 )
		return TRUE;

	memset( &rcChar, 0x00, sizeof(rcChar));
	pChar = m_aryChar.GetAt( nCount -1 );
	pDC->DrawText( pChar->strValue, &rcChar, DT_CALCRECT );
	nHalfWidth = ( rcChar.right - rcChar.left + 1 ) >> 1 ;

	if( x < m_rcWord.left - nLeftHalfWidth )
	{
		return FALSE;
	}
	if( m_rcWord.right - nHalfWidth <= x )
	{
		nStartPos += nCount;
		nLeftHalfWidth = nHalfWidth;
		return FALSE;
	}

	rcChar = m_rcWord;

	for( i=0 ; i<nCount ; i++ )
	{
		pChar = m_aryChar.GetAt( i );
		if( pChar->strValue == _T("&") )
			strValue = _T("&&");
		else
			strValue = pChar->strValue;
		pDC->DrawText( strValue, &rcChar, DT_CALCRECT );
		nHalfWidth = ( rcChar.right - rcChar.left + 1 ) >> 1 ;
		if( x < rcChar.left - nLeftHalfWidth )
		{
			rcChar.left = rcChar.right;
			nLeftHalfWidth = nHalfWidth;
			continue;
		}
		if( rcChar.right - nHalfWidth <= x )
		{
			rcChar.left = rcChar.right;
			nLeftHalfWidth = nHalfWidth;
			continue;
		}
		bRet = TRUE;
		break;
	}
	nStartPos += i;
	
	return bRet;
}

void		CScriptTextWord::Draw					(CDC*					pDC				,
													 RECT*					pRect			, 
													 const RECT&			rcClient		,
													 int&					nCurrCharPos	,
													 int					nLineNum		,
													 S_SELECTION_INFO*		pSelection		,
													 BOOL&					bStartedComment	,
													 BOOL&					bStartedDblQuot	,
													 BOOL					bCursorLine		, 
													 IScriptTextColorBase*	pTextColor		)
{

	CString strValue	= m_strWord;
	S_CHAR* pChar		= NULL;
	BOOL bSelected		= FALSE;
	S_KEYWORD_COLOR_INFO* pColorInfo = NULL;
	COLORREF			crBk = C_BK_COLOR_NORMAL;
	COLORREF			crBkNormal = C_BK_COLOR_NORMAL;
	COLORREF			crText = C_TEXT_COLOR_NORMAL;
	int nLength			= m_aryChar.GetSize();

	m_rcWord.left		= pRect->left;
	m_rcWord.top		= pRect->top;
	m_rcWord.right	= 0;
	m_rcWord.bottom	= 0;


	if( pSelection->strWord.GetLength() != 0 )
	{
		bSelected = !strValue.Compare( pSelection->strWord );
	}

	pColorInfo = pTextColor->GetKeywordColor( m_strWord );
	

	if( bSelected == TRUE )
	{
		crBk = C_BK_COLOR_SELECTED ;
	}
	else if( bCursorLine )
	{
		crBk = C_BK_COLOR_SELLINE;
	}

	if( bStartedComment == TRUE )
	{
		crText = C_TEXT_COLOR_COMMENT;
	}
	else
	{
		if( m_strWord.Left(2) == "//" )
		{
			bStartedComment = TRUE;
			crText = C_TEXT_COLOR_COMMENT;
		}
		else if( m_strWord.Left(1) == "\"" || m_strWord.Left(1) == "\'" )
		{
			if( bStartedDblQuot == FALSE )
				bStartedDblQuot = TRUE;
			else
				bStartedDblQuot = FALSE;
			crText = C_TEXT_COLOR_STRING;
		}
		else if( bStartedDblQuot == TRUE )
		{
			crText = C_TEXT_COLOR_STRING;
		}
		else if( pColorInfo != NULL )
		{
			crText = pColorInfo->crColor;
		}
		else if( m_dwType == C_WORD_TYPE_NUMBER )
		{
			crText = C_TEXT_COLOR_NUMBER;
		}
		else
		{
			crText = C_TEXT_COLOR_NORMAL;
		}
	}
	pDC->SetTextColor( crText );


	if( pSelection->bSelection == FALSE ||
		nLineNum < pSelection->nStartLine ||
		pSelection->nEndLine < nLineNum )
	{
		DrawText( pDC, rcClient, &m_rcWord, crBk, strValue );
	}
	else if( pSelection->nStartLine == nLineNum && 
			 pSelection->nEndLine == nLineNum )
	{
		int nIntsecStart	= 0;
		int nIntsedEnd		= 0;
		nIntsecStart	= max( nCurrCharPos, pSelection->nStartPos );
		nIntsedEnd		= min( nCurrCharPos + nLength , pSelection->nEndPos );

		if( nIntsedEnd <= nIntsecStart )
		{
			// Draw Normal
			DrawText( pDC, rcClient, &m_rcWord, crBk, strValue );
		}
		else
		{
			RECT	rcTemp;
			int nSelLen	= nIntsedEnd - nIntsecStart;
			int nStart	= nCurrCharPos - nIntsecStart;
			// Seperate 3 parts of Chars.
			if( nStart == 0 )
			{
				rcTemp = m_rcWord;
				// Maybe Seperate 2 parts of Chars.
				// Draw Normal
				strValue = CStringUtils::Left( m_strWord, nSelLen );
				crBk = bSelected ? C_BK_COLOR_CLIPPED_SELECTED : C_BK_COLOR_CLIPPED;
				DrawText( pDC, rcClient, &rcTemp, crBk, strValue );

				rcTemp.left = rcTemp.right;
				nLength -= nSelLen;
				nStart = nSelLen;
				if( nLength )
				{
					crBk = bSelected ? C_BK_COLOR_CLIPPED_SELECTED : ( bCursorLine ? C_BK_COLOR_SELLINE : C_BK_COLOR_NORMAL );
					strValue = CStringUtils::Mid( m_strWord, nStart, nLength );
					DrawText( pDC, rcClient, &rcTemp, crBk, strValue  );
				}
				m_rcWord.right = rcTemp.right;
			}
			else
			{
				nStart = nIntsecStart - nCurrCharPos;
				rcTemp = m_rcWord;
				// Maybe Seperate 3 parts of Chars.
				// Draw Normal
				strValue = CStringUtils::Left( m_strWord, nStart );
				crBk = bCursorLine ? C_BK_COLOR_SELLINE : C_BK_COLOR_NORMAL;
				DrawText( pDC, rcClient, &rcTemp, crBk, strValue  );
				rcTemp.left = rcTemp.right;
				nLength -= nStart ;
				if( nLength )
				{
					strValue = CStringUtils::Mid( m_strWord, nStart, nSelLen );
					crBk = bSelected ? C_BK_COLOR_CLIPPED_SELECTED : C_BK_COLOR_CLIPPED;
					DrawText( pDC, rcClient, &rcTemp, crBk, strValue  );
					rcTemp.left = rcTemp.right;
				}
				nLength -= nSelLen;
				if( nLength )
				{
					strValue = CStringUtils::Mid( m_strWord, nStart + nSelLen, nLength );
//					crBk = bSelected ? C_BK_COLOR_CLIPPED_SELECTED : ( bCursorLine ? C_BK_COLOR_SELLINE : C_BK_COLOR_NORMAL );
					crBk = bCursorLine ? C_BK_COLOR_SELLINE : C_BK_COLOR_NORMAL;
					DrawText( pDC, rcClient, &rcTemp, crBk, strValue  );
				}

				m_rcWord.right = rcTemp.right;
			}
			
		}
	}
	else if( pSelection->nStartLine == nLineNum )
	{
		if( pSelection->nStartPos == nCurrCharPos )
		{
			// Draw Normal
			crBk = bSelected ? C_BK_COLOR_CLIPPED_SELECTED : C_BK_COLOR_CLIPPED;
			DrawText( pDC, rcClient, &m_rcWord, crBk, strValue  );
		}
		else if( nCurrCharPos < pSelection->nStartPos  && 
			    pSelection->nStartPos < nCurrCharPos + nLength )
		{
			RECT rcTemp;
			rcTemp = m_rcWord;

			int nStart = pSelection->nStartPos - nCurrCharPos;
			strValue = CStringUtils::Left( m_strWord, nStart );
			crBk = bSelected ? C_BK_COLOR_CLIPPED_SELECTED : ( bCursorLine ? C_BK_COLOR_SELLINE : C_BK_COLOR_NORMAL );
			DrawText( pDC, rcClient, &rcTemp, crBk, strValue  );
			rcTemp.left = rcTemp.right;
			nLength -= nStart ;
			if( nLength )
			{
				strValue = CStringUtils::Mid( m_strWord, nStart, nLength );
				crBk = bSelected ? C_BK_COLOR_CLIPPED_SELECTED : C_BK_COLOR_CLIPPED;
				DrawText( pDC, rcClient, &rcTemp, crBk, strValue  );
			}
			m_rcWord.right = rcTemp.right;

		}
		else if( pSelection->nStartPos < nCurrCharPos + nLength )
		{
			crBk = bSelected ? C_BK_COLOR_CLIPPED_SELECTED : C_BK_COLOR_CLIPPED;
			DrawText( pDC, rcClient, &m_rcWord, crBk, strValue  );
		}
		else
		{
			crBk = bSelected ? C_BK_COLOR_CLIPPED_SELECTED : ( bCursorLine ? C_BK_COLOR_SELLINE : C_BK_COLOR_NORMAL );
			DrawText( pDC, rcClient, &m_rcWord, crBk, strValue  );
		}
		

	}
	else if( pSelection->nEndLine == nLineNum )
	{
		if( pSelection->nEndPos == nCurrCharPos )
		{
			// Draw Normal
			crBk = bSelected ? C_BK_COLOR_CLIPPED_SELECTED : ( bCursorLine ? C_BK_COLOR_SELLINE : C_BK_COLOR_NORMAL );
			DrawText( pDC, rcClient, &m_rcWord, crBk, strValue  );
		}
		else if( nCurrCharPos < pSelection->nEndPos  && 
			    pSelection->nEndPos < nCurrCharPos + nLength )
		{
			RECT rcTemp;
			rcTemp = m_rcWord;

			int nStart = pSelection->nEndPos - nCurrCharPos;
			strValue = CStringUtils::Left( m_strWord, nStart );
			crBk = bSelected ? C_BK_COLOR_CLIPPED_SELECTED : C_BK_COLOR_CLIPPED;
			DrawText( pDC, rcClient, &rcTemp, crBk, strValue  );
			rcTemp.left = rcTemp.right;
			nLength -= nStart ;
			if( nLength )
			{
				strValue = CStringUtils::Mid( m_strWord, nStart, nLength );
				crBk = bSelected ? C_BK_COLOR_CLIPPED_SELECTED : ( bCursorLine ? C_BK_COLOR_SELLINE : C_BK_COLOR_NORMAL );
				DrawText( pDC, rcClient, &rcTemp, crBk, strValue  );
			}
			m_rcWord.right = rcTemp.right;
		}
		else if( pSelection->nEndPos < nCurrCharPos + nLength )
		{
			// Draw Normal
			crBk = bSelected ? C_BK_COLOR_CLIPPED_SELECTED : ( bCursorLine ? C_BK_COLOR_SELLINE : C_BK_COLOR_NORMAL );
			DrawText( pDC, rcClient, &m_rcWord, crBk, strValue  );
		}
		else
		{
			// Draw Normal
			crBk = bSelected ? C_BK_COLOR_CLIPPED_SELECTED : C_BK_COLOR_CLIPPED;
			DrawText( pDC, rcClient, &m_rcWord, crBk, strValue  );
		}
	}
	else
	{
		crBk = bSelected ? C_BK_COLOR_CLIPPED_SELECTED : C_BK_COLOR_CLIPPED;
		DrawText( pDC, rcClient, &m_rcWord, crBk, strValue  );
	}

	nCurrCharPos += m_aryChar.GetSize();
	*pRect = m_rcWord;
}

void		CScriptTextWord::DrawText				( CDC* pDC, const RECT&	rcClient, RECT* pRect, COLORREF crBkColor, CString& strValue )
{
	if( strValue.Find( _T("&") ) != -1 )
		strValue.Replace( _T("&"), _T("&&") );
	pDC->SetBkColor( crBkColor );
	pDC->DrawText( strValue, pRect, DT_CALCRECT );
	if( rcClient.right < pRect->left )
		return;
	if( pRect->right < rcClient.left )
		return;

	pDC->DrawText( strValue, pRect, DT_LEFT | DT_SINGLELINE | DT_VCENTER );
}