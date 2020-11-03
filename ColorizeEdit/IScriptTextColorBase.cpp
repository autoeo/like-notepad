// IScriptTextColorBase.cpp: implementation of the IScriptTextColorBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IScriptTextColorBase.h"
#include "ScriptTextColor_UBCT.h"
#include "ScriptTextColor_CPP.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IScriptTextColorBase*	CreateTextColorTable( int nType )
{
	IScriptTextColorBase* pObject = NULL;
	switch( nType )
	{
	case E_CT_UBCT:
		pObject = static_cast< IScriptTextColorBase *> ( new CScriptTextColor_UBCT() );
		break;
	case E_CT_CPLUSPLUS:
		pObject = static_cast< IScriptTextColorBase *> ( new CScriptTextColor_CPP() );
		break;
	}
	return pObject;
}

void					DestroyTextColorTable( IScriptTextColorBase** ppObject )
{
	if( ppObject == NULL || *ppObject == NULL )
		return;

	(*ppObject)->Release();
	(*ppObject) = NULL;

}
