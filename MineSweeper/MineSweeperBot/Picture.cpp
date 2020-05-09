#include "Picture.h"
#include <iostream>




Picture::Picture(HBITMAP hBitmap):
m_hBitmap(hBitmap)
{

}

Picture::~Picture()
{
	DeleteObject(m_hBitmap);
}


cv::Mat Picture::GetCVImg()
{
	PBITMAPINFOHEADER pbih;     // bitmap info-header  
	LPBYTE lpBits;              // memory pointer  


	auto Info = CreateBitmapInfoStruct();

	cv::Mat src;

	pbih = (PBITMAPINFOHEADER)Info;
	lpBits = (LPBYTE)GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);

	if (!lpBits)
	{
		std::cerr << "Picture -> GetCVImg -> GlobalAlloc ";
		return src;
	}

	BITMAP bmp;

	// Retrieve the bitmap color format, width, and height.  
	if (!GetObject(m_hBitmap, sizeof(BITMAP), (LPSTR)& bmp))
	{
		std::cerr << "Picture -> GetCVImg -> GetObject ";
		return src;
	}


	src.create(bmp.bmHeight, bmp.bmWidth, CV_8UC4);


	auto hDC = GetDC(0);

	if (!GetDIBits(hDC, m_hBitmap, 0, (WORD)pbih->biHeight, src.data, Info, DIB_RGB_COLORS))
	{
		std::cerr << "Picture -> GetCVImg -> GetDIBits ";
		return src;
	}

	GlobalFree((HGLOBAL)lpBits);

	DeleteDC(hDC);

	return src;
}







PBITMAPINFO Picture::CreateBitmapInfoStruct()
{
	BITMAP bmp;
	PBITMAPINFO pbmi;
	WORD    cClrBits;

	// Retrieve the bitmap color format, width, and height.  
	if (!GetObject(m_hBitmap, sizeof(BITMAP), (LPSTR)& bmp))
	{
		std::cerr << "Picture -> CreateBitmapInfoStruct -> GetObject ";
		return nullptr;
	}

	// Convert the color format to a count of bits.  
	cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel);
	if (cClrBits == 1)
	{
		cClrBits = 1;
	}
	else if (cClrBits <= 4)
	{
		cClrBits = 4;
	}
	else if (cClrBits <= 8)
	{
		cClrBits = 8;
	}
	else if (cClrBits <= 16)
	{
		cClrBits = 16;
	}
	else if (cClrBits <= 24)
	{
		cClrBits = 24;
	}
	else
	{
		cClrBits = 32;
	}

	// Allocate memory for the BITMAPINFO structure. (This structure  
	// contains a BITMAPINFOHEADER structure and an array of RGBQUAD  
	// data structures.)  

	if (cClrBits < 24)
	{
		pbmi = (PBITMAPINFO)LocalAlloc(LPTR,sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * (1 << cClrBits));
	}
		// There is no RGBQUAD array for these formats: 24-bit-per-pixel or 32-bit-per-pixel 
	else
	{
		pbmi = (PBITMAPINFO)LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER));
	}

	// Initialize the fields in the BITMAPINFO structure.  

	pbmi->bmiHeader.biSize		= sizeof(BITMAPINFOHEADER);
	pbmi->bmiHeader.biWidth		= bmp.bmWidth;
	pbmi->bmiHeader.biHeight	= bmp.bmHeight;
	pbmi->bmiHeader.biPlanes	= bmp.bmPlanes;
	pbmi->bmiHeader.biBitCount	= bmp.bmBitsPixel;

	if (cClrBits < 24)
	{
		pbmi->bmiHeader.biClrUsed = (1 << cClrBits);
	}

	// If the bitmap is not compressed, set the BI_RGB flag.  
	pbmi->bmiHeader.biCompression = BI_RGB;

	// Compute the number of bytes in the array of color  
	// indices and store the result in biSizeImage.  
	// The width must be DWORD aligned unless the bitmap is RLE 
	// compressed. 
	pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits + 31) & ~31) / 8
		* pbmi->bmiHeader.biHeight;
	// Set biClrImportant to 0, indicating that all of the  
	// device colors are important.  
	pbmi->bmiHeader.biClrImportant = 0;

	return pbmi;
}



void Picture::SaveFile(const std::string& FileName)
{
	HANDLE hf;                 // file handle  
	BITMAPFILEHEADER hdr;       // bitmap file-header  
	PBITMAPINFOHEADER pbih;     // bitmap info-header  
	LPBYTE lpBits;              // memory pointer  
	DWORD dwTotal;              // total count of bytes  
	DWORD cb;                   // incremental count of bytes  
	BYTE* hp;                   // byte pointer  
	DWORD dwTmp;

	auto Info = CreateBitmapInfoStruct();

	pbih = (PBITMAPINFOHEADER)Info;
	lpBits = (LPBYTE)GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);

	if (!lpBits)
	{
		std::cerr << "Picture -> SaveFile -> GlobalAlloc ";
		return;
	}

	// Retrieve the color table (RGBQUAD array) and the bits  
	// (array of palette indices) from the DIB.  

	if (!GetDIBits(GetDC(0), m_hBitmap, 0, (WORD)pbih->biHeight, lpBits, Info,
		DIB_RGB_COLORS))
	{
		std::cerr << "Picture -> SaveFile -> GetDIBits ";
		return;
	}

	// Create the .BMP file.  


	
	LPTSTR pszFile = (LPTSTR)FileName.c_str();

	hf = CreateFile(pszFile, GENERIC_READ | GENERIC_WRITE, (DWORD)0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);
	if (hf == INVALID_HANDLE_VALUE)
	{
		std::cerr << "Picture -> SaveFile -> CreateFile ";
		return;
	}

	hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M"  
	// Compute the size of the entire file.  
	hdr.bfSize = (DWORD)(sizeof(BITMAPFILEHEADER) + pbih->biSize + pbih->biClrUsed * sizeof(RGBQUAD) + pbih->biSizeImage);
	hdr.bfReserved1 = 0;
	hdr.bfReserved2 = 0;

	// Compute the offset to the array of color indices.  
	hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) +
		pbih->biSize + pbih->biClrUsed
		* sizeof(RGBQUAD);

	// Copy the BITMAPFILEHEADER into the .BMP file.  
	if (!WriteFile(hf, (LPVOID)& hdr, sizeof(BITMAPFILEHEADER),
		(LPDWORD)& dwTmp, NULL))
	{
		std::cerr << "Picture -> SaveFile -> WriteFile ";
		return;
	}

	// Copy the BITMAPINFOHEADER and RGBQUAD array into the file.  
	if (!WriteFile(hf, (LPVOID)pbih, sizeof(BITMAPINFOHEADER)
		+ pbih->biClrUsed * sizeof(RGBQUAD),
		(LPDWORD)& dwTmp, (NULL)))
	{
		std::cerr << "Picture -> SaveFile -> WriteFile ";
		return;
	}

	// Copy the array of color indices into the .BMP file.  
	dwTotal = cb = pbih->biSizeImage;
	hp = lpBits;
	if (!WriteFile(hf, (LPSTR)hp, (int)cb, (LPDWORD)& dwTmp, NULL))
	{
		std::cerr << "Picture -> SaveFile -> WriteFile ";
		return;
	}

	// Close the .BMP file.  
	if (!CloseHandle(hf))
	{
		std::cerr << "Picture -> SaveFile -> CloseHandle ";
		return;
	}

	// Free memory.  
	GlobalFree((HGLOBAL)lpBits);
}