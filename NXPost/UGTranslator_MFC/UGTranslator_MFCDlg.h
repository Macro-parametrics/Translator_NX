
// UGTranslator_MFCDlg.h : 헤더 파일
//

#pragma once


// CUGTranslator_MFCDlg 대화 상자
class CUGTranslator_MFCDlg : public CDialogEx
{
// 생성입니다.
public:
	CUGTranslator_MFCDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.
	typedef UINT(WINAPIV* StartService)(void);
	typedef UINT(WINAPIV* Translate)(char * path);
	int TransCADStatus;

// 대화 상자 데이터입니다.
	enum { IDD = IDD_UGTRANSLATOR_MFC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedImportPath();
	afx_msg void OnBnClickedRunUGPre();
	afx_msg void OnBnClickedExportPath();
	afx_msg void OnBnClickedRunUGPost();
	afx_msg void OnBnClickedRunTransCAD();
	afx_msg void OnBnClickedExitTransCAD();
	afx_msg void OnBnClickedExitTranslator();
};
