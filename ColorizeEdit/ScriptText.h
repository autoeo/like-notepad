// ScriptText.h: interface for the CScriptText class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCRIPTTEXT_H__6E264AA8_FC35_4C8D_B403_71D211A93B9C__INCLUDED_)
#define AFX_SCRIPTTEXT_H__6E264AA8_FC35_4C8D_B403_71D211A93B9C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>
#include "ScriptTextLine.h"
#include "StringUtils.h"
#include "IScriptTextColorBase.h"

#define		C_TIMVER_CARET_SHOW		100

//class IListenerLoadThread
//{
//public:
//	virtual void		OnStartedLoading		() = 0;
//	virtual	void		OnLoadedVisibleLines	() = 0;
//	virtual	void		OnFinishedLoading		() = 0;
//};

class IListenerScriptText
{
public:
	virtual		void			OnReadFile			( DWORD dwFileSize, BOOL bIsCRLF, DWORD dwLineCount )= 0;
	virtual		BOOL			OnClickedDebugPoint	( int nLine, const CString& strLine ) = 0;
	virtual		void			OnMovedCursor		( int nLine, int nPos, int nSelChars, int nSelLines ) = 0;
	virtual		void			OnContextMenu		( CWnd* pWnd, CPoint point, const int nLine, const CString& strLine ) = 0;
};

class CScriptText  // : public IListenerLoadThread
{
public:
	CScriptText();
	virtual ~CScriptText();

public:
	BOOL				Init					(CWnd* pWndOnwer , IListenerScriptText* pListener, UINT nImgId );
	void				Release					();
	BOOL				Load					( const CString& strFileName );
	BOOL				Save					( const CString& strPathName );
	void				SetDefault				();
	// GET // SET
	inline void			SetScrollOffsetX		( int nX ) { m_nScrollOffetX = nX; }
	inline int			GetScrollOffsetX		() const { return m_nScrollOffetX; }
	inline void			SetScrollOffsetY		( int nY ) { m_nScrollOffetY = nY; }
	inline int			GetScrollOffsetY		() const { return m_nScrollOffetY; }
	inline int			GetLineHeight			() const { return m_nLineHeight; }
	inline int			GetLineCount			() const { return m_aryLine.GetSize(); }
	inline int			GetMaxWidth				() const { return m_nMaxWidth; }
	inline int			GetVisableLineCount		() const { return m_rcClient.Height() / m_nLineHeight; }

	inline UINT			GetFont					( CString& strFontName ) const { strFontName = m_strFontName; return m_nFontSize; }

	void				SetFont					( int nFontSize, const CString& strFontName );
	CString				GetSelectedText			();
	void				GetSelCharsAndLines		( int& nChars, int& nLines );
	CString				GetLineTextFromPoint	( const POINT& pt, int& nLineNumer );

	
	// Event 
	BOOL				HitTest					( int x, int y, int& nLine, int& nStartPos );
	BOOL				HitTest					( int x, int y, int& nLine, int& nStartPos, CString& strSelWord );
	CScriptTextLine*	GetCaretPos				( int nLine, int nStartPos, POINT& pt, int& nCharWidth );
	CScriptTextLine*	GetCursorLine			( int& nLine );
	void				MoveToLine				( const int& nLine );
	
	void				Draw					(CDC* pDC, int cx, int cy );
	void				DrawLineNum				(CDC* pDC, RECT& rc, int nLineNum, CBrush* pBrush );
	void				DrawMark				(CDC* pDC, RECT& rc, int nIconIndex, CBrush* pBrush, const DWORD& dwFlags, BOOL m_bIsDebugingLine   );


	// Message 
	void				OnSize					(int cx, int cy );
	void				OnVScroll				(UINT nSBCode, UINT nPos );
	void				OnHScroll				(UINT nSBCode, UINT nPos );
	void				OnLButtonDblClk			(UINT nFlags, CPoint point);
	void				OnLButtonDown			(UINT nFlags, CPoint point);
	void				OnLButtonUp				(UINT nFlags, CPoint point);
	void				OnContextMenu			(CWnd* pWnd, CPoint point, int& nLine, CString& strLine );
	void				OnKeyDown				(UINT nChar, UINT nRepCnt, UINT nFlags);
	void				OnKeyDown_Return		(UINT nChar, UINT nRepCnt, UINT nFlags);
	BOOL				OnKeyDown_FunctionKey	(UINT nChar);
	void				OnChar					(UINT nChar, UINT nRepCnt, UINT nFlags);
	void				OnChar					(const CString& strChar, BOOL bComplete );
	void				OnMouseMove				(UINT nFlags, CPoint point);
	void				OnMouseWheel			(short zDelta);
	void				OnMouseWheelWithFuncKey	(UINT nFlags, short zDelta);
	void				OnSetFocus				();
	void				OnKillFocus				();
	BOOL				OnSetCursor				(UINT nHitTest, UINT message) ;
	void				OnTimer					(UINT nIDEvent) ;
	void				UpdateScrollInfo		();
	void				UpdateCaretPos			();
	void				UpdateKeySelection		(BOOL bSkip = FALSE );
	void				MoveScrollToCaret		();

public:	// Find Text
	void				SetFindTextOption		(BOOL				bMatchCase			,
												BOOL				bWholeWord			, 
												const CString&		strWord				);
	void				GetFindTextOption		(BOOL*				pMatchCase			,
												BOOL*				pWholeWord			,
												CString*			pStrWord			);
	BOOL				FindNext				();


public:	// Debug
	BOOL				IsBreakPoint			( int	nLineNum );
	void				SetBreakPoints			(CUIntArray*		pBreakPoints		);
	void				AddBreakPoint			(UINT				nLineNum			);
	void				DeleteBreakPoint		(UINT				nLineNum			);
	void				SetDebugingLineNum		(int				nLineNum			);
	void				SetErrorPoints			(CUIntArray*		pErrorPoints		);

