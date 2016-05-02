// Path_TreeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Intelligent_Scissor.h"
#include "Path_TreeDlg.h"
#include "afxdialogex.h"


// CPath_TreeDlg dialog

IMPLEMENT_DYNAMIC(CPath_TreeDlg, CDialog)

CPath_TreeDlg::CPath_TreeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPath_TreeDlg::IDD, pParent)
{

	//  m_EditNumNodes = _T("");
	//  m_EditNodeNum = 0;
	m_EditNodeNum = 0;
}

CPath_TreeDlg::~CPath_TreeDlg()
{
}

void CPath_TreeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//  DDX_Text(pDX, IDC_EDIT_NUMNODES, m_EditNumNodes);
	//  DDX_Text(pDX, IDC_EDIT_NUMNODES, m_EditNodeNum);
	DDX_Text(pDX, IDC_EDIT_NUMNODES, m_EditNodeNum);
}


BEGIN_MESSAGE_MAP(CPath_TreeDlg, CDialog)
END_MESSAGE_MAP()


// CPath_TreeDlg message handlers


void CPath_TreeDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	UpdateData(TRUE);

	CDialog::OnOK();
}
