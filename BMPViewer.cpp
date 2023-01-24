#include "stdafx.h"
#include "BMPViewer.h"

BMPViewer::~BMPViewer()
{
	HeapFree(GetProcessHeap(), 0, this->file);
}

bool BMPViewer::loadDIB(CString path)
{
	/* Open file and get its size */
	CFile bmp_file;

	if(!bmp_file.Open(path, CFile::modeReadWrite))
		return false;



	/* Read BMP File Header */
	bmp_file.Read((BYTE*)&header, sizeof(header));
	
	/* Set pointer to begining of file */
	if (this->file != nullptr) {
		HeapFree(GetProcessHeap(), 0, this->file);
		this->file = nullptr;
	}
	this->file = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, header.bfSize - sizeof(BITMAPFILEHEADER));
	/* Read BMPINFO */
	bmp_file.Read(file, header.bfSize - sizeof(BITMAPFILEHEADER));


	/* Manage BMP headers */
	this->infoHeader = *(BITMAPINFOHEADER*)file;
	height = infoHeader.biHeight;
	width = infoHeader.biWidth;
	

	bytesPerPixel = infoHeader.biBitCount;

	//this->table = ((BYTE*)this->file + this->header.bfOffBits - sizeof(BITMAPFILEHEADER));

	/* Get bytesPerLine value */
	this->bytesPerLine = ((infoHeader.biBitCount) * (width) + 31) / 32;
	this->bytesPerLine *= 4;

	this->bytesPerPixel = infoHeader.biBitCount;

	BMPOffset = header.bfOffBits - sizeof(BITMAPFILEHEADER);

	bmp_file.Close();

	if (height == 0 || width == 0)
	{
		return false;
	}
	return true;
}

bool BMPViewer::PaintDIB(HDC context, CRect destination, CRect source) 
{
	/* Scaling */
	double srcProportions = (double)source.Width() / (double)source.Height();
	double proportions = (double)destination.Width() / (double)destination.Height();
	double srcProportionsH = (double)source.Height() / (double)source.Width();
	double proportionsH = (double)destination.Height() / (double)destination.Width();

	double hScale = (double)source.Height() / (double)destination.Height();
	double wScale = (double)source.Width() / (double)destination.Width();

	double multiplierH = 1.0f, multiplierW = 1.0f;

	double multiplierH1 = 1.0f, multiplierH2 = 1.0f, multiplierW1 = 1.0f, multiplierW2 = 1.0f;
	if (srcProportions < proportions) {
		//reduce height
		multiplierH1 = srcProportions / proportions;
		multiplierW1 = multiplierH1 * srcProportions * proportionsH;
	}
	else if (srcProportions > proportions) {
		//reduce width
		multiplierW1 = proportions / srcProportions;
		multiplierH1 = multiplierW1 * srcProportionsH * proportions;
	}


	if (srcProportionsH < proportionsH) {
		//reduce height
		multiplierH2 = srcProportionsH / proportionsH;
		multiplierW2 = multiplierH2 * srcProportions * proportionsH;
	}
	else if (srcProportionsH > proportionsH) {
		//reduce width
		multiplierW2 = proportionsH / srcProportionsH;
		multiplierH2 = multiplierW2 * srcProportionsH * proportions;
	}

	double field1 = (double)destination.Width() * multiplierW1 * (double)destination.Height() * multiplierH1;
	double field2 = (double)destination.Width() * multiplierW2 * (double)destination.Height() * multiplierH2;

	if (field1 > field2)
	{
		multiplierH = multiplierH1;
		multiplierW = multiplierW1;
	}
	else
	{
		multiplierH = multiplierH2;
		multiplierW = multiplierW2;
	}

	destination.right = destination.left + destination.Width() * multiplierW;
	destination.bottom = destination.top + destination.Height() * multiplierH;

		StretchDIBits(
			context,
			0,
			0,
			destination.Width(),
			destination.Height(),
			0,
			0,
			source.Width(),
			source.Height(),
			(BYTE*)file + header.bfOffBits - sizeof(BITMAPFILEHEADER),
			(BITMAPINFO*)file,
			DIB_RGB_COLORS,
			SRCCOPY
		);
	
	return true;
}

int BMPViewer::getWidth()
{
	return width;
}

int BMPViewer::getHeight()
{
	return height;
}

int BMPViewer::getBytesPerLine()
{
	return bytesPerLine;
}

int BMPViewer::getBytesPerPixel()
{
	return bytesPerPixel;
}

