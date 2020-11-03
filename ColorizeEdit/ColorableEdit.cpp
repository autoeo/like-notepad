// ColorableEdit.cpp : implementation file
//

#include "stdafx.h"
//#include "ColorEdit.h"
#include "ColorableEdit.h"
#include <imm.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorableEdit
//IMPLEMENT_CONTROL(CColorableEdit, CWnd);
IMPLEMENT_DYNAMIC(CColorableEdit, CWnd);


CColorableEdit::CColorableEdit(CWnd * pParent, const CRect& rc, int nID )
{
	m_pListener = NULL;
	DWORD dwFlags = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_VSCROLL | WS_HSCROLL;

	SetScrollRange(SB_HORZ,0,200,false);
	SetScrollRange(SB_VERT,0,200,false);
	m_nID	= nID;
	CWnd::Create(NULL, NULL, dwFlags, rc, pParent, nID, NULL);

	m_bBeginComposition = FALSE;
	memset( m_szBuffer, 0x00, sizeof(m_szBuffer));

}

CColorableEdit::~CColorableEdit()
{
}


BEGIN_MESSAGE_MAP(CColorableEdit, CWnd)
	//{{AFX_MSG_MAP(CColorableEdit)
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_SHOWWINDOW()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_SETCURSOR()
	ON_WM_TIMER()
	ON_WM_GETDLGCODE()
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
	ON_MESSAGE( WM_IME_COMPOSITION, OnImeComposition )
	ON_MESSAGE( WM_IME_ENDCOMPOSITION , OnImeEndComposition )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorableEdit message handlers
void CColorableEdit::OnDestroy() 
{
	CWnd::OnDestroy();
	
	m_stScriptText.Release();
	
}


void CColorableEdit::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	CRect rcClient;
	GetClientRect( &rcClient ); 
	
	DWORD dwTick = ::GetTickCount();
	m_stScriptText.Draw( &dc, rcClient.Width(), rcClient.Height() );

	dwTick = ::GetTickCount() - dwTick;

//	CUtils::DebugPrint( _T("CColorableEdit::OnPaint()  dwTick = %d"), dwTick );
}

void CColorableEdit::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);

	
	m_stScriptText.OnSize( cx, cy );
	
}


void CColorableEdit::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	m_stScriptText.OnVScroll( nSBCode, nPos );
}

void CColorableEdit::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	m_stScriptText.OnHScroll( nSBCode, nPos );

}

BOOL CColorableEdit::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	DWORD dwStyle = GetStyle();
	CScrollBar* pBar = GetScrollBarCtrl(SB_VERT);
	BOOL bEnabledScrollBar = ((pBar != NULL) && pBar->IsWindowEnabled()) || (dwStyle & WS_VSCROLL);
	
	if( nFlags & ( MK_SHIFT | MK_CONTROL ) )
	{
		m_stScriptText.OnMouseWheelWithFuncKey( nFlags, zDelta );
		Invalidate( FALSE );
	}
	else if ( bEnabledScrollBar  )
	{
		m_stScriptText.OnMouseWheel( zDelta );
		Invalidate( FALSE );
		return TRUE;
	}

	return TRUE;	

}

void CColorableEdit::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	m_stScriptText.OnMouseMove(nFlags, point);

	CWnd::OnMouseMove(nFlags, point);
}

void CColorableEdit::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	m_stScriptText.OnLButtonDblClk(nFlags, point) ;
	
	CWnd::OnLButtonDblClk(nFlags, point);
}

void CColorableEdit::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CWnd* pWnd = SetFocus();
	// TODO: Add your message handler code here and/or call default
	if( m_bBeginComposition )
	{
		// Finish Compositionning
		GetParent()->SetFocus();
		SetFocus();
	}

	m_stScriptText.OnLButtonDown( nFlags, point );

	CWnd::OnLButtonDown(nFlags, point);
}

void CColorableEdit::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	m_stScriptText.OnLButtonUp( nFlags, point);
	
	CWnd::OnLButtonUp(nFlags, point);
}

void CColorableEdit::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	int nLine = 0;
	CString strLine;
	ScreenToClient( &point );

	m_stScriptText.OnContextMenu( pWnd, point, nLine, strLine );
	if( m_pListener )
		m_pListener->OnContextMenu( pWnd, point, nLine, strLine );
	
}

void CColorableEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	m_stScriptText.OnKeyDown(nChar, nRepCnt, nFlags);
	
	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}



void CColorableEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	
	m_stScriptText.OnChar( nChar, nRepCnt, nFlags );

	CWnd::OnChar(nChar, nRepCnt, nFlags);
}

BOOL		CColorableEdit::Init			( IListenerScriptText * pListener )
{
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_VSCROLL | WS_HSCROLL ;
	
	dwStyle = ::GetWindowLong( m_hWnd, GWL_STYLE );

	dwStyle |= WS_VSCROLL | WS_HSCROLL;
	::SetWindowLong( m_hWnd, GWL_STYLE, dwStyle );

	m_pListener = pListener;
	m_stScriptText.Init( this, pListener, IDB_DEBUG_VIEW_MARK );

	return TRUE;

}

void		CColorableEdit::SetFont					( int nFontSize, const CString& strFontName )
{
	m_stScriptText.SetFont( nFontSize, strFontName );
}

BOOL		CColorableEdit::ReadFile		( const CString& strFileName )
{

//	return m_stScriptText.Load1( strFileName );
	
	return m_stScriptText.Load( strFileName );
}

BOOL		CColorableEdit::SaveFile				( const CString& strFileName )
{
	return m_stScriptText.Save( strFileName );
}

void		CColorableEdit::SetFindTextOption		(BOOL				bMatchCase			,
													BOOL				bWholeWord			, 
													const CString&		strWord				)
{
	m_stScriptText.SetFindTextOption( bMatchCase, bWholeWord, strWord );
}

void		CColorableEdit::GetFindTextOption		(BOOL*				pMatchCase			,
													BOOL*				pWholeWord			,
													CString*			pStrWord			)
{
	m_stScriptText.GetFindTextOption( pMatchCase, pWholeWord, pStrWord );
}

BOOL		CColorableEdit::FindNext				()
{
	return m_stScriptText.FindNext();
}

CString		CColorableEdit::GetLineTextFromCursor	( int&	nLineNum )
{
	CScriptTextLine* pLine = m_stScriptText.GetCursorLine( nLineNum );
	return pLine->m_strLine;
}

CString		CColorableEdit::GetLineTextFromPoint	( const POINT&				pt,
													  int&						nLineNumer )
{
	return m_stScriptText.GetLineTextFromPoint( pt, nLineNumer );
}

void		CColorableEdit::MoveToLine				( const int& nLine )
{
	m_stScriptText.MoveToLine( nLine );
	SetFocus();

}

BOOL		CColorableEdit::IsBreakPoint			( int	nLineNum )
{
	return m_stScriptText.IsBreakPoint( nLineNum );
}

void		CColorableEdit::SetBreakPoints			(CUIntArray*		pBreakPoints		)
{
	m_stScriptText.SetBreakPoints( pBreakPoints );
}

void		CColorableEdit::AddBreakPoint			(UINT				nLineNum			)
{
	m_stScriptText.AddBreakPoint( nLineNum );
}

void		CColorableEdit::DeleteBreakPoint		(UINT				nLineNum			)
{
	m_stScriptText.DeleteBreakPoint( nLineNum );
}

void		CColorableEdit::SetDebugingLineNum		(int				nLineNum			)
{
	m_stScriptText.SetDebugingLineNum( nLineNum );
}

void		CColorableEdit::SetErrorPoints			(CUIntArray*		pErrorPoints		)
{
	m_stScriptText.SetErrorPoints( pErrorPoints );
}


void CColorableEdit::OnShowWindow(BOOL bShow, UINT nStatus) 
{

	CWnd::OnShowWindow(bShow, nStatus);
	
}

void CColorableEdit::OnSetFocus(CWnd* pOldWnd) 
{
	CWnd::OnSetFocus(pOldWnd);

	m_stScriptText.OnSetFocus();
	
	m_stScriptText.UpdateCaretPos();
	
}

void CColorableEdit::OnKillFocus(CWnd* pNewWnd) 
{
	CWnd::OnKillFocus(pNewWnd);
	
	m_stScriptText.OnKillFocus();
	
}

