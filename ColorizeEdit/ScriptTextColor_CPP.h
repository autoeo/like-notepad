// ScriptTextColor_CPP.h: interface for the CScriptTextColor_CPP class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCRIPTTEXTCOLOR_CPP_H__02939A10_5BBF_4F4C_A1F5_EACA530F8D83__INCLUDED_)
#define AFX_SCRIPTTEXTCOLOR_CPP_H__02939A10_5BBF_4F4C_A1F5_EACA530F8D83__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IScriptTextColorBase.h"

class CScriptTextColor_CPP  : public IScriptTextColorBase
{
public:
	CScriptTextColor_CPP();
	virtual ~CScriptTextColor_CPP();
public:
	virtual void					Release					();
	virtual void					LoadDefaultStyle		();
	virtual S_KEYWORD_COLOR_INFO*	GetKeywordColor			( const CString& strKeyword );

};

#endif // !defined(AFX_SCRIPTTEXTCOLOR_CPP_H__02939A10_5BBF_4F4C_A1F5_EACA530F8D83__INCLUDED_)
