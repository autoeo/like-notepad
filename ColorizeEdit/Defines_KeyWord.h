// Defines_KeyWord.h: interface for the CUtils class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEFINES_KEYWORD_H__80D1170C_0F8D_4C1F_A310_D1A893D9BA97__INCLUDED_)
#define AFX_DEFINES_KEYWORD_H__80D1170C_0F8D_4C1F_A310_D1A893D9BA97__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef struct tagS_KEYWORD_COLOR_INFO
{
	tagS_KEYWORD_COLOR_INFO()
	{
		memset( szKeyWord, 0x00, sizeof(szKeyWord));
		crColor		= 0;
	}
	tagS_KEYWORD_COLOR_INFO( const char* pszKeyword, unsigned long color )
	{
#ifdef _VS_2017
		strcpy_s( szKeyWord, pszKeyword );
#else
		strcpy( szKeyWord, pszKeyword ); 
#endif // _VS_2017
		crColor		= color;
	}
	tagS_KEYWORD_COLOR_INFO( const tagS_KEYWORD_COLOR_INFO& st )
	{
		memcpy( szKeyWord, st.szKeyWord, sizeof(szKeyWord)); 
		crColor		= st.crColor;
	}
	tagS_KEYWORD_COLOR_INFO& operator = ( const tagS_KEYWORD_COLOR_INFO& st )
	{
		memcpy( szKeyWord, st.szKeyWord, sizeof(szKeyWord)); 
		crColor		= st.crColor;
		return *this;
	}

	char			szKeyWord[32]		;
	COLORREF		crColor				;
}S_KEYWORD_COLOR_INFO;




#endif // !defined(AFX_DEFINES_KEYWORD_H__80D1170C_0F8D_4C1F_A310_D1A893D9BA97__INCLUDED_)