bool BMPViewer::CreateGrayScaleDIB(CRect picture_size, int xPPM, int yPPM)
{
	width = picture_size.Width(); height = picture_size.Height();
	bytesPerLine = ((8 * width + 31) / 32) * 4;
	BMPOffset = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256;

	infoHeader =
	{
		sizeof(BITMAPINFOHEADER),
		width,
		height,
		1,
		8,
		BI_RGB,
		0,
		xPPM,
		yPPM,
		256,
		256
	};
	header =
	{
		0x4D42,
		BMPOffset + sizeof(BITMAPFILEHEADER) + (bytesPerLine * height),
		0,
		0,
		BMPOffset + sizeof(BITMAPFILEHEADER)
	};
	RGBQUAD colors[256];
	for (int i = 0; i < 256; ++i)
	{
		colors[i].rgbBlue = i;
		colors[i].rgbGreen = i;
		colors[i].rgbRed = i;
	}

	/* Set pointer to BMP structure */
	if (this->file != nullptr) {
		HeapFree(GetProcessHeap(), 0, this->file);
		this->file = nullptr;
	}
	file = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256 + (bytesPerLine * height));
	
	memcpy(file, &infoHeader, sizeof(infoHeader));
	memcpy((BYTE*)file + sizeof(infoHeader), colors, sizeof(RGBQUAD) * 256);


	return true;
}

bool BMPViewer::getPixel1(int x, int y)
{
	if (x < width && y < height) {
		int row = (height - y - 1) * bytesPerLine;
		return *((BYTE*)file + BMPOffset + (row + (x / 8))) & (256 >> ((x % 8) + 1));
	}
	return false;
}

BYTE BMPViewer::GetPixel8(int x, int y)
{
	if (x < width && y < height && x > 0 && y > 0) {
		int row = (height - y - 1) * bytesPerLine;
		return *((BYTE*)file + BMPOffset + row + x);
	}
	return -1;
}

RGBTRIPLE BMPViewer::GetPixel24(int x, int y)
{
	if (x < width && y < height) {
		int row = (height - y - 1) * bytesPerLine;
		return *(RGBTRIPLE*)((BYTE*)file + BMPOffset + row + (x * 3));
	}
	return RGBTRIPLE();
}

bool BMPViewer::setPixel8(int x, int y, BYTE val)
{
	if (x < width && y < height) {
		int row = (height - y - 1) * bytesPerLine;
		*((BYTE*)file + BMPOffset + row + x) = val;
		if (*((BYTE*)file + BMPOffset + row + x) == 0) {
			int alarm = 0;
		}
		return true;
	}

	return false;
}

bool BMPViewer::saveDIB(CString path)
{

	/* Open file and get its size */
	CFile bmp_file;

	if (!bmp_file.Open(path, CFile::modeCreate | CFile::modeWrite))
		return false;

	/* Get and write entire BMP file data program */
	bmp_file.Write((BYTE*)&header, sizeof(BITMAPFILEHEADER));
	bmp_file.Write((BYTE*)file, header.bfSize - sizeof(BITMAPFILEHEADER));

	bmp_file.Close();
	return true;
}

bool BMPViewer::changeLuminosity(BMPViewer* orig, int val)
{

	CRect imageRect(0, 0, orig->getWidth(), orig->getHeight());
	bool result = this->CreateGrayScaleDIB(imageRect, 0, 0);
	if (result == false) return result;

	for (int y = 0; y < orig->getHeight(); y++)
	{
		for (int x = 0; x < orig->getWidth(); x++)
		{
			int color = get8BitColour(orig, x, y) + val;
			if (color > 255) color = 255;
			else if (color < 0) color = 0;
			result = this->setPixel8(x, y, color);
			if (result == false) return result;
		}
	}
	return true;
}

bool BMPViewer::exponentiation(BMPViewer* orig, double ex)
{

	CRect imageRect(0, 0, orig->getWidth(), orig->getHeight());
	bool result = this->CreateGrayScaleDIB(imageRect, 0, 0);
	if (result == false) return result;
	for (int y = 0; y < orig->getHeight(); y++)
	{
		for (int x = 0; x < orig->getWidth(); x++)
		{
			double value = pow((double)(get8BitColour(orig, x, y)) / 255.0f, ex) * 255;
			result = this->setPixel8(x, y, value);
			if (result == false) return result;
		}
	}
	return true;
}

bool BMPViewer::changeContrast(BMPViewer* orig, double ex)
{
	CRect imageRect(0, 0, orig->getWidth(), orig->getHeight());
	bool result = this->CreateGrayScaleDIB(imageRect, 0, 0);
	if (result == false) return result;

	for (int y = 0; y < orig->getHeight(); y++)
	{
		for (int x = 0; x < orig->getWidth(); x++)
		{
			int offset = 127 * ex - 127;
			int color = get8BitColour(orig, x, y)* ex - offset;
			if (color == 0)
			{
				int alarm = 0;
			}
			if (color > 255) color = 255;
			else if (color < 0) color = 0;
			if (color == 0)
			{
				int alarm = 0;
			}
			result = this->setPixel8(x, y, color);
			if (color == 0)
			{
				int alarm = 0;
			}
			if (result == false) return result;
		}
	}
	return true;
}

