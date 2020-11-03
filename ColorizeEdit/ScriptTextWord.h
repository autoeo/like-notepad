	// ScriptTextWord.h: interface for the CScriptTextWord class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCRIPTTEXTWORD_H__8BDA78F4_7F52_445C_9E15_0F601DD3D0B3__INCLUDED_)
#define AFX_SCRIPTTEXTWORD_H__8BDA78F4_7F52_445C_9E15_0F601DD3D0B3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>
#include "Defines_ColorEdit.h"
#include "ScriptTextColor.h"

typedef struct tagS_CHAR
{
	tagS_CHAR()
	{
		strValue		= _T("");
//		memset( &rcRect, 0x00, sizeof(rcRect));
	}
	tagS_CHAR( const tagS_CHAR& st )
	{
		strValue		= st.strValue;
//		rcRect			= st.rcRect;
	}
	tagS_CHAR& operator = ( const tagS_CHAR& st )
	{
		strValue		= st.strValue;
//		rcRect			= st.rcRect;
		return *this;
	}
	CString		strValue		;
//	RECT		rcRect			;
}S_CHAR;

class CScriptTextWord  
{
public:
	CScriptTextWord();
	virtual ~CScriptTextWord();
	CScriptTextWord( const CScriptTextWord& st );
	CScriptTextWord& operator = (const CScriptTextWord& st );


public:
	void				SetWord					( const CString& strValue ) ;
	void				RemoveAll				();
public:
	BOOL				HitTest					(	CDC*					pDC						,
													int						x						,
													int&					nLeftHalfWidth			,
													int&					nStartPos				);
	void				Draw					(	CDC*					pDC						,
													RECT*					pRect					,
													const RECT&				rcClient				,
													int&					nCurrCharPos			,
													int						nLineNum				,
													S_SELECTION_INFO*		pSelection				,
													BOOL&					bStartedComment			,
													BOOL&					bStartedDblQuot			,
													BOOL					bCursorLine				,
													IScriptTextColorBase*	pTextColor				);
	inline	void		DrawText				(	CDC*					pDC						,
													const RECT&				rcClient				,
													RECT*					pRect					,
													COLORREF				crBkColor				,
													CString&				strValue				);
public:
	CString				m_strWord						;
	RECT				m_rcWord						;
	DWORD				m_dwType						;
	int					m_nSelStart						;
	int					m_nSelLength					;
	CArray<S_CHAR*, S_CHAR*>		m_aryChar			;
};

#endif // !defined(AFX_SCRIPTTEXTWORD_H__8BDA78F4_7F52_445C_9E15_0F601DD3D0B3__INCLUDED_)
