#include "xGL2FileTexture.h"
#include "xOpenGL2API.h"
#include "xGL2ConstLexer.h"
#include <BaseLib/xI18N.h>
#include <BaseLib/xBaseMath.h>
#include <Image/xImage.h>
#include <fs/xFileSystem.h>
#include <D3Dx9.h>
BEGIN_NAMESPACE_XEVOL3D

IMPL_BASE_OBJECT_CLASSID(xGL2FileTexture  , IBaseTexture);

xGL2FileTexture::xGL2FileTexture(xGL2RenderApi* pAPI )
:xGL2UnkwonTexture(pAPI)
{

}

xGL2FileTexture::~xGL2FileTexture()
{
   unload();
}




bool fillLoadInfo(D3DX10_IMAGE_LOAD_INFO& loadInfo , bool lockAble)
{
	ZeroMemory( &loadInfo, sizeof(D3DX10_IMAGE_LOAD_INFO) );
	loadInfo.BindFlags      = GL2_BIND_SHADER_RESOURCE | GL2_BIND_RENDER_TARGET ;//GL2_BIND_SHADER_RESOURCE;
	loadInfo.CpuAccessFlags = lockAble? GL2_CPU_ACCESS_WRITE : 0 ;
	loadInfo.FirstMipLevel  = 0;
	loadInfo.Format         = DXGI_FORMAT(-1) ;//DXGI_FORMAT_R8G8B8A8_UNORM;
	loadInfo.Width          = D3DX_FROM_FILE;
	loadInfo.Height         = D3DX_FROM_FILE;
	loadInfo.Depth          = D3DX_FROM_FILE;
	loadInfo.MipLevels      = D3DX10_DEFAULT;
	loadInfo.Usage          = GL2_USAGE_DEFAULT;
	loadInfo.MipFilter      = D3DX10_FILTER_TRIANGLE;
	loadInfo.MiscFlags      = GL2_RESOURCE_MISC_GENERATE_MIPS;// | GL2_RESOURCE_MISC_TEXTURECUBE;
	return true;
}

bool xGL2FileTexture::__loadImageFile(const wchar_t* fileName , const unsigned int8* buf , size_t bufLen, unsigned long arg)
{
	xImage* pImg = xImageLib::load(IMAGE_UNKNOWN,fileName , (const int8*)buf , (int)bufLen , PIXELFORMAT_R8G8B8A8 );
	if( pImg == NULL )
		return false;

	xImageSize _size = pImg->getSize();
	GL2_TEXTURE2D_DESC desc;
	ZeroMemory( &desc, sizeof(desc) );
	desc.Width            = (UINT)_size.w;
	desc.Height           = (UINT)_size.h;
	desc.MipLevels        = (UINT)pImg->nMipmapLevel();
	desc.ArraySize        = 1;
	desc.Format           = DXGI_FORMAT_R8G8B8A8_UNORM ; //DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.BindFlags        = GL2_BIND_SHADER_RESOURCE  ;//| GL2_BIND_RENDER_TARGET; 
	desc.Usage            = GL2_USAGE_DEFAULT;
	desc.CPUAccessFlags   = 0;
    
	int pitch = _size.pitch;

	if(pImg->nImage() == 1)
	{
		ID3D10Texture2D * pTexture = NULL;
		GL2_SUBRESOURCE_DATA* pInitData = new GL2_SUBRESOURCE_DATA [ pImg->nMipmapLevel() ] ;
		for(int i = 0 ; i < (int)pImg->nMipmapLevel() ; i ++)
		{
			pInitData[i].pSysMem = pImg->data(i);
			pInitData[i].SysMemPitch = pitch;
			pInitData[i].SysMemSlicePitch = 0;
			pitch/=2;
		}   

		m_pD10Api->d10Device()->CreateTexture2D( &desc, pInitData, &pTexture );
		xImageLib::kill(pImg);
		delete [] pInitData;
		if(pTexture == NULL)
			return false;
		return _load(pTexture);
	}
	else if(pImg->nImage() == 6)
	{
        XEVOL_WARNNING_NOT_IMPLEMENT_INFO("CubeMap目前还不支持dds以外格式\n");
	}
	return true;
}

bool xGL2FileTexture::load(const wchar_t* fileName , unsigned long  arg)
{
	ID3D10Resource* pTexture = NULL;

	D3DX10_IMAGE_LOAD_INFO loadInfo;
	fillLoadInfo(loadInfo, false);
	D3DX10CreateTextureFromFileW( m_pD10Api->d10Device() , fileName ,  &loadInfo, NULL, &pTexture, NULL );
	if(pTexture)
    {
        m_TexInfo.m_ShaderViewFmt = DXGI_FORMAT_UNKNOWN;
        m_TexInfo.m_RTViewFmt  = DXGI_FORMAT_UNKNOWN;
        m_TexInfo.m_ResFmt     = DXGI_FORMAT_UNKNOWN;
		return _load(pTexture);
    }
	return __loadImageFile(fileName , NULL , 0 ,arg);
}

bool xGL2FileTexture::load(const wchar_t* fileName , const unsigned int8* buf , size_t bufLen, unsigned long arg)
{
	if(bufLen == 0 || buf == NULL)
		return load(fileName , arg);

	D3DX10_IMAGE_LOAD_INFO loadInfo;
	fillLoadInfo(loadInfo , false);

	ID3D10Resource* pTexture = NULL;
	D3DX10CreateTextureFromMemory(m_pD10Api->d10Device() , buf , bufLen , &loadInfo, NULL, &pTexture, NULL );
	if(pTexture)
    {
        m_TexInfo.m_ShaderViewFmt = DXGI_FORMAT_UNKNOWN;
        m_TexInfo.m_RTViewFmt  = DXGI_FORMAT_UNKNOWN;
        m_TexInfo.m_ResFmt     = DXGI_FORMAT_UNKNOWN;
        return _load(pTexture);
    }
	return __loadImageFile(fileName , buf , bufLen ,arg);
}




END_NAMESPACE_XEVOL3D