// ColorEditDlg.h : header file
//

#if !defined(AFX_COLOREDITDLG_H__7ED3F07A_5A33_46DC_95DA_69E189C1CF5F__INCLUDED_)
#define AFX_COLOREDITDLG_H__7ED3F07A_5A33_46DC_95DA_69E189C1CF5F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CColorEditDlg dialog
#include "ColorableEdit.h"

class CColorEditDlg : public CDialog, IListenerScriptText
{
// Construction
public:
	CColorEditDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CColorEditDlg)
	enum { IDD = IDD_COLOREDIT_DIALOG };
	CStatic	m_ctrlStatusBar;
	CString	m_strPathName;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorEditDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL
	virtual void OnOK() {}
// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CColorEditDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBtnOpenInputTextPathname();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	// CListenerScriptText
	virtual		void			OnReadFile			( DWORD dwFileSize, BOOL bIsCRLF, DWORD dwLineCount );
	virtual		BOOL			OnClickedDebugPoint	( int nLine, const CString& strLine );
	virtual		void			OnMovedCursor		( int nLine, int nPos, int nSelChars, int nSelLines );
	virtual		void			OnContextMenu		( CWnd* pWnd, CPoint point, const int nLine, const CString& strLine );

public:
	CColorableEdit*	m_pEdit;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLOREDITDLG_H__7ED3F07A_5A33_46DC_95DA_69E189C1CF5F__INCLUDED_)