bool BMPViewer::negative(BMPViewer* orig)
{
	CRect imageRect(0, 0, orig->getWidth(), orig->getHeight());
	bool result = this->CreateGrayScaleDIB(imageRect, 0, 0);
	if (result == false) return result;
	
		for (int y = 0; y < orig->getHeight(); y++)
		{
			for (int x = 0; x < orig->getWidth(); x++)
			{
				int color = get8BitColour(orig, x, y);

				result = this->setPixel8(x, y, 255 - color);
				if (result == false) return result;
			}
		}
	return true;
}
/* ALL BIT VERSIONS */
bool BMPViewer::alignHistogram(BMPViewer* orig)
{
	CRect imageRect(0, 0, orig->getWidth(), orig->getHeight());
	bool result = this->CreateGrayScaleDIB(imageRect, 0, 0);
	if (result == false) return result;

	int 
		histogramColors[256] = { 0 },
		newLuminosity = 0,
		countOfPoints = imageRect.Width() * imageRect.Height();

	float distribution[256] = { 0.0f };
	for (int y = 0; y < orig->getHeight(); y++)
	{
		for (int x = 0; x < orig->getWidth(); x++)
		{
			int color = get8BitColour(orig, x, y);
			histogramColors[color]++;
		}
	}

	for (int i = 0; i < 256; i++)
	{
		newLuminosity += histogramColors[i];
		distribution[i] = ((float)newLuminosity * 255.0f) / (float)countOfPoints;
	}

	for (int x = 0; x < imageRect.Width(); x++)
	{
		for (int y = 0; y < imageRect.Height(); y++)
		{
			int color = get8BitColour(orig, x, y);
			this->setPixel8(x, y, (int)distribution[color]);
		}
	}

	return true;
}

int BMPViewer::get8BitColour(BMPViewer* picture, int x, int y)
{
	int colour = 0;
	if (picture->getBytesPerPixel() == 1) {
		colour = picture->getPixel1(x, y) * 255;
	}
	else if (picture->getBytesPerPixel() == 8)
	{
		colour = picture->GetPixel8(x, y);
	}
	else if (picture->getBytesPerPixel() == 24)
	{
		RGBTRIPLE rgbtriple = picture->GetPixel24(x, y);
		colour = rgbtriple.rgbtRed * 0.299 + rgbtriple.rgbtGreen * 0.587 + rgbtriple.rgbtBlue * 0.114;
	}
	return colour;
}

bool BMPViewer::thresholding(BMPViewer* orig, int t)
{
	CRect imageRect(0, 0, orig->getWidth(), orig->getHeight());
	bool result = this->CreateGrayScaleDIB(imageRect, 0, 0);

	for (int x = 0; x < this->getWidth(); x++)
	{
		for (int y = 0; y < this->getHeight(); y++)
		{
			int color = get8BitColour(orig, x, y);
			if (color <= t)
				result = this->setPixel8(x, y, 0);
			else
				result = this->setPixel8(x, y, 255);

			if (result == false) return result;
		}
	}
	return true;
}

int BMPViewer::IterationalThresholding(BMPViewer* orig, int epsilon)
{
	this->alignHistogram(orig);

	int histogramColors[256] = { 0 };
	for (int y = 0; y < orig->getHeight(); y++)
	{
		for (int x = 0; x < orig->getWidth(); x++)
		{
			int color = get8BitColour(orig, x, y);
			histogramColors[color]++;
		}
	}

	int t0 = 127, t_previous = 1;
	int avargeBlack = 0, avargeWhite = 0;
	int p0 = 0, p1 = 0;
	int m0 = 0, m1 = 0;

	do {
		p0 = 0; p1 = 0; m0 = 0; m1 = 0;

		for (int i = 0; i <= t0; i++)
		{
			p0 += histogramColors[i];
		}

		for (int i = t0 + 1; i < 256; i++)
		{
			p1 += histogramColors[i];
		}

		for (int i = 0; i <= t0; i++)
		{
			m0 += (i * histogramColors[i])/p0;
		}

		for (int i = t0 + 1; i < 256; i++)
		{
			m1 += (i * histogramColors[i])/p1;
		}
		t_previous = t0;
		t0 = (m0 + m1) / 2;
	}
	while (abs(t0 - t_previous) >= epsilon);
	this->thresholding(orig, t0);
	return t0;
}

