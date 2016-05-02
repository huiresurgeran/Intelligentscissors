
// Intelligent_ScissorView.h : interface of the CIntelligent_ScissorView class
//

#pragma once
#include "highgui.h"
#include "cv.h"
#include "scale.h"
#include "CvvImage.h"
// #include "Path_TreeDlg.h"
using namespace cv;

const int IMAGE_ONLY = 1;
const int IMAGE_CONTOUR = 2;

const int PIXEL_NODE = 1;
const int COST_GRAPH = 2;
const int PATH_TREE = 3;
const int MINI_PATH = 4;

class CIntelligent_ScissorView : public CView
{
protected: // create from serialization only
	CIntelligent_ScissorView();
	DECLARE_DYNCREATE(CIntelligent_ScissorView)


	/*
	BITMAPINFO* m_bmi;
	BITMAPINFOHEADER* m_bmih;
	unsigned int m_buffer[sizeof(BITMAPINFOHEADER) + 
		sizeof(RGBQUAD)*256];

	BITMAPINFO* dibInfo;
	BITMAPINFOHEADER* dibiHeader;
	*/

protected:
	// The displayed image
	Mat m_cvImg;

	// For file save
	HACCEL m_hAccel;

	bool m_bZoomin;
	bool m_bZoomout;
	bool m_bUndo;

	bool m_bImgOpen;
	bool m_bClicked;
	
	// For Work Mode
	int m_iWorkMode;
	// For Debug Mode
	int m_iDebugMode;

	bool m_bSwitch;
	int m_iNodeNum;
// Attributes
public:
	CIntelligent_ScissorDoc* GetDocument() const;

// Operations
public:
	void ZoomOut();
	void ZoomIn();
	void ClearScreen();
	void Undo();
	void Finish();

	bool isWorkMode(); // TRUE: work mode; FALSE: debug mode
	void SwitchToWorkMode();
	void SwitchToDebugMode();
	void AdjustSize(Mat& image);

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CIntelligent_ScissorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()


public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	afx_msg void OnButtonZoomin();
	afx_msg void OnButtonZoomout();
	afx_msg void OnButtonFinish();
	afx_msg void OnButtonUndo();
	afx_msg void OnButtonClear();

	afx_msg void OnUpdateButtonZoomin(CCmdUI *pCmdUI);
	afx_msg void OnUpdateButtonZoomout(CCmdUI *pCmdUI);
	afx_msg void OnUpdateButtonUndo(CCmdUI *pCmdUI);
	afx_msg void OnUpdateButtonFinish(CCmdUI *pCmdUI);
	afx_msg void OnUpdateButtonClear(CCmdUI *pCmdUI);

	afx_msg void OnWorkmodeImageonly();
	afx_msg void OnWorkmodeImagewithcontour();
	afx_msg void OnDebugmodePixelnode();
	afx_msg void OnDebugmodeCostgraph();
	afx_msg void OnDebugmodePathtree();
	afx_msg void OnDebugmodeMinimumpath();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

#ifndef _DEBUG  // debug version in Intelligent_ScissorView.cpp
inline CIntelligent_ScissorDoc* CIntelligent_ScissorView::GetDocument() const
   { return reinterpret_cast<CIntelligent_ScissorDoc*>(m_pDocument); }
#endif

