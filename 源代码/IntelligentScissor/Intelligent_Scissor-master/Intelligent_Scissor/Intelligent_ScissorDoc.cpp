
// Intelligent_ScissorDoc.cpp : implementation of the CIntelligent_ScissorDoc class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "Intelligent_Scissor.h"
#endif

#include "Intelligent_ScissorDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CIntelligent_ScissorDoc

IMPLEMENT_DYNCREATE(CIntelligent_ScissorDoc, CDocument)

BEGIN_MESSAGE_MAP(CIntelligent_ScissorDoc, CDocument)
	ON_UPDATE_COMMAND_UI(ID_ACTION_INITIAL, &CIntelligent_ScissorDoc::OnUpdateActionInitial)
	ON_COMMAND(ID_FILE_SAVECONTOUR, &CIntelligent_ScissorDoc::OnFileSavecontour)
	ON_COMMAND(ID_FILE_SAVEMASK, &CIntelligent_ScissorDoc::OnFileSavemask)
END_MESSAGE_MAP()


// CIntelligent_ScissorDoc construction/destruction

CIntelligent_ScissorDoc::CIntelligent_ScissorDoc()
{
	// TODO: add one-time construction code here
	m_bInitial = false;
	m_bReload = false;

	close = false;
	seed_count = 0;

	b_seed = Vec2i(0, 0);
	seed = Vec2i(0, 0);
	current = Vec2i(0, 0);
	scale = Vec2d(1.0, 1.0);

	cost_map = NULL;
	node_list.clear();
	cost_map_row = 0;
	cost_map_col = 0;
}

CIntelligent_ScissorDoc::~CIntelligent_ScissorDoc()
{
}

BOOL CIntelligent_ScissorDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CIntelligent_ScissorDoc serialization

void CIntelligent_ScissorDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CIntelligent_ScissorDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CIntelligent_ScissorDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data. 
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CIntelligent_ScissorDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CIntelligent_ScissorDoc diagnostics

#ifdef _DEBUG
void CIntelligent_ScissorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CIntelligent_ScissorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CIntelligent_ScissorDoc commands





BOOL CIntelligent_ScissorDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	ReleaseAndInitialize();
	// TODO:  Add your specialized creation code here
	if (lpszPathName != NULL)
	{
		// 将lpszPathName 转换成 std::string类型 文件名
#ifdef UNICODE
		// 中文路径时会存在问题
		size_t i = 0;
		char* nstring = new char[(wcslen(lpszPathName)+1)*2];
		wcstombs_s(&i, nstring, (wcslen(lpszPathName)+1)*2, lpszPathName, (wcslen(lpszPathName)+1)*2);
		std::string filename(nstring);
		delete[] nstring;
#else
		std::string filename = std::string(lpszPathName);
#endif

		// Load original file
		o_image = imread(filename, -1);




		// Copy to the Wnd::document
		o_image.copyTo(image);
		image.copyTo(tem1);

		if (!image.data)
		{
			MessageBox(NULL, lpszPathName, _T("Faile to open the image."), MB_OK);
			return FALSE;
		}
		/*
		while (image.rows > 260) {
		scale[0] = scale[0] * 2;
		smaller(image);
		}
		*/
		// cout << scale[0] << endl;

		Mat l_img(image.rows+2, image.cols+2, CV_8UC3);
		larger(image, l_img);

		// freopen("output.txt", "w", stdout);
		// cout << image.rows << endl;
		cost_map_row = image.rows;
		cost_map_col = image.cols;

		cost_map = new node*[image.rows];

		for(int i = 0; i < image.rows; i++){
			cost_map[i] = new node [image.cols];
		}
		cal_cost_map(cost_map, l_img);

		mask = Mat(image.rows, image.cols, CV_8UC3, Scalar(100, 100, 100));
		tem1.copyTo(tem2);


		// Create Pixel Graph and Cost Graph
		CreatePixelAndCostGraph(o_image, m_cvPixelGraph, m_cvCostGraph, cost_map);	
		m_cvCostGraph.copyTo(m_cvTreeMask);


		m_bInitial = true;
		m_bReload = true;
	}

	return TRUE;
}


BOOL CIntelligent_ScissorDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	// TODO: Add your specialized code here and/or call the base class
#ifdef UNICODE
        // 中文路径时会存在问题
        size_t i = 0;
        char* nstring = new char[(wcslen(lpszPathName)+1)*2];
        wcstombs_s(&i, nstring, (wcslen(lpszPathName)+1)*2, lpszPathName, (wcslen(lpszPathName)+1)*2);
        std::string filename(nstring);
        delete[] nstring;
#else
        std::string filename(lpszPathName);
#endif
 
	if (!image.data)
		MessageBox(NULL, lpszPathName, _T("No data"),MB_OK);
	cv::imwrite(filename, image);

	//return CDocument::OnSaveDocument(lpszPathName);
	return TRUE;
}


void CIntelligent_ScissorDoc::OnUpdateActionInitial(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(m_bInitial);
}


void CIntelligent_ScissorDoc::ReleaseAndInitialize() {
	close = false;
	seed_count = 0;

	b_seed = Vec2i(0, 0);
	seed = Vec2i(0, 0);
	current = Vec2i(0, 0);
	scale = Vec2d(1.0, 1.0);
	

	if (cost_map != NULL) {
		for (int i = 0; i < cost_map_row; i++) {
			delete [] cost_map[i];
		}
		delete []cost_map;
		cost_map = NULL;
	}
	cost_map_row = 0;
	cost_map_col = 0;

}

void CIntelligent_ScissorDoc::OnFileSavecontour()
{
	// TODO: Add your command handler code here
	static TCHAR BASED_CODE szFilter[] = _T(
		"JPG files (*.jpg)|*.jpg|BMP files (*.bmp)|*.bmp|TIF files (*.tif)|*.tif|All files (*.*)|*.*||");
	CFileDialog fd(FALSE, "jpg", "untitled", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);
	if (fd.DoModal() == IDOK) {
		CString fname = fd.GetPathName();
		//CString filename = fd.GetFileTitle();
		string contour_fname = fname;
		imwrite(contour_fname, tem1);
		
	}
	return ;
}


void CIntelligent_ScissorDoc::OnFileSavemask()
{
	static TCHAR BASED_CODE szFilter[] = _T(
		"JPG files (*.jpg)|*.jpg|BMP files (*.bmp)|*.bmp|TIF files (*.tif)|*.tif|All files (*.*)|*.*||");
	CFileDialog fd(FALSE, "jpg", "untitled", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);
	if (fd.DoModal() == IDOK) {
		CString fname = fd.GetPathName();
		string a = fname;
		imwrite(a, mask);
		
	}
	return ;// TODO: Add your command handler code here
}
