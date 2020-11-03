// ScriptTextLine.cpp: implementation of the CScriptTextLine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ScriptTextLine.h"
#include "StringUtils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScriptTextLine::CScriptTextLine()
{
	m_strLine			= _T("");
	memset( &m_rcLine, 0x00, sizeof(m_rcLine));
	m_dwFlags			= C_LINE_MARK_NONE;
}

CScriptTextLine::~CScriptTextLine()
{
	RemoveAll();
}

CScriptTextLine::CScriptTextLine( const CScriptTextLine& st )
{
	m_strLine	= st.m_strLine;
	CScriptTextWord* pWordNew = NULL;

	m_dwFlags	= st.m_dwFlags;

	int i;
	int nCount = st.m_aryWrod.GetSize();

	RemoveAll();

	for( i=0 ; i<nCount ; i++ )
	{
		pWordNew = new CScriptTextWord( *st.m_aryWrod.GetAt(i) );
		m_aryWrod.Add( pWordNew );
	}

}

CScriptTextLine& CScriptTextLine::operator = (const CScriptTextLine& st )
{
	m_strLine	= st.m_strLine;
	CScriptTextWord* pWordNew = NULL;

	m_dwFlags	= st.m_dwFlags;

	int i;
	int nCount = st.m_aryWrod.GetSize();

	RemoveAll();

	for( i=0 ; i<nCount ; i++ )
	{
		pWordNew = new CScriptTextWord( *st.m_aryWrod.GetAt(i) );
		m_aryWrod.Add( pWordNew );
	}

	return *this;
}

int				CScriptTextLine::SetLine			( const CString& strLine )
{

	LPCTSTR pszBuf = (LPCTSTR)strLine;
	CString strWord = _T("");
	int nLength = strLine.GetLength();
	int i = 0;
	BOOL bOnSpace		= FALSE;
	BOOL bOnComment		= FALSE;

	RemoveAll();

	m_strLine			= strLine;
	memset( &m_rcLine, 0x00, sizeof(m_rcLine));

	strWord = _T("");
	for( i=0 ; i<nLength ; i++ )
	{
		if ( pszBuf[i] == '"' || pszBuf[i] == '\'' )
		{
			if( strWord.GetLength() != 0 )
			{
				OnAddWord( strWord );
				strWord.Empty();
			}
			strWord = pszBuf[i];
			OnAddWord( strWord );

			strWord.Empty();
		}
		else if ( pszBuf[i] == ' ' )
		{
			if( bOnSpace == FALSE )
			{
				bOnSpace = TRUE;
				OnAddWord( strWord );

				strWord.Empty();
				strWord += pszBuf[i];
			}
			else
			{
				strWord += pszBuf[i];
			}
		}
		else if( bOnSpace == TRUE )
		{
			bOnSpace = FALSE;
			OnAddWord( strWord );
			strWord.Empty();
			strWord += pszBuf[i];
		}
		else
		{
			bOnSpace = FALSE;
			strWord += pszBuf[i];
		}
	}

	OnAddWord( strWord ); // Last Word;

	return m_aryWrod.GetSize();
}

CScriptTextWord*	CScriptTextLine::OnAddWord			( const CString& strWord )
{
	if( strWord.GetLength() == 0 )
		return NULL;

	CScriptTextWord* pNewWord = new CScriptTextWord();

	pNewWord->SetWord( strWord );

	m_aryWrod.Add( pNewWord );

	return pNewWord;
}

void				CScriptTextLine::RemoveAll				()
{
	int i = 0;
	int nCount = m_aryWrod.GetSize();
	CScriptTextWord* pWord = NULL;
	for( i=0 ; i<nCount ; i++ )
	{
		pWord = m_aryWrod.GetAt(i);
		pWord->RemoveAll();
		delete pWord;
	}

	m_aryWrod.RemoveAll();
}

