
// PO1Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "PO1.h"
#include "PO1Dlg.h"
#include "ParamsDlg.h"
#include "afxdialogex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define IMG_WND_ID_IN	100
#define IMG_WND_ID_OUT	101


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CPODlg dialog



CPODlg::CPODlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPODlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPODlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_combo1);
	DDX_Control(pDX, IDC_SLIDER1, CSlider);
}

BEGIN_MESSAGE_MAP(CPODlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_LOAD, &CPODlg::OnBnClickedButtonLoad)
	ON_BN_CLICKED(IDC_BUTTON_PROCESS, &CPODlg::OnBnClickedButtonProcess)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CPODlg::OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_PARAMS, &CPODlg::OnBnClickedButtonParams)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER1, &CPODlg::OnNMCustomdrawSlider1)
END_MESSAGE_MAP()


// CPODlg message handlers

BOOL CPODlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	CRect rDlg(7,7,407,407);
	MapDialogRect(rDlg);

	m_imgIN.Create(rDlg, this, IMG_WND_ID_IN);
		
	rDlg = CRect(530, 7, 930, 407);
	MapDialogRect(rDlg);

	m_imgOUT.Create(rDlg, this, IMG_WND_ID_OUT);
	
	m_combo1.AddString(L"1. convert to greyscale");
	m_combo1.SelectString(0, L"1. convert to greyscale");
	m_combo1.AddString(L"2. change luminosity");
	m_combo1.AddString(L"2. change contrast");
	m_combo1.AddString(L"2. exponentiation");
	m_combo1.AddString(L"2. negative");
	m_combo1.AddString(L"2. show histogram");
	m_combo1.AddString(L"2. align histogram");

	m_combo1.AddString(L"3. thresholding");
	m_combo1.AddString(L"3. iterational thresholding");
	m_combo1.AddString(L"3. gradient thresholding");


	m_combo1.AddString(L"4.1 averging filter");
	m_combo1.AddString(L"4.1 Gauss filter");
	m_combo1.AddString(L"4.1 Sobel horizontal filter");
	m_combo1.AddString(L"4.1 Sobel vertical filter");
	m_combo1.AddString(L"4.1 laplace filter");
	m_combo1.AddString(L"4.1 sharpening filter");

	m_combo1.AddString(L"4.2 Median filter 3x3 square");
	m_combo1.AddString(L"4.2 Median filter 5x5 square");
	m_combo1.AddString(L"4.2 Median filter 5x5 cross"); 

	m_combo1.AddString(L"4.3 LoG");

	m_combo1.AddString(L"5.1 skeletonization");

	m_combo1.AddString(L"6.1 dylatation");
	m_combo1.AddString(L"6.1 erosion");
	m_combo1.AddString(L"6.1 opening");
	m_combo1.AddString(L"6.1 closing");
	

	m_combo1.AddString(L"6.2 inner countur");
	m_combo1.AddString(L"6.2 outer countur");

	m_combo1.AddString(L"6.3 dylatation 8bit");
	m_combo1.AddString(L"6.3 erosion 8bit");
	m_combo1.AddString(L"6.3 opening 8bit");
	m_combo1.AddString(L"6.3 closing 8bit");
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPODlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPODlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting
		
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPODlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CPODlg::OnBnClickedButtonLoad()
{
	WCHAR strFilter[] = { L"Image Files (*.bmp)|*.bmp|All Files (*.*)|*.*||" };

	CFileDialog FileDlg(TRUE, NULL, NULL, 0, strFilter);
	/* Clear rects */
	int width = 598;
	int height = 648;
	CRect imageRect(0, 0, width, height);
	this->m_imgIN.imageLoaded = false;
	this->m_imgOUT.imageLoaded = false;
	m_imgOUT.image.CreateGrayScaleDIB(imageRect, 0, 0);
	m_imgIN.image.CreateGrayScaleDIB(imageRect, 0, 0);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++)
		{
			BYTE greyscale = 255;
			m_imgOUT.image.setPixel8(x, y, greyscale);
			m_imgIN.image.setPixel8(x, y, greyscale);

		}
	}
	m_imgOUT.InvalidateRect(NULL);
	m_imgIN.InvalidateRect(NULL);
	/* Load file */
	if (FileDlg.DoModal() == IDOK)
	{
		this->m_imgIN.image.loadDIB(FileDlg.GetPathName());
		this->m_imgIN.imageLoaded = true;
		this->m_imgIN.InvalidateRect(NULL);	
	}

	
}