int BMPViewer::gradientThresholding(BMPViewer* orig)
{
	int width = orig->getWidth();
	int height = orig->getHeight();
	int G = 0;
	int t_up = 0, t_down = 0;
	for (int x = 0; x < this->getWidth(); x++)
	{
		for (int y = 0; y < this->getHeight(); y++)
		{
			int Gx = max(get8BitColour(orig, x + 1, y), 0) - max(get8BitColour(orig, x - 1, y), 0);
			int Gy = max(get8BitColour(orig, x, y + 1), 0) - max(get8BitColour(orig, x, y - 1), 0);

			G = max(Gx, Gy);
			t_up += get8BitColour(orig, x, y) * G;
			t_down += G;
		}
	}
	int t = t_up / t_down;
	this->thresholding(orig, t);
	return t;
}

bool BMPViewer::linearFilter(BMPViewer* orig, std::vector<std::vector<int>> mask, bool dontAdd127)
{
	CRect imageRect(0, 0, orig->getWidth(), orig->getHeight());
	bool result = this->CreateGrayScaleDIB(imageRect, 0, 0);
	bool hasNegativeValues = false;
	for (int x = 0; x < this->getWidth(); x++)
	{
		for (int y = 0; y < this->getHeight(); y++)
		{
			double color_up = 0.0f, color_down = 0.0f;
			int color = 0;
			for (int i = 0; i < mask.size(); i++) {
				for (int j = 0; j < mask.size(); j++) {
					color_up += (double)max(get8BitColour(orig, x - i, y - j), 0)*mask[i][j];
					color_down+= mask[i][j];
					if (mask[i][j] < 0) hasNegativeValues = true;
				}
			}
			if(!hasNegativeValues || dontAdd127) color = color_up / color_down;
			else color = color_up +  127;

			if (color > 255) color = 255;
			else if (color < 0) color = 0;

			result = this->setPixel8(x, y, color);
			if (result == false) return result;
		}
	}
	return true;
}

bool BMPViewer::averagingFilter(BMPViewer* orig)
{
	std::vector<std::vector<int>> mask = { {1, 1, 1}, {1, 1, 1}, {1, 1, 1} };
	return this->linearFilter(orig, mask);
}

bool BMPViewer::GaussFilter(BMPViewer* orig)
{
	std::vector<std::vector<int>> mask = { {1, 4, 1}, {4, 12, 4}, {1, 4, 1} };
	return this->linearFilter(orig, mask);
}

bool BMPViewer::SobelHorizontalFilter(BMPViewer* orig)
{
	std::vector<std::vector<int>> mask = { {1, 2, 1}, {0, 0, 0}, {-1, -2, -1} };
	return this->linearFilter(orig, mask);
}

bool BMPViewer::SobelVerticalFilter(BMPViewer* orig)
{
	std::vector<std::vector<int>> mask = { {-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1} };
	return this->linearFilter(orig, mask);
}

bool BMPViewer::laplaceFilter(BMPViewer* orig)
{
	std::vector<std::vector<int>> mask = { {-2, 1, -2}, {1, 4, 1}, {-2, 1, -2} };
	return this->linearFilter(orig, mask);
}

bool BMPViewer::sharpeningFilter(BMPViewer* orig)
{
	std::vector<std::vector<int>> mask = { {0, -1, 0}, {-1, 5, -1}, {0, -1, 0} };
	return this->linearFilter(orig, mask, true);
}

bool BMPViewer::medianFilterSquare3x3(BMPViewer* orig)
{	
	CRect imageRect(0, 0, orig->getWidth(), orig->getHeight());
	bool result = this->CreateGrayScaleDIB(imageRect, 0, 0);
	bool hasNegativeValues = false;
	for (int x = 0; x < this->getWidth(); x++)
	{
		for (int y = 0; y < this->getHeight(); y++)
		{
			std::vector<int> median;
			double color_up = 0.0f, color_down = 0.0f;
			
			for (int i = -1; i < 2; i++) {
				for (int j = -1; j < 2; j++) {
					median.push_back(max(get8BitColour(orig, x - i, y - j), 0));
				}
			}
			sort(median.begin(), median.end());
			int color = (median[4]+median[5])/2;
			if (color > 255) color = 255;
			else if (color < 0) color = 0;

			result = this->setPixel8(x, y, color);
			if (result == false) return result;
		}
	}
	return true;
}

