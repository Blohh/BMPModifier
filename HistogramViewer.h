#pragma once
#include "BMPViewer.h"

// Okno dialogowe HistogramViewer

class HistogramViewer : public CDialogEx
{
	DECLARE_DYNAMIC(HistogramViewer);

public:
	HistogramViewer(CWnd* pParent = nullptr);   // konstruktor standardowy
	virtual ~HistogramViewer();

// Dane okna dialogowego
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HistogramViewer };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // obsługa DDX/DDV

	DECLARE_MESSAGE_MAP();

public:
	void OnPaint();
	virtual BOOL OnInitDialog();
	BMPViewer image;
	bool displayBitmap = false;
	BMPViewer accumulator;
	int prog = 0;
	int bPP;
	CRect cRect;
};
