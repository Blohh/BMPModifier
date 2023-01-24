#pragma once

// CImgWnd
#include "BMPViewer.h"

class CImgWnd : public CWnd
{
	DECLARE_DYNAMIC(CImgWnd)

public:
	BMPViewer image;
	bool imageLoaded = false;
	CImgWnd();
	virtual ~CImgWnd();

	BOOL Create(const RECT& rect, CWnd*  pParentWnd, UINT nID);
	CRect getClientRect();

protected:
	

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
};


