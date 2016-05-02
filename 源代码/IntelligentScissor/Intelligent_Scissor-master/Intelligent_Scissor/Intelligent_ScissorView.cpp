// Intelligent_ScissorView.cpp : implementation of the CIntelligent_ScissorView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "Intelligent_Scissor.h"
#endif

#include "Intelligent_ScissorDoc.h"
#include "Intelligent_ScissorView.h"
#include "Path_TreeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CIntelligent_ScissorView

IMPLEMENT_DYNCREATE(CIntelligent_ScissorView, CView)

BEGIN_MESSAGE_MAP(CIntelligent_ScissorView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CIntelligent_ScissorView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_BUTTON_ZOOMIN, &CIntelligent_ScissorView::OnButtonZoomin)
	ON_COMMAND(ID_BUTTON_ZOOMOUT, &CIntelligent_ScissorView::OnButtonZoomout)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_BUTTON_FINISH, &CIntelligent_ScissorView::OnButtonFinish)
	ON_COMMAND(ID_BUTTON_UNDO, &CIntelligent_ScissorView::OnButtonUndo)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_ZOOMIN, &CIntelligent_ScissorView::OnUpdateButtonZoomin)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_ZOOMOUT, &CIntelligent_ScissorView::OnUpdateButtonZoomout)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_FINISH, &CIntelligent_ScissorView::OnUpdateButtonFinish)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_UNDO, &CIntelligent_ScissorView::OnUpdateButtonUndo)
	ON_COMMAND(ID_BUTTON_CLEAR, &CIntelligent_ScissorView::OnButtonClear)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_CLEAR, &CIntelligent_ScissorView::OnUpdateButtonClear)
	ON_COMMAND(ID_WORKMODE_IMAGEONLY, &CIntelligent_ScissorView::OnWorkmodeImageonly)
	ON_COMMAND(ID_WORKMODE_IMAGEWITHCONTOUR, &CIntelligent_ScissorView::OnWorkmodeImagewithcontour)
	ON_COMMAND(ID_DEBUGMODE_PIXELNODE, &CIntelligent_ScissorView::OnDebugmodePixelnode)
	ON_COMMAND(ID_DEBUGMODE_COSTGRAPH, &CIntelligent_ScissorView::OnDebugmodeCostgraph)
	ON_COMMAND(ID_DEBUGMODE_PATHTREE, &CIntelligent_ScissorView::OnDebugmodePathtree)
	ON_COMMAND(ID_DEBUGMODE_MINIMUMPATH, &CIntelligent_ScissorView::OnDebugmodeMinimumpath)
END_MESSAGE_MAP()

// CIntelligent_ScissorView construction/destruction

CIntelligent_ScissorView::CIntelligent_ScissorView()
{
	// TODO: add construction code here

	m_bZoomin = false;
	m_bZoomout = false;
	m_bUndo = false;

	m_bClicked = true;

	// For Work Mode
	m_iWorkMode = IMAGE_CONTOUR;
	// For Debug Mode
	m_iDebugMode = 0;
	m_bSwitch = true;

	m_iNodeNum = 0;

	// Enable hotkey accelerator
	m_hAccel = ::LoadAccelerators(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
}

CIntelligent_ScissorView::~CIntelligent_ScissorView()
{
}

BOOL CIntelligent_ScissorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CIntelligent_ScissorView drawing
 
void CIntelligent_ScissorView::OnDraw(CDC* pDC)
{
	CIntelligent_ScissorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
	
	pDoc->m_bReload = false;

	if (isWorkMode()) {
		switch (m_iWorkMode) {
		case IMAGE_ONLY:
			m_cvImg = pDoc->image;
			AdjustSize(m_cvImg);
			break;
		case IMAGE_CONTOUR:
			if (pDoc->close) {
				m_cvImg = pDoc->mask;
			} else {
				m_cvImg = pDoc->tem2;
			}
			break;
		}
	} else {
		switch (m_iDebugMode) {
		case PIXEL_NODE:
			m_cvImg = pDoc->m_cvPixelGraph;
			break;
		case COST_GRAPH:
			m_cvImg = pDoc->m_cvCostGraph;
			break;
		case PATH_TREE:
			m_cvImg = pDoc->m_cvTreeMask;
			break;
		case MINI_PATH:
			m_cvImg = pDoc->m_cvMiniPath;
			break;

		}
	}

	HDC hDC = pDC->GetSafeHdc();
	IplImage img = m_cvImg;
	CvvImage cimg;
	cimg.CopyOf( &img );
	CRect rect;
	rect.SetRect(0, 0, m_cvImg.cols, m_cvImg.rows);
	cimg.DrawToHDC(hDC, &rect);

	ReleaseDC( pDC );
	
	
}


// CIntelligent_ScissorView printing


void CIntelligent_ScissorView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CIntelligent_ScissorView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CIntelligent_ScissorView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CIntelligent_ScissorView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CIntelligent_ScissorView::OnRButtonUp(UINT  nFlags, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CIntelligent_ScissorView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CIntelligent_ScissorView diagnostics

#ifdef _DEBUG
void CIntelligent_ScissorView::AssertValid() const
{
	CView::AssertValid();
}

void CIntelligent_ScissorView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CIntelligent_ScissorDoc* CIntelligent_ScissorView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CIntelligent_ScissorDoc)));
	return (CIntelligent_ScissorDoc*)m_pDocument;
}
#endif //_DEBUG







