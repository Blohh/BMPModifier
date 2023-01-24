// HistogramViewer.cpp: plik implementacji
//

#include "stdafx.h"
#include "PO1.h"
#include "HistogramViewer.h"
#include "afxdialogex.h"


// Okno dialogowe HistogramViewer

IMPLEMENT_DYNAMIC(HistogramViewer, CDialogEx)

HistogramViewer::HistogramViewer(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_HistogramViewer, pParent)
{

}

HistogramViewer::~HistogramViewer()
{

}

void HistogramViewer::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

void HistogramViewer::OnPaint()
{
    CPaintDC dc(this);
    CRect imageRect(0, 0, image.getWidth(), image.getHeight());
    if (!displayBitmap) {
        if (bPP == 1)
        {
            /* TODO */
        }
        else if (bPP == 8)
        {
            int histogram[256] = { 0 };
            int histogramColsMax = 0;

            for (int x = 0; x < imageRect.Width(); x++)
            {
                for (int y = 0; y < imageRect.Height(); y++)
                {
                    int color = image.GetPixel8(x, y);
                    histogram[color]++;
                    if (histogram[color] > histogramColsMax)
                    {
                        histogramColsMax = histogram[color];
                    }
                }
            }

            for (int i = 0; i < 256; i++) {
                dc.MoveTo(cRect.left + i, cRect.bottom);
                dc.LineTo(cRect.left + i, cRect.bottom - histogram[i] * 255 / histogramColsMax);
            }
            dc.MoveTo(cRect.left + prog, cRect.bottom);
            dc.LineTo(cRect.left + prog, cRect.bottom - histogramColsMax);
        }
        else if (bPP == 24)
        {
            int histogramR[256] = { 0 };
            int histogramG[256] = { 0 };
            int histogramB[256] = { 0 };
            int histogramColsMax = 0;

            for (int x = 0; x < imageRect.Width(); x++)
            {
                for (int y = 0; y < imageRect.Height(); y++)
                {
                    RGBTRIPLE color = image.GetPixel24(x, y);
                    histogramR[color.rgbtRed]++;
                    histogramG[color.rgbtGreen]++;
                    histogramB[color.rgbtBlue]++;
                    if (histogramR[color.rgbtRed] > histogramColsMax)
                    {
                        histogramColsMax = histogramR[color.rgbtRed];
                    }
                    if (histogramG[color.rgbtGreen] > histogramColsMax)
                    {
                        histogramColsMax = histogramG[color.rgbtGreen];
                    }
                    if (histogramB[color.rgbtBlue] > histogramColsMax)
                    {
                        histogramColsMax = histogramB[color.rgbtBlue];
                    }
                }
            }

            for (int i = 0; i < 256; i++) {
                dc.MoveTo(cRect.left + i, cRect.bottom);
                dc.LineTo(cRect.left + i, cRect.bottom - histogramR[i] * 255 / histogramColsMax);

                dc.MoveTo(cRect.right + cRect.left + i, cRect.bottom);
                dc.LineTo(cRect.right + cRect.left + i, cRect.bottom - histogramG[i] * 255 / histogramColsMax);

                dc.MoveTo(cRect.right * 2 + cRect.left + i, cRect.bottom);
                dc.LineTo(cRect.right * 2 + cRect.left + i, cRect.bottom - histogramB[i] * 255 / histogramColsMax);
            }
        }
        return;
    }

    float maxVal = 0;

    for (uint32_t x = 0; x < accumulator.getWidth(); ++x)
    {
        for (uint32_t y = 0; y < accumulator.getHeight(); ++y)
        {
            if (accumulator.GetPixel8(x, y) > maxVal)
            {
                maxVal = accumulator.GetPixel8(x, y);
            }
        }
    }

    for (uint32_t x = 0; x < accumulator.getWidth(); ++x)
    {
        for (uint32_t y = 0; y < accumulator.getHeight(); ++y)
        {
            float color = accumulator.GetPixel8(x, y);
            color = 255 - (color / maxVal) * 255;
            accumulator.setPixel8(x, y, color);
        }
    }

    CRect clientRect, imgRect(0, 0, accumulator.getWidth(), accumulator.getHeight());
    GetClientRect(clientRect);
    accumulator.PaintDIB(dc, clientRect, imgRect);

    this->image.~BMPViewer();
    this->accumulator.~BMPViewer();
}

BOOL HistogramViewer::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    bPP = image.getBytesPerPixel();
    cRect = CRect(50, 50, 300 , 300);

    int histogramDialogWidth = 0;

    if (bPP == 1)
    {
        int histChannelWindowSize = image.getHeight();
        if (image.getWidth() > image.getHeight())
        {
            histChannelWindowSize = image.getWidth();
        }
        cRect.right = cRect.left + histChannelWindowSize;
        cRect.bottom = cRect.top + histChannelWindowSize;
        histogramDialogWidth = cRect.right * 2 + cRect.left;
    }
    else if (bPP == 8)
    {
        histogramDialogWidth = cRect.right + cRect.left;
    }
    else if (bPP == 24)
    {
        histogramDialogWidth = cRect.right * 3 + cRect.left;
    }

    SetWindowPos(NULL, 0, 0, histogramDialogWidth, cRect.bottom + cRect.top, SWP_NOMOVE | SWP_NOZORDER);
	return TRUE;
}


BEGIN_MESSAGE_MAP(HistogramViewer, CDialogEx)
    ON_WM_PAINT()
END_MESSAGE_MAP()


// Procedury obsługi komunikatów HistogramViewer