bool BMPViewer::medianFilterSquare5x5(BMPViewer* orig)
{
	CRect imageRect(0, 0, orig->getWidth(), orig->getHeight());
	bool result = this->CreateGrayScaleDIB(imageRect, 0, 0);
	bool hasNegativeValues = false;
	for (int x = 0; x < this->getWidth(); x++)
	{
		for (int y = 0; y < this->getHeight(); y++)
		{
			std::vector<int> median;
			double color_up = 0.0f, color_down = 0.0f;

			for (int i = -2; i < 2; i++) {
				for (int j = -2; j < 2; j++) {
					median.push_back(max(get8BitColour(orig, x - i, y - j), 0));
				}
			}
			sort(median.begin(), median.end());
			int color = (median[7] + median[8]) / 2;
			if (color > 255) color = 255;
			else if (color < 0) color = 0;

			result = this->setPixel8(x, y, color);
			if (result == false) return result;
		}
	}
	return true;
}

bool BMPViewer::medianFilterCross5x5(BMPViewer* orig)
{
	CRect imageRect(0, 0, orig->getWidth(), orig->getHeight());
	bool result = this->CreateGrayScaleDIB(imageRect, 0, 0);
	bool hasNegativeValues = false;
	for (int x = 0; x < this->getWidth(); x++)
	{
		for (int y = 0; y < this->getHeight(); y++)
		{
			std::vector<int> median;
			double color_up = 0.0f, color_down = 0.0f;


			for (int i = -2; i < 0; i++) {
				median.push_back(max(get8BitColour(orig, x - i, y), 0));
				median.push_back(max(get8BitColour(orig,x, y - i), 0));
			}
			median.push_back(max(get8BitColour(orig,x, y), 0));
			for (int i = 1; i < 3; i++) {
				median.push_back(max(get8BitColour(orig,x - i, y ), 0));
				median.push_back(max(get8BitColour(orig,x, y - i), 0));
			}
			sort(median.begin(), median.end());
			int color = (median[4] + median[5]) / 2;
			if (color > 255) color = 255;
			else if (color < 0) color = 0;

			result = this->setPixel8(x, y, color);
			if (result == false) return result;
		}
	}
	return true;
}

bool BMPViewer::filterLoG(BMPViewer* orig, double ex)
{
	std::vector<std::vector<double>> LoG;

	int delt = 1 + 2 * ceil(2.5 * ex);

	LoG.resize(delt);
	double sum = 0;
	for (int x = 0; x < delt; x++) {
		LoG[x].resize(delt);
		
		for (int y = 0; y < delt; y++) {

			double tmp = -(pow(x, 2) + pow(y, 2) / (2 * pow(ex, 2)));
			double val = -((pow(x, 2) + pow(y, 2) - pow(ex, 2)) / (pow(ex, 4))) * exp(tmp);
			LoG[x][y] = val;
			sum += val;
		}
	}

	logsharpening(orig, LoG);
	return false;
}

bool BMPViewer::logsharpening(BMPViewer* orig, std::vector<std::vector<double>> mask)
{
	CRect imageRect(0, 0, orig->getWidth(), orig->getHeight());
	bool result = this->CreateGrayScaleDIB(imageRect, 0, 0);
	for (int x = 0; x < this->getWidth(); x++)
	{
		for (int y = 0; y < this->getHeight(); y++)
		{
			int color = 0;
			for (int i = 0; i < mask.size(); i++) {
				for (int j = 0; j < mask.size(); j++) {
					color += (double)max(get8BitColour(orig,x - i, y - j), 0) * mask[i][j];
				}
			}
			color += 127;

			if (color > 0 ) color = 255;
			else color = 0;

			result = this->setPixel8(x, y, color);
			if (result == false) return result;
		}
	}
	return true;
}

