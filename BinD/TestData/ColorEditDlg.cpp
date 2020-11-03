// ColorEditDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ColorEdit.h"
#include "ColorEditDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorEditDlg dialog

CColorEditDlg::CColorEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CColorEditDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CColorEditDlg)
	m_strPathName = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pEdit	= NULL;
}

void CColorEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CColorEditDlg)
	DDX_Control(pDX, IDC_STC_STATUS_BAR, m_ctrlStatusBar);
	DDX_Text(pDX, IDC_EDT_INPUT_TEXT_PATHNAME, m_strPathName);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CColorEditDlg, CDialog)
	//{{AFX_MSG_MAP(CColorEditDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_OPEN_INPUT_TEXT_PATHNAME, OnBtnOpenInputTextPathname)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorEditDlg message handlers

BOOL CColorEditDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
//	CRect rc;
//	GetDlgItem( IDC_STC_SCRIPT_VIEW )->GetWindowRect( rc );
//	ScreenToClient( rc );

	CRect rc(0,0,0,0);

	m_pEdit = new CColorableEdit( this, rc, IDC_CUS_SCRIPT_VIEW  );
	m_pEdit->Init( static_cast<IListenerScriptText*>( this ) );
	m_pEdit->SetFont( 16, _T("Fixedsys") );

	CRect rcClient;
	GetClientRect( &rcClient );
	SendMessage( WM_SIZE, SIZE_RESTORED, MAKELPARAM( rcClient.Width(), rcClient.Height() ) );

	DWORD dwTick = ::GetTickCount();

//	m_pEdit->ReadFile( ".\\TestData\\01_[UBJ31_G28_DWRJES_01]_Download_OS.txt" );
	m_pEdit->ReadFile( ".\\TestData\\01_[UBJ31_G28_DWRJES_01]_Download_OS.txt" );

	dwTick = ::GetTickCount() - dwTick;

	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CColorEditDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CColorEditDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CColorEditDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CColorEditDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	if( m_pEdit )
	{
		if( m_pEdit->GetSafeHwnd() )
			m_pEdit->DestroyWindow();
		delete m_pEdit;
		m_pEdit = NULL;
	}
}

#define C_LAYOUT_EDIT_HEIGHT		24
#define C_LAYOUT_BTN_WIDTH			24

void CColorEditDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	if( m_pEdit == NULL || m_pEdit->GetSafeHwnd() == NULL )
		return;

	CWnd* pWnd = NULL;

	RECT rcEdtPathName;
	pWnd = GetDlgItem( IDC_EDT_INPUT_TEXT_PATHNAME );
	rcEdtPathName.left		= 10;
	rcEdtPathName.top		= 10;
	rcEdtPathName.right		= cx - 10 - C_LAYOUT_BTN_WIDTH;
	rcEdtPathName.bottom	= rcEdtPathName.top + C_LAYOUT_EDIT_HEIGHT;
	pWnd->MoveWindow( &rcEdtPathName );

	RECT rcBtnPathName;
	pWnd = GetDlgItem( IDC_BTN_OPEN_INPUT_TEXT_PATHNAME );
	rcBtnPathName.left		= cx - 10 - C_LAYOUT_BTN_WIDTH;
	rcBtnPathName.top		= 10;
	rcBtnPathName.right		= rcBtnPathName.left + C_LAYOUT_BTN_WIDTH;
	rcBtnPathName.bottom	= rcEdtPathName.bottom;
	pWnd->MoveWindow( &rcBtnPathName );

	RECT rcColorEdt;
	rcColorEdt.left		= 10;
	rcColorEdt.top		= rcEdtPathName.bottom + 4;
	rcColorEdt.right	= cx - 10;
	rcColorEdt.bottom	= cy - 10 - 18;

	m_pEdit->MoveWindow( &rcColorEdt );

	RECT rcStatusBar;
	rcStatusBar.left		= 10;
	rcStatusBar.top		= rcColorEdt.bottom;
	rcStatusBar.right	= cx - 10;
	rcStatusBar.bottom	= cy - 10;

	m_ctrlStatusBar.MoveWindow( &rcStatusBar );
	
}

void CColorEditDlg::OnBtnOpenInputTextPathname() 
{
	UpdateData( );

	CString strPathName;

	TCHAR tszBuffer[MAX_PATH]				= { NULL, };
	(LPCTSTR)::GetCurrentDirectory( MAX_PATH, tszBuffer );
	OPENFILENAME OFN;
	memset(&OFN, 0, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = m_hWnd;
	OFN.lpstrFilter = _T("TXT 파일(*.txt)\0*.TXT\0;C/C++ 파일(*.h,*.c,*.cpp)\0*.h;*.c;*.cpp\0");
	OFN.lpstrInitialDir = tszBuffer;
	OFN.lpstrFile = tszBuffer;
	OFN.nMaxFile = MAX_PATH;
	OFN.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;
	if (!GetOpenFileName(&OFN))
		return ;

	m_strPathName = OFN.lpstrFile;

	UpdateData( FALSE );

	m_pEdit->ReadFile( m_strPathName );

	
}

void CColorEditDlg::OnReadFile			( DWORD dwFileSize, BOOL bIsCRLF, DWORD dwLineCount )
{
	// for a Status Bar.
	CString strStatus;

	strStatus.Format( _T("File Length : %-10d bytes | Lines : %-10d"), dwFileSize, dwLineCount );
	m_ctrlStatusBar.SetWindowText( strStatus );

}

BOOL CColorEditDlg::OnClickedDebugPoint		( int nLine, const CString& strLine )
{
	return TRUE;
}

void CColorEditDlg::OnMovedCursor	( int nLine, int nPos, int nSelChars, int nSelLines )
{
	CString strStatus;

	strStatus.Format( _T("[ Line : %d | Col : %d ] [ Sel %d | %d ]"), nLine, nPos, nSelChars, nSelLines );
	m_ctrlStatusBar.SetWindowText( strStatus );
}

void CColorEditDlg::OnContextMenu		( CWnd* pWnd, CPoint point, const int nLine, const CString& strLine )
{
	int nLineNumber = -1;
//	CString strLine = m_pEdit->GetLineTextFromPoint( point, nLineNumber );

}