void CIntelligent_ScissorView::OnButtonZoomin()
{
	// TODO: Add your command handler code here
	ZoomIn();
	Invalidate(TRUE);
}


void CIntelligent_ScissorView::OnButtonZoomout()
{
	// TODO: Add your command handler code here
	ZoomOut();
	Invalidate(TRUE);
}


void CIntelligent_ScissorView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CIntelligent_ScissorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	/*
		close(bool): true: The contour is to be closed
					 fasle: Do not close the contour
	*/
	// IF the picture is loaded:
	m_bClicked = true;

	Mat& o_image = pDoc->o_image;
	Mat& image = pDoc->image;
	Mat& tem1 = pDoc->tem1;
	Mat& tem2 = pDoc->tem2;
	Mat& mask = pDoc->mask;
	
	double x = point.x;
	double y = point.y;

	if (pDoc->m_bInitial && m_iWorkMode == IMAGE_CONTOUR && !pDoc->close && // pDoc->seed_count != 0 && 
		(pDoc->seed_count != 0 || (pDoc->seed_count == 0 && (nFlags & MK_CONTROL))) &&
		x >= 0 && x < tem2.cols && y >= 0 && y < tem2.rows) {

		int& seed_count = pDoc->seed_count;
		bool& close = pDoc->close;
		Vec2i& seed = pDoc->seed;
		Vec2i& b_seed = pDoc->b_seed;
		Vec2i& current = pDoc->current;
		Vec2d& scale = pDoc->scale;

		stack<node**>& map_stack = pDoc->map_stack;
		stack<Mat>& mask_stack = pDoc->mask_stack;
		stack<Mat>& tem1_stack = pDoc->tem1_stack;
		stack<Vec2d>& scale_stack = pDoc->scale_stack;
		stack<vector<node*>>& node_list_stack = pDoc->node_list_stack;

		tem1.copyTo(tem2);

		current[0] = x;
		current[1] = y;
		
		seed_count++;
		seed[0] = current[0] = x / (scale[0]*scale[1]);
		seed[1] = current[1] = y / (scale[0] * scale[1]);

		if (seed_count == 1) {
			b_seed[0] = x / (scale[0] * scale[1]);
			b_seed[1] = y / (scale[0] * scale[1]);
		}
		// CHECK: Whether the clicked point matches the most beggining seed:
		//			if these two points overlapped: close <- true;

		// cout << "Seed Point: " << seed[0] << ' ' << seed[1] << endl;
		// cout << "Seed count: " << seed_count << endl;

		if (!close) {
			// Only if there are more than 1 seed
			if (seed_count > 1 ) {
				// Calculate the distance between the clicked point and the 
				// most beginning seed:
				double dis = sqrt((double)((seed[0]-b_seed[0])*(seed[0]-b_seed[0]) +
					(seed[1]-b_seed[1])*(seed[1]-b_seed[1])));
				// Define: if distance < 5
				//			Regard these two seed points being overlapped
				int minimum_gap = 5;
				if (dis < minimum_gap) {
					close = true;
				}
			}
		}

		// CHECK: Draw the path from the previous seed to the newly clicked point
		// case 1: The newly drawed path WILL form a contour:
		if (close) {
			draw_path(tem1, mask, pDoc->cost_map, b_seed, true, scale[0] * scale[1]);
			draw_circle(tem1, b_seed, 0.5, false, scale);

			save_stack(mask,mask_stack);
			save_stack(tem1,tem1_stack);
			save_mask(mask, o_image, scale);
			save_scale(scale, scale_stack);
			save_node_list_stack(pDoc->node_list, pDoc->node_list_stack);
			
			
			// cal = true;
		}
		// case 2: The newly drawn path will NOT form a contour:	
		else {
			cout << "Enter Draw Path Mode" << endl;
			if(seed_count > 1) {
				cout << "Draw Path: " << endl;	
				draw_path(tem1, mask, pDoc->cost_map, seed, true, scale[0] * scale[1]);
				pDoc->cost_map = new_tree(pDoc->cost_map, image.rows, image.cols);
			}
			draw_circle(tem1, seed, 0.5, true, scale);

			save_stack(mask, mask_stack);
			save_stack(tem1, tem1_stack);
			save_scale(scale, scale_stack);

			cout << "computing...";
			generate_tree(pDoc->cost_map, seed, image.rows, image.cols, pDoc->node_list);
			map_stack.push(pDoc->cost_map);
			cout << "finished" << endl << endl;

			save_node_list_stack(pDoc->node_list, pDoc->node_list_stack);
		}
		Invalidate(TRUE);
	}
	m_bClicked = false;
	CView::OnLButtonDown(nFlags, point);
}