bool BMPViewer::Pavlidis(BMPViewer* orig)
{
	/* Create empty picture */
	CRect imageRect(0, 0, orig->getWidth(), orig->getHeight());
	bool result = this->CreateGrayScaleDIB(imageRect, 0, 0);

	int X_SIZE = orig->getWidth(), Y_SIZE = orig->getHeight();
	/* Vector used to store image used to skeletezation */
	std::vector<std::vector<double>> skl;
	/* Get a picture in form of 0-1 meaning white-black */
	skl.resize(X_SIZE);
	for (int x = 0; x < X_SIZE; x++) {
		skl[x].resize(Y_SIZE);

		for (int y = 0; y < Y_SIZE; y++) {
			skl[x][y] = (get8BitColour(orig,x, y) == 255) ? 0 : 1;
		}
	}
	/* Skeletezation */
	bool remain = true;
	while (remain) {
		for (int j = 1; j <= 7; j += 2) {
			/* Iterate through all pixels */
			remain = false;
			for (int x = 1; x < X_SIZE - 1; x++) {
				for (int y = 1; y < Y_SIZE - 1; y++) {
					if (skl[x][y] == 1) {
						bool j_neightbourEqualsZero = false;
						switch (j) {
						case 1:
							j_neightbourEqualsZero = (skl[x + 1][y] == 0);
							break;
						case 3:
							j_neightbourEqualsZero = (skl[x][y + 1] == 0);
							break;
						case 5:
							j_neightbourEqualsZero = (skl[x - 1][y] == 0);
							break;
						case 7:
							j_neightbourEqualsZero = (skl[x][y - 1] == 0);
							break;
						default:
							return false;
						}
						if (j_neightbourEqualsZero) {
							bool skel = false;
							/* CHECK ALL MASKS */
							while (true) {
								/* 1
								X X X
								0 P 0
								Y Y Y
								*/
								if (skl[x - 1][y] == 0 && skl[x + 1][y] == 0) {
									if (skl[x - 1][y + 1] != 0 || skl[x][y + 1] != 0 || skl[x + 1][y + 1] != 0) {
										if (skl[x - 1][y - 1] != 0 || skl[x][y - 1] != 0 || skl[x + 1][y - 1] != 0) {
											skel = true;
										}
									}
								}
								if (skel) break;
								/* 2
								Y 0 X
								Y P X
								Y 0 X
								*/
								if (skl[x][y - 1] == 0 && skl[x][y + 1] == 0) {
									if (skl[x - 1][y + 1] != 0 || skl[x - 1][y] != 0 || skl[x - 1][y - 1] != 0) {
										if (skl[x + 1][y + 1] != 0 || skl[x + 1][y] != 0 || skl[x + 1][y - 1] != 0) {
											skel = true;
										}
									}
								}
								if (skel) break;
								/* 3
								X X X
								0 P X
								1 0 X
								*/
								if (skl[x - 1][y] == 0 && skl[x][y + 1] == 0 && (skl[x - 1][y + 1] == 1 || skl[x - 1][y + 1] == 2) ) {
									if (skl[x - 1][y - 1] != 0 || 
										skl[x][y - 1] != 0 || 
										skl[x + 1][y - 1] != 0 || 
										skl[x + 1][y] != 0 || 
										skl[x + 1][y + 1] != 0) {
										skel = true;	
									}
								}
								if (skel) break;
								/* 4
								1 0 X
								0 P X
								X X X
								*/
								if (skl[x - 1][y] == 0 && skl[x][y - 1] == 0 && (skl[x - 1][y - 1] == 1 || skl[x - 1][y - 1] == 2)) {
									if (skl[x - 1][y + 1] != 0 ||
										skl[x][y + 1] != 0 ||
										skl[x + 1][y - 1] != 0 ||
										skl[x + 1][y] != 0 ||
										skl[x + 1][y + 1] != 0) {
										skel = true;
									}
								}
								if (skel) break;
								/* 5
								X 0 1
								X P 0
								X X X
								*/
								if (skl[x + 1][y] == 0 && skl[x][y - 1] == 0 && (skl[x + 1][y - 1] == 1 || skl[x + 1][y - 1] == 2) ) {
									if (skl[x - 1][y + 1] != 0 ||
										skl[x][y + 1] != 0 ||
										skl[x - 1][y - 1] != 0 ||
										skl[x - 1][y] != 0 ||
										skl[x + 1][y + 1] != 0) {
										skel = true;
									}
								}
								if (skel) break;
								/* 6
								X X X
								X P 0
								X 0 1
								*/
								if (skl[x + 1][y] == 0 && skl[x][y + 1] == 0 &&(skl[x + 1][y + 1] == 1 || skl[x + 1][y + 1] == 2) ) {
									if (skl[x - 1][y - 1] != 0 ||
										skl[x][y - 1] != 0 ||
										skl[x - 1][y - 1] != 0 ||
										skl[x - 1][y] != 0 ||
										skl[x - 1][y + 1] != 0) {
										skel = true;
									}
								}
								break;
							}
							if (skel) {
								skl[x][y] = 2; // skeleton
							}
							else {
								skl[x][y] = 3; // deletable value
								remain = true;
							}
						}
					}
					
				}
			}
			/* delete deletable values */
			/* here translate skl to pixels values */
			for (int x = 0; x < X_SIZE; x++) {
				for (int y = 0; y < Y_SIZE; y++) {
					if (skl[x][y] == 3) skl[x][y] = 0; // delete pixel

					if (skl[x][y] == 0) {
						this->setPixel8(x, y, 255);
					}
					else this->setPixel8(x, y, 0);
				}
			}
		}
	}

}

