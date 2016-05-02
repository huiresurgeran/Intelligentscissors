
#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "IntelScissor.h"
#endif

#include "Document.h"
#include "View.h"
#include "PathTreeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CView

IMPLEMENT_DYNCREATE(CView, CView)

BEGIN_MESSAGE_MAP(CView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_BUTTON_ZOOMIN, &CView::OnButtonZoomin)
	ON_COMMAND(ID_BUTTON_ZOOMOUT, &CView::OnButtonZoomout)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_BUTTON_FINISH, &CView::OnButtonFinish)
	ON_COMMAND(ID_BUTTON_UNDO, &CView::OnButtonUndo)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_ZOOMIN, &CView::OnUpdateButtonZoomin)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_ZOOMOUT, &CView::OnUpdateButtonZoomout)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_FINISH, &CView::OnUpdateButtonFinish)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_UNDO, &CView::OnUpdateButtonUndo)
	ON_COMMAND(ID_BUTTON_CLEAR, &CView::OnButtonClear)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_CLEAR, &CView::OnUpdateButtonClear)
	ON_COMMAND(ID_WORKMODE_IMAGEONLY, &CView::OnWorkmodeImageonly)
	ON_COMMAND(ID_WORKMODE_IMAGEWITHCONTOUR, &CView::OnWorkmodeImagewithcontour)
	ON_COMMAND(ID_DEBUGMODE_PIXELNODE, &CView::OnDebugmodePixelnode)
	ON_COMMAND(ID_DEBUGMODE_COSTGRAPH, &CView::OnDebugmodeCostgraph)
	ON_COMMAND(ID_DEBUGMODE_PATHTREE, &CView::OnDebugmodePathtree)
	ON_COMMAND(ID_DEBUGMODE_MINIMUMPATH, &CView::OnDebugmodeMinimumpath)
END_MESSAGE_MAP()

// CView construction/destruction

CView::CView()
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

CView::~CView()
{
}

BOOL CView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CView drawing
 
void CView::OnDraw(CDC* pDC)
{
	  // 读取文件
	CDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// 使重载按钮失效
	pDoc->m_bReload = false;
	//判断是哪种工作模式
	if (isWorkMode()) {
		switch (m_iWorkMode) {
		case IMAGE_ONLY:// 仅为图片，不能画点并选择区域
			m_cvImg = pDoc->image;
			AdjustSize(m_cvImg);
			break;
		case IMAGE_CONTOUR: // 可以画点并选择区域，使用智能剪刀算法
			if (pDoc->close) {// 如果路线闭合，则裁剪出闭合区域的图片
				m_cvImg = pDoc->mask;
			} else {
				m_cvImg = pDoc->tem2;
			}
			break;
		}
	} else { // 其他模式，包括Pixel graph，Cost graph等
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
	// 将opencv-IplImage结构图像画到HDC上
	HDC hDC = pDC->GetSafeHdc();
	IplImage img = m_cvImg;
	CvvImage cimg;
	cimg.CopyOf( &img );// 将IplImage复制为CvvImage
	CRect rect;
	rect.SetRect(0, 0, m_cvImg.cols, m_cvImg.rows);// 设置区域大小
	cimg.DrawToHDC(hDC, &rect); // 在hdc上显示IpImage

	ReleaseDC( pDC );// 释放hdc
	
	
}


// CView printing


void CView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CView::OnRButtonUp(UINT  nFlags, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CView diagnostics

#ifdef _DEBUG
void CView::AssertValid() const
{
	CView::AssertValid();
}

void CView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CDoc* CView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDoc)));
	return (CDoc*)m_pDocument;
}
#endif //_DEBUG







void CView::OnButtonZoomin()
{
	// TODO: Add your command handler code here
	ZoomIn();
	Invalidate(TRUE);
}


void CView::OnButtonZoomout()
{
	// TODO: Add your command handler code here
	ZoomOut();
	Invalidate(TRUE);
}


