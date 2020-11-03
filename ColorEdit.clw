; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CColorableEdit
LastTemplate=CEdit
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "ColorEdit.h"

ClassCount=4
Class1=CColorEditApp
Class2=CColorEditDlg
Class3=CAboutDlg

ResourceCount=3
Resource1=IDD_COLOREDIT_DIALOG
Resource2=IDR_MAINFRAME
Class4=CColorableEdit
Resource3=IDD_ABOUTBOX

[CLS:CColorEditApp]
Type=0
HeaderFile=ColorEdit.h
ImplementationFile=ColorEdit.cpp
Filter=N

[CLS:CColorEditDlg]
Type=0
HeaderFile=ColorEditDlg.h
ImplementationFile=ColorEditDlg.cpp
Filter=D
LastObject=CColorEditDlg
BaseClass=CDialog
VirtualFilter=dWC

[CLS:CAboutDlg]
Type=0
HeaderFile=ColorEditDlg.h
ImplementationFile=ColorEditDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_COLOREDIT_DIALOG]
Type=1
Class=CColorEditDlg
ControlCount=3
Control1=IDC_EDT_INPUT_TEXT_PATHNAME,edit,1350633600
Control2=IDC_BTN_OPEN_INPUT_TEXT_PATHNAME,button,1342242816
Control3=IDC_STC_STATUS_BAR,static,1342308352

[CLS:CColorableEdit]
Type=0
HeaderFile=\50_SVN_DEV\Tool\UBCT_MFC\00_Projects\00_commonsrc\21_consrols\colorizeedit\colorableedit.h
ImplementationFile=\50_SVN_DEV\Tool\UBCT_MFC\00_Projects\00_commonsrc\21_consrols\colorizeedit\colorableedit.cpp
BaseClass=CWnd
Filter=W
VirtualFilter=WC
LastObject=CColorableEdit