BOOL CColorableEdit::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if( m_stScriptText.OnSetCursor( nHitTest, message ) == FALSE )
		return TRUE;
	
	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

void CColorableEdit::OnTimer(UINT nIDEvent) 
{
	m_stScriptText.OnTimer( nIDEvent );
	
	CWnd::OnTimer(nIDEvent);
}

UINT CColorableEdit::OnGetDlgCode()
{
	// SDI 또는 MDI 에 포함 시킬 경우 반듯이 이 코드 포함해야 함. 
	return DLGC_WANTALLKEYS;
}


LRESULT CColorableEdit::OnImeComposition( WPARAM wParam, LPARAM lParam )
{

	//	DWORD dwEvent = IMN_OPENCANDIDATE;
	DWORD dwBufLen; // composition 길이
	char szCompStr[4] = { NULL, }; // composition 메모리
	char * lpCompStrAttr; // composition 속성 메모리
	DWORD dwBufLenAttr;
	LPARAM    CompFlag = (LPARAM)lParam;
	HIMC    hIMC;

	// 기본 IME를 취득한다.
	hIMC = ImmGetContext( GetSafeHwnd() );
	if( !hIMC )
		return 0;

	// composition의 길이를 구한다.
	if( CompFlag & GCS_COMPSTR )
		dwBufLen = ::ImmGetCompositionString( hIMC, GCS_COMPSTR, (void FAR*)NULL, 0l );
	else if( CompFlag & GCS_RESULTSTR )
		dwBufLen = ::ImmGetCompositionString( hIMC, GCS_RESULTSTR, (void FAR*)NULL, 0l );

	if( dwBufLen < 0 )
		goto exit2;

	// composition을 얻는다.
	if( CompFlag & GCS_COMPSTR )
		::ImmGetCompositionString( hIMC, GCS_COMPSTR, szCompStr, dwBufLen );
	else if( CompFlag & GCS_RESULTSTR )
		::ImmGetCompositionString( hIMC, GCS_RESULTSTR, szCompStr, dwBufLen );


	szCompStr[dwBufLen] = 0x00;

	if( CompFlag & GCS_COMPATTR )
	{

		if( (dwBufLenAttr = ::ImmGetCompositionString( hIMC, GCS_COMPATTR, (void FAR *)NULL, 0l )) < 0 )
			goto nothing;

		if( (lpCompStrAttr = new char[dwBufLenAttr + 2]) == NULL )
			goto nothing;
		memset( lpCompStrAttr, 0x00, dwBufLenAttr + 2 );

		::ImmGetCompositionString( hIMC, GCS_COMPATTR, lpCompStrAttr, dwBufLenAttr );

		lpCompStrAttr[dwBufLenAttr] = 0;

	}
	else
	{
nothing:
		lpCompStrAttr = NULL;
	}

	if( dwBufLen > 0 )
	{
		if( CompFlag & GCS_RESULTSTR )
		{
			// 완성된 한글에 대한 처리를 하는 곳
			// szCompStr 
			m_bBeginComposition = FALSE;
			memcpy( m_szBuffer, szCompStr, dwBufLen );
			m_stScriptText.OnChar( m_szBuffer, !m_bBeginComposition );
			goto exit1;
		}
		else
		{
			// 미 완성된 한글에 대한 처리를 하는 곳
			m_bBeginComposition = TRUE;
			memcpy( m_szBuffer, szCompStr, dwBufLen );
			m_stScriptText.OnChar( m_szBuffer, !m_bBeginComposition );
			goto exit1;
		}
	}
	else
	{
		// 한글입력이 들어오다가 지워진 경우이다.
		// (가ㄴ에서 Back Space가 들어 오는 경우)
		// (이런 경우에는 길이가 0이다.)
		m_stScriptText.OnChar( "", TRUE );
	}

exit1:
	if( lpCompStrAttr )
		delete[] lpCompStrAttr;
exit2:

	// 기본 IME를 반환한다.
	ImmReleaseContext( GetSafeHwnd(), hIMC );
	return 0;
}

LRESULT CColorableEdit::OnImeEndComposition( WPARAM wParam, LPARAM lParam )
{
	return 0;
}