void CView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// 图片是否被加载
	m_bClicked = true;

	Mat& o_image = pDoc->o_image;
	Mat& image = pDoc->image;
	Mat& tem1 = pDoc->tem1;
	Mat& tem2 = pDoc->tem2;
	Mat& mask = pDoc->mask;
	
	double x = point.x;
	double y = point.y;
	//当工作模式处于可以选择点使用智能剪刀算法裁剪图片并且debug模式处于最短路径模式
	// x、y坐标在正常范围，标记点大于0，路径还未闭合
	if (pDoc->m_bInitial && m_iWorkMode == IMAGE_CONTOUR && !pDoc->close && // pDoc->seed_count != 0 && 
		(pDoc->seed_count != 0 || (pDoc->seed_count == 0 && (nFlags & MK_CONTROL))) &&
		x >= 0 && x < tem2.cols && y >= 0 && y < tem2.rows) {

		int& seed_count = pDoc->seed_count;// 标记点的个数
		bool& close = pDoc->close;// 路径是否闭合
		Vec2i& seed = pDoc->seed;
		Vec2i& b_seed = pDoc->b_seed;// 上一个标记点的坐标
		Vec2i& current = pDoc->current;// 当前点击的点坐标
		Vec2d& scale = pDoc->scale;// 缩放的指数

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

		 // 判断路径是否闭合
		if (!close) {
			//不闭合
			// 如果标记点个数大于1
			if (seed_count > 1 ) {
				// 计算鼠标点击的点与最开始标记点的距离
				double dis = sqrt((double)((seed[0]-b_seed[0])*(seed[0]-b_seed[0]) +
					(seed[1]-b_seed[1])*(seed[1]-b_seed[1])));
				//如果两个标记点的距离小于5，则视为覆盖
				int minimum_gap = 5;
				if (dis < minimum_gap) {
					close = true;
				}
			}
		}

		 //如果画完最新一个点以后，路径闭合
		if (close) {
			//画出路径和裁剪区域
			draw_path(tem1, mask, pDoc->cost_map, b_seed, true, scale[0] * scale[1]);
			draw_circle(tem1, b_seed, 0.5, false, scale);
			 //保存裁剪区域
			save_stack(mask,mask_stack);
			save_stack(tem1,tem1_stack);
			save_mask(mask, o_image, scale);
			save_scale(scale, scale_stack);
			save_node_list_stack(pDoc->node_list, pDoc->node_list_stack);
			
		}
		// 画完新的点和路径，路径不闭合
		else {
			if(seed_count > 1) {//标记点大于1个，画出标记点形成的路径
				draw_path(tem1, mask, pDoc->cost_map, seed, true, scale[0] * scale[1]);
				pDoc->cost_map = new_tree(pDoc->cost_map, image.rows, image.cols);
			}
			draw_circle(tem1, seed, 0.5, true, scale);

			save_stack(mask, mask_stack);
			save_stack(tem1, tem1_stack);
			save_scale(scale, scale_stack);

			generate_tree(pDoc->cost_map, seed, image.rows, image.cols, pDoc->node_list);
			map_stack.push(pDoc->cost_map);

			save_node_list_stack(pDoc->node_list, pDoc->node_list_stack);
		}
		Invalidate(TRUE);
	}
	m_bClicked = false;
	CView::OnLButtonDown(nFlags, point);
}


