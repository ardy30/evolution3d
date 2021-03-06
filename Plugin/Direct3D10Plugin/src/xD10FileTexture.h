#ifndef _XEVOL_DIRECT3D10_FILE_2DTEXTURE_H_
#define _XEVOL_DIRECT3D10_FILE_2DTEXTURE_H_
#include "xD10BaseTexture.h"
BEGIN_NAMESPACE_XEVOL3D
class xD10FileTexture : public xD10UnkwonTexture
{
	IMPL_BASE_OBJECT_CLASSID_INTERFACE(xD10FileTexture);
	bool                  __loadImageFile(const wchar_t* fileName , const unsigned int8* buf , size_t bufLen, unsigned long arg);
public:
	xD10FileTexture(xD3D10RenderApi* pAPI );
	~xD10FileTexture();
	virtual bool          load(const wchar_t* fileName , unsigned long  arg);
	virtual bool          load(const wchar_t* fileName , const unsigned int8* buf , size_t bufLen, unsigned long arg);
public:
	virtual bool          create(const  xTextureInitDesc& initDesc , xTextureInitData* pInitData = NULL, int nInitData = 0) { return false ;}
protected:
	bool _prepareLoadInfo(D3DX10_IMAGE_INFO& imgInfo , D3DX10_IMAGE_LOAD_INFO& loadInfo);
};


END_NAMESPACE_XEVOL3D

#endif