	int					FindNextPoint			(int				nStartLine			,
												BOOL				bForward			, 
												DWORD				dwMask				);
	void				ClearAllBookMark		();
	void				ClearAllErrorPoint		();
	void				ClearAllBreakPoint		();

private:
	
	void				InsertString			( int& nLine, int& nStartPos, const CString& strText );
	BOOL				DeleteSelectedTexts		( int& nLine, int& nStartPos, S_SELECTION_INFO& stSelectionSrc, BOOL bAddUndo );
	BOOL				DeleteCharToBackward	( int& nLine, int& nStartPos, BOOL bAddUndo  );
	BOOL				DeleteCharToforward		( int& nLine, int& nStartPos, BOOL bAddUndo, BOOL bIsContinue  );
	void				RemoveAll				();
	void				RemoveAll				(CArray<CScriptTextLine*, CScriptTextLine*>* pLines );
	int					CalcMaxNumberWidth		( CDC* pDC, int nMaxLine );
	int					CalcMaxWidth			();
	int					ParseString				( const CString& strData, CArray<CScriptTextLine*, CScriptTextLine*>* pLines );


public:	// Undo , ReDo
	BOOL				IsSelected				() { S_SELECTION_INFO	stSelection	= m_stSelection; return stSelection.bSelection; }
	void				Copy					();
	void				Cut						();
	void				Paste					();
	void				SelectAll				();
	inline BOOL			CanUnDo					() { return m_aryUndo.GetSize() != 0; }
	void				UnDo					();
	inline BOOL			CanReDo					() { return m_aryRedo.GetSize() != 0; }
	void				ReDo					();
private:
	void				AddUnDoAction			( BOOL bIsContinue, BYTE byteType, int nLine, int nPos, const CString& strText );
	void				AddUnDoAction			( const S_ACTION_INFO& stAction );
	S_ACTION_INFO		GetLastUnDoAction		();
	void				AddReDoAction			( const S_ACTION_INFO& stAction );
	S_ACTION_INFO		GetLastReDoAction		();

private:
	CString						m_strPathName			;
	CString						m_strTextBuffer			;
	BOOL						m_bIsCRLF				;
	CArray<CScriptTextLine*, CScriptTextLine*>			m_aryLine			;
	CArray<S_ACTION_INFO, S_ACTION_INFO>				m_aryUndo			;
	CArray<S_ACTION_INFO, S_ACTION_INFO>				m_aryRedo			;
	IListenerScriptText*		m_pListener				;
	HANDLE						m_hEvent				;
	BOOL						m_bRunThread			;

	UINT						m_nFontSize				;
	CString						m_strFontName			;
	UINT						m_nVisibleEndLine		;
	int							m_nLeftMargin			;
	int							m_nMaxNumWidth			;
	long						m_nScrollOffetX			;
	long						m_nScrollOffetY			;
	UINT						m_nMaxWidth				;
	int							m_nLineHeight			;
	HCURSOR						m_hCursor				;
	HCURSOR						m_hCursorArrow			;
	CImageList					m_imglistMark			;
	int							m_nDebugingLineNum		;
	BYTE						m_nInputMode			;
	BOOL						m_bShowCaret			;
	int							m_nCaretLine			;
	int							m_nCaretStartPos		;
	POINT						m_ptCaret				;
	int							m_nCaretWidth			;
	S_SELECTION_INFO			m_stSelection			;
	BOOL						m_bLButtonDown			;
	CPoint						m_ptLbuttonDown			;
	S_FIND_TEXT_INFO			m_stFindText			;
	
	CWnd*						m_pWnd					;
	CRect						m_rcClient				;
	IScriptTextColorBase*		m_pTextColorTable		;
	LOGFONT						m_logFont				;
	CFont						m_font					;
	CFont*						m_pOldFont				;
	CDC							m_memDC_Font			;

};

#endif // !defined(AFX_SCRIPTTEXT_H__6E264AA8_FC35_4C8D_B403_71D211A93B9C__INCLUDED_)