BOOL				CScriptTextLine::HitTest					( int x, int y, int& nStartPos, CDC* pDC )
{
	if( y < m_rcLine.top )
		return FALSE;

	if( m_rcLine.bottom < y )
		return FALSE;
	
	CScriptTextWord* pWord = NULL;
	int i;
	int nMaxWord = m_aryWrod.GetSize();
	int nHalfWidth = 0;

	for( i=0 ; i<nMaxWord ; i++ )
	{
		pWord = m_aryWrod.GetAt( i );
		
		if( pWord->HitTest( pDC, x, nHalfWidth, nStartPos )  == FALSE )
			continue;
		break;
	}

	return TRUE;
}

BOOL				CScriptTextLine::HitTest					( int x, int y, int& nStartPos, CDC* pDC, CString& strSelWord )
{
	if( y < m_rcLine.top )
		return FALSE;

	if( m_rcLine.bottom < y )
		return FALSE;
	
	CScriptTextWord* pWord = NULL;
	int i;
	int nMaxWord = m_aryWrod.GetSize();
	int nHalfWidth = 0;
	int nOldStartPos = nStartPos;

	for( i=0 ; i<nMaxWord ; i++ )
	{
		pWord = m_aryWrod.GetAt( i );
		nOldStartPos = nStartPos;
		if( pWord->HitTest( pDC, x, nHalfWidth, nStartPos )  == FALSE )
			continue;
		nStartPos = nOldStartPos;
		strSelWord = pWord->m_strWord;
		if( strSelWord.Left(1) == _T("\"") )
		{
			nStartPos ++;
			strSelWord.Replace( _T("\""), _T(""));
		}
		else if( strSelWord.Left(1) == _T("\'") )
		{
			nStartPos ++;
			strSelWord.Replace( _T("\'"), _T(""));
		}
		break;
	}

	return TRUE;
}

int					CScriptTextLine::GetStartPosFromX		( int x, int& nStartPos, CDC* pDC )
{
	CScriptTextWord* pWord = NULL;
	int i;
	int nMaxWord = m_aryWrod.GetSize();
	int nHalfWidth = 0;

	for( i=0 ; i<nMaxWord ; i++ )
	{
		pWord = m_aryWrod.GetAt( i );
		
		if( pWord->HitTest( pDC, x, nHalfWidth, nStartPos )  == FALSE )
			continue;
		break;
	}

	return TRUE;
}

BOOL				CScriptTextLine::IsEndPos				( int nCaretPos )
{
	int nLength = CStringUtils::GetLength( m_strLine );
	return nLength <= nCaretPos;
}

int					CScriptTextLine::GetCaretPosX			(CDC* pDC, int nStartPos, int& nCharWidth )
{
	CScriptTextWord* pWord = NULL;
	int nCaretPosX = 0;
	int nCurrPos = 0;
	int nMaxWord = m_aryWrod.GetSize();

	if( nMaxWord == 0 )
		return 0;

	RECT rcTemp ;
	::SetRect( &rcTemp, 0, 0, 0, 0 );
	int nLength = 0;
	int nLengthLeft = 0;
	int nLengthRight = 0;
	CString strValue, strLeft, strRight;

	nLength = CStringUtils::GetLength( m_strLine );
	if( nLength < nStartPos ) // End of the Line.
		nStartPos = nLength;

	nLengthRight = nLength - nStartPos;
	nLengthLeft = nLength - nLengthRight;

	strLeft = CStringUtils::Left( m_strLine, nLengthLeft );
	strLeft.Replace( _T("&"), _T("&&") );
	pDC->DrawText( strLeft, &rcTemp, DT_CALCRECT );

	nCaretPosX = rcTemp.right;

	if( nLengthRight == 0 )
	{
		nCharWidth = 2;
	}
	else
	{
		strRight = CStringUtils::Mid( m_strLine, nStartPos, 1 );
		strRight.Replace( _T("&"), _T("&&") );
		pDC->DrawText( strRight, &rcTemp, DT_CALCRECT );
		nCharWidth = rcTemp.right - rcTemp.left;
	}

	return nCaretPosX;
}