void CPODlg::OnBnClickedButtonProcess()
{
	CString sOption;
	m_combo1.GetLBText(m_combo1.GetCurSel(), sOption);


	if (sOption == L"1. convert to greyscale")
	{
		CRect imageRect(0, 0, m_imgIN.image.getWidth(), m_imgIN.image.getHeight());
		m_imgOUT.image.CreateGrayScaleDIB(imageRect, 0, 0);
		for (int x = 0; x < m_imgIN.image.getWidth(); x++)
		{
			for (int y = 0; y < m_imgIN.image.getHeight(); y++)
			{
				BYTE color = 0;
				if (m_imgIN.image.getBytesPerPixel() == 1) {
					color = m_imgIN.image.getPixel1(x, y) * 255;
				}
				else if (m_imgIN.image.getBytesPerPixel() == 8)
				{
					color = m_imgIN.image.GetPixel8(x, y);
				}
				else if (m_imgIN.image.getBytesPerPixel() == 24)
				{
					RGBTRIPLE rgbtriple = m_imgIN.image.GetPixel24(x, y);
					color = rgbtriple.rgbtRed * 0.299 + rgbtriple.rgbtGreen * 0.587 + rgbtriple.rgbtBlue * 0.114;
				}
				m_imgOUT.image.setPixel8(x, y, color);
			}
		}

		this->m_imgOUT.imageLoaded = true;

		this->m_imgOUT.InvalidateRect(NULL);
	}
	else if (sOption == L"2. negative") {
		m_imgOUT.image.negative(&m_imgIN.image);
		this->m_imgOUT.imageLoaded = true;
		m_imgOUT.InvalidateRect(NULL);
	}
	else if (sOption == L"2. change luminosity") {
		CSlider.SetRange(-100, 100);
		m_imgOUT.image.changeLuminosity(&m_imgIN.image, CSlider.GetPos());
		this->m_imgOUT.imageLoaded = true;
		m_imgOUT.InvalidateRect(NULL);
	}

	else if (sOption == L"2. change contrast") {
		CSlider.SetRange(1, 50);
		m_imgOUT.image.changeContrast(&m_imgIN.image, (float)((double)CSlider.GetPos()) / 10.0f);
		this->m_imgOUT.imageLoaded = true;
		m_imgOUT.InvalidateRect(NULL);
	}
	else if (sOption == L"2. exponentiation") {
		CSlider.SetRange(1, 30); 
		m_imgOUT.image.exponentiation(&m_imgIN.image, (float)((double)CSlider.GetPos()) / 10.0f);
		this->m_imgOUT.imageLoaded = true;
		m_imgOUT.InvalidateRect(NULL);
	}
	else if (sOption == L"2. align histogram") {
		m_imgOUT.image.alignHistogram(&m_imgIN.image);
		this->m_imgOUT.imageLoaded = true;
		InvalidateRect(NULL);
	}
	else if (sOption == L"2. show histogram") {
		hViewer.image = m_imgIN.image;
		hViewer.DoModal();
	}

	else if (sOption == L"3. thresholding") {
		CSlider.SetRange(0, 255);
		m_imgOUT.image.thresholding(&m_imgIN.image, CSlider.GetPos());
		this->m_imgOUT.imageLoaded = true;
		m_imgOUT.InvalidateRect(NULL);
		hViewer.image = m_imgIN.image;
		hViewer.prog = CSlider.GetPos();
		hViewer.DoModal();
	}
	else if (sOption == L"3. iterational thresholding") {
		hViewer.prog = m_imgOUT.image.IterationalThresholding(&m_imgIN.image, 2);
		this->m_imgOUT.imageLoaded = true;
		m_imgOUT.InvalidateRect(NULL);
		hViewer.image = m_imgIN.image;
		hViewer.DoModal();
	}
	else if (sOption == L"3. gradient thresholding") {
		hViewer.prog = m_imgOUT.image.gradientThresholding(&m_imgIN.image);
		this->m_imgOUT.imageLoaded = true;
		m_imgOUT.InvalidateRect(NULL);
		hViewer.image = m_imgIN.image;
		hViewer.DoModal();
	}
	else if (sOption == L"4.1 averging filter") {
	m_imgOUT.image.averagingFilter(&m_imgIN.image);
	this->m_imgOUT.imageLoaded = true;
	m_imgOUT.InvalidateRect(NULL);
	}
	else if (sOption == L"4.1 Gauss filter") {
	m_imgOUT.image.GaussFilter(&m_imgIN.image);
	this->m_imgOUT.imageLoaded = true;
	m_imgOUT.InvalidateRect(NULL);
	}
	else if (sOption == L"4.1 Sobel horizontal filter") {
	m_imgOUT.image.SobelHorizontalFilter(&m_imgIN.image);
	this->m_imgOUT.imageLoaded = true;
	m_imgOUT.InvalidateRect(NULL);
	}
	else if (sOption == L"4.1 Sobel vertical filter") {
	m_imgOUT.image.SobelVerticalFilter(&m_imgIN.image);
	this->m_imgOUT.imageLoaded = true;
	m_imgOUT.InvalidateRect(NULL);
	}
	else if (sOption == L"4.1 laplace filter") {
	m_imgOUT.image.laplaceFilter(&m_imgIN.image);
	this->m_imgOUT.imageLoaded = true;
	m_imgOUT.InvalidateRect(NULL);
	}
	else if (sOption == L"4.1 sharpening filter") {
	m_imgOUT.image.sharpeningFilter(&m_imgIN.image);
	this->m_imgOUT.imageLoaded = true;
	m_imgOUT.InvalidateRect(NULL);
	}
	else if (sOption == L"4.2 Median filter 3x3 square") {
	m_imgOUT.image.medianFilterSquare3x3(&m_imgIN.image);
	this->m_imgOUT.imageLoaded = true;
	m_imgOUT.InvalidateRect(NULL);
	}
	else if (sOption == L"4.2 Median filter 5x5 square") {
	m_imgOUT.image.medianFilterSquare5x5(&m_imgIN.image);
	this->m_imgOUT.imageLoaded = true;
	m_imgOUT.InvalidateRect(NULL);
	}
	else if (sOption == L"4.2 Median filter 5x5 cross") {
	m_imgOUT.image.medianFilterCross5x5(&m_imgIN.image);
	this->m_imgOUT.imageLoaded = true;
	m_imgOUT.InvalidateRect(NULL);
	}
	else if (sOption == L"4.3 LoG") {
	CSlider.SetRange(1, 18);
	double val = ((double)CSlider.GetPos()) / 10.0f + 0.6;
	m_imgOUT.image.filterLoG(&m_imgIN.image, val);
	this->m_imgOUT.imageLoaded = true;
	m_imgOUT.InvalidateRect(NULL);
	}
	else if (sOption == L"5.1 skeletonization") {
	m_imgOUT.image.Pavlidis(&m_imgIN.image);
	this->m_imgOUT.imageLoaded = true;
	m_imgOUT.InvalidateRect(NULL);
	}
	else if (sOption == L"6.1 dylatation") {
	m_imgOUT.image.dylatation(&m_imgIN.image);
	this->m_imgOUT.imageLoaded = true;
	m_imgOUT.InvalidateRect(NULL);
	}
	else if (sOption == L"6.1 erosion") {
	m_imgOUT.image.erosion(&m_imgIN.image);
	this->m_imgOUT.imageLoaded = true;
	m_imgOUT.InvalidateRect(NULL);
	}
	else if (sOption == L"6.1 opening") {
	m_imgOUT.image.opening(&m_imgIN.image);
	this->m_imgOUT.imageLoaded = true;
	m_imgOUT.InvalidateRect(NULL);
	}
	else if (sOption == L"6.1 closing") {
	m_imgOUT.image.closing(&m_imgIN.image);
	this->m_imgOUT.imageLoaded = true;
	m_imgOUT.InvalidateRect(NULL);
	}

	else if (sOption == L"6.2 inner countur") {
	m_imgOUT.image.morfInt(&m_imgIN.image);
	this->m_imgOUT.imageLoaded = true;
	m_imgOUT.InvalidateRect(NULL);
	}
	else if (sOption == L"6.2 outer countur") {
	m_imgOUT.image.morfExt(&m_imgIN.image);
	this->m_imgOUT.imageLoaded = true;
	m_imgOUT.InvalidateRect(NULL);
	}

	else if (sOption == L"6.3 dylatation 8bit") {
	m_imgOUT.image.dylatationGrayscale(&m_imgIN.image);
	this->m_imgOUT.imageLoaded = true;
	m_imgOUT.InvalidateRect(NULL);
	}
	else if (sOption == L"6.3 erosion 8bit") {
	m_imgOUT.image.erosionGrayscale(&m_imgIN.image);
	this->m_imgOUT.imageLoaded = true;
	m_imgOUT.InvalidateRect(NULL);
	}
	else if (sOption == L"6.3 opening 8bit") {
	m_imgOUT.image.openingGrayscale(&m_imgIN.image);
	this->m_imgOUT.imageLoaded = true;
	m_imgOUT.InvalidateRect(NULL);
	}
	else if (sOption == L"6.3 closing 8bit") {
	m_imgOUT.image.closingGrayscale(&m_imgIN.image);
	this->m_imgOUT.imageLoaded = true;
	m_imgOUT.InvalidateRect(NULL);
	}
}


void CPODlg::OnBnClickedButtonSave()
{
	WCHAR strFilter[] = { L"Image Files (*.bmp)|*.bmp|All Files (*.*)|*.*||" };

	CFileDialog FileDlg(FALSE, NULL, NULL, 0, strFilter);

	if (FileDlg.DoModal() == IDOK)
	{
		m_imgOUT.image.saveDIB(FileDlg.GetPathName());
	}
}


void CPODlg::OnBnClickedButtonParams()
{
	CParamsDlg paramsDlg;
	CString s;
	
	if (paramsDlg.DoModal() == IDOK)
	{
		s = paramsDlg.m_sParams;
	}
}


void CPODlg::OnNMCustomdrawSlider1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Dodaj tutaj swój kod procedury obs³ugi powiadamiania kontrolki
	*pResult = 0;
}
