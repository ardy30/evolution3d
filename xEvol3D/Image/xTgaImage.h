/*
XEvol3D Rendering Engine . (http://gforge.osdn.net.cn/projects/xevol3d/) 
Stanly.Lee 2006


This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef __XHDR_IMAGE_H__
#define __XHDR_IMAGE_H__
#include "xBaseImage.h"
#include <fstream>
BEGIN_NAMESPACE_XEVOL3D
struct TgaImageData
{
    unsigned char*    data;
    //unsigned red_mask;			// bit layout of the red components
    //unsigned green_mask;		// bit layout of the green components
    //unsigned blue_mask;			// bit layout of the blue components
    int     biWidth;
    int     biHeight;
    int     biBitCount;
};

class _XEVOL_BASE_API_  xTgaImage : public xImage
{
	IMAGE_ORIGIN m_ImgOrigin;
    TgaImageData m_TgaBitmap;
public:
	xTgaImage();
	~xTgaImage();
	const wchar_t*  supportExt(bool bSave);
	int          byteAlign();
	bool         convert(ePIXEL_FORMAT newFmt){ return false ; }; 
	bool         scale(int w , int h , IMAGE_FILTER filter = eFILTER_BOX){ return false ; }; 
	bool         scalePOT(IMAGE_FILTER filter = eFILTER_BOX){ return false ; }; 
	bool         scaleFit(int w , int h , IMAGE_FILTER filter = eFILTER_BOX){ return false ; }; 

	bool         load(xImageSize size){ return false ; }; 
	bool         load(int w , int h , ePIXEL_FORMAT fmt){ return false ; }; 
	bool         load(const wchar_t* fileName, IMAGE_ORIGIN img_origin = IO_LEFT_LOWER);
	bool         load(const wchar_t* fileName, const int8* mem_buf , int buf_len, IMAGE_ORIGIN img_origin = IO_LEFT_LOWER);
	bool         load(const wchar_t* filename, const int8* mem_buf , int buf_len, ePIXEL_FORMAT format, IMAGE_ORIGIN img_origin = IO_LEFT_LOWER);
	bool         save(const wchar_t* fileName){ return false ; }; 
	bool         free();
	xImageSize   getSize();
	void*        data(size_t mipmapLevel = 0 , size_t cubeMapImageIndex = 0/*-1 means not a cubemap*/ , bool bCompressed = false);
	bool         data(const int8 * image_buffer,int mWidth,int mHeight , ePIXEL_FORMAT format);
	size_t       nMipmapLevel(){return 1 ; };
	size_t       nImage(){return 1 ; };
    size_t       nSlice(){return 1 ; };
	
protected:
    bool         LoadTga(IStreamIO *io);
    bool         Unload();
    xColor_4ub*  GetScanLine(int scanline, int flipvert) ;
    bool         Allocate(int width, int height, int bpp) ;

};

END_NAMESPACE_XEVOL3D

#endif
