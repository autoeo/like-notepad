// ScriptText.cpp: implementation of the CScriptText class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ScriptText.h"
#include "StringUtils.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#define C_INITIAL_LEFT_MARGIN		32
#define C_MARKER_WIDTH				18
#define C_LINE_LEFT_SPACE			12

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScriptText::CScriptText()
{
	m_strPathName			= _T("");
	m_strTextBuffer			= _T("");
	m_bIsCRLF				= FALSE;
	m_pListener				= NULL;
	m_hEvent				= INVALID_HANDLE_VALUE;
	m_bRunThread			= FALSE;
	
	m_nFontSize				= 0;
	m_strFontName			= _T("");
	m_nVisibleEndLine		= 0;
	m_nLeftMargin			= C_INITIAL_LEFT_MARGIN;
	m_nMaxNumWidth			= 64;
	m_nScrollOffetX			= 0;
	m_nScrollOffetY			= 145;
	m_nScrollOffetY			= 0;

	m_nMaxWidth				= 0;
	m_nLineHeight			= 16;
	m_bShowCaret			= FALSE;
	m_hCursor				= AfxGetApp()->LoadStandardCursor(IDC_IBEAM);
	m_hCursorArrow			= AfxGetApp()->LoadStandardCursor(IDC_ARROW);
	m_nDebugingLineNum		= -1;
	m_nInputMode = C_INPUT_MODE_INSERT;
	m_nCaretLine			= 0;
	m_nCaretStartPos		= 0;
	m_nCaretWidth			= 2;

	m_stFindText			= S_FIND_TEXT_INFO();

	m_pWnd					= ::CWnd::FromHandle( ::GetDesktopWindow() );
	m_rcClient.SetRect( 0, 0, 0, 0 );
	m_pTextColorTable		= NULL;
	memset( &m_logFont, 0x00, sizeof(m_logFont));
	m_pOldFont				= NULL;

	CClientDC dc( m_pWnd );
	m_memDC_Font.CreateCompatibleDC( &dc );

	SetFont( 16, _T( "Fixedsys" ) );

}

CScriptText::~CScriptText()
{
	Release();
}

BOOL	CScriptText::Init					(CWnd* pWndOnwer , IListenerScriptText* pListener, UINT nImgId )
{
	m_pWnd			= pWndOnwer;
	m_pListener		= pListener;

	m_pWnd->SetTimer( C_TIMVER_CARET_SHOW, 5000, NULL );

	m_imglistMark.Create( nImgId, 16, 0, RGB(255, 255, 255));

	if( m_memDC_Font.GetSafeHdc() )
		m_memDC_Font.DeleteDC();

	CClientDC dc( m_pWnd );
	m_memDC_Font.CreateCompatibleDC( &dc );

	if( GetLineCount() == 0 )
	{
		CScriptTextLine* pLine = new CScriptTextLine();
		m_aryLine.Add( pLine );
	}

	return TRUE;
}

void	CScriptText::Release					()
{
	RemoveAll();
	m_aryUndo.RemoveAll();
	m_aryRedo.RemoveAll();

	if( m_memDC_Font.GetSafeHdc() )
	{
		if( m_pOldFont && m_pOldFont->GetSafeHandle() )
			m_memDC_Font.SelectObject( m_pOldFont );
		m_pOldFont	= NULL;

//		if( m_pOldBmpLine && m_pOldBmpLine->GetSafeHandle() )
//			m_memDC_Line.SelectObject( m_pOldBmpLine );
//		m_pOldBmpLine = NULL;
		m_memDC_Font.DeleteDC();
	}

//	if( m_bmpLine.GetSafeHandle() )
//		m_bmpLine.DeleteObject();

	if( m_font.GetSafeHandle() )
		m_font.DeleteObject();

	::DestroyTextColorTable( &m_pTextColorTable );

	m_imglistMark.DeleteImageList();
}

