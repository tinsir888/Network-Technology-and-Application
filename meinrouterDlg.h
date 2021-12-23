
// meinrouterDlg.h : ͷ�ļ�
//

#pragma once


// CmeinrouterDlg �Ի���
class CmeinrouterDlg : public CDialogEx
{
// ����
public:
	CmeinrouterDlg(CWnd* pParent = NULL);	// ��׼���캯��


// �Ի�������
	enum { IDD = IDD_MYROUTER9_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnStartClickedButton();
	afx_msg void OnBnClickedButton();
//	afx_msg void OnAddClickedRouterButton();
//	afx_msg void OnBnClickedRouterButton();
	afx_msg void OnAddRouterButton();
//	afx_msg void OnDeleteRouterButton4();
	afx_msg void OnDeleteRouterButton();
		void CmeinrouterDlg::OnDestroy();
		void CmeinrouterDlg::OnTimer(UINT nIDEvent) ;
	CListBox	Logger;
	CListBox	m_RouteTable;
	CIPAddressCtrl	m_Destination;
	CIPAddressCtrl	m_NextHop;
	CIPAddressCtrl	m_Mask;

	afx_msg void OnLbnSelchangeList();
};
