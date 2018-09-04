
// UGTranslator_MFCDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "UGTranslator_MFC.h"
#include "UGTranslator_MFCDlg.h"
#include "afxdialogex.h"
#include <TlHelp32.h>	// for TransCAD exit

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CUGTranslator_MFCDlg 대화 상자




CUGTranslator_MFCDlg::CUGTranslator_MFCDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CUGTranslator_MFCDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	TransCADStatus = 0;
}

void CUGTranslator_MFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CUGTranslator_MFCDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CUGTranslator_MFCDlg::OnBnClickedImportPath)
	ON_BN_CLICKED(IDC_BUTTON2, &CUGTranslator_MFCDlg::OnBnClickedRunUGPre)
	ON_BN_CLICKED(IDC_BUTTON3, &CUGTranslator_MFCDlg::OnBnClickedExportPath)
	ON_BN_CLICKED(IDC_BUTTON4, &CUGTranslator_MFCDlg::OnBnClickedRunUGPost)
	ON_BN_CLICKED(IDC_BUTTON5, &CUGTranslator_MFCDlg::OnBnClickedRunTransCAD)
	ON_BN_CLICKED(IDC_BUTTON6, &CUGTranslator_MFCDlg::OnBnClickedExitTransCAD)
	ON_BN_CLICKED(IDC_BUTTON7, &CUGTranslator_MFCDlg::OnBnClickedExitTranslator)
END_MESSAGE_MAP()


// CUGTranslator_MFCDlg 메시지 처리기

BOOL CUGTranslator_MFCDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CUGTranslator_MFCDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CUGTranslator_MFCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CUGTranslator_MFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CUGTranslator_MFCDlg::OnBnClickedImportPath()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CString filter = _T("UG part file (*.prt) |*.prt|");
	// If role of dlg is selection --> TRUE
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER, filter);

	if (dlg.DoModal())
	{
		CString _path = dlg.m_ofn.lpstrFile;

		if (!_path.IsEmpty())
		{
			SetDlgItemText(IDC_EDIT1, dlg.m_ofn.lpstrFile);
		}
	}
}


void CUGTranslator_MFCDlg::OnBnClickedRunUGPre()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CUGTranslator_MFCDlg::OnBnClickedExportPath()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CString filter = _T("UG part file (*.prt) |*.prt|");
	// If role of dlg is save --> FALSE
	CFileDialog dlg(FALSE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER, filter);

	CString extention = _T(".prt");

	if (dlg.DoModal())
	{
		CString _path = dlg.m_ofn.lpstrFile;

		if (!_path.IsEmpty())
		{
			if (_path.Find(L".prt") != -1)
				SetDlgItemText(IDC_EDIT2, dlg.m_ofn.lpstrFile);
			else
				SetDlgItemText(IDC_EDIT2, dlg.m_ofn.lpstrFile + extention);			
		}
	}
}


void CUGTranslator_MFCDlg::OnBnClickedRunUGPost()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	//HINSTANCE hDLL;
	HMODULE hDLL;
	StartService fnStartService;
	Translate fnTranslate;

	hDLL = LoadLibrary(_T("dllUGPost.dll"));

	
	if (!hDLL)
		AfxMessageBox(_T("Error : UGpost dll isn't loaded..."));

	TRACE("%d", GetLastError());

	CString cstrUGfileName;
	wchar_t* wchar_str;
	char* ch_UGfileName;
	int char_str_len;

	GetDlgItemText(IDC_EDIT2, cstrUGfileName);
	
	// Unicode환경에서 CString to char * 변환
	wchar_str = cstrUGfileName.GetBuffer(cstrUGfileName.GetLength());
	char_str_len = WideCharToMultiByte(CP_ACP, 0, wchar_str, -1, NULL, 0, NULL, NULL);
	ch_UGfileName = new char[char_str_len];
	WideCharToMultiByte(CP_ACP, 0, wchar_str, -1, ch_UGfileName, char_str_len, 0, 0);


	if (hDLL != NULL && !cstrUGfileName.IsEmpty())
	{
		// 불러온 DLL에서 아래의 함수를 탐색 --> 인스턴스 생성
		fnStartService = (StartService)GetProcAddress(hDLL, "UGPostStartService");
		fnTranslate = (Translate)GetProcAddress(hDLL, "UGPostTranslate");

		if (!fnStartService || !fnTranslate)
		{
			// handle the error
			FreeLibrary(hDLL);
			AfxMessageBox(_T("There are NO FUNCTIONS(StartService or Translate) in dll file"));
		}
		else
		{
			fnStartService();
			bool status = fnTranslate(ch_UGfileName) ? true : false;
			//bool status = fnTranslate((char *)(LPCTSTR)UG_fileName) ? true : false;

			if (!status)
				AfxMessageBox(_T("Make TransCAD Model First!"));
			else
				TransCADStatus = 1;
		}
	}

	FreeLibrary(hDLL);
}


void CUGTranslator_MFCDlg::OnBnClickedRunTransCAD()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (TransCADStatus == 0)
	{
		HRESULT hr = ::CoInitialize(NULL);	// COM Automation API Initialization
		if (FAILED(hr))
		{

			AfxMessageBox(_T("COM Automation API Initialization Failed!"));
		}
		else
		{
			TransCAD::IApplicationPtr g_spApplication;

			// create TransCAD
			hr = g_spApplication.CreateInstance(__uuidof(TransCAD::Application));

			if (FAILED(hr))
				AfxMessageBox(_T("COM Automation API Initialization Failed!"));
			else
			{
				// visualize TransCAD
				g_spApplication->Visible = VARIANT_TRUE;

				TransCAD::IDocsPtr _spDocuments = g_spApplication->Documents;
				TransCAD::IPartDocumentPtr	_spDocument = _spDocuments->AddPartDocument();
				TransCAD::IPartPtr _spPart = _spDocument->Part;

				TransCADStatus = 1;
			}
		}
	}
}


void CUGTranslator_MFCDlg::OnBnClickedExitTransCAD()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (TransCADStatus == 1)
	{
		DWORD dwSize = 250;
		HANDLE hSnapShot;
		PROCESSENTRY32 pEntry;
		BOOL bCrrent = FALSE;


		hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);

		pEntry.dwSize = sizeof(pEntry);
		// 실행중인 프로세스들의 첫번재 정보를 가져온다.
		Process32First(hSnapShot, &pEntry);

		// Tool이 실행중인지 확인
		while (1)
		{
			// 다음번 프로세스의 정보를 가져온다.
			BOOL hRes = Process32Next(hSnapShot, &pEntry);

			if (hRes == FALSE)
				break;
			if (!strncmp(_bstr_t(pEntry.szExeFile), "TransCAD.exe", 15))
			{
				bCrrent = TRUE;
			}
			if (bCrrent)
			{
				HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pEntry.th32ProcessID);
				if (hProcess)
				{
					if (TerminateProcess(hProcess, 0))
					{
						unsigned long nCode; //프로세스 종료 상태
						GetExitCodeProcess(hProcess, &nCode);
					}
					CloseHandle(hProcess);
				}
				break;
			}
		}

		TransCADStatus = 0;
	}
}


void CUGTranslator_MFCDlg::OnBnClickedExitTranslator()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnOK();
}