void CView::OnMouseMove(UINT nFlags, CPoint point)
{
	
	CDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	bool& close = pDoc->close;// 路径是否闭合
	int& seed_count = pDoc->seed_count;// 标记点的个数
	Mat& mask = pDoc->mask; // 被标记裁剪部分
	Mat& tem2 = pDoc->tem2;

	Vec2i& seed = pDoc->seed;
	Vec2i& b_seed = pDoc->b_seed;
	Vec2i& current = pDoc->current;
	Vec2d& scale = pDoc->scale;// 缩放指数

	bool is_debug = (m_iDebugMode == MINI_PATH);	
	double x = point.x;
	double y = point.y;


	if (!is_debug){ // 判断是否处于最短路径模式
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

	if (/* 当工作模式处于可以选择点使用智能剪刀算法裁剪图片并且debug模式处于最短路径模式*/
		(m_iWorkMode == IMAGE_CONTOUR || m_iDebugMode == MINI_PATH) &&
		/* x、y坐标在正常范围，标记点大于0，路径还未闭合*/
		x >= 0 && x < temp.cols-1 &&
		y >= 0 && y < temp.rows-1 && 
		seed_count > 0 && !m_bClicked && 
		pDoc->m_bInitial && !pDoc->close){

		// 画出从b_seed到当前鼠标点击点的贴合边缘的路径
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

 		//如果两个标记点的距离小于5，则视为覆盖
		int minimum_gap = 5;
		if (dis < minimum_gap) {
			draw_circle(tem2,current,0.5,false, scale);
		}

		Invalidate(TRUE);
	}
		
	CView::OnMouseMove(nFlags, point);
}


BOOL CView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	CDoc* pDoc = GetDocument();

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


void CView::OnButtonFinish()
{
	// TODO: Add your command handler code here
	Finish();
	Invalidate(TRUE);
}


void CView::OnButtonUndo()
{
	// TODO: Add your command handler code here
	Undo();
	Invalidate(TRUE);
}


void CView::OnUpdateButtonZoomin(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	CDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	pCmdUI->Enable(pDoc->m_bInitial && isWorkMode());
}


void CView::OnUpdateButtonZoomout(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	CDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	pCmdUI->Enable(pDoc->m_bInitial && isWorkMode());
}


void CView::OnUpdateButtonFinish(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	CDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	pCmdUI->Enable(pDoc->m_bInitial && pDoc->seed_count > 1 && isWorkMode());
}


void CView::OnUpdateButtonUndo(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	CDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	pCmdUI->Enable(pDoc->m_bInitial && pDoc->seed_count != 0 && !pDoc->close && isWorkMode());
}






void CView::OnButtonClear()
{
	// TODO: Add your command handler code here
	ClearScreen();
	Invalidate(TRUE);
}


void CView::OnUpdateButtonClear(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	CDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	pCmdUI->Enable(pDoc->m_bInitial && pDoc->seed_count != 0);
}



// Modifying functions for the image
void CView::ClearScreen() {

	CDoc* pDoc = GetDocument();
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

void CView::Undo() {

	CDoc* pDoc = GetDocument();
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

void CView::Finish() {

	CDoc* pDoc = GetDocument();
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

void CView::ZoomOut() {

	CDoc* pDoc = GetDocument();
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

void CView::ZoomIn() {

	CDoc* pDoc = GetDocument();
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


void CView::AdjustSize(Mat& image) {
	CDoc* pDoc = GetDocument();
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

void CView::OnWorkmodeImageonly()
{
	// TODO: Add your command handler code here
	CDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	m_iWorkMode = IMAGE_ONLY;

	SwitchToWorkMode();
	Invalidate(TRUE);
}


void CView::OnWorkmodeImagewithcontour()
{
	// TODO: Add your command handler code here
	CDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	m_iWorkMode = IMAGE_CONTOUR;

	SwitchToWorkMode();
	Invalidate(TRUE);
}


void CView::OnDebugmodePixelnode()
{
	// TODO: Add your command handler code here
	CDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	m_iDebugMode = PIXEL_NODE;

	SwitchToDebugMode();
	Invalidate(TRUE);
}


void CView::OnDebugmodeCostgraph()
{
	// TODO: Add your command handler code here
	CDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	m_iDebugMode = COST_GRAPH;
	
	SwitchToDebugMode();
	Invalidate(TRUE);
}


void CView::OnDebugmodePathtree()
{
	// TODO: Add your command handler code here
	CDoc* pDoc = GetDocument();
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


void CView::OnDebugmodeMinimumpath()
{
	// TODO: Add your command handler code here
	CDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	m_iDebugMode = MINI_PATH;

	SwitchToDebugMode();
	Invalidate(TRUE);

}

void CView::SwitchToWorkMode() {
	m_bSwitch = true;
	m_iDebugMode = 0;
}

void CView::SwitchToDebugMode() {
	m_bSwitch = true;
	m_iWorkMode = 0;
}

bool CView::isWorkMode() {
	return m_iWorkMode != 0;
}




BOOL CView::PreTranslateMessage(MSG* pMsg)
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



