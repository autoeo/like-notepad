// ScriptTextLine.h: interface for the CScriptTextLine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCRIPTTEXTLINE_H__498A2C34_9F64_4266_9941_6BF279D75586__INCLUDED_)
#define AFX_SCRIPTTEXTLINE_H__498A2C34_9F64_4266_9941_6BF279D75586__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <afxtempl.h>
#include "ScriptTextWord.h"

#define		C_LINE_MARK_NONE			0x00
#define		C_LINE_MARK_BOOKMARK		0x01
#define		C_LINE_MARK_ERRORPOINT		0x02
#define		C_LINE_MARK_BREAKPOINT		0x04


class CScriptTextLine  
{
public:
	CScriptTextLine();
	virtual ~CScriptTextLine();
	CScriptTextLine( const CScriptTextLine& st );
	CScriptTextLine& operator = (const CScriptTextLine& st );

public:
	inline void			SetFlags				( const DWORD& dwFlags ) { m_dwFlags = dwFlags; }
	inline void			ClearAllFlags			() { m_dwFlags = C_LINE_MARK_NONE; }
	inline void			ToggleBookMark			() { m_dwFlags ^= C_LINE_MARK_BOOKMARK; }
	inline void			SetBookMark				() { m_dwFlags |= C_LINE_MARK_BOOKMARK; }
	inline void			ClearBookMark			() { m_dwFlags &= ~C_LINE_MARK_BOOKMARK; }
	inline void			ToggleErrorPoint		() { m_dwFlags ^= C_LINE_MARK_ERRORPOINT; }
	inline void			SetErrorPoint			() { m_dwFlags |= C_LINE_MARK_ERRORPOINT; }
	inline void			ClearErrorPoint			() { m_dwFlags &= ~C_LINE_MARK_ERRORPOINT; }
	inline void			ToggleBreakPoint		() { m_dwFlags ^= C_LINE_MARK_BREAKPOINT; }
	inline void			SetBreakPoint			() { m_dwFlags |= C_LINE_MARK_BREAKPOINT; }
	inline void			ClearBreakPoint			() { m_dwFlags &= ~C_LINE_MARK_BREAKPOINT; }

	int					SetLine					(	const CString&			strLine					);
	CScriptTextWord*	OnAddWord				(	const CString&			strWord					);
	void				RemoveAll				();

public:
	BOOL				HitTest					(	int						x						,
													int						y						,
													int&					nStartPos				,
													CDC*					pDC						);
	BOOL				HitTest					(	int						x						,
													int						y						,
													int&					nStartPos				,
													CDC*					pDC						,
													CString&				strSelWord				);
	int					GetStartPosFromX		(	int						x						,
													int&					nStartPos				,
													CDC*					pDC						);
	BOOL				IsEndPos				(	int						nCaretPos				);
	int					GetCaretPosX			(	CDC*					pDC						,
													int						nStartPos				,
													int&					nCharWidth				);
	RECT&				GetRect					();
	void				GetRect					(	CDC*					pDC						,
													CFont*					pFont					,
													RECT*					pRect					);
	int					GetNextWordPos			(	int						nStartCharPos			);
	int					GetPrefWordPos			(	int						nStartCharPos			);
	void				Draw					(	CDC*					pDC						,
													int						nLineHeight				,
													int						x						,
													int						y						,
													const RECT&				rcClient				,
													int						nLineNum				,
													S_SELECTION_INFO*		pSelection				,
													BOOL					bCursorLine				,
													IScriptTextColorBase*	pTextColor				);
	BOOL				FindText				(	const BOOL&				bMatchCase				,
													const BOOL&				bWholeWord				,
													const CString&			strWord					, 
													const int&				nLength					, 
													int&					nStartPos				);
public:
	CString				m_strLine			;
	RECT				m_rcLine			;
	DWORD				m_dwFlags			;		// for Jump Mark, BreakPoints, ErrorPoints.
	CArray<CScriptTextWord*, CScriptTextWord*>			m_aryWrod			;

};

#endif // !defined(AFX_SCRIPTTEXTLINE_H__498A2C34_9F64_4266_9941_6BF279D75586__INCLUDED_)

