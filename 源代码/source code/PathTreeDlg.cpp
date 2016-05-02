
#include "stdafx.h"
#include "IntelScissor.h"
#include "PathTreeDlg.h"
#include "afxdialogex.h"


// CPathTreeDlg dialog

IMPLEMENT_DYNAMIC(CPathTreeDlg, CDialog)

CPathTreeDlg::CPathTreeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPathTreeDlg::IDD, pParent)
{

	m_EditNodeNum = 0;
}

CPathTreeDlg::~CPathTreeDlg()
{
}

void CPathTreeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_NUMNODES, m_EditNodeNum);
}


BEGIN_MESSAGE_MAP(CPathTreeDlg, CDialog)
END_MESSAGE_MAP()


// CPathTreeDlg message handlers


void CPathTreeDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	UpdateData(TRUE);

	CDialog::OnOK();
}
