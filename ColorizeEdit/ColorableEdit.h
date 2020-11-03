#if !defined(AFX_COLORABLEEDIT_H__F9844BCA_48CC_43DE_973A_4DD977A514FD__INCLUDED_)
#define AFX_COLORABLEEDIT_H__F9844BCA_48CC_43DE_973A_4DD977A514FD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorableEdit.h : header file
//

#include "ScriptText.h"
/////////////////////////////////////////////////////////////////////////////
// CColorableEdit window


class CColorableEdit : public CWnd 
{
// Construction
public:
//	DECLARE_CONTROL(CColorableEdit, CWnd);
	DECLARE_DYNAMIC(CColorableEdit);

	CColorableEdit(CWnd * pParent, const CRect & rect, int nID );

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorableEdit)
	public:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CColorableEdit();
	
	// Generated message map functions
protected:
	//{{AFX_MSG(CColorableEdit)
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg UINT OnGetDlgCode();
	
	//}}AFX_MSG
	afx_msg LRESULT OnImeComposition( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnImeEndComposition( WPARAM wParam, LPARAM lParam );
	
	DECLARE_MESSAGE_MAP()


public:
	CScriptText				m_stScriptText			;
	IListenerScriptText*	m_pListener				;

public:
	BOOL				Init					( IListenerScriptText * pListener );
	void				SetFont					( int nFontSize, const CString& strFontName );
	BOOL				ReadFile				( const CString& strFileName );
	BOOL				SaveFile				( const CString& strFileName );

public:	// Find Text
	void				SetFindTextOption		(BOOL				bMatchCase			,
												BOOL				bWholeWord			, 
												const CString&		strWord				);
	void				GetFindTextOption		(BOOL*				pMatchCase			,
												BOOL*				pWholeWord			,
												CString*			pStrWord			);
	BOOL				FindNext				();

public:	// Screen
	void				MoveToLine				( const int& nLine );

public:
	CString				GetLineTextFromCursor	( int&	nLineNum );
	CString				GetLineTextFromPoint	( const POINT&				pt,
												  int&						nLineNumer );

	BOOL				IsBreakPoint			( int	nLineNum );
	void				SetBreakPoints			(CUIntArray*		pBreakPoints		);
	void				AddBreakPoint			(UINT				nLineNum			);
	void				DeleteBreakPoint		(UINT				nLineNum			);
	void				SetDebugingLineNum		(int				nLineNum			);
	void				SetErrorPoints			(CUIntArray*		pErrorPoints		);

public:	// Undo , ReDo
	BOOL				IsSelected				() { return m_stScriptText.IsSelected(); }
	CString				GetSelectedText			() { return m_stScriptText.GetSelectedText(); }
	void				Copy					() { m_stScriptText.Copy(); }
	void				Cut						() { m_stScriptText.Cut(); }
	void				Paste					() { m_stScriptText.Paste(); }
	void				SelectAll				() { m_stScriptText.SelectAll(); }
	inline BOOL			CanUnDo					() { return m_stScriptText.CanUnDo(); }
	void				UnDo					() { m_stScriptText.UnDo(); } 
	inline BOOL			CanReDo					() { return m_stScriptText.CanReDo(); }
	void				ReDo					() { m_stScriptText.ReDo(); }


public:

public:
	BOOL		m_bBeginComposition	;
	char		m_szBuffer[4];
	UINT		m_nID	;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORABLEEDIT_H__F9844BCA_48CC_43DE_973A_4DD977A514FD__INCLUDED_)
