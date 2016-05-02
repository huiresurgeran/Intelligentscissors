
// Intelligent_ScissorDoc.h : interface of the CIntelligent_ScissorDoc class
//


#pragma once

#include <stack>
#include <iostream>
#include <math.h>
#include <vector>
#include "highgui.h"
#include "cv.h"
#include "cost.h"
#include "my_draw.h"
#include "scale.h"
using namespace cv;


class CIntelligent_ScissorDoc : public CDocument
{
protected: // create from serialization only
	CIntelligent_ScissorDoc();
	DECLARE_DYNCREATE(CIntelligent_ScissorDoc);

public:
	bool m_bInitial;
	bool m_bReload;

	// For OpenCV
public:
	// Work Mode
	Mat o_image;
	Mat image;
	Mat tem1;
	Mat tem2;
	Mat mask;
	
	// Debug Mode
	Mat m_cvPixelGraph;
	Mat m_cvCostGraph;
	Mat m_cvTreeMask;
	Mat m_cvMiniPath;

	bool close;
	int seed_count;
	Vec2i b_seed;
	Vec2i seed;
	Vec2i current;
	Vec2d scale;

	node** cost_map;
	vector<node*> node_list;
	int cost_map_col;
	int cost_map_row;

	stack<node**> map_stack;
	stack<Mat> mask_stack;
	stack<Mat> tem1_stack;
	stack<Vec2d> scale_stack;
	stack<vector<node*>> node_list_stack;

	/*
	Mat image_ori;
	Mat tem1_ori;
	Mat tem2_ori;
	Mat mask_ori;

	Mat m_cvPixelGraphOri;
	Mat m_cvCostGraphOri;
	Mat m_cvTreeMaskOri;
	Mat m_cvMiniPathOri;
	*/

	// Attributes
public:

	// Operations
public:

	// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

	// Implementation
public:
	virtual ~CIntelligent_ScissorDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	void ReleaseAndInitialize();
	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	afx_msg void OnUpdateActionInitial(CCmdUI *pCmdUI);
	afx_msg void OnFileSavecontour();
	afx_msg void OnFileSavemask();
};
