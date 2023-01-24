// ImgWnd.cpp : implementation file
//

#include "stdafx.h"
#include "PO1.h"
#include "ImgWnd.h"


// CImgWnd

IMPLEMENT_DYNAMIC(CImgWnd, CWnd)

CImgWnd::CImgWnd()
{

}

CImgWnd::~CImgWnd()
{
}


BEGIN_MESSAGE_MAP(CImgWnd, CWnd)
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CImgWnd message handlers

BOOL CImgWnd::Create(const RECT& rect, CWnd*  pParentWnd, UINT nID)
{
	BOOL bRes;

	bRes = CWnd::Create(NULL, NULL,
		WS_BORDER | WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL, rect, pParentWnd, nID, NULL);

	ShowScrollBar(SB_BOTH, FALSE);

	return bRes;
}



void CImgWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	if (imageLoaded)
	{
		HDC context = dc.GetSafeHdc();

		CRect destination;
		GetClientRect(destination);

		CRect source;
		source.SetRect(0, 0, this->image.getWidth(), this->image.getHeight());

		this->image.PaintDIB(context, destination, source);
	}
	// Do not call CWnd::OnPaint() for painting messages
}