void CIntelligent_ScissorView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default	
	CIntelligent_ScissorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	bool& close = pDoc->close;
	int& seed_count = pDoc->seed_count;
	Mat& mask = pDoc->mask;
	Mat& tem2 = pDoc->tem2;

	Vec2i& seed = pDoc->seed;
	Vec2i& b_seed = pDoc->b_seed;
	Vec2i& current = pDoc->current;
	Vec2d& scale = pDoc->scale;

	bool is_debug = (m_iDebugMode == MINI_PATH);	
	double x = point.x;
	double y = point.y;


	if (!is_debug){
		current[0] = x / (scale[0] * scale[1]);
		current[1] = y / (scale[0] * scale[1]);
	}
	else {
		current[0] = x;
		current[1] = y;
	}

	Mat temp;
	if (is_debug){
		pDoc->m_cvCostGraph.copyTo(temp);
	}
	else {	
		pDoc->tem1.copyTo(temp);
	}

	if (/* For Control*/
		(m_iWorkMode == IMAGE_CONTOUR || m_iDebugMode == MINI_PATH) &&
		/* When activated */
		x >= 0 && x < temp.cols-1 &&
		y >= 0 && y < temp.rows-1 && 
		seed_count > 0 && !m_bClicked && 
		pDoc->m_bInitial && !pDoc->close){

		
		// DRAW the path from the b_seed point to the current clicked point
		if (is_debug){
			Mat& m_cvMiniPath = pDoc->m_cvMiniPath;
			pDoc->m_cvCostGraph.copyTo(m_cvMiniPath);
			draw_path(m_cvMiniPath, temp, pDoc->cost_map, current, false, 1, is_debug);
		}
		else {
			pDoc->tem1.copyTo(tem2);
			draw_path(tem2, mask, pDoc->cost_map, current, false, scale[0] * scale[1]);
		}


		double dis = sqrt((double)((current[0]-b_seed[0])*(current[0]-b_seed[0]) +
						(current[1]-b_seed[1])*(current[1]-b_seed[1])));

		// Define: if distance < 5
		//			Regard these two seed points being overlapped
		int minimum_gap = 5;
		if (dis < minimum_gap) {
			draw_circle(tem2,current,0.5,false, scale);
		}

		Invalidate(TRUE);
	}
		
	CView::OnMouseMove(nFlags, point);
}


BOOL CIntelligent_ScissorView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	CIntelligent_ScissorDoc* pDoc = GetDocument();

	if (m_bZoomin || m_bZoomout || pDoc->m_bReload || m_bSwitch || m_bUndo ||
		m_iDebugMode == PIXEL_NODE || m_iDebugMode == COST_GRAPH || m_iDebugMode == PATH_TREE) {
		if (m_bZoomin)
			m_bZoomin = false;

		if (m_bZoomout)
			m_bZoomout = false;

		if (pDoc->m_bReload){
			ClearScreen();
		}

		m_bUndo = false;
		m_bSwitch = false;
		return CView::OnEraseBkgnd(pDC);
	} else {
		return TRUE;
	}
}


void CIntelligent_ScissorView::OnButtonFinish()
{
	// TODO: Add your command handler code here
	Finish();
	Invalidate(TRUE);
}


void CIntelligent_ScissorView::OnButtonUndo()
{
	// TODO: Add your command handler code here
	Undo();
	Invalidate(TRUE);
}


void CIntelligent_ScissorView::OnUpdateButtonZoomin(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	CIntelligent_ScissorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	pCmdUI->Enable(pDoc->m_bInitial && isWorkMode());
}


void CIntelligent_ScissorView::OnUpdateButtonZoomout(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	CIntelligent_ScissorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	pCmdUI->Enable(pDoc->m_bInitial && isWorkMode());
}


