// Defines.h: interface for the CUtils class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEFINES_H__80D1170C_0F8D_4C1F_A310_D1A893D9BA97__INCLUDED_)
#define AFX_DEFINES_H__80D1170C_0F8D_4C1F_A310_D1A893D9BA97__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define		C_BK_COLOR_NORMAL				RGB(255,255,255)
#define		C_BK_COLOR_CLIPPED				RGB(192,192,192)
#define		C_BK_COLOR_SELLINE				RGB(232,232,255)
#define		C_BK_COLOR_SELECTED				RGB(155,255,155)
#define		C_BK_COLOR_CLIPPED_SELECTED		RGB(117,217,117)
#define		C_TEXT_COLOR_NORMAL				RGB(0,0,0)
#define		C_TEXT_COLOR_COMMENT			RGB(0,128,0)
#define		C_TEXT_COLOR_STRING				RGB(255,0,0)
#define		C_TEXT_COLOR_NUMBER				RGB(255,0,0)
#define		C_TEXT_COLOR_ENUM				RGB(128,0,255)
#define		C_TEXT_COLOR_COMMAND			RGB(0,0,255)


#define		C_WORD_TYPE_UNKNOWN		0
#define		C_WORD_TYPE_COMMENT		1
#define		C_WORD_TYPE_STRING		2
#define		C_WORD_TYPE_NUMBER		3
#define		C_WORD_TYPE_ENUM		4
#define		C_WORD_TYPE_COMMAND		5

#define		C_INPUT_MODE_INSERT				0x00
#define		C_INPUT_MODE_OVERWRITE			0x01
#define		C_INPUT_MASK_ON_INSERT			0x02



typedef struct tagS_SELECTION_INFO
{
	tagS_SELECTION_INFO()
	{
		bSelection			= FALSE;
		nStartLine			= -1;
		nStartPos			= -1;
		nEndLine			= -1;
		nEndPos				= -1;
		strWord				= _T("");
	}
	tagS_SELECTION_INFO( const tagS_SELECTION_INFO& st )
	{
		if( st.nEndLine < st.nStartLine )
		{
			nEndLine		= st.nStartLine;
			nEndPos			= st.nStartPos;
			nStartLine		= st.nEndLine;
			nStartPos		= st.nEndPos;
		}
		else if( st.nEndLine == st.nStartLine )
		{
			nStartLine			= st.nStartLine		;
			nStartPos			= min( st.nStartPos, st.nEndPos );
			nEndLine			= st.nEndLine		;
			nEndPos				= max( st.nStartPos, st.nEndPos );
		}
		else
		{
			nStartLine			= st.nStartLine		;
			nStartPos			= st.nStartPos		;
			nEndLine			= st.nEndLine		;
			nEndPos				= st.nEndPos		;
		}
		bSelection = TRUE;
		if( nStartLine	 == -1 || nStartPos	== -1 || 
			nEndLine	 == -1 || nEndPos	== -1 )
			bSelection = FALSE;

		strWord = st.strWord;
	}
	tagS_SELECTION_INFO& operator =( const tagS_SELECTION_INFO& st )
	{
		if( st.nEndLine < st.nStartLine )
		{
			nEndLine		= st.nStartLine;
			nEndPos			= st.nStartPos;
			nStartLine		= st.nEndLine;
			nStartPos		= st.nEndPos;
		}
		else if( st.nEndLine == st.nStartLine )
		{
			nStartLine			= st.nStartLine		;
			nStartPos			= min( st.nStartPos, st.nEndPos );
			nEndLine			= st.nEndLine		;
			nEndPos				= max( st.nStartPos, st.nEndPos );
		}
		else
		{
			nStartLine			= st.nStartLine		;
			nStartPos			= st.nStartPos		;
			nEndLine			= st.nEndLine		;
			nEndPos				= st.nEndPos		;
		}
		bSelection = TRUE;
		if( nStartLine	 == -1 || nStartPos	== -1 || 
			nEndLine	 == -1 || nEndPos	== -1 )
			bSelection = FALSE;

		strWord = st.strWord;
		return *this;
	}
	BOOL			bSelection			;
	int				nStartLine			;
	int				nStartPos			;
	int				nEndLine			;
	int				nEndPos				;
	CString			strWord				;

}S_SELECTION_INFO;

#define		C_UNDO_ACT_NONE				0
#define		C_UNDO_ACT_DELETED			1
#define		C_UNDO_ACT_INSERTED			2
//#define		C_UNDO_ACT_DELETED_LINE		3
//#define		C_UNDO_ACT_INSERTED_LINE	4

#define		C_MAX_UNDO_COUNT			100

typedef struct tagS_ACTION_INFO
{
	tagS_ACTION_INFO()
	{
		bIsContinue		= FALSE;
		byteType		= C_UNDO_ACT_NONE;
		nLine			= -1;
		nPos			= -1;
		strText			= _T("");
	}
	tagS_ACTION_INFO( const tagS_ACTION_INFO& st )
	{
		bIsContinue		= st.bIsContinue	;
		byteType		= st.byteType		;
		nLine			= st.nLine			;
		nPos			= st.nPos			;
		strText			= st.strText		;
	}
	tagS_ACTION_INFO& operator = ( const tagS_ACTION_INFO& st )
	{
		bIsContinue		= st.bIsContinue	;
		byteType		= st.byteType		;
		nLine			= st.nLine			;
		nPos			= st.nPos			;
		strText			= st.strText		;
		return *this;
	}
	BOOL		bIsContinue		;
	BYTE		byteType		;
	int			nLine			;
	int			nPos			;
	CString		strText			;

}S_ACTION_INFO;

typedef struct tagS_FIND_TEXT_INFO
{
	tagS_FIND_TEXT_INFO()
	{
		bMatchCase		= FALSE;
		bWholeWord		= FALSE;
		nLine			= -1;
		nPos			= -1;
		strText			= _T("");
	}
	tagS_FIND_TEXT_INFO(const tagS_FIND_TEXT_INFO& st )
	{
		bMatchCase		= st.bMatchCase		;
		bWholeWord		= st.bWholeWord		;
		nLine			= st.nLine			;
		nPos			= st.nPos			;
		strText			= st.strText		;
	}
	tagS_FIND_TEXT_INFO& operator = (const tagS_FIND_TEXT_INFO& st )
	{
		bMatchCase		= st.bMatchCase		;
		bWholeWord		= st.bWholeWord		;
		nLine			= st.nLine			;
		nPos			= st.nPos			;
		strText			= st.strText		;
		return *this;
	}

	BOOL		bMatchCase		;
	BOOL		bWholeWord		;
	int			nLine			;
	int			nPos			;
	CString		strText			;
}S_FIND_TEXT_INFO;

#endif // !defined(AFX_DEFINES_H__80D1170C_0F8D_4C1F_A310_D1A893D9BA97__INCLUDED_)