RECT&				CScriptTextLine::GetRect				()
{
	return m_rcLine;
}

void				CScriptTextLine::GetRect				(CDC* pDC, CFont* pFont, RECT* pRect )
{
	RECT rc;
	
	CDC memDC;

	CString strLine = m_strLine;

	strLine.Replace( _T("&"), _T("&&"));

	memDC.CreateCompatibleDC( pDC );
	CFont* pOldFont = (CFont*)memDC.SelectObject( pFont );

	memset( &rc, 0x00, sizeof(rc));
	memDC.DrawText( strLine, &rc, DT_CALCRECT );

	memDC.SelectObject( pOldFont );
	memDC.DeleteDC();
	*pRect = rc;
}

#define C_FOUND_OPTION_NONE				0x00
#define C_FOUND_OPTION_FIRST_SPACE		0x01
#define C_FOUND_OPTION_PREV_SPACE		0x02
#define C_FOUND_OPTION_FOUND			0x04
#define C_FOUND_OPTION_FINDING			0x08


int					CScriptTextLine::GetNextWordPos			( int nStartCharPos )
{
	int i;
	int nLength = m_strLine.GetLength();
	if( nStartCharPos == nLength )
		return nStartCharPos;

	int nFlagFound = C_FOUND_OPTION_NONE;
	LPCTSTR plpData = (LPCTSTR)m_strLine;

	for( i=nStartCharPos ; i<nLength ; i++ )
	{
		if( nFlagFound == C_FOUND_OPTION_NONE )
		{
			if( plpData[i] == ' ' )
				nFlagFound = C_FOUND_OPTION_FIRST_SPACE;
			else
				nFlagFound = C_FOUND_OPTION_FINDING;
		}
		else if( nFlagFound == C_FOUND_OPTION_FIRST_SPACE )
		{
			if( plpData[i] != ' ' )
				return i;	
		}
		else if( nFlagFound == C_FOUND_OPTION_FINDING )
		{
			if( plpData[i] == ' ' )
				nFlagFound = C_FOUND_OPTION_FOUND;
		}
		else if( nFlagFound == C_FOUND_OPTION_FOUND )
		{
			if( plpData[i] != ' ' )
				return i;
		}
		
		if( plpData[i] & 0x80 )
			i++;

	}
	return nLength;	
}

int					CScriptTextLine::GetPrefWordPos			( int nStartCharPos )
{

	int i;
	int nLength = m_strLine.GetLength();
	if( nStartCharPos == 0 )
		return nStartCharPos;

	int nFlagFound = C_FOUND_OPTION_NONE;
	LPCTSTR plpData = (LPCTSTR)m_strLine;

	nStartCharPos = max( 0, nStartCharPos -1 );

	for( i=nStartCharPos ; 0<i ; i-- )
	{
		if( nFlagFound == C_FOUND_OPTION_NONE )
		{
			if( plpData[i] == ' ' )
				nFlagFound = C_FOUND_OPTION_FIRST_SPACE;
			else
				nFlagFound = C_FOUND_OPTION_FINDING;
		}
		else if( nFlagFound == C_FOUND_OPTION_FIRST_SPACE )
		{
			if( plpData[i] == ' ' )
				nFlagFound = C_FOUND_OPTION_PREV_SPACE;
			else
				nFlagFound = C_FOUND_OPTION_FINDING;
		}
		else if( nFlagFound == C_FOUND_OPTION_PREV_SPACE )
		{
			if( plpData[i] != ' ' )
				nFlagFound = C_FOUND_OPTION_FINDING;

		}
		else if( nFlagFound == C_FOUND_OPTION_FINDING )
		{
			if( plpData[i] == ' ' )
				return i+1;
		}
		else if( nFlagFound == C_FOUND_OPTION_FOUND )
		{
			if( plpData[i] != ' ' )
				return i;
		}
//		
//		if( plpData[i] & 0x80 )
//			i--;

	}
	return 0;	

//	int i;
//	int nLength = CStringUtils::GetLength( m_strLine );
//	if( nStartCharPos <= 1 )
//		return 0;
//
//	BOOL bFound = FALSE;
//	nStartCharPos -=2 ;
//	LPCTSTR plpData = (LPCTSTR)m_strLine;
//
////	CString strRight = CStringUtils::Mid( m_strLine, nStartCharPos );
//
//	for( i=nStartCharPos ; 0<i ; i-- )
//	{
//		if( plpData[i] & 0x80 )
//		{
//			i--;
//			continue;
//		}
//
//		if( plpData[i] == ' ' )
//			return i+1;
//
//	}
//	return 0;	
}

