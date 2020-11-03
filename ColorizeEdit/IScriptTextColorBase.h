// IScriptTextColorBase.h: interface for the IScriptTextColorBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ISCRIPTTEXTCOLORBASE_H__DCC371A3_9227_4B82_9D50_7489810BEFDE__INCLUDED_)
#define AFX_ISCRIPTTEXTCOLORBASE_H__DCC371A3_9227_4B82_9D50_7489810BEFDE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning (disable : 4786)

#include <map>
using namespace std;

#include "Defines_KeyWord.h"

typedef pair<CString,S_KEYWORD_COLOR_INFO> PairColorTable;
typedef map<CString,S_KEYWORD_COLOR_INFO> MapColorTable;

enum tagE_TEXTCOLOR_TABLE_TYPE
{
	E_CT_UBCT			,
	E_CT_CPLUSPLUS		,
};

class IScriptTextColorBase  : public MapColorTable
{
public:
	virtual void					Release					() = 0;
	virtual void					LoadDefaultStyle		() = 0;
	virtual S_KEYWORD_COLOR_INFO*	GetKeywordColor			( const CString& strKeyword ) = 0;

};

IScriptTextColorBase*	CreateTextColorTable( int nType );
void					DestroyTextColorTable( IScriptTextColorBase** ppObject );

#endif // !defined(AFX_ISCRIPTTEXTCOLORBASE_H__DCC371A3_9227_4B82_9D50_7489810BEFDE__INCLUDED_)
