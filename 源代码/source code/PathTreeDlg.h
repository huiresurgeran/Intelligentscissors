
#pragma once

// CPathTreeDlg dialog

class CPathTreeDlg : public CDialog
{
	DECLARE_DYNAMIC(CPathTreeDlg)

public:
	CPathTreeDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPathTreeDlg();

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
