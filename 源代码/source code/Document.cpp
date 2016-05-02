
#include "stdafx.h"
#include "IntelScissor.h"

#include "Document.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CDoc, CDocument)

BEGIN_MESSAGE_MAP(CDoc, CDocument)
	ON_UPDATE_COMMAND_UI(ID_ACTION_INITIAL, &CDoc::OnUpdateActionInitial)
	ON_COMMAND(ID_FILE_SAVECONTOUR, &CDoc::OnFileSavecontour)
	ON_COMMAND(ID_FILE_SAVEMASK, &CDoc::OnFileSavemask)
END_MESSAGE_MAP()

CDoc::CDoc()
{
	// TODO: add one-time construction code here
	m_bInitial = false;
	m_bReload = false;

	close = false;
	seed_count = 0;

	b_seed = Point(0, 0);
	seed = Point(0, 0);
	current = Point(0, 0);
	scale = Vec2d(1.0, 1.0);

	nodes = NULL;
	node_list.clear();
	nodes_row = 0;
	nodes_col = 0;
}

CDoc::~CDoc()
{
}

BOOL CDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CDoc serialization

void CDoc::Serialize(CArchive& ar)
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
void CDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
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


void CDoc::InitializeSearchContent()
{
	CString strSearchContent;
	SetSearchContent(strSearchContent);
}

void CDoc::SetSearchContent(const CString& value)
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

// CDoc diagnostics

#ifdef _DEBUG
void CDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


BOOL CDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	ReleaseAndInitialize();

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

		// 加载原始的图片文件
		o_image = imread(filename, -1);

		 // 复制图片信息
		o_image.copyTo(image);
		image.copyTo(tem1);
		 // 如果图片信息为空，则打开图片失败
		if (!image.data)
		{
			MessageBox(NULL, lpszPathName, _T("Fail to open the image."), MB_OK);
			return FALSE;
		}
		 // 将放大的image存储在l_img
		Mat l_img(image.rows+2, image.cols+2, CV_8UC3);
		larger(image, l_img);
		  // 绘制Cost graph
		nodes_row = image.rows;
		nodes_col = image.cols;

		nodes = new Node*[image.rows];

		for(int i = 0; i < image.rows; i++){
			nodes[i] = new Node [image.cols];
		}
		cal_local_cost(nodes, l_img);

		mask = Mat(image.rows, image.cols, CV_8UC3, Scalar(100, 100, 100));
		tem1.copyTo(tem2);

		 // 创建Cost graph和Pixel graph
		create_pixel_and_cost_graph(o_image, m_cvPixelGraph, m_cvCostGraph, nodes);	
		m_cvCostGraph.copyTo(m_cvTreeMask);

		m_bInitial = true;
		m_bReload = true;
	}

	return TRUE;
}


BOOL CDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	// TODO: Add your specialized code here and/or call the base class
#ifdef UNICODE
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


void CDoc::OnUpdateActionInitial(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(m_bInitial);
}


void CDoc::ReleaseAndInitialize() {
	close = false;
	seed_count = 0;

	b_seed = Point(0, 0);
	seed = Point(0, 0);
	current = Point(0, 0);
	scale = Vec2d(1.0, 1.0);
	

	if (nodes != NULL) {
		for (int i = 0; i < nodes_row; i++) {
			delete [] nodes[i];
		}
		delete []nodes;
		nodes = NULL;
	}
	nodes_row = 0;
	nodes_col = 0;

}

void CDoc::OnFileSavecontour()
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


void CDoc::OnFileSavemask()
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
