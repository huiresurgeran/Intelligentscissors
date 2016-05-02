#pragma once


// CPath_TreeDlg dialog

class CPath_TreeDlg : public CDialog
{
	DECLARE_DYNAMIC(CPath_TreeDlg)

public:
	CPath_TreeDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPath_TreeDlg();

// Dialog Data
	enum { IDD = IDD_PATHTREE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
//	CString m_EditNumNodes;
	virtual void OnOK();
//	int m_EditNodeNum;
	int m_EditNodeNum;
};