BOOL	CScriptText::Load					( const CString& strFileName )
{
	BYTE* pData = NULL;
	DWORD dwRed;
	DWORD dwFileSize = 0;
	HANDLE hFile = INVALID_HANDLE_VALUE;

	SetDefault();

	DWORD dwTick = ::GetTickCount();
	::DestroyTextColorTable( &m_pTextColorTable );

	if( strFileName.Right(4).CompareNoCase( _T(".txt") ) == 0 )
		m_pTextColorTable		= ::CreateTextColorTable( E_CT_UBCT );
	else if( strFileName.Right(4).CompareNoCase( _T(".cpp") ) == 0 ||
			strFileName.Right(4).CompareNoCase( _T(".c") ) == 0 ||
			strFileName.Right(4).CompareNoCase( _T(".h") ) == 0 )
		m_pTextColorTable		= ::CreateTextColorTable( E_CT_CPLUSPLUS );
	else
		m_pTextColorTable		= ::CreateTextColorTable( E_CT_CPLUSPLUS );

	m_pTextColorTable->LoadDefaultStyle();

	dwTick = ::GetTickCount() - dwTick ;

	hFile = ::CreateFile( strFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if( hFile == INVALID_HANDLE_VALUE )
		return FALSE;

	m_strPathName	= strFileName;

	dwFileSize = ::GetFileSize( hFile, &dwFileSize );
	pData = new BYTE[dwFileSize + 1];
	memset( pData, 0x00, dwFileSize  + 1 );

	::ReadFile( hFile, pData, dwFileSize, &dwRed, NULL );

	CloseHandle( hFile );
	hFile = INVALID_HANDLE_VALUE;

	m_strTextBuffer = pData;
	if( m_strTextBuffer.Find( _T("\r") ) != -1 )
	{
		m_bIsCRLF = TRUE;
		m_strTextBuffer.Replace( _T("\r"), _T("") );
	}
	
	dwTick = ::GetTickCount();

	RemoveAll();
	m_aryUndo.RemoveAll();
	m_aryRedo.RemoveAll();

	ParseString( m_strTextBuffer, &m_aryLine );

	delete [] pData;
	pData = NULL;

	dwTick = ::GetTickCount() - dwTick;
	CStringUtils::DebugPrint( _T(" CScriptText::OnRead - dwTick = %d"), dwTick );

	dwTick = ::GetTickCount();

	m_nMaxWidth = CalcMaxWidth();

	dwTick = ::GetTickCount() - dwTick;
	CStringUtils::DebugPrint( _T(" CScriptText::CalcMaxWidth - dwTick = %d"), dwTick );

	UpdateCaretPos();
	UpdateScrollInfo();
	m_pWnd->Invalidate( FALSE );

	if( m_pListener )
		m_pListener->OnReadFile( dwFileSize , m_bIsCRLF , m_aryLine.GetSize() );

	return TRUE;
}


BOOL	CScriptText::Save					( const CString& strPathName )
{
	int i;
	int nCount = GetLineCount();
	CScriptTextLine* pLine = NULL;
	CString strCRLF = _T("\n");
	if( m_bIsCRLF )
		strCRLF = _T("\r\n");

	DWORD dwWriton = 0;
	CString strData = _T("");
	HANDLE hFile = INVALID_HANDLE_VALUE;
	hFile = ::CreateFile( strPathName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if( hFile == INVALID_HANDLE_VALUE )
		return FALSE;

	for( i=0 ; i<nCount ; i++ )
	{
		pLine = m_aryLine.GetAt( i );
		strData += pLine->m_strLine;
		if( nCount == i + 1 )
			break;
		strData += strCRLF;
	}
	
	::WriteFile( hFile, (LPCTSTR)strData, strData.GetLength(), &dwWriton, NULL );

	::CloseHandle( hFile );
	hFile = INVALID_HANDLE_VALUE;
	
	return TRUE;
}

void	CScriptText::SetDefault				()
{
	m_strTextBuffer			= _T("");
	
	m_nVisibleEndLine		= 0;
	m_nScrollOffetX			= 0;
	m_nScrollOffetY			= 0;

	m_nMaxWidth				= 0;
	m_nDebugingLineNum		= -1;
	m_nLineHeight			= 16;
	m_bShowCaret			= FALSE;

	m_nInputMode			= C_INPUT_MODE_INSERT;
	m_nCaretLine			= 0;
	m_nCaretStartPos		= 0;
	m_nCaretWidth			= 2;

	::DestroyTextColorTable( &m_pTextColorTable );

	m_stSelection			= S_SELECTION_INFO();
	m_bLButtonDown			= FALSE;
	m_ptLbuttonDown.x		= 0;
	m_ptLbuttonDown.y		= 0;
	m_ptCaret.x				= m_nLeftMargin;
	m_ptCaret.y				= 0;
	m_pWnd->SetCaretPos( m_ptCaret );
}


void	CScriptText::SetFont				( int nFontSize, const CString& strFontName )
{
	
	m_nFontSize		= nFontSize;
	m_strFontName	= strFontName;

	m_logFont.lfHeight = nFontSize;
	m_logFont.lfEscapement = 0;
	m_logFont.lfOrientation = 0;
	m_logFont.lfWeight = FW_NORMAL;
	m_logFont.lfItalic = 0;
	m_logFont.lfUnderline = 0;
	m_logFont.lfStrikeOut = 0 ;
	if( m_nFontSize == 16 )
		m_logFont.lfCharSet = DEFAULT_CHARSET;
	else
		m_logFont.lfCharSet = ANSI_CHARSET;
	m_logFont.lfOutPrecision = OUT_STROKE_PRECIS;
	m_logFont.lfClipPrecision = CLIP_STROKE_PRECIS;
	m_logFont.lfQuality = PROOF_QUALITY;
	m_logFont.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
	lstrcpy( (LPSTR)m_logFont.lfFaceName, (LPCTSTR)strFontName );
	
	if( m_font.GetSafeHandle() != NULL )
		m_font.DeleteObject();

	m_font.CreateFontIndirect( &m_logFont );

	if( m_pOldFont )
		m_memDC_Font.SelectObject( m_pOldFont );
	m_pOldFont = m_memDC_Font.SelectObject( &m_font );

	DWORD dwTick = ::GetTickCount();

	m_nMaxWidth = CalcMaxWidth();

	dwTick = ::GetTickCount() - dwTick;
//	CStringUtils::DebugPrint( _T(" CScriptText::SetFont - CalcMaxWidth - dwTick = %d"), dwTick );

}


CString	CScriptText::GetSelectedText			()
{
	CString strResult = _T("");

	S_SELECTION_INFO stSelection = m_stSelection;
	if( stSelection.bSelection == FALSE )
		return _T("");

	if( stSelection.nStartLine == stSelection.nEndLine )
	{
		CScriptTextLine* pLine = m_aryLine.GetAt( stSelection.nStartLine );
		strResult = CStringUtils::Mid( pLine->m_strLine, stSelection.nStartPos, stSelection.nEndPos - stSelection.nStartPos );
	}
	else
	{
		int nLineNum;

		CScriptTextLine* pLine = m_aryLine.GetAt( stSelection.nStartLine );
		strResult = CStringUtils::Mid( pLine->m_strLine, stSelection.nStartPos );
		strResult += _T("\n");
		for( nLineNum = stSelection.nStartLine+1 ; nLineNum < stSelection.nEndLine ; nLineNum ++ )
		{
			pLine = m_aryLine.GetAt( nLineNum );
			strResult += pLine->m_strLine;
			strResult += _T("\n");
		}
		pLine = m_aryLine.GetAt( stSelection.nEndLine );
		strResult += CStringUtils::Left( pLine->m_strLine, stSelection.nEndPos );
	}

	return strResult;
}

void	CScriptText::GetSelCharsAndLines		( int& nChars, int& nLines )
{
	int nTotalChars = 0;
	int nLenCRLF = 1;
	if( m_bIsCRLF )
		nLenCRLF = 2;

	S_SELECTION_INFO stSelection = m_stSelection;
	if( stSelection.bSelection == FALSE )
	{
		nChars = 0;
		nLines = 0;
		return;
	}

	if( stSelection.nStartLine == stSelection.nEndLine )
	{
		CScriptTextLine* pLine = m_aryLine.GetAt( stSelection.nStartLine );
		nTotalChars = stSelection.nEndPos - stSelection.nStartPos;
	}
	else
	{
		int nLineNum;

		CScriptTextLine* pLine = m_aryLine.GetAt( stSelection.nStartLine );
		nTotalChars = CStringUtils::GetLength( pLine->m_strLine ) - stSelection.nStartPos;
		nTotalChars += nLenCRLF;
		for( nLineNum = stSelection.nStartLine+1 ; nLineNum < stSelection.nEndLine ; nLineNum ++ )
		{
			pLine = m_aryLine.GetAt( nLineNum );
			nTotalChars += CStringUtils::GetLength( pLine->m_strLine );
			nTotalChars += nLenCRLF;
		}
		nTotalChars += stSelection.nEndPos;
	}

	nChars = nTotalChars;
	nLines = stSelection.nEndLine - stSelection.nStartLine + 1;
	if( nChars == 0 )
		nLines = 0;
}

CString	CScriptText::GetLineTextFromPoint	( const POINT& pt, int& nLineNumer )
{
	BOOL bRet = FALSE;
	int nLineIndex = 0, nPos = 0;
	int nLineNum = m_nScrollOffetY;
	int nHeight = m_rcClient.Height();
	int nMaxLine = m_aryLine.GetSize();
	CScriptTextLine* pLine = NULL;
	int nStartPos = 0;
	CString strWord;

	int nLineCount = min( GetVisableLineCount() + 1, nMaxLine - m_nScrollOffetY  );

	for( nLineIndex=0 ; nLineIndex<nLineCount ; nLineIndex++, nLineNum++ )
	{
		pLine	= m_aryLine.GetAt( nLineNum );

		if( pLine->m_rcLine.right < pt.x )
			continue;

		if( pLine->HitTest( pt.x, pt.y , nStartPos, &m_memDC_Font, strWord ) == FALSE )
			continue;

		bRet = TRUE;
		nLineNumer			= nLineNum;
		
		break;
	}
	if( bRet == FALSE )
	{
		nLineNumer			= -1;
	}

	return strWord;
}

BOOL	CScriptText::HitTest				( int x, int y, int& nLine, int& nStartPos )
{
	BOOL bRet = FALSE;
	int nLineIndex = 0, nPos = 0;
	int nLineNum = m_nScrollOffetY;
	int nHeight = m_rcClient.Height();
	int nMaxLine = m_aryLine.GetSize();
	CScriptTextLine* pLine = NULL;

	int nLineCount = min( GetVisableLineCount() + 1, nMaxLine - m_nScrollOffetY  );

	if( nLineCount == 0 )
		return TRUE;

	for( nLineIndex=0 ; nLineIndex<nLineCount ; nLineIndex++, nLineNum++ )
	{
		pLine	= m_aryLine.GetAt( nLineNum );
		if( pLine->HitTest( x, y , nStartPos, &m_memDC_Font ) == FALSE )
			continue;

		bRet = TRUE;
		nLine			= nLineNum;
		m_nCaretLine	= nLineNum;
		m_nCaretStartPos = nStartPos;
		
		break;
	}
	if( bRet == FALSE )
	{
		nLine			= nLineNum - 1;
		m_nCaretLine	= nLineNum - 1;
		m_nCaretStartPos = CStringUtils::GetLength( pLine->m_strLine );
	}
	m_pWnd->InvalidateRect( &m_rcClient, FALSE );

	return bRet;
}

BOOL	CScriptText::HitTest				( int x, int y, int& nLine, int& nStartPos, CString& strSelWord  )
{
	BOOL bRet = FALSE;
	int nLineIndex = 0, nPos = 0;
	int nLineNum = m_nScrollOffetY;
	int nHeight = m_rcClient.Height();
	int nMaxLine = m_aryLine.GetSize();
	CScriptTextLine* pLine = NULL;

	int nLineCount = min( GetVisableLineCount() + 1, nMaxLine - m_nScrollOffetY  );

	for( nLineIndex=0 ; nLineIndex<nLineCount ; nLineIndex++, nLineNum++ )
	{
		pLine	= m_aryLine.GetAt( nLineNum );
		if( pLine->HitTest( x, y , nStartPos, &m_memDC_Font, strSelWord ) == FALSE )
			continue;

		bRet = TRUE;
		nLine			= nLineNum;
		m_nCaretLine	= nLineNum;
		m_nCaretStartPos = nStartPos;
		
		break;
	}
	if( bRet == FALSE )
	{
		nLine			= nLineNum - 1;
		m_nCaretLine	= nLineNum - 1;
		m_nCaretStartPos = CStringUtils::GetLength( pLine->m_strLine );
	}

	m_pWnd->InvalidateRect( &m_rcClient, FALSE );

	return bRet;
}	

CScriptTextLine*	CScriptText::GetCaretPos				( int nLine, int nStartPos, POINT& pt, int& nCharWidth )
{
	CScriptTextLine* pLine = NULL;
	int nMaxLine = m_aryLine.GetSize();
	int nVisibleLineCount	= 0;

	if( nMaxLine == 0 )
	{
		pt.x = 0;
		pt.y = 0;
		return NULL;
	}

	if( nMaxLine <= nLine )
		pLine = m_aryLine.GetAt( nMaxLine-1);
	else
		pLine = m_aryLine.GetAt( nLine );

	pt.x = pLine->GetCaretPosX( &m_memDC_Font, nStartPos, nCharWidth ) - m_nScrollOffetX  + m_nLeftMargin;
	pt.y = ( nLine - m_nScrollOffetY ) * m_nLineHeight;

	return pLine;
}

CScriptTextLine*	CScriptText::GetCursorLine			( int& nLine )
{
	nLine = m_nCaretLine;
	return m_aryLine.GetAt( m_nCaretLine );
}

void	CScriptText::MoveToLine				( const int& nLine )
{
	m_nCaretLine = nLine;
	m_nCaretStartPos = 0;
	if( m_pListener )
	{
		int nSelChars, nSelLines ;
		GetSelCharsAndLines( nSelChars, nSelLines );
		m_pListener->OnMovedCursor( m_nCaretLine+1 , m_nCaretStartPos+1, nSelChars, nSelLines );
	}
	MoveScrollToCaret();
	UpdateCaretPos();
	m_pWnd->Invalidate( FALSE );
}


void	CScriptText::Draw(CDC* pDC, int cx, int cy )
{
	RECT rc;
	
	CDC memDC;
	CBitmap bmp, *pOldBmp = NULL;
	CBrush brush, brushSelLine, brushLineNum, brushMarker;
	CFont* pOldFont = NULL;
	memDC.CreateCompatibleDC( pDC );

	brush.CreateSolidBrush( RGB(255,255,255) );
	brushSelLine.CreateSolidBrush( RGB(232,232,255) );
	brushLineNum.CreateSolidBrush( RGB(228,228,228) );
	brushMarker.CreateSolidBrush( RGB(244,244,244) );

	bmp.CreateCompatibleBitmap( pDC, cx, cy );
	pOldBmp = (CBitmap*)memDC.SelectObject( &bmp );

	pOldFont = memDC.SelectObject( &m_font );

	S_SELECTION_INFO stSelection( m_stSelection );
	CScriptTextLine* pLine = NULL;
	SCROLLINFO info;
	m_pWnd->GetScrollInfo( SB_VERT, &info );
	m_nScrollOffetY = min( (int)m_nScrollOffetY, info.nMax - (int)info.nPage + 1 );
	int nLineNum = m_nScrollOffetY;
	int nLineIndex = 0;
	int nPosX = 0;
	int nPosY;
	int nMaxLine = m_aryLine.GetSize();
	int nLineCount = min( ( cy / m_nLineHeight) + 1, nMaxLine - m_nScrollOffetY  );
	m_nMaxNumWidth = CalcMaxNumberWidth( &memDC, nMaxLine ) + 2 ;
	m_nLeftMargin = m_nMaxNumWidth + C_MARKER_WIDTH + C_LINE_LEFT_SPACE;

	SetRect( &rc, 0, 0, min( m_rcClient.Width() , cx ), cy );
	memDC.FillRect( &rc, &brush );

	int nTickBitBlt = 0;
	DWORD dwTick = 0, dwTickTotal = 0;;

	nPosX = -m_nScrollOffetX + m_nLeftMargin;

	for( nLineIndex=0 ; nLineIndex<nLineCount ; nLineNum++, nLineIndex++ )
	{
		dwTick = GetTickCount();

		pLine = m_aryLine.GetAt( nLineNum );
		
		nPosY = nLineIndex * m_nLineHeight;


		if( nLineNum == m_nCaretLine )
		{
			// Selected Line
			::SetRect( &rc, m_nLeftMargin, nPosY, cx, nPosY + m_nLineHeight );
			memDC.FillRect( &rc, &brushSelLine );
		}

		pLine->Draw( &memDC, m_nLineHeight, nPosX, nPosY, m_rcClient, nLineNum, &stSelection, nLineNum == m_nCaretLine, m_pTextColorTable );

		::SetRect( &rc, 0, nPosY, m_nMaxNumWidth, nPosY + m_nLineHeight );
		DrawLineNum( &memDC, rc, nLineNum+1, &brushLineNum );

		::SetRect( &rc, rc.right, rc.top, rc.right + C_MARKER_WIDTH, rc.bottom );
		DrawMark( &memDC, rc, nLineNum, &brushMarker, pLine->m_dwFlags, m_nDebugingLineNum == nLineNum );

		::SetRect( &rc, rc.right, rc.top, rc.right + C_LINE_LEFT_SPACE, rc.bottom );
		memDC.FillRect( &rc, &brush );

		dwTick = GetTickCount() - dwTick;
		dwTickTotal += dwTick;

	}	
#ifdef _DEBUG
	RECT rcTemp;
	CString strMsg;
	strMsg.Format( _T("DRAW : %d / BitBlt = %d"), dwTickTotal, nTickBitBlt );
	rcTemp.left = 0;
	rcTemp.top = 0;
	rcTemp.right = 300;
	rcTemp.bottom = 30;
	memDC.SetBkColor( 0xFFFFFFFF );
	memDC.SetTextColor( RGB(255,0,0));
	memDC.DrawText( strMsg, &rcTemp, DT_LEFT | DT_SINGLELINE | DT_VCENTER );

#endif // _DEBUG

	pDC->BitBlt( 0, 0, cx , cy, &memDC, 0, 0, SRCCOPY );

	memDC.SelectObject( pOldFont );

	brush.DeleteObject();
	brushSelLine.DeleteObject();
	brushLineNum.DeleteObject();
	brushMarker.DeleteObject();
	memDC.SelectObject( pOldBmp );
	bmp.DeleteObject();
	memDC.DeleteDC();

	UpdateCaretPos();
	
}

void	CScriptText::DrawLineNum			(CDC* pDC, RECT& rc, int nLineNum, CBrush* pBrush )
{
	CString strValue;
	strValue.Format( _T("%d"), nLineNum );
	pDC->FillRect( &rc, pBrush );
	pDC->SetBkColor( RGB(228,228,228));
	pDC->SetTextColor( RGB( 128,128,128));
	pDC->DrawText( strValue, &rc, DT_RIGHT | DT_SINGLELINE | DT_VCENTER );

}

void	CScriptText::DrawMark				(CDC* pDC, RECT& rc, int nIconIndex, CBrush* pBrush, const DWORD& dwFlags, BOOL m_bIsDebugingLine )
{
	pDC->FillRect( &rc, pBrush );

	if( m_imglistMark.GetSafeHandle() == NULL )
		return;
	POINT pt;
	pt.x = rc.left + 1;
	pt.y = ( rc.top + rc.bottom ) >> 1;
	pt.y -= 8;
	if( dwFlags & C_LINE_MARK_BOOKMARK )
		m_imglistMark.Draw( pDC, 2, pt, ILD_TRANSPARENT );

	if( dwFlags & C_LINE_MARK_ERRORPOINT )
		m_imglistMark.Draw( pDC, 13, pt, ILD_TRANSPARENT );

	if( dwFlags & C_LINE_MARK_BREAKPOINT )
		m_imglistMark.Draw( pDC, 1, pt, ILD_TRANSPARENT );

	if( m_bIsDebugingLine )
		m_imglistMark.Draw( pDC, 0, pt, ILD_TRANSPARENT );

}

void	CScriptText::OnSize					(int cx, int cy )
{
	CRect rc;
	
	rc.SetRect( 0, 0, cx, cy );

	if( rc == m_rcClient )
		return;

	m_rcClient = rc;

	m_nMaxWidth = CalcMaxWidth();
	UpdateScrollInfo();
}

void	CScriptText::OnVScroll				(UINT nSBCode, UINT nPos )
{
	
	int nVisibleLine;
	UINT nScrollOffsetY = GetScrollOffsetY();
	UINT nLineCount = GetLineCount(); //in lines
	int nLineHeight = GetLineHeight();
	SCROLLINFO info;
	m_pWnd->GetScrollInfo( SB_VERT, &info );

	nVisibleLine = GetVisableLineCount() + 1;
	
	switch( nSBCode )
	{
	case SB_LINEDOWN:		//Scroll one line down.
		if ( nScrollOffsetY < nLineCount - info.nPage )
			nScrollOffsetY++;
		nScrollOffsetY = min( nScrollOffsetY, info.nMax - info.nPage + 1 );
		break;
		
	case SB_LINEUP:			//Scroll one line up.
		if ( 0 < nScrollOffsetY )
			nScrollOffsetY--;
		break;
		
	case SB_PAGEDOWN:		//Scroll one page down.
		nScrollOffsetY += info.nPage;
		if( nLineCount - info.nPage < nScrollOffsetY )
			nScrollOffsetY = nLineCount - info.nPage;
		break;
	case SB_PAGEUP:			//Scroll one page up.
		nScrollOffsetY	-= info.nPage;
		if( nScrollOffsetY < 0 )
			nScrollOffsetY = 0;
		break;
	case SB_THUMBPOSITION:	//Scroll to the absolute position. The current position is provided in nPos.
	case SB_THUMBTRACK:		//Drag scroll box to specified position. The current position is provided in nPos.
		nScrollOffsetY = nPos;
		break;

	case SB_ENDSCROLL:		//End scroll.
		break;
	default:
		ASSERT( FALSE );
		break;
	}
	if( (int)nScrollOffsetY < 0 )
		nScrollOffsetY = 0;
	SetScrollOffsetY( nScrollOffsetY );
	m_pWnd->SetScrollPos( SB_VERT, nScrollOffsetY, true );
	UpdateCaretPos();
	m_pWnd->Invalidate( FALSE );
	// redraw now!
	m_pWnd->UpdateWindow();
	
}

void	CScriptText::OnHScroll				(UINT nSBCode, UINT nPos )
{
	CRect rcClient;
	m_pWnd->GetClientRect( &rcClient );

	int nWidth			= rcClient.Width();
	UINT nScrollOffsetX	= GetScrollOffsetX();
	UINT nMaxLineWidth	= GetMaxWidth();
	int nPageSize		= nWidth * 3 / 4;

	SCROLLINFO info;
	m_pWnd->GetScrollInfo( SB_HORZ, &info );

	switch( nSBCode )
	{
	case SB_LINELEFT:
		nScrollOffsetX -= 20;
		nScrollOffsetX = max( 0, nScrollOffsetX );
		break;
	case SB_LINERIGHT:		//Scroll right one line.
		nScrollOffsetX += 20;
		if( nMaxLineWidth - info.nPage <= nScrollOffsetX )
			nScrollOffsetX = nMaxLineWidth - info.nPage;
		break;
		
	case SB_PAGERIGHT:		//Scroll one page right.
		nScrollOffsetX	+= info.nPage;
		break;
		
	case SB_THUMBPOSITION:	//Scroll to absolute position. The current position is specified by the nPos parameter.
	case SB_THUMBTRACK:		//Drag scroll box to specified position. The current position is specified by the nPos parameter.
			nScrollOffsetX = nPos;
		break;

	case SB_ENDSCROLL:		//End scroll.
			break;
	default:
		ASSERT( FALSE );
		break;
	}

	if( (int)nScrollOffsetX < 0 )
		nScrollOffsetX = 0;
	SetScrollOffsetX( nScrollOffsetX );
	m_pWnd->SetScrollPos(SB_HORZ, nScrollOffsetX, true );
	UpdateCaretPos();
	m_pWnd->Invalidate( FALSE );
	m_pWnd->UpdateWindow();                           
	
	// redraw now!
}

void	CScriptText::OnLButtonDblClk		(UINT nFlags, CPoint point)
{
	CString strSelWord = _T("");
	int nLine = 0;
	int nStartPos = 0;

	if( GetLineCount() == 0 )
		return ;

	HitTest( point.x, point.y, nLine, nStartPos, strSelWord );

	m_stSelection = S_SELECTION_INFO();
	m_stSelection.strWord		= strSelWord;
	m_stSelection.nStartLine	= nLine;
	m_stSelection.nStartPos		= nStartPos;
	m_stSelection.nEndLine		= nLine;
	m_stSelection.nEndPos		= nStartPos + CStringUtils::GetLength( strSelWord );

	UpdateCaretPos();

	if( m_pListener )
	{
		int nSelChars, nSelLines ;
		GetSelCharsAndLines( nSelChars, nSelLines );
		m_pListener->OnMovedCursor( m_nCaretLine+1 , m_nCaretStartPos+1, nSelChars, nSelLines );
	}

}

void	CScriptText::OnLButtonDown			(UINT nFlags, CPoint point)
{
	int nLine = 0;
	int nStartPos = 0;
	int nCaretWidth = 0;
	int nNewCaretPosX = 0;
	POINT ptCaret;
	CScriptTextLine* pLine = NULL;

	m_bLButtonDown = TRUE;
	m_ptLbuttonDown = point;
	
	HitTest( point.x, point.y, nLine, nStartPos );

	GetCaretPos( nLine, nStartPos, ptCaret, nCaretWidth );

	if( ptCaret.x < 0 )
		MoveScrollToCaret();

	if( nFlags & MK_SHIFT )
	{
		m_stSelection.nEndLine		= m_nCaretLine;
		m_stSelection.nEndPos		= m_nCaretStartPos;
		if( point.x < m_nLeftMargin )
		{
			
			if( m_stSelection.nStartLine == -1 )
				m_stSelection.nStartLine = 0;
			m_stSelection.nEndLine		= nLine;
			if( m_stSelection.nStartLine <= m_stSelection.nEndLine )
			{
				pLine = m_aryLine.GetAt( m_stSelection.nEndLine );
				m_stSelection.nStartPos		= 0;
				m_stSelection.nEndPos		= CStringUtils::GetLength( pLine->m_strLine );
			}
			else
			{
				pLine = m_aryLine.GetAt( m_stSelection.nStartLine );
				m_stSelection.nStartPos		= CStringUtils::GetLength( pLine->m_strLine );
				m_stSelection.nEndPos		= 0;
			}
		}
	}
	else
	{
		m_stSelection = S_SELECTION_INFO();
		m_stSelection.nStartLine	= nLine;
		m_stSelection.nStartPos		= nStartPos;
		if( point.x < m_nLeftMargin )
		{
			pLine = m_aryLine.GetAt( m_stSelection.nStartLine );
			m_stSelection.nEndLine		= nLine;
			m_stSelection.nEndPos		= CStringUtils::GetLength( pLine->m_strLine );
		}
	}

	if( m_nMaxNumWidth < point.x && point.x < m_nMaxNumWidth + C_MARKER_WIDTH )
	{
		pLine = m_aryLine.GetAt( nLine );
		
		if( m_pListener ) 
		{
			if( m_pListener->OnClickedDebugPoint( nLine, pLine->m_strLine ) == TRUE )
				pLine->ToggleBreakPoint();
		}
	}

	if( m_pListener )
	{
		int nSelChars, nSelLines ;
		GetSelCharsAndLines( nSelChars, nSelLines );
		m_pListener->OnMovedCursor( m_nCaretLine+1 , m_nCaretStartPos+1, nSelChars, nSelLines );
	}

	::SetCapture( m_pWnd->GetSafeHwnd() );

	UpdateCaretPos();
}


void	CScriptText::OnLButtonUp				(UINT nFlags, CPoint point)
{
	::ReleaseCapture();
	m_bLButtonDown = FALSE;
}

void	CScriptText::OnContextMenu				(CWnd* pWnd, CPoint point, int& nLine, CString& strLine )
{
	int nPos = 0;
	int nCaretWidth = 0;
	int nNewCaretPosX = 0;
	POINT ptCaret;
	CScriptTextLine* pLine = NULL;

	nLine = 0;
	

	m_ptLbuttonDown = point;
	
	HitTest( point.x, point.y, nLine, nPos );

	GetCaretPos( nLine, nPos, ptCaret, nCaretWidth );

	if( ptCaret.x < 0 )
		MoveScrollToCaret();

	if( nLine != -1 )
	{
		CScriptTextLine* pLine = m_aryLine.GetAt( nLine );
		strLine = pLine->m_strLine;
	}

	UpdateCaretPos();

}

void	CScriptText::OnKeyDown					(UINT nChar, UINT nRepCnt, UINT nFlags)
{

	BOOL bInvalidRect = FALSE;
	if( GetLineCount() == 0 )
		return;

	switch( nChar )
	{
	case VK_F9:
		{
			CScriptTextLine* pLine = m_aryLine.GetAt( m_nCaretLine );
			if( GetKeyState( VK_CONTROL ) & 0x8000 && GetKeyState( VK_SHIFT ) & 0x8000 )
				ClearAllBreakPoint();
			else
				pLine->ToggleBreakPoint();
			
			MoveScrollToCaret();
			m_pWnd->Invalidate( FALSE );
			UpdateCaretPos();	
		}
		break;

	case VK_F2:
		{
			int nLine = -1;
			CScriptTextLine* pLine = m_aryLine.GetAt( m_nCaretLine );
			if( GetKeyState( VK_CONTROL ) & 0x8000 )
			{
				if( GetKeyState( VK_SHIFT ) & 0x8000 )
					ClearAllBookMark();
				else
					pLine->ToggleBookMark();
			}
			else if( GetKeyState( VK_SHIFT ) & 0x8000 )
			{
				nLine = FindNextPoint( m_nCaretLine, FALSE, C_LINE_MARK_BOOKMARK );
			}
			else
			{
				nLine = FindNextPoint( m_nCaretLine, TRUE, C_LINE_MARK_BOOKMARK );
			}
			if( nLine != -1 )
			{
				m_nCaretLine = nLine;
			}
			
			MoveScrollToCaret();
			m_pWnd->Invalidate( FALSE );
			UpdateCaretPos();	
		}
		break;
	case VK_INSERT:
		{
			if( m_nInputMode == C_INPUT_MODE_INSERT )
				m_nInputMode = C_INPUT_MODE_OVERWRITE;
			else
				m_nInputMode = C_INPUT_MODE_INSERT;
			UpdateCaretPos();
		}
		break;
	case VK_DELETE:
		bInvalidRect = DeleteSelectedTexts( m_nCaretLine, m_nCaretStartPos, m_stSelection, TRUE );
		if( bInvalidRect == FALSE )
			bInvalidRect = DeleteCharToforward( m_nCaretLine, m_nCaretStartPos, TRUE, FALSE );

		break;
	case VK_RETURN:
		OnKeyDown_Return( nChar, nRepCnt, nFlags );
		break;
	case VK_HOME:
		{
			if( GetKeyState( VK_CONTROL ) & 0x8000 )
				m_nCaretLine = 0;
			m_nCaretStartPos = 0;
			bInvalidRect = TRUE;
		}
		break;
	case VK_END:
		{
			if( GetKeyState( VK_CONTROL ) & 0x8000 )
				m_nCaretLine = GetLineCount() -1;

			CScriptTextLine* pLine = m_aryLine.GetAt( m_nCaretLine );
			if( pLine == NULL )
				break;
			m_nCaretStartPos = CStringUtils::GetLength( pLine->m_strLine );
			bInvalidRect = TRUE;
		}
		break;
	case VK_PRIOR: // Page UP
		{
			m_nCaretLine -= GetVisableLineCount();
			if( m_nCaretLine < 0 )
			{
				m_nCaretLine = 0;
				m_nCaretStartPos = 0;
			}
			bInvalidRect = TRUE;
		}
		break;
	case VK_NEXT: // Page DOWN
		{
			m_nCaretLine += GetVisableLineCount();
			int nLineCount = GetLineCount();
			if( nLineCount-1 <= m_nCaretLine )
			{
				int nEndOfLinePos = 0;
				m_nCaretLine = nLineCount-1;
				CScriptTextLine* pLine = m_aryLine.GetAt( m_nCaretLine );
				if( pLine == NULL )
					break;
				nEndOfLinePos = CStringUtils::GetLength( pLine->m_strLine );
				m_nCaretStartPos = min( nEndOfLinePos, m_nCaretStartPos );
			}
			bInvalidRect = TRUE;
		}
		break;

	case VK_LEFT:
		{
			if( 0 == m_nCaretStartPos )
			{
				if( m_nCaretLine <= 0 )
					break;
				
				CScriptTextLine* pLine = m_aryLine.GetAt( m_nCaretLine - 1 );
				if( pLine == NULL )
					break;
				m_nCaretLine--;
				m_nCaretStartPos = CStringUtils::GetLength( pLine->m_strLine );
			}
			else
			{
				int nLineLength = 0;
				CScriptTextLine* pLine = m_aryLine.GetAt( m_nCaretLine );
				if( pLine == NULL )
					break;

				nLineLength = CStringUtils::GetLength( pLine->m_strLine );
				if( nLineLength < m_nCaretStartPos )
					m_nCaretStartPos = nLineLength;
				if( GetKeyState( VK_CONTROL ) & 0x8000 )
				{
					m_nCaretStartPos = pLine->GetPrefWordPos( m_nCaretStartPos );
				}
				else
				{
					m_nCaretStartPos--;
				}
			}

			bInvalidRect = TRUE;
		}
		break;
	case VK_RIGHT:
		{
			CScriptTextLine* pLine = m_aryLine.GetAt( m_nCaretLine );
			if( pLine == NULL )
				return;
			if( pLine->IsEndPos( m_nCaretStartPos ) == FALSE )
			{
				if( GetKeyState( VK_CONTROL ) & 0x8000 )
				{
					m_nCaretStartPos = pLine->GetNextWordPos( m_nCaretStartPos );
				}
				else
				{
					m_nCaretStartPos ++;
				}
			}
			else
			{
				if( GetLineCount() <= m_nCaretLine+1 )
					break;
				m_nCaretLine++;
				m_nCaretStartPos = 0;
			}

			bInvalidRect = TRUE;
		}
		break;
	case VK_UP:
		{
			CPoint ptCaret = m_pWnd->GetCaretPos();
			m_nCaretLine--;
			m_nCaretLine = max( 0, m_nCaretLine );
			m_nCaretStartPos = 0;
			CScriptTextLine* pLine = m_aryLine.GetAt( m_nCaretLine );
			pLine->GetStartPosFromX( ptCaret.x, m_nCaretStartPos, &m_memDC_Font );

			bInvalidRect = TRUE;
		}
		break;
	case VK_DOWN:
		{	
			CPoint ptCaret = m_pWnd->GetCaretPos();
			int nLineCount = GetLineCount();
		
			m_nCaretLine++;

			if( nLineCount <= m_nCaretLine )
				m_nCaretLine = nLineCount - 1;

			m_nCaretStartPos = 0;
			CScriptTextLine* pLine = m_aryLine.GetAt( m_nCaretLine );
			pLine->GetStartPosFromX( ptCaret.x , m_nCaretStartPos, &m_memDC_Font );

			bInvalidRect = TRUE;
		}
		break;
	default:
		if( GetKeyState( VK_CONTROL ) & 0x8000 )
		{
			if( OnKeyDown_FunctionKey( nChar ) == FALSE )
				break;

			MoveScrollToCaret();
			// UpdateKeySelection(); // Must be skipped!!
			m_pWnd->Invalidate( FALSE );
			UpdateCaretPos();	
			break;
		}
		break;
	}

	if( bInvalidRect )
	{
		MoveScrollToCaret();
		UpdateKeySelection();
		m_pWnd->Invalidate( FALSE );
		UpdateCaretPos();		
	}

	if( m_pListener )
	{
		int nSelChars, nSelLines ;
		GetSelCharsAndLines( nSelChars, nSelLines );
		m_pListener->OnMovedCursor( m_nCaretLine+1 , m_nCaretStartPos+1, nSelChars, nSelLines );
	}

	CStringUtils::DebugPrint( _T("OnKeyDown :: nChar = [%c] %d ( 0x%X ) / nFlags = %d ( 0x%X ) \n") , nChar, nChar, nChar, nFlags, nFlags );

}

void	CScriptText::OnKeyDown_Return		(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CScriptTextLine* pLine = m_aryLine.GetAt( m_nCaretLine );
	int nLength = CStringUtils::GetLength( pLine->m_strLine );

	// Delete Selected Texts.
	DeleteSelectedTexts( m_nCaretLine, m_nCaretStartPos, m_stSelection, TRUE );


	if( nLength < m_nCaretStartPos )
		m_nCaretStartPos = nLength;

	AddUnDoAction( FALSE, C_UNDO_ACT_INSERTED, m_nCaretLine, m_nCaretStartPos, _T("\n") );

	CString strValue = pLine->m_strLine;
	CString strLeft		= CStringUtils::Left( strValue, m_nCaretStartPos );
	pLine->SetLine( strLeft );
	CString strRight	= CStringUtils::Mid( strValue, m_nCaretStartPos );
	CScriptTextLine* pLineNew = new CScriptTextLine();
	if( m_nCaretStartPos == 0 )
	{
		pLineNew->SetFlags( pLine->m_dwFlags );
		pLine->ClearAllFlags();		
	}
	pLineNew->SetLine( strRight );
	m_aryLine.InsertAt( m_nCaretLine+1, pLineNew );
	m_nCaretLine++;
	m_nCaretStartPos = 0;

	
	m_stSelection = S_SELECTION_INFO();
	MoveScrollToCaret();
	
	m_pWnd->Invalidate( FALSE );
	UpdateCaretPos();

}

BOOL	CScriptText::OnKeyDown_FunctionKey	(UINT nChar)
{
	BOOL bInvalidate = FALSE;
//	CStringUtils::DebugPrint( _T("OnKeyDown_FunctionKey :: Ctrl + %c , %x"), (char)nChar, nChar  );
	
	switch( nChar )
	{
	case 'S':
		Save( m_strPathName );
		break;
	case 'A':
		SelectAll();
		bInvalidate = TRUE;
		break;
	case 'Y':	// Redo
		ReDo();
		bInvalidate = TRUE;
		break;
	case 'Z':	// UnDo
		UnDo();
		bInvalidate = TRUE;
		break;
	case 'C':	
		Copy();
		bInvalidate = TRUE;
		break;
	case 'X':
		Cut();
		bInvalidate = TRUE;
		break;
	case 'V':
		Paste();
		bInvalidate = TRUE;
		break;
	}

	return bInvalidate;
}

void	CScriptText::OnChar						(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	BOOL bInvalidRect = FALSE;
	CStringUtils::DebugPrint( _T("OnChar :: nChar = [%c] %d ( 0x%X ) / nFlags = %d ( 0x%X ) \n") , nChar, nChar, nChar, nFlags, nFlags );
	int nLineCount = m_aryLine.GetSize();
	if( nLineCount <= m_nCaretLine )
		m_nCaretLine = nLineCount - 1;

	switch( nChar )
	{
	case VK_BACK:
		bInvalidRect = DeleteSelectedTexts( m_nCaretLine, m_nCaretStartPos, m_stSelection, TRUE );
		if( bInvalidRect == FALSE )
			bInvalidRect = DeleteCharToBackward( m_nCaretLine, m_nCaretStartPos, TRUE );

		m_stSelection = S_SELECTION_INFO();
		m_stSelection.nStartLine = m_nCaretLine;
		m_stSelection.nStartPos = m_nCaretStartPos;
		break;
	case VK_RETURN:
	case VK_ESCAPE:
		break;
	default:
		{
			if( GetKeyState( VK_CONTROL ) & 0x8000 )
				break;

			bInvalidRect = DeleteSelectedTexts( m_nCaretLine, m_nCaretStartPos, m_stSelection, TRUE );
			
			CScriptTextLine* pLine = m_aryLine.GetAt( m_nCaretLine );
			CString strNewChar;
			if( nChar == VK_TAB )
				strNewChar = CStringUtils::TabToSpace( 0 );
			else
				strNewChar.Format( _T("%c"), nChar );

			if( m_nInputMode & C_INPUT_MODE_OVERWRITE )
			{
				if( m_nCaretStartPos != CStringUtils::GetLength( pLine->m_strLine ) && bInvalidRect == FALSE )
					DeleteCharToforward( m_nCaretLine, m_nCaretStartPos, TRUE, TRUE );
			}
			
			AddUnDoAction( FALSE, C_UNDO_ACT_INSERTED, m_nCaretLine, m_nCaretStartPos, strNewChar );

			CString strNewLine = CStringUtils::Insert( pLine->m_strLine, m_nCaretStartPos, strNewChar );

			pLine->SetLine( strNewLine );

			m_nCaretStartPos += CStringUtils::GetLength( strNewChar );

			m_stSelection = S_SELECTION_INFO();
			m_stSelection.nStartLine = m_nCaretLine;
			m_stSelection.nStartPos = m_nCaretStartPos;
			bInvalidRect = TRUE;
		}
		break;
	}

	if( bInvalidRect )
	{
		MoveScrollToCaret();
		UpdateKeySelection( TRUE );
		m_pWnd->Invalidate( FALSE );
		UpdateCaretPos();		
	}

}

void	CScriptText::OnChar					(const CString& strChar, BOOL bComplete )
{
	DeleteSelectedTexts( m_nCaretLine, m_nCaretStartPos, m_stSelection, TRUE );
	
	CScriptTextLine* pLine = m_aryLine.GetAt( m_nCaretLine );
	CString strNewLine;

	if( bComplete == TRUE )
	{
		if( m_nInputMode & C_INPUT_MODE_OVERWRITE )
		{
			if( CStringUtils::GetLength( pLine->m_strLine) != m_nCaretStartPos )
				DeleteCharToforward( m_nCaretLine, m_nCaretStartPos, FALSE, FALSE );
		}
		else if( m_nInputMode & C_INPUT_MASK_ON_INSERT )
		{
			if( CStringUtils::GetLength( pLine->m_strLine) != m_nCaretStartPos )
				DeleteCharToforward( m_nCaretLine, m_nCaretStartPos, FALSE, FALSE );
		}

		AddUnDoAction( FALSE, C_UNDO_ACT_INSERTED, m_nCaretLine, m_nCaretStartPos, strChar );

		strNewLine = CStringUtils::Insert( pLine->m_strLine, m_nCaretStartPos, strChar );
		
		pLine->SetLine( strNewLine );
		m_nCaretStartPos++;
		m_stSelection = S_SELECTION_INFO();
		m_stSelection.nStartLine	= m_nCaretLine;
		m_stSelection.nStartPos		= m_nCaretStartPos;

		m_nInputMode &= ~C_INPUT_MASK_ON_INSERT;
	}
	else
	{
		if( m_nInputMode == C_INPUT_MODE_OVERWRITE )
		{
			if( CStringUtils::GetLength( pLine->m_strLine) != m_nCaretStartPos )
				DeleteCharToforward( m_nCaretLine, m_nCaretStartPos, TRUE, TRUE );
		}
		else if( m_nInputMode == ( C_INPUT_MODE_OVERWRITE | C_INPUT_MASK_ON_INSERT ) )
		{
			if( CStringUtils::GetLength( pLine->m_strLine) != m_nCaretStartPos )
				DeleteCharToforward( m_nCaretLine, m_nCaretStartPos, FALSE, TRUE );
		}
		else if( m_nInputMode == C_INPUT_MASK_ON_INSERT )
		{
			if( CStringUtils::GetLength( pLine->m_strLine) != m_nCaretStartPos )
				DeleteCharToforward( m_nCaretLine, m_nCaretStartPos, FALSE, TRUE );
		}
		m_nInputMode |= C_INPUT_MASK_ON_INSERT;
		strNewLine = CStringUtils::Insert( pLine->m_strLine, m_nCaretStartPos, strChar );

		m_stSelection = S_SELECTION_INFO();
		m_stSelection.nStartLine	= m_nCaretLine;
		m_stSelection.nStartPos		= m_nCaretStartPos;
		
		pLine->SetLine( strNewLine );
	}

	MoveScrollToCaret();
	UpdateKeySelection( TRUE );
	m_pWnd->Invalidate( FALSE );
	UpdateCaretPos();		

}

void	CScriptText::OnMouseMove				(UINT nFlags, CPoint point)
{
	//BOOL bMoved = m_ptLbuttonDown != point;

	if( nFlags & MK_SHIFT )
	{
		if( m_stSelection.nStartLine == -1 )
		{
			m_stSelection.nStartLine = m_nCaretLine;
			m_stSelection.nStartPos = m_nCaretStartPos;
		}

	}

	if( nFlags & MK_LBUTTON && m_bLButtonDown )
	{
		int nGap = 0;
		int nLine = 0;
		int nStartPos = 0;
		HitTest( point.x, max( 0, point.y ), nLine, nStartPos);

		m_stSelection.nEndLine		= m_nCaretLine;
		m_stSelection.nEndPos		= m_nCaretStartPos;

		if( point.x < m_nLeftMargin )
		{
			nGap = point.x - m_nLeftMargin;
			m_nScrollOffetX += nGap;
			
			m_nScrollOffetX = max( 0, m_nScrollOffetX );
			UpdateScrollInfo();
		}
		else if( m_rcClient.right < point.x )
		{
			UINT nMaxLineWidth	= GetMaxWidth();
			SCROLLINFO info;
			m_pWnd->GetScrollInfo( SB_HORZ, &info );

			nGap = point.x - m_rcClient.right;
			m_nScrollOffetX += nGap;

			if( (int)( nMaxLineWidth - info.nPage ) < m_nScrollOffetX )
				m_nScrollOffetX = nMaxLineWidth - info.nPage;
			UpdateScrollInfo();
		}

		if( m_rcClient.bottom < point.y )
		{
			nGap = ( point.y - m_rcClient.bottom ) / m_nLineHeight + 1;

			UINT nLineCount = GetLineCount(); //in lines
			SCROLLINFO info;
			m_pWnd->GetScrollInfo( SB_VERT, &info );

			m_nScrollOffetY += nGap;
			if( (int)( nLineCount - info.nPage ) < m_nScrollOffetY )
				m_nScrollOffetY = nLineCount - info.nPage;

			m_nScrollOffetY = min( m_nScrollOffetY, info.nMax );

			UpdateScrollInfo();
		}
		else if( point.y < m_rcClient.top )
		{
			nGap = ( point.y - m_rcClient.top ) / m_nLineHeight - 1;

			m_nScrollOffetY += nGap ;
			if( m_nScrollOffetY < 0 )
				m_nScrollOffetY = 0;
			UpdateScrollInfo();
		}

		UpdateCaretPos();
		
		if( m_pListener )
		{
			int nSelChars, nSelLines ;
			GetSelCharsAndLines( nSelChars, nSelLines );
			m_pListener->OnMovedCursor( m_nCaretLine+1 , m_nCaretStartPos+1, nSelChars, nSelLines );
		}
	}

	
}

void	CScriptText::OnMouseWheel			(short zDelta)
{
	bool bResult = FALSE;
	
	UINT uLineCount = GetLineCount();
	int nLineHeight = GetLineHeight();
	SCROLLINFO info;
	m_pWnd->GetScrollInfo( SB_VERT, &info );
	CRect rcClient;
	m_pWnd->GetClientRect( &rcClient );

	int nScrollOffsetY = GetScrollOffsetY();
	UINT nVisibleLine = rcClient.Height() / nLineHeight + 1;
	
	nScrollOffsetY += 3 * MulDiv( -zDelta, 1, WHEEL_DELTA);
	if( nScrollOffsetY < 0 )
		nScrollOffsetY = 0;
	nScrollOffsetY = max( 0, nScrollOffsetY );
	nScrollOffsetY = min( nScrollOffsetY, (int)( info.nMax - info.nPage + 1 ) );
	
	m_pWnd->SetScrollPos( SB_VERT, nScrollOffsetY, true );

	if( nScrollOffsetY == m_nScrollOffetY )
		return;
	SetScrollOffsetY( nScrollOffsetY );

	if (bResult)
		m_pWnd->UpdateWindow();

	UpdateCaretPos();	
}

void	CScriptText::OnMouseWheelWithFuncKey	(UINT nFlags, short zDelta)
{
	if( nFlags & ( MK_SHIFT & MK_CONTROL ) )
	{

	}
	else if( nFlags & MK_SHIFT )
	{

	}
	else if( nFlags & MK_CONTROL )
	{
		CString strFontName;
		UINT nFontSize = GetFont( strFontName );
		if( 0 < zDelta )
			nFontSize += 2;
		else
			nFontSize -= 2;
		nFontSize = min( 40, max( nFontSize, 16 ) );
		SetFont( nFontSize, strFontName );
		UpdateScrollInfo();
	}

}

void	CScriptText::OnSetFocus				()
{

//	HANDLE hFileNotification = FindFirstChangeNotification( m_strPathName, FALSE, FILE_NOTIFY_CHANGE_LAST_WRITE );
//	if( hFileNotification == INVALID_HANDLE_VALUE )
//	{
//		DWORD dwError = GetLastError();
//	}
	CStringUtils::DebugPrint( _T("OnSetFocus ----------------"));

	if( m_bShowCaret == TRUE )
		return;

	if( m_pWnd == NULL || m_pWnd->GetSafeHwnd() == NULL )
		return;

	m_bShowCaret	= TRUE;
	int nCaretWidth = 2;
	if( m_nInputMode & C_INPUT_MODE_OVERWRITE ||
		m_nInputMode & C_INPUT_MASK_ON_INSERT )
		nCaretWidth = m_nCaretWidth;
	m_pWnd->CreateSolidCaret( nCaretWidth, m_nLineHeight );
	m_pWnd->ShowCaret();
}

void	CScriptText::OnKillFocus			()
{
	if( m_bShowCaret == FALSE )
		return;

	if( m_pWnd == NULL || m_pWnd->GetSafeHwnd() == NULL )
		return;

	m_bShowCaret	= FALSE;
	m_pWnd->HideCaret();
	::DestroyCaret();

}

BOOL	CScriptText::OnSetCursor				(UINT nHitTest, UINT message) 
{

	CPoint pt;
	::GetCursorPos(&pt);
	m_pWnd->ScreenToClient(&pt);
	if ( m_rcClient.PtInRect( pt ))
	{
		if( pt.x < m_nLeftMargin )
			::SetCursor(m_hCursorArrow);
		else
			::SetCursor(m_hCursor);
		return FALSE;		
	}
	return TRUE;
}

void	CScriptText::OnTimer					(UINT nIDEvent) 
{
	switch( nIDEvent )
	{
		case C_TIMVER_CARET_SHOW:
			{
				if( m_bShowCaret )
				{
					//CPoint pt = m_pWnd->GetCaretPos();
					int nCaretWidth = 2;
					if( m_nInputMode & C_INPUT_MODE_OVERWRITE ||
						m_nInputMode & C_INPUT_MASK_ON_INSERT )
						nCaretWidth = m_nCaretWidth;
					m_pWnd->CreateSolidCaret( nCaretWidth, m_nLineHeight );
					m_pWnd->SetCaretPos( m_ptCaret );
					m_pWnd->ShowCaret();
				}
			}

			break;
	}
}

void	CScriptText::UpdateScrollInfo		()
{
	int nPageSizeX ;
	int nPageSizeY ;
	int nVisibleLine;
	int nLineCount = m_aryLine.GetSize();

	nVisibleLine = GetVisableLineCount() + 1;

	nPageSizeY	= nVisibleLine * 1 / 4;
	nPageSizeX	= m_rcClient.Width() * 3 / 4;

	SCROLLINFO info;
	info.cbSize = sizeof(info);
	info.fMask = SIF_PAGE | SIF_POS | SIF_RANGE | SIF_DISABLENOSCROLL;

	info.nMin	= 0;

	info.nMax	= nLineCount - GetVisableLineCount() + nPageSizeY;
	info.nPage	= nPageSizeY;
	info.nPos	= m_nScrollOffetY;
	m_pWnd->SetScrollInfo( SB_VERT, &info);

	info.nMax	= m_nMaxWidth;
	info.nPage	= nPageSizeX;
	info.nPos	= m_nScrollOffetX;
	m_pWnd->SetScrollInfo( SB_HORZ, &info);

}

void	CScriptText::UpdateCaretPos			()
{
	memset( &m_ptCaret, 0x00, sizeof( m_ptCaret ));
	CRect rcClip = m_rcClient;
	CScriptTextLine* pLine = NULL;

	if( m_nCaretLine != 0 )
	{
		int kkk  = 0;
	}
	CStringUtils::DebugPrint( _T("UpdateCaretPos :: m_nCaretLine = %d / m_nCaretStartPos = %d"), m_nCaretLine, m_nCaretStartPos );
	pLine = GetCaretPos( m_nCaretLine, m_nCaretStartPos, m_ptCaret, m_nCaretWidth );

	rcClip.left = m_nLeftMargin;

	if( ::PtInRect( &rcClip, m_ptCaret ) )
	{
		int nCaretWidth = 2;
		if( m_nInputMode & C_INPUT_MODE_OVERWRITE ||
			m_nInputMode & C_INPUT_MASK_ON_INSERT )
			nCaretWidth = m_nCaretWidth;
		m_pWnd->CreateSolidCaret( nCaretWidth, m_nLineHeight );
		m_bShowCaret	= TRUE;
		m_pWnd->SetCaretPos( m_ptCaret );
		m_pWnd->ShowCaret();
	}
	else
	{
		m_pWnd->SetCaretPos( m_ptCaret );
		if( m_pWnd->GetSafeHwnd() )
			m_pWnd->HideCaret();
		::DestroyCaret();
		m_bShowCaret = FALSE;
	}
}

void	CScriptText::UpdateKeySelection		(BOOL bSkip /*= FALSE */ )
{
	if( bSkip )
		return;

	if( GetKeyState( VK_SHIFT ) & 0x8000 )
	{
		m_stSelection.nEndLine = m_nCaretLine;
		m_stSelection.nEndPos = m_nCaretStartPos;
	}
	else
	{
		m_stSelection = S_SELECTION_INFO();
		m_stSelection.nStartLine = m_nCaretLine;
		m_stSelection.nStartPos = m_nCaretStartPos;
	}

}

void	CScriptText::MoveScrollToCaret			()
{
	int nCaretWidth = 0;
	POINT ptCaret;
	SCROLLINFO info;

	GetCaretPos( m_nCaretLine, m_nCaretStartPos, ptCaret, nCaretWidth );
	
	UINT nMaxLineWidth	= 0;

	if( m_rcClient.right <= ptCaret.x )
	{
		nMaxLineWidth	= GetMaxWidth();
		m_pWnd->GetScrollInfo( SB_HORZ, &info );

		m_nScrollOffetX -= ( m_nLeftMargin - ptCaret.x ) + info.nPage;
		m_nScrollOffetX = max( 0, m_nScrollOffetX );

		if( (int)( nMaxLineWidth - info.nPage ) < m_nScrollOffetX )
			m_nScrollOffetX = nMaxLineWidth - info.nPage;
		UpdateScrollInfo();
	}
	else if( ptCaret.x < m_nLeftMargin )
	{
		// Scroll to be shown the Caret.
		m_nScrollOffetX -= ( m_nLeftMargin - ptCaret.x ) + 20;
		m_nScrollOffetX = max( 0, m_nScrollOffetX );
		UpdateScrollInfo();
	}
	else if( ptCaret.x <= 0 )
	{
		m_nScrollOffetX = 0;
		UpdateScrollInfo();
	}
	
	UINT nLineCount = GetLineCount(); //in lines
	int nVisibleLineCount = min( GetVisableLineCount() - 1 , GetLineCount() - m_nScrollOffetY  );
	if( m_nCaretLine <= m_nScrollOffetY )
	{
		m_pWnd->GetScrollInfo( SB_VERT, &info );
		m_nScrollOffetY = m_nCaretLine - info.nPage;
		m_nScrollOffetY = max( 0, m_nScrollOffetY );
		UpdateScrollInfo();
	}
	else if( m_nScrollOffetY + nVisibleLineCount <= m_nCaretLine )
	{
		m_pWnd->GetScrollInfo( SB_VERT, &info );
		m_nScrollOffetY  = m_nCaretLine - nVisibleLineCount + 1;
		if( (int)( nLineCount - info.nPage ) < m_nScrollOffetY + 1 )
			m_nScrollOffetY = nLineCount - info.nPage;
			
		m_nScrollOffetY = min( m_nScrollOffetY, info.nMax );

		UpdateScrollInfo();
	}

}

void				CScriptText::SetFindTextOption		(BOOL				bMatchCase			,
														BOOL				bWholeWord			, 
														const CString&		strWord				)
{
	m_stFindText	= S_FIND_TEXT_INFO();
	m_stFindText.bMatchCase	= bMatchCase;
	m_stFindText.bWholeWord	= bWholeWord;
	m_stFindText.strText	= strWord;
}

void				CScriptText::GetFindTextOption		(BOOL*				pMatchCase			,
														BOOL*				pWholeWord			,
														CString*			pStrWord			)
{
	if( pMatchCase )
		*pMatchCase = m_stFindText.bMatchCase;
	if( pWholeWord )
		*pWholeWord	= m_stFindText.bWholeWord;
	if( pStrWord )
		*pStrWord	= m_stFindText.strText;
}

BOOL				CScriptText::FindNext				()
{
	if( m_stFindText.nLine == -1 )
	{
		m_stFindText.nLine	= m_nCaretLine;
		m_stFindText.nPos	= m_nCaretStartPos;
	}

	BOOL bRet = FALSE;
	CString strFindWord = m_stFindText.strText;
	CScriptTextLine* pLine = NULL;
	int i;
	int nLineCount = GetLineCount();

	pLine = m_aryLine.GetAt( m_stFindText.nLine );

	if( m_stFindText.bMatchCase == FALSE )
		strFindWord.MakeUpper();

	int nStartPos = m_stFindText.nPos;
	int nLength = CStringUtils::GetLength( strFindWord );
	bRet = pLine->FindText( m_stFindText.bMatchCase, m_stFindText.bWholeWord, strFindWord, nLength, nStartPos );
	if( bRet == TRUE )
		goto e;

	for( i=m_stFindText.nLine+1 ; i<nLineCount ; i++ )
	{
		nStartPos = 0;
		pLine = m_aryLine.GetAt( i );
		bRet = pLine->FindText( m_stFindText.bMatchCase, m_stFindText.bWholeWord, strFindWord, strFindWord.GetLength(), nStartPos );
		if( bRet != TRUE )
			continue;
		m_stFindText.nLine = i;
		goto e;
	}

e:
	if( bRet == TRUE )
	{
		m_stFindText.nPos	= nStartPos;
		m_stSelection		= S_SELECTION_INFO();
		m_stSelection.nStartLine	= m_stFindText.nLine;
		m_stSelection.nStartPos		= m_stFindText.nPos;
		m_stSelection.nEndLine		= m_stFindText.nLine;
		m_stSelection.nEndPos		= m_stSelection.nStartPos + CStringUtils::GetLength( strFindWord );

		m_nCaretLine		= m_stSelection.nEndLine;
		m_nCaretStartPos	= m_stSelection.nEndPos;
		if( m_pListener )
		{
			int nSelChars, nSelLines ;
			GetSelCharsAndLines( nSelChars, nSelLines );
			m_pListener->OnMovedCursor( m_nCaretLine+1 , m_nCaretStartPos+1, nSelChars, nSelLines );
		}

		MoveScrollToCaret();
		UpdateScrollInfo();
		m_pWnd->Invalidate( FALSE );
	}

	return bRet;
}

BOOL				CScriptText::IsBreakPoint			( int	nLineNum )
{
	CScriptTextLine* pLine = m_aryLine.GetAt( nLineNum );
	return  ( pLine->m_dwFlags & C_LINE_MARK_BREAKPOINT ) == C_LINE_MARK_BREAKPOINT;
}

void				CScriptText::SetBreakPoints			(CUIntArray*		pBreakPoints		)
{
	ClearAllBreakPoint();
	
	CScriptTextLine* pLine = NULL;
	int i;
	int nLineNum;
	int nCount = pBreakPoints->GetSize();

	for( i=0 ; i<nCount ; i++ )
	{
		nLineNum = pBreakPoints->GetAt( i );
		pLine = m_aryLine.GetAt( nLineNum );
		pLine->SetBreakPoint();
	}
}

void				CScriptText::AddBreakPoint			(UINT				nLineNum			)
{
	CScriptTextLine* pLine = NULL;
	pLine = m_aryLine.GetAt( nLineNum );
	pLine->SetBreakPoint();

}

void				CScriptText::DeleteBreakPoint		(UINT				nLineNum			)
{
	CScriptTextLine* pLine = NULL;
	pLine = m_aryLine.GetAt( nLineNum );
	pLine->ClearBreakPoint();
}

void				CScriptText::SetDebugingLineNum		(int				nLineNum			)
{
	m_nDebugingLineNum	= nLineNum;
}

void				CScriptText::SetErrorPoints			(CUIntArray*		pErrorPoints		)
{
	ClearAllErrorPoint();
	
	CScriptTextLine* pLine = NULL;
	int i;
	int nCount = pErrorPoints->GetSize();
	int nLineNum = -1;

	for( i=0 ; i<nCount ; i++ )
	{
		nLineNum = pErrorPoints->GetAt( i );
		pLine = m_aryLine.GetAt( nLineNum );
		pLine->SetErrorPoint();
	}

}

int					CScriptText::FindNextPoint			(int				nStartLine			,
														BOOL				bForward			, 
														DWORD				dwMask				)
{
	CScriptTextLine* pLine = NULL;
	int i;
	int nLineCount = GetLineCount();
	if( bForward == TRUE )
	{

		for( i=nStartLine+1 ; i<nLineCount ; i++ )
		{
			pLine = m_aryLine.GetAt( i );
			if( ( pLine->m_dwFlags & dwMask ) == dwMask )
				return i;
		}
	}
	else
	{
		for( i=nStartLine-1 ; 0<=i ; i-- )
		{
			pLine = m_aryLine.GetAt( i );
			if( ( pLine->m_dwFlags & C_LINE_MARK_BOOKMARK ) == dwMask )
				return i;
		}
	}
	return -1;
}

void				CScriptText::ClearAllBookMark		()
{
	CScriptTextLine* pLine = NULL;
	int i;
	int nLineCount = GetLineCount();
	for( i=0 ; i<nLineCount ; i++ )
	{
		pLine = m_aryLine.GetAt( i );
		pLine->ClearBookMark();
	}

}

void				CScriptText::ClearAllErrorPoint		()
{
	CScriptTextLine* pLine = NULL;
	int i;
	int nLineCount = GetLineCount();
	for( i=0 ; i<nLineCount ; i++ )
	{
		pLine = m_aryLine.GetAt( i );
		pLine->ClearErrorPoint();
	}

}

void				CScriptText::ClearAllBreakPoint		()
{
	CScriptTextLine* pLine = NULL;
	int i;
	int nLineCount = GetLineCount();
	for( i=0 ; i<nLineCount ; i++ )
	{
		pLine = m_aryLine.GetAt( i );
		pLine->ClearBreakPoint();
	}
}

void				CScriptText::InsertString			( int& nLine, int& nStartPos, const CString& strText )
{
	CArray<CScriptTextLine*, CScriptTextLine*> aryLines;

	DWORD dwFlags = 0;
	int nNewLines = ParseString( strText, &aryLines );
	if( nNewLines == 0 )
		return;

	int i = 0;
	CScriptTextLine* pLine = NULL;
	CScriptTextLine* pLineNew = NULL;
	CScriptTextLine* pLineAdd = NULL;
	CString strHead, strTail;

	pLine = m_aryLine.GetAt( nLine );
	dwFlags = pLine->m_dwFlags;
	strHead = CStringUtils::Left( pLine->m_strLine, nStartPos );
	strTail = CStringUtils::Mid( pLine->m_strLine, nStartPos );

	if( nNewLines == 1 )
	{
		pLineAdd = aryLines.GetAt( 0 );
		strHead += pLineAdd->m_strLine;
		nStartPos = CStringUtils::GetLength( strHead );
		strHead += strTail;
		delete pLineAdd;
		pLineAdd = NULL;
		pLine->SetLine( strHead );
		aryLines.RemoveAll();

		return;
	}
	pLineAdd = aryLines.GetAt( 0 );
	strHead += pLineAdd->m_strLine;
	pLine->ClearAllFlags();
	pLine->SetLine( strHead );

	delete pLineAdd;
	pLineAdd = NULL;

	for( i=1 ; i<nNewLines-1 ; i++ )
	{
		pLineAdd = aryLines.GetAt( i );
		pLineNew	= new CScriptTextLine();
		pLineNew->SetLine( pLineAdd->m_strLine );
		delete pLineAdd;
		m_aryLine.InsertAt( nLine + i, pLineNew );
	}

	pLineAdd	= aryLines.GetAt( i );
	
	nStartPos	= CStringUtils::GetLength( pLineAdd->m_strLine );
	strTail		= pLineAdd->m_strLine + strTail;
	delete pLineAdd;
	pLineNew	= new CScriptTextLine();
	pLineNew->SetFlags( dwFlags );
	pLineNew->SetLine( strTail );
	m_aryLine.InsertAt( nLine + i, pLineNew );
	nLine		= nLine + i;
}

BOOL				CScriptText::DeleteSelectedTexts		(int& nLine, int& nStartPos, S_SELECTION_INFO& stSelectionSrc, BOOL bAddUndo )
{
	S_SELECTION_INFO stSelection = stSelectionSrc;

	if( stSelection.bSelection == FALSE )
		return FALSE;

	nLine = stSelection.nStartLine;
	nStartPos = stSelection.nStartPos;

	int i;
	CScriptTextLine* pLine = NULL;
	int nLineCount = stSelection.nEndLine - stSelection.nStartLine;
	int nLength = 0;
	CString strValue;
	CString strLeft;
	CString strRight;

	if( bAddUndo == TRUE )
	{
		strValue = GetSelectedText();
		AddUnDoAction( FALSE, C_UNDO_ACT_DELETED, stSelection.nStartLine, stSelection.nStartPos, strValue );
	}

	if( stSelection.nStartLine == stSelection.nEndLine )
	{
		pLine = m_aryLine.GetAt( stSelection.nEndLine );
		nLength			= CStringUtils::GetLength( pLine->m_strLine );
		strRight		= CStringUtils::Mid( pLine->m_strLine, stSelection.nEndPos );
		strLeft			= CStringUtils::Left( pLine->m_strLine, stSelection.nStartPos );
		strValue	= strLeft + strRight;
		pLine->SetLine( strValue );
	}
	else
	{
		CScriptTextLine* pLineEnd = NULL;
		pLine = m_aryLine.GetAt( stSelection.nStartLine );
		pLineEnd = m_aryLine.GetAt( stSelection.nEndLine );

		strLeft			= CStringUtils::Left( pLine->m_strLine, stSelection.nStartPos );
		strRight		= CStringUtils::Mid( pLineEnd->m_strLine, stSelection.nEndPos );
		strValue		= strLeft + strRight;
		pLine->SetFlags( pLineEnd->m_dwFlags );
		pLine->SetLine( strValue );
	}

	for( i=stSelection.nEndLine; stSelection.nStartLine<i ; i-- )
	{
		pLine = m_aryLine.GetAt( i );
		delete pLine ;
		m_aryLine.RemoveAt( i, 1 );
	}

	stSelectionSrc = S_SELECTION_INFO();
	if( GetKeyState( VK_SHIFT ) & 0x8000 )
	{
		stSelectionSrc.nStartLine = nLine;
		stSelectionSrc.nStartPos = nStartPos;
	}

	return TRUE;
}

BOOL				CScriptText::DeleteCharToBackward	( int& nLine, int& nStartPos, BOOL bAddUndo )
{
	int nLineCount = GetLineCount();
	CScriptTextLine* pLine = NULL;
	int nLength = 0;
	CString strValue;

	if( nLine == 0 && nStartPos == 0 )
		return FALSE;

	if( nStartPos == 0 )
	{
		CScriptTextLine* pLinePrev = m_aryLine.GetAt( nLine - 1 );
		pLine = m_aryLine.GetAt( nLine );
		strValue = pLinePrev->m_strLine + pLine->m_strLine;

		if( bAddUndo == TRUE )
			AddUnDoAction( FALSE, C_UNDO_ACT_DELETED, nLine - 1, CStringUtils::GetLength( pLinePrev->m_strLine ), _T("\n"));
		nStartPos = CStringUtils::GetLength( pLinePrev->m_strLine );
		pLinePrev->SetFlags( pLine->m_dwFlags );
		pLinePrev->SetLine( strValue );
		delete pLine;
		m_aryLine.RemoveAt( nLine, 1 );
		nLine --;
	}
	else
	{
		pLine = m_aryLine.GetAt( nLine );
		CString strLeft			= CStringUtils::Left( pLine->m_strLine, nStartPos -1 );
		CString strBeDeleted	= CStringUtils::Mid( pLine->m_strLine, nStartPos -1, 1 );
		CString strRight		= CStringUtils::Mid( pLine->m_strLine, nStartPos );
		strValue = strLeft + strRight;
		pLine->SetLine( strValue );
		nStartPos--;
		if( bAddUndo == TRUE )
			AddUnDoAction( FALSE, C_UNDO_ACT_DELETED, nLine, nStartPos, strBeDeleted );
	}

	

	return TRUE;
	

}

BOOL				CScriptText::DeleteCharToforward	( int& nLine, int& nStartPos, BOOL bAddUndo, BOOL bIsContinue  )
{
	// For DELETE key
	int nLineCount = GetLineCount();
	CScriptTextLine* pLine = NULL;
	int nLength = 0;
	CString strValue;

	if( nLine == nLineCount - 1 ) 
	{
		pLine = m_aryLine.GetAt( nLineCount - 1 );
		nLength = CStringUtils::GetLength( pLine->m_strLine );
		if( nStartPos == nLength )
			return FALSE;
	}

	pLine = m_aryLine.GetAt( nLine );
	nLength = CStringUtils::GetLength( pLine->m_strLine );
	if( nStartPos == nLength )
	{
		// delete line
		CScriptTextLine* pLineNext = m_aryLine.GetAt( nLine + 1 );
		strValue = pLine->m_strLine + pLineNext->m_strLine;
		if( bAddUndo == TRUE )
			AddUnDoAction( bIsContinue, C_UNDO_ACT_DELETED, nLine, CStringUtils::GetLength( pLine->m_strLine ), _T("\n"));
		if( nStartPos == 0 )
			pLine->SetFlags( pLineNext->m_dwFlags );
		pLine->SetLine( strValue );
		delete pLineNext;
		m_aryLine.RemoveAt(nLine + 1 , 1 );
		
	}
	else
	{
		CString strLeft			= CStringUtils::Left( pLine->m_strLine, nStartPos );
		CString strBeDeleted	= CStringUtils::Mid( pLine->m_strLine, nStartPos, 1 );
		CString strRight		= CStringUtils::Mid( pLine->m_strLine, nStartPos+1 );
		strValue = strLeft + strRight;
		pLine->SetLine( strValue );
		if( bAddUndo == TRUE )
			AddUnDoAction( bIsContinue, C_UNDO_ACT_DELETED, nLine, CStringUtils::GetLength( strLeft ), strBeDeleted );
	}


	return TRUE;
}



void				CScriptText::RemoveAll					()
{
	int i = 0;
	int nCount = m_aryLine.GetSize();
	CScriptTextLine* pLine = NULL;
	for( i=0 ; i<nCount ; i++ )
	{
		pLine = m_aryLine.GetAt(i);
		delete pLine;
	}

	m_aryLine.RemoveAll();
}

void				CScriptText::RemoveAll				(CArray<CScriptTextLine*, CScriptTextLine*>* pLines )
{
	int i = 0;
	int nCount = pLines->GetSize();
	CScriptTextLine* pLine = NULL;
	for( i=0 ; i<nCount ; i++ )
	{
		pLine = pLines->GetAt(i);
		delete pLine;
	}

	pLines->RemoveAll();
}

int					CScriptText::CalcMaxNumberWidth		( CDC* pDC, int nMaxLine )
{
	CString strValue;
	strValue.Format( _T("%d"), nMaxLine );
	RECT rc;
	SetRect( &rc, 0, 0, 0, 0 );
	pDC->DrawText( strValue, &rc, DT_CALCRECT );

	return rc.right;
}

int					CScriptText::CalcMaxWidth			()
{
	RECT rc;
	int nMaxWidth = 0;
	int nMaxLength = 0;
	int i = 0;
	int nCount = m_aryLine.GetSize();
	CScriptTextLine* pLine = NULL;

	if( m_pWnd == NULL )
		return 1000;

//	LOGFONT logFont;
//	m_font.GetLogFont( &logFont );
	TEXTMETRIC tm;
	CClientDC dc( m_pWnd );
	dc.SelectObject( &m_font );
	dc.GetTextMetrics( &tm );
	m_nLineHeight = tm.tmHeight;

	int nCaretWidth = 2;
	if( m_nInputMode & C_INPUT_MODE_OVERWRITE ||
		m_nInputMode & C_INPUT_MASK_ON_INSERT )
		nCaretWidth = m_nCaretWidth;
	m_pWnd->CreateSolidCaret( nCaretWidth, m_nLineHeight );

	for( i=0 ; i<nCount ; i++ )
	{
		pLine = m_aryLine.GetAt(i);

		if ( pLine->m_strLine.GetLength() <= nMaxLength )
			continue;

		nMaxLength = pLine->m_strLine.GetLength();
		rc.left = 0 ;
		rc.top = 0;
		dc.DrawText( pLine->m_strLine, &rc, DT_CALCRECT );

		if( rc.right < nMaxWidth )
			continue;

		nMaxWidth = rc.right ;
	}

	return nMaxWidth;
}

int					CScriptText::ParseString		( const CString& strData, CArray<CScriptTextLine*, CScriptTextLine*>* pLines )
{
	CScriptTextLine* pNewLine	= NULL;
	
	LPCTSTR pszBuf = (LPCTSTR)strData;
	CString strLine;
	int nLength = strData.GetLength();
	int nPos = 0;
	int i = 0;
	char szTemp[4] = { NULL, };
	BOOL bFoundNewLine = FALSE;

	DWORD dwTick = 0, dwTotalTick = 0;;

	strLine = _T("");
	for( i=0 ; i<nLength ; i++ )
	{
		if( pszBuf[i] == '\r' )
			continue;

		if ( pszBuf[i] == '\n' )
		{
			pNewLine = new CScriptTextLine();

			dwTick = ::GetTickCount();
			pNewLine->SetLine( strLine );
			dwTick = ::GetTickCount() - dwTick ;

			dwTotalTick += dwTick;
			pLines->Add( pNewLine );

			strLine.Empty();
			nPos = 0;
			bFoundNewLine = TRUE;

		}
		else if ( pszBuf[i] == '\t')
		{
			CString tab = CStringUtils::TabToSpace( nPos );
			nPos		+= tab.GetLength();
			strLine		+= tab;

			bFoundNewLine = FALSE;
		}
		else if ( pszBuf[i] & 0x80 )
		{
			nPos++;
			memcpy( szTemp, &pszBuf[i], 2 );
			szTemp[2] = NULL;
			strLine	+= szTemp;
			bFoundNewLine = FALSE;
			i++;
		}
		else if ( isprint( pszBuf[i] ) )
		{
			nPos++;
			strLine	+= pszBuf[i];

			bFoundNewLine = FALSE;
		}
		
		else
		{
			continue;
			//ASSERT( FALSE );
		}
	}

	if( strLine.GetLength() || bFoundNewLine == TRUE )
	{
		pNewLine = new CScriptTextLine();
		
		dwTick = ::GetTickCount();
		pNewLine->SetLine( strLine );
		dwTick = ::GetTickCount() - dwTick ;
		
		dwTotalTick += dwTick;
		pLines->Add( pNewLine );
		
		strLine.Empty();
	}

	return pLines->GetSize();

}

void				CScriptText::Copy					()
{
	CString strResult = GetSelectedText();

	if( strResult.GetLength() == 0 )
		return;
	
	CStringUtils::PutToClipboard( m_pWnd->GetSafeHwnd(), (LPCTSTR)strResult );

}

void				CScriptText::Cut					()
{
	Copy();
	DeleteSelectedTexts( m_nCaretLine, m_nCaretStartPos, m_stSelection, TRUE );
}

void				CScriptText::Paste					()
{
	DeleteSelectedTexts( m_nCaretLine, m_nCaretStartPos, m_stSelection, TRUE );
	
	CString strValue = CStringUtils::GetFromClipboard( m_pWnd->GetSafeHwnd() );
	
	AddUnDoAction( FALSE, C_UNDO_ACT_INSERTED, m_nCaretLine, m_nCaretStartPos, strValue );
	
	InsertString( m_nCaretLine, m_nCaretStartPos, strValue );
	
	m_stSelection	= S_SELECTION_INFO();
	m_stSelection.nStartLine = m_nCaretLine;
	m_stSelection.nStartPos = m_nCaretStartPos;
	
}

void				CScriptText::SelectAll				()
{
	m_stSelection.nStartLine = 0;
	m_stSelection.nStartPos = 0;
	m_stSelection.nEndLine = GetLineCount() - 1;
	CScriptTextLine* pLine = m_aryLine.GetAt( m_stSelection.nEndLine );
	m_stSelection.nEndPos = CStringUtils::GetLength( pLine->m_strLine );

	if( m_pListener )
	{
		int nSelChars, nSelLines ;
		GetSelCharsAndLines( nSelChars, nSelLines );
		m_pListener->OnMovedCursor( m_nCaretLine+1 , m_nCaretStartPos+1, nSelChars, nSelLines );
	}

}

void				CScriptText::UnDo					()
{
	while( CanUnDo() )
	{
		S_ACTION_INFO	stUnDoData	= GetLastUnDoAction();
		switch( stUnDoData.byteType )
		{
		case C_UNDO_ACT_DELETED:
			{
				AddReDoAction( stUnDoData );

				m_nCaretLine = stUnDoData.nLine;
				m_nCaretStartPos = stUnDoData.nPos;
				InsertString( stUnDoData.nLine, stUnDoData.nPos, stUnDoData.strText );
			}
			break;
		case C_UNDO_ACT_INSERTED:
			{
				AddReDoAction( stUnDoData );

				int nLineCount = 0;
				CScriptTextLine* pLine = NULL;
				CArray<CScriptTextLine*, CScriptTextLine*> aryLines;

				ParseString( stUnDoData.strText, &aryLines );
	
				nLineCount = aryLines.GetSize(); 
				if( nLineCount ==  0 )
					break;
				pLine = aryLines.GetAt( nLineCount -1 );

				S_SELECTION_INFO stSelection;
				stSelection.bSelection	= TRUE;
				stSelection.nStartLine	= stUnDoData.nLine;
				stSelection.nStartPos	= stUnDoData.nPos;
				stSelection.nEndLine	= stSelection.nStartLine + nLineCount-1;
				stSelection.nEndPos		= CStringUtils::GetLength( pLine->m_strLine );
				if( stSelection.nStartLine == stSelection.nEndLine )
					stSelection.nEndPos	+= stSelection.nStartPos;
				DeleteSelectedTexts( m_nCaretLine, m_nCaretStartPos, stSelection, FALSE );

				RemoveAll( &aryLines );

				
			}
			break;
		default:
			return;
		}

		if( stUnDoData.bIsContinue == FALSE )
			break;
	}
	m_stSelection	= S_SELECTION_INFO();
	m_stSelection.nStartLine = m_nCaretLine;
	m_stSelection.nStartPos = m_nCaretStartPos;


}

void				CScriptText::ReDo					()
{
	while( CanReDo() )
	{
		S_ACTION_INFO	stReDoData	= GetLastReDoAction();
		switch( stReDoData.byteType )
		{
		case C_UNDO_ACT_DELETED:
			{
				AddUnDoAction( stReDoData );

				int nLineCount = 0;
				CScriptTextLine* pLine = NULL;
				CArray<CScriptTextLine*, CScriptTextLine*> aryLines;

				ParseString( stReDoData.strText, &aryLines );
	
				nLineCount = aryLines.GetSize(); 
				if( nLineCount ==  0 )
					break;
				pLine = aryLines.GetAt( nLineCount -1 );

				S_SELECTION_INFO stSelection;
				stSelection.bSelection	= TRUE;
				stSelection.nStartLine	= stReDoData.nLine;
				stSelection.nStartPos	= stReDoData.nPos;
				stSelection.nEndLine	= stSelection.nStartLine + nLineCount-1;
				stSelection.nEndPos		= CStringUtils::GetLength( pLine->m_strLine );
				if( stSelection.nStartLine == stSelection.nEndLine )
					stSelection.nEndPos	+= stSelection.nStartPos;
				DeleteSelectedTexts( m_nCaretLine, m_nCaretStartPos, stSelection, FALSE );

				RemoveAll( &aryLines );
			}
			break;
		case C_UNDO_ACT_INSERTED:
			{
				AddUnDoAction( stReDoData );

				m_nCaretLine = stReDoData.nLine;
				m_nCaretStartPos = stReDoData.nPos;
				InsertString( m_nCaretLine, m_nCaretStartPos, stReDoData.strText );
			}
			break;
		default:
			return;
		}

		if( stReDoData.bIsContinue == FALSE )
			break;
	}
	m_stSelection	= S_SELECTION_INFO();
	m_stSelection.nStartLine = m_nCaretLine;
	m_stSelection.nStartPos = m_nCaretStartPos;

}

void				CScriptText::AddUnDoAction			( BOOL bIsContinue, BYTE byteType, int nLine, int nPos, const CString& strText )
{
	if( strText.GetLength() == 0 )
		return;

	m_aryRedo.RemoveAll();

	S_ACTION_INFO stNewAction;
	stNewAction.bIsContinue		= bIsContinue;
	stNewAction.byteType		= byteType;
	stNewAction.nLine			= nLine;
	stNewAction.nPos			= nPos;
	stNewAction.strText			= strText;
	m_aryUndo.Add( stNewAction );

	if( C_MAX_UNDO_COUNT < m_aryUndo.GetSize() )
		m_aryUndo.RemoveAt( 0 );

}

void				CScriptText::AddUnDoAction			( const S_ACTION_INFO& stAction )
{
	m_aryUndo.Add( stAction );

	if( C_MAX_UNDO_COUNT < m_aryUndo.GetSize() )
		m_aryUndo.RemoveAt( 0 );

}

S_ACTION_INFO		CScriptText::GetLastUnDoAction		()
{
	int nCount = m_aryUndo.GetSize();
	if( nCount == 0 )
		return S_ACTION_INFO();
	S_ACTION_INFO stUnDo = m_aryUndo.GetAt( nCount -1 );
	m_aryUndo.RemoveAt( nCount -1 );

	return stUnDo;
}

void				CScriptText::AddReDoAction			( const S_ACTION_INFO& stAction )
{
	m_aryRedo.Add( stAction );

	if( C_MAX_UNDO_COUNT < m_aryRedo.GetSize() )
		m_aryRedo.RemoveAt( 0 );

}

S_ACTION_INFO		CScriptText::GetLastReDoAction		()
{
	int nCount = m_aryRedo.GetSize();
	if( nCount == 0 )
		return S_ACTION_INFO();
	S_ACTION_INFO stReDo = m_aryRedo.GetAt( nCount -1 );
	m_aryRedo.RemoveAt( nCount -1 );

	return stReDo;
}

