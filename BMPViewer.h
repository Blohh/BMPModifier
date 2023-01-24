#pragma once
#include <algorithm>
#include <vector>


class BMPViewer
{
private:
	/* LPVOID pointer to entire BMP Image stored in program data */
	LPVOID file = nullptr;

	/* Pointer to BMP byte table */
	void* table;

	/* BMP structures */
	BITMAPFILEHEADER header;
	BITMAPINFO info;
	BITMAPINFOHEADER infoHeader;

	int BMPOffset = 0;

	int height = 0, width = 0;
	int bytesPerPixel = 0;

	/* How much bytes is in one BMP line */
	int bytesPerLine = 0;

public:
	~BMPViewer();
	bool loadDIB(CString path);
	bool PaintDIB(HDC context, CRect destination, CRect source);
	int getWidth();
	int getHeight();
	int getBytesPerLine();
	int getBytesPerPixel();
	bool CreateGrayScaleDIB(CRect picture_size, int xPPM, int yPPM);
	bool getPixel1(int x, int y);
	BYTE GetPixel8(int x, int y);
	RGBTRIPLE GetPixel24(int x, int y);
	bool setPixel8(int x, int y, BYTE val);
	bool saveDIB(CString path);
	bool changeLuminosity(BMPViewer* orig, int val);
	bool exponentiation(BMPViewer* orig, double ex);
	bool changeContrast(BMPViewer* orig, double ex);
	bool negative(BMPViewer* orig);
	bool alignHistogram(BMPViewer* orig);
	int get8BitColour(BMPViewer* picture, int x, int y);
	/* LAB 3 */
	bool thresholding(BMPViewer* orig, int t);
	int IterationalThresholding(BMPViewer* orig, int epsilon);
	int gradientThresholding(BMPViewer* orig);
	int OtsuThresholding(BMPViewer* orig, int epsilon);

	/* LAB 4*/
	bool linearFilter(BMPViewer* orig,std::vector<std::vector<int>> mask, bool dontAdd127 = false);
	bool averagingFilter(BMPViewer* orig);
	bool GaussFilter(BMPViewer* orig);
	bool SobelHorizontalFilter(BMPViewer* orig);
	bool SobelVerticalFilter(BMPViewer* orig);
	bool laplaceFilter(BMPViewer* orig);
	bool sharpeningFilter(BMPViewer* orig);


	bool medianFilterSquare3x3(BMPViewer* orig);
	bool medianFilterSquare5x5(BMPViewer* orig);
	bool medianFilterCross5x5(BMPViewer* orig);

	bool filterLoG(BMPViewer* orig, double ex);
	bool logsharpening(BMPViewer* orig, std::vector<std::vector<double>> mask);

	/* LAB 5 */
	bool Pavlidis(BMPViewer* orig);

	/* Lab 6 */
	bool dylatation(BMPViewer* orig);
	bool erosion(BMPViewer* orig);
	bool opening(BMPViewer* orig);
	bool closing(BMPViewer* orig);

	bool erosion8b(BMPViewer* orig);
	bool dylatation8b(BMPViewer* orig);

	bool morfExt(BMPViewer* orig);
	bool morfInt(BMPViewer* orig);
	
	bool erosionGrayscale(BMPViewer* orig);
	bool dylatationGrayscale(BMPViewer* orig);

	bool openingGrayscale(BMPViewer* orig);
	bool closingGrayscale(BMPViewer* orig);
};