void				CScriptTextLine::Draw					(CDC*				pDC					,
															int					nLineHeight			,
															int					x					,
															int					y					,
															const RECT&			rcClient			, 
															int					nLineNum			,
															S_SELECTION_INFO*	pSelection			,
															BOOL				bCursorLine			,
															IScriptTextColorBase*	pTextColor		)
{
	RECT rc, rcTemp;
	
	CScriptTextWord* pWord = NULL;
	int i;
	int nMaxWord = m_aryWrod.GetSize();
	int nCurrCharPos = 0;

	::SetRect( &m_rcLine, x, y, x, y + nLineHeight );
	rc = m_rcLine;

	BOOL	bStartedComment = FALSE;
	BOOL	bStartedDblQuot	= FALSE;
	BOOL	bClipedLine	= FALSE;

	if( pSelection->bSelection && pSelection->nStartLine <= nLineNum && nLineNum < pSelection->nEndLine )
		bClipedLine = TRUE;

	
	for( i=0 ; i<nMaxWord ; i++ )
	{
		pWord = m_aryWrod.GetAt( i );

		pWord->Draw( pDC, &rc, rcClient, nCurrCharPos, nLineNum, pSelection,
					bStartedComment, bStartedDblQuot, bCursorLine, pTextColor );


		rc.left = rc.right;
	}
	if( bClipedLine )
	{
		::SetRect( &rcTemp, rc.right, rc.top, rc.right + 8, rc.bottom );
		pDC->SetBkColor( C_BK_COLOR_CLIPPED );
		pDC->DrawText( " ", 1, &rcTemp, DT_LEFT );
	}
	m_rcLine.right = rc.right;

}

BOOL				CScriptTextLine::FindText				(	const BOOL&				bMatchCase				,
																const BOOL&				bWholeWord				,
																const CString&			strWord					, 
																const int&				nLength					,
																int&					nStartPos				)
{
	BOOL bRet	= FALSE;
	int nLineLength = CStringUtils::GetLength( m_strLine );
	if( nLineLength < nLength )
		return FALSE;;

	int nTemp = 0;
	int nPos	= -1;
	CString strLine ;
	CString strTemp ;
	if( nStartPos != 0 )
		strLine = CStringUtils::Mid( m_strLine, nStartPos );
	else
		strLine = m_strLine;

	if( bMatchCase == FALSE )
		strLine.MakeUpper();

	nPos = strLine.Find( strWord );
	if( nPos == -1 )
		goto e;

	if( bWholeWord == TRUE )
	{
		strTemp = strLine.Left( nPos );
		nTemp = CStringUtils::GetLength( strTemp );

		if( nStartPos + nTemp + nLength != nLineLength )
		{
			strTemp = CStringUtils::Mid( m_strLine, nStartPos + nTemp + nLength, 1 );
			if( CStringUtils::IsAlphabet( (char)*strTemp.GetBuffer(1) ) )
				goto e;
		}

		if( 0 < nStartPos + nTemp )
		{
			strTemp = CStringUtils::Mid( m_strLine, nStartPos + nTemp -1, 1 );
			if( CStringUtils::IsAlphabet( (char)*strTemp.GetBuffer(1) ) )
				goto e;
		}

	}

	strLine = strLine.Left( nPos );
	nStartPos += CStringUtils::GetLength( strLine );

	bRet = TRUE;
e:
	return bRet;
}