void CIntelligent_ScissorView::OnUpdateButtonFinish(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	CIntelligent_ScissorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	pCmdUI->Enable(pDoc->m_bInitial && pDoc->seed_count > 1 && isWorkMode());
}


void CIntelligent_ScissorView::OnUpdateButtonUndo(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	CIntelligent_ScissorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	pCmdUI->Enable(pDoc->m_bInitial && pDoc->seed_count != 0 && !pDoc->close && isWorkMode());
}






void CIntelligent_ScissorView::OnButtonClear()
{
	// TODO: Add your command handler code here
	ClearScreen();
	Invalidate(TRUE);
}


void CIntelligent_ScissorView::OnUpdateButtonClear(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	CIntelligent_ScissorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	pCmdUI->Enable(pDoc->m_bInitial && pDoc->seed_count != 0);
}



// Modifying functions for the image
void CIntelligent_ScissorView::ClearScreen() {

	CIntelligent_ScissorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	bool& close = pDoc->close;
	int& seed_count = pDoc->seed_count;

	Vec2i& seed = pDoc->seed;
	Vec2i& b_seed = pDoc->b_seed;
	Vec2d& scale = pDoc->scale;

	Mat& o_image = pDoc->o_image;
	Mat& image = pDoc->image;
	Mat& tem1 = pDoc->tem1;
	Mat& tem2 = pDoc->tem2;
	Mat& mask = pDoc->mask;

	stack<node**>& map_stack = pDoc->map_stack;
	stack<Mat>& mask_stack = pDoc->mask_stack;
	stack<Mat>& tem1_stack = pDoc->tem1_stack;
	stack<Vec2d>& scale_stack = pDoc->scale_stack;

	stop(tem1, o_image, mask, scale[1], map_stack, tem1_stack, mask_stack, pDoc->cost_map, image.rows, image.cols, close, seed_count, scale_stack, pDoc->node_list_stack);


	m_bZoomin = false;
	m_bZoomout = false;
	m_bClicked = true;
	
	m_iWorkMode = IMAGE_CONTOUR;
	SwitchToWorkMode();
	
	tem1.copyTo(tem2);

}

void CIntelligent_ScissorView::Undo() {

	CIntelligent_ScissorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	int& seed_count = pDoc->seed_count;

	if(seed_count != 0) {
		bool& close = pDoc->close;

		Vec2i& seed = pDoc->seed;
		Vec2i& b_seed = pDoc->b_seed;
		Vec2d& scale = pDoc->scale;

		Mat& o_image = pDoc->o_image;
		Mat& image = pDoc->image;
		Mat& tem1 = pDoc->tem1;
		Mat& tem2 = pDoc->tem2;
		Mat& mask = pDoc->mask;

		stack<node**>& map_stack = pDoc->map_stack;
		stack<Mat>& mask_stack = pDoc->mask_stack;
		stack<Mat>& tem1_stack = pDoc->tem1_stack;
		stack<Vec2d>& scale_stack = pDoc->scale_stack;

		if(seed_count == 1) {
			stop(tem1, o_image, mask, scale[1], map_stack, tem1_stack, mask_stack, pDoc->cost_map, image.rows, image.cols, close, seed_count, scale_stack, pDoc->node_list_stack);
		}
		else {
			pDoc->cost_map = undo(tem1, mask, map_stack, mask_stack, tem1_stack, pDoc->scale_stack, scale, pDoc->node_list_stack, pDoc->node_list);
			freopen("input.txt", "w", stdout);
			cout << "The number of nodes: " << pDoc->node_list.size() << endl;
			/*
			for (int i = 0; i < image.rows; i++ ){
				for (int j = 0; j < image.cols; j++) {
					if (pDoc->cost_map[i][j].pre[0] == -1 && pDoc->cost_map[i][j].pre[1] == -1) {
						seed[0] = j;
						seed[1] = i;
					}
				}
			}
			generate_tree(pDoc->cost_map, seed, image.rows, image.cols, pDoc->node_list);
			*/

			seed_count--;
		}
		
		tem1.copyTo(tem2);
		m_bUndo = true;
	}

}

