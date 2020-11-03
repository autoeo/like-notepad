// ScriptTextColor_UBCT.cpp: implementation of the CScriptTextColor_UBCT class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ScriptTextColor_CPP.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
const S_KEYWORD_COLOR_INFO G_DEFAULT_KEYWORD [] = 
{
	S_KEYWORD_COLOR_INFO( "__asm"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "else"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "main"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "struct"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "__assume"					, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "enum"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "__multiple_inheritance"		, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "switch"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "auto"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "__except"					, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "__single_inheritance"		, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "template"					, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "__based"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "explicit"					, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "__virtual_inheritance"		, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "this"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "bool"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "extern"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "mutable"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "thread"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "break"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "false"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "naked"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "throw"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "case"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "__fastcall"					, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "namespace"					, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "true"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "catch"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "__finally"					, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "new"							, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "try"							, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "__cdecl"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "float"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "noreturn"					, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "__try"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "char"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "for"							, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "operator"					, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "typedef"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "class"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "friend"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "private"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "typeid"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "const"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "goto"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "protected"					, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "typename"					, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "const_cast"					, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "if"							, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "public"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "union"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "continue"					, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "inline"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "register"					, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "unsigned"					, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "__declspec"					, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "__inline"					, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "reinterpret_cast"			, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "using"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "declaration"					, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "directive"					, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "default"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "int"							, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "return"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "uuid"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "delete"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "__int8"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "short"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "__uuidof"					, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "dllexport"					, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "__int16"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "signed"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "virtual"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "dllimport"					, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "__int32"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "sizeof"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "void"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "do"							, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "__int64"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "static"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "volatile"					, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "double"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "__leave"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "static_cast"					, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "wmain"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "dynamic_cast"				, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "long"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "__stdcall"					, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "while"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "#define"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "#elif"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "#else"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "#endif"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "#error"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "#ifdef"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "#ifndef"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "#import"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "#include"					, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "#line"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "#pragma"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "#undef"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "private:"					, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "public:"						, RGB(0,0,255) ), 
	S_KEYWORD_COLOR_INFO( "protected:"					, RGB(0,0,255) ), 

};

CScriptTextColor_CPP::CScriptTextColor_CPP()
{

}

CScriptTextColor_CPP::~CScriptTextColor_CPP()
{
	clear();
}

void	CScriptTextColor_CPP::Release					()
{
	delete this;
}

void	CScriptTextColor_CPP::LoadDefaultStyle			()
{
	int i;
	int nCount = sizeof(G_DEFAULT_KEYWORD)/ sizeof(S_KEYWORD_COLOR_INFO);
	clear();
	for( i=0 ; i<nCount ; i++ )
	{
		insert( PairColorTable( G_DEFAULT_KEYWORD[i].szKeyWord, G_DEFAULT_KEYWORD[i]) );
	}
}

S_KEYWORD_COLOR_INFO* CScriptTextColor_CPP::GetKeywordColor		( const CString& strKeyword )
{
	MapColorTable::iterator it;

	it = find( strKeyword );
	if( it == end() )
		return NULL;
	return &it->second;
}
