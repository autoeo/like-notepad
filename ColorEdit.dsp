# Microsoft Developer Studio Project File - Name="ColorEdit" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=ColorEdit - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ColorEdit.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ColorEdit.mak" CFG="ColorEdit - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ColorEdit - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ColorEdit - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ColorEdit - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "ColorizeEdit" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 imm32.lib /nologo /subsystem:windows /machine:I386 /out:"Bin/ColorEdit.exe"

!ELSEIF  "$(CFG)" == "ColorEdit - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "ColorizeEdit" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 imm32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"BinD/ColorEdit.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "ColorEdit - Win32 Release"
# Name "ColorEdit - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ColorEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\ColorEdit.rc
# End Source File
# Begin Source File

SOURCE=.\ColorEditDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ColorEdit.h
# End Source File
# Begin Source File

SOURCE=.\ColorEditDlg.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\ColorEdit.ico
# End Source File
# Begin Source File

SOURCE=.\res\ColorEdit.rc2
# End Source File
# Begin Source File

SOURCE=.\res\debug_view_icons.bmp
# End Source File
# End Group
# Begin Group "ColorEdit"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ColorizeEdit\ColorableEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\ColorizeEdit\ColorableEdit.h
# End Source File
# Begin Source File

SOURCE=.\ColorizeEdit\Defines_ColorEdit.h
# End Source File
# Begin Source File

SOURCE=.\ColorizeEdit\Defines_KeyWord.h
# End Source File
# Begin Source File

SOURCE=.\ColorizeEdit\IScriptTextColorBase.cpp
# End Source File
# Begin Source File

SOURCE=.\ColorizeEdit\IScriptTextColorBase.h
# End Source File
# Begin Source File

SOURCE=.\ColorizeEdit\ScriptText.cpp
# End Source File
# Begin Source File

SOURCE=.\ColorizeEdit\ScriptText.h
# End Source File
# Begin Source File

SOURCE=.\ColorizeEdit\ScriptTextColor.h
# End Source File
# Begin Source File

SOURCE=.\ColorizeEdit\ScriptTextColor_CPP.cpp
# End Source File
# Begin Source File

SOURCE=.\ColorizeEdit\ScriptTextColor_CPP.h
# End Source File
# Begin Source File

SOURCE=.\ColorizeEdit\ScriptTextColor_UBCT.cpp
# End Source File
# Begin Source File

SOURCE=.\ColorizeEdit\ScriptTextColor_UBCT.h
# End Source File
# Begin Source File

SOURCE=.\ColorizeEdit\ScriptTextLine.cpp
# End Source File
# Begin Source File

SOURCE=.\ColorizeEdit\ScriptTextLine.h
# End Source File
# Begin Source File

SOURCE=.\ColorizeEdit\ScriptTextWord.cpp
# End Source File
# Begin Source File

SOURCE=.\ColorizeEdit\ScriptTextWord.h
# End Source File
# Begin Source File

SOURCE=.\ColorizeEdit\StringUtils.cpp
# End Source File
# Begin Source File

SOURCE=.\ColorizeEdit\StringUtils.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