void CIntelligent_ScissorView::Finish() {

	CIntelligent_ScissorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	bool& close = pDoc->close;
	int& seed_count = pDoc->seed_count;


	if (!close && seed_count > 1) {
		Vec2i& seed = pDoc->seed;
		Vec2i& b_seed = pDoc->b_seed;
		Vec2d& scale = pDoc->scale;

		Mat& o_image = pDoc->o_image;
		Mat& tem1 = pDoc->tem1;
		//Mat& tem2 = pDoc->tem2;
		Mat& mask = pDoc->mask;

		stack<node**>& map_stack = pDoc->map_stack;
		stack<Mat>& mask_stack = pDoc->mask_stack;
		stack<Mat>& tem1_stack = pDoc->tem1_stack;

		seed[0] = b_seed[0];
		seed[1] = b_seed[1];

		draw_path(tem1, mask, pDoc->cost_map, b_seed, true, scale[0] * scale[1]);
		draw_circle(tem1, b_seed, 0.5, false, scale);
		save_stack(mask,mask_stack);
		save_stack(tem1,tem1_stack);
		save_mask(mask, o_image, scale);
		save_node_list_stack(pDoc->node_list, pDoc->node_list_stack);
		//save_scale(scale[1], pDoc->scale_stack);

		close = true;
	}
}

void CIntelligent_ScissorView::ZoomOut() {

	CIntelligent_ScissorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if (isWorkMode()) {
		Mat& tem1 = pDoc->tem1;
		smaller(tem1);
		tem1.copyTo(pDoc->tem2);
	}
	Vec2d& scale = pDoc->scale;
	scale[1] = scale[1] / 2.0;

	m_bZoomout = true;

}

void CIntelligent_ScissorView::ZoomIn() {

	CIntelligent_ScissorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (isWorkMode()) {
		Mat& tem1 = pDoc->tem1;
		bigger(tem1);
		tem1.copyTo(pDoc->tem2);
	}
	Vec2d& scale = pDoc->scale;
	scale[1] = scale[1] * 2.0;

	m_bZoomin = true;
}


void CIntelligent_ScissorView::AdjustSize(Mat& image) {
	CIntelligent_ScissorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	
	double scale = pDoc->scale[1] * pDoc->scale[0];
	if (scale > 1) {
		while (scale > 1) {
			bigger(image);
			scale = scale / 2.0;
		}
	}
	else {
		while (scale < 1) {
			smaller(image);
			scale = scale * 2.0;
		}
	}
}

void CIntelligent_ScissorView::OnWorkmodeImageonly()
{
	// TODO: Add your command handler code here
	CIntelligent_ScissorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	m_iWorkMode = IMAGE_ONLY;

	SwitchToWorkMode();
	Invalidate(TRUE);
}


void CIntelligent_ScissorView::OnWorkmodeImagewithcontour()
{
	// TODO: Add your command handler code here
	CIntelligent_ScissorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	m_iWorkMode = IMAGE_CONTOUR;

	SwitchToWorkMode();
	Invalidate(TRUE);
}


void CIntelligent_ScissorView::OnDebugmodePixelnode()
{
	// TODO: Add your command handler code here
	CIntelligent_ScissorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	m_iDebugMode = PIXEL_NODE;

	SwitchToDebugMode();
	Invalidate(TRUE);
}


void CIntelligent_ScissorView::OnDebugmodeCostgraph()
{
	// TODO: Add your command handler code here
	CIntelligent_ScissorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	m_iDebugMode = COST_GRAPH;
	
	SwitchToDebugMode();
	Invalidate(TRUE);
}


void CIntelligent_ScissorView::OnDebugmodePathtree()
{
	// TODO: Add your command handler code here
	CIntelligent_ScissorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CPath_TreeDlg dlg;
	int result = dlg.DoModal();
	if (result == IDOK) {
		m_iDebugMode = PATH_TREE; 
		m_iNodeNum = dlg.m_EditNodeNum;
		DrawPathTree(pDoc->m_cvTreeMask, pDoc->m_cvCostGraph, m_iNodeNum, 
					 pDoc->cost_map, pDoc->seed, 
					 pDoc->cost_map_row, pDoc->cost_map_col, pDoc->node_list);				
		SwitchToDebugMode();
		Invalidate(TRUE);
	}

}


void CIntelligent_ScissorView::OnDebugmodeMinimumpath()
{
	// TODO: Add your command handler code here
	CIntelligent_ScissorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	m_iDebugMode = MINI_PATH;

	SwitchToDebugMode();
	Invalidate(TRUE);

}

void CIntelligent_ScissorView::SwitchToWorkMode() {
	m_bSwitch = true;
	m_iDebugMode = 0;
}

void CIntelligent_ScissorView::SwitchToDebugMode() {
	m_bSwitch = true;
	m_iWorkMode = 0;
}

bool CIntelligent_ScissorView::isWorkMode() {
	return m_iWorkMode != 0;
}




BOOL CIntelligent_ScissorView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (m_hAccel) 
   {
      if (::TranslateAccelerator(m_hWnd, m_hAccel, pMsg)) 
      {
         return(TRUE);
      }
   }
	return CView::PreTranslateMessage(pMsg);
}