bool BMPViewer::dylatation(BMPViewer* orig)
{
	CRect imageRect(0, 0, orig->getWidth(), orig->getHeight());
	bool result = this->CreateGrayScaleDIB(imageRect, 0, 0);

	int X_SIZE = orig->getWidth(), Y_SIZE = orig->getHeight();
	/* Vector used to store image used to skeletezation */
	std::vector<std::vector<double>> skl;
	/* Get a picture in form of 0-1 meaning white-black */
	skl.resize(X_SIZE);
	for (int x = 0; x < X_SIZE; x++) {
		skl[x].resize(Y_SIZE);

		for (int y = 0; y < Y_SIZE; y++) {
			skl[x][y] = orig->getPixel1(x, y);
		}
	}

	for (int x = 0; x < this->getWidth(); x ++)
	{
		for (int y = 0; y < this->getHeight(); y ++)
		{
			this->setPixel8(x, y, 255*skl[x][y]);
		}
	}

	for (int x = 0; x < this->getWidth(); x++)
	{
		for (int y = 0; y < this->getHeight(); y++)
		{
			if(skl[x][y] == 0) {
				for (int i = -1; i < 2; i++) {
					for (int j = -1; j < 2; j++) {
						if (x + i < this->getWidth() && x + i > 0 && y + j < this->getHeight() && y + j > 0)
							this->setPixel8(x + i, y + j, 0);
					}
				}
			}
		}
	}
	return true;
}

bool BMPViewer::erosion(BMPViewer* orig)
{
	CRect imageRect(0, 0, orig->getWidth(), orig->getHeight());
	bool result = this->CreateGrayScaleDIB(imageRect, 0, 0);

	int X_SIZE = orig->getWidth(), Y_SIZE = orig->getHeight();
	/* Vector used to store image used to skeletezation */
	std::vector<std::vector<double>> skl;
	/* Get a picture in form of 0-1 meaning white-black */
	skl.resize(X_SIZE);
	for (int x = 0; x < X_SIZE; x++) {
		skl[x].resize(Y_SIZE);

		for (int y = 0; y < Y_SIZE; y++) {
			skl[x][y] = orig->getPixel1(x, y);
		}
	}

	for (int x = 0; x < this->getWidth(); x++)
	{
		for (int y = 0; y < this->getHeight(); y++)
		{
			this->setPixel8(x, y, 255 * skl[x][y]);
		}
	}

	for (int x = 0; x < this->getWidth(); x ++)
	{
		for (int y = 0; y < this->getHeight(); y ++)
		{
			if (skl[x][y] == 1) {
				for (int i = -1; i < 2; i++) {
					for (int j = -1; j < 2; j++) {
						if (x + i < this->getWidth() && x + i > 0 && y + j < this->getHeight() && y + j > 0)
							this->setPixel8(x + i, y + j, 255);
					}
				}
			}
		}
	}
	return true;
}

bool BMPViewer::opening(BMPViewer* orig)
{
	BMPViewer tmp;
	tmp.erosion(orig);
	return this->dylatation8b(&tmp);
}

bool BMPViewer::closing(BMPViewer* orig)
{
	BMPViewer tmp;
	tmp.dylatation (orig);
	return this->erosion8b(&tmp);
}

bool BMPViewer::dylatation8b(BMPViewer* orig)
{
	CRect imageRect(0, 0, orig->getWidth(), orig->getHeight());
	bool result = this->CreateGrayScaleDIB(imageRect, 0, 0);

	int X_SIZE = orig->getWidth(), Y_SIZE = orig->getHeight();
	/* Vector used to store image used to skeletezation */
	std::vector<std::vector<double>> skl;
	/* Get a picture in form of 0-1 meaning white-black */
	skl.resize(X_SIZE);
	for (int x = 0; x < X_SIZE; x++) {
		skl[x].resize(Y_SIZE);

		for (int y = 0; y < Y_SIZE; y++) {
			skl[x][y] = get8BitColour(orig,x, y);
		}
	}

	for (int x = 0; x < this->getWidth(); x++)
	{
		for (int y = 0; y < this->getHeight(); y++)
		{
			this->setPixel8(x, y, skl[x][y]);
		}
	}

	for (int x = 0; x < this->getWidth(); x++)
	{
		for (int y = 0; y < this->getHeight(); y++)
		{
			if (skl[x][y] == 0) {
				for (int i = -1; i < 2; i++) {
					for (int j = -1; j < 2; j++) {
						if (x + i < this->getWidth() && x + i > 0 && y + j < this->getHeight() && y + j > 0)
							this->setPixel8(x + i, y + j, 0);
					}
				}
			}
		}
	}
	return true;
}

bool BMPViewer::morfExt(BMPViewer* orig)
{
	this->dylatation(orig);
	for (int x = 0; x < this->getWidth(); x++)
	{
		for (int y = 0; y < this->getHeight(); y++)
		{
			if (this->GetPixel8(x, y) == 255 * orig->getPixel1(x, y)) {

				this->setPixel8(x, y, 255);
			}
		}
	}
	return false;
}

bool BMPViewer::morfInt(BMPViewer* orig)
{
	this->erosion(orig);

	int X_SIZE = orig->getWidth(), Y_SIZE = orig->getHeight();
	/* Vector used to store image used to skeletezation */
	std::vector<std::vector<double>> skl;
	/* Get a picture in form of 0-1 meaning white-black */
	skl.resize(X_SIZE);
	for (int x = 0; x < X_SIZE; x++) {
		skl[x].resize(Y_SIZE);

		for (int y = 0; y < Y_SIZE; y++) {
			skl[x][y] = orig->getPixel1(x, y);
		}
	}


	for (int x = 0; x < this->getWidth(); x++)
	{
		for (int y = 0; y < this->getHeight(); y++)
		{
			if (this->GetPixel8(x, y) == 255 * skl[x][y]) {

				skl[x][y] = 1;
			}
		}
	}

	for (int x = 0; x < this->getWidth(); x++)
	{
		for (int y = 0; y < this->getHeight(); y++)
		{
			this->setPixel8(x, y, 255 * skl[x][y]);
		}
	}

	return false;
}

bool BMPViewer::erosionGrayscale(BMPViewer* orig)
{
	CRect imageRect(0, 0, orig->getWidth(), orig->getHeight());
	bool result = this->CreateGrayScaleDIB(imageRect, 0, 0);
	for (int x = 0; x < this->getWidth(); x++)
	{
		for (int y = 0; y < this->getHeight(); y++)
		{
			int max = 0;
			for (int i = -1; i < 2; i++) {
				for (int j = -1; j < 2; j++) {
					if (x + i < this->getWidth() && x + i > 0 && y + j < this->getHeight() && y + j > 0) {
						if (get8BitColour(orig,x + i, y + j) > max)
							max = get8BitColour(orig,x + i, y + j);
					}
				}
			}
			result = this->setPixel8(x, y, max);
			if (result == false) return result;
		}
	}
	return true;
}

bool BMPViewer::dylatationGrayscale(BMPViewer* orig)
{
	CRect imageRect(0, 0, orig->getWidth(), orig->getHeight());
	bool result = this->CreateGrayScaleDIB(imageRect, 0, 0);
	for (int x = 0; x < this->getWidth(); x++)
	{
		for (int y = 0; y < this->getHeight(); y++)
		{
			int min = 255;
			for (int i = -1; i < 2; i++) {
				for (int j = -1; j < 2; j++) {
					if (x + i < this->getWidth() && x + i > 0 && y + j < this->getHeight() && y + j > 0) {
						if (get8BitColour(orig,x + i, y + j) < min)
							min = get8BitColour(orig,x + i, y + j);
					}
				}
			}
			result = this->setPixel8(x, y, min);
			if (result == false) return result;
		}
	}
	return true;
}

bool BMPViewer::openingGrayscale(BMPViewer* orig)
{
	BMPViewer tmp;
	tmp.erosionGrayscale(orig);
	return this->dylatationGrayscale(&tmp);
}

bool BMPViewer::closingGrayscale(BMPViewer* orig)
{
	BMPViewer tmp;
	tmp.dylatationGrayscale(orig);
	return this->erosionGrayscale(&tmp);
}

bool BMPViewer::erosion8b(BMPViewer* orig)
{
	CRect imageRect(0, 0, orig->getWidth(), orig->getHeight());
	bool result = this->CreateGrayScaleDIB(imageRect, 0, 0);

	int X_SIZE = orig->getWidth(), Y_SIZE = orig->getHeight();
	/* Vector used to store image used to skeletezation */
	std::vector<std::vector<double>> skl;
	/* Get a picture in form of 0-1 meaning white-black */
	skl.resize(X_SIZE);
	for (int x = 0; x < X_SIZE; x++) {
		skl[x].resize(Y_SIZE);

		for (int y = 0; y < Y_SIZE; y++) {
			skl[x][y] = get8BitColour(orig,x, y);
		}
	}

	for (int x = 0; x < this->getWidth(); x++)
	{
		for (int y = 0; y < this->getHeight(); y++)
		{
			this->setPixel8(x, y, skl[x][y]);
		}
	}

	for (int x = 0; x < this->getWidth(); x++)
	{
		for (int y = 0; y < this->getHeight(); y++)
		{
			if (skl[x][y] == 255) {
				for (int i = -1; i < 2; i++) {
					for (int j = -1; j < 2; j++) {
						if (x + i < this->getWidth() && x + i > 0 && y + j < this->getHeight() && y + j > 0)
							this->setPixel8(x + i, y + j, 255);
					}
				}
			}
		}
	}
	return true;
}
