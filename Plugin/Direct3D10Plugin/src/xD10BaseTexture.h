#ifndef _XEVOL_DIRECT3D10_BASE_TEXTURE_H_
#define _XEVOL_DIRECT3D10_BASE_TEXTURE_H_
#include <RenderApi/xRenderApi.h>
#include <d3d10.h>
#include <D3DX10.h>
#include <D3DX10tex.h>
#include <string>
#include <vector>
#include <RenderAPI/xBaseTexture.h>
#include "xD10ConstLexer.h"


BEGIN_NAMESPACE_XEVOL3D
class xD3D10RenderApi;
struct xD10TexInfo
{
	DXGI_FORMAT   m_ShaderViewFmt;
    DXGI_FORMAT   m_ResFmt;
    DXGI_FORMAT   m_RTViewFmt;
	int           m_TexWidth;
	int           m_TexHeight;
	int           m_TexDepth;
	int           m_MipmapLevel;
	int           m_ArraySize;
	eResourceType m_Type;
	ePIXEL_FORMAT m_xfmt;
	int           m_MemSize;
	int           m_Pitch;
	int           m_SlicePitch;
	int           m_nBytePerPixel;
	D3D10_USAGE   m_Usage;
};

class xD10TextureBuffer
{
public:
	char* m_pixel;
	int   m_pitch;
	int   m_depth;
	int   m_dataLen;
	void* m_destBuffer;
public:
	xD10TextureBuffer();
	~xD10TextureBuffer();
	void  setDestBuffer(void* destBuffer);
	bool  create(int pitch , int h);
	bool  unload();
	bool  flush(int h ,  int dest_pitch , int src_pitch = 0);
};

class xD10BaseTexture : public IBaseTexture
{
protected:
	xD3D10RenderApi*          m_pD10Api;
	ID3D10ShaderResourceView* m_pTextureView;
	xD10TexInfo               m_TexInfo;
	DXGI_SAMPLE_DESC          m_SampleDesc;
	IMPL_REFCOUNT_OBJECT_INTERFACE(xD10BaseTexture);
public:
	xD10BaseTexture(xD3D10RenderApi* pD10Api);
	virtual ~xD10BaseTexture();
	virtual unsigned long   memUsage();
	virtual eResourceType   res_type();
	virtual IRenderTarget*  toRenderTarget(size_t iSlice = 0 , size_t iMipMapLevel = 0){return NULL ; }
	virtual ePIXEL_FORMAT   format(){ return m_TexInfo.m_xfmt ; }
	virtual bool            validate(){return true; }
	virtual bool            desc(xTextureDesc& _desc);
	virtual bool            grabRenderTagetData(void* pData , int x , int y , int w , int h , int arraySlice , int mipmapLevel) { return false ; }
    virtual bool            unload();
	virtual bool            saveToFile(const wchar_t* fileName);
	virtual ID3D10Resource* GetD10Resource();
};

class xD10UnkwonTexture : public xD10BaseTexture
{
protected:
	ID3D10Resource*           m_pTexture;
public:
	void*                 handle();
	virtual bool          isLoaded();
	virtual bool          unload();
protected:
	virtual bool          load(const wchar_t* fileName , unsigned long  arg) = 0;
	virtual bool          load(const wchar_t* fileName , const unsigned int8* buf , size_t bufLen, unsigned long arg) = 0;

	virtual bool          update(void* data  , int dateLen ,int rowPitch , int depthPicth =0 , int mipmapLevel = 0 , int arraySlice = 0){ return NULL ;}
	virtual bool          lock(eLockPolicy lockPolicy, xTextureLockArea& lockInfo, int mipmapLevel = 0 , int arraySlice = 0){ return false ;}
	virtual bool          unlock(xTextureLockArea& lockInfo){return false;}
protected:
	virtual bool          _load(ID3D10Resource* pTexture , bool bCreateTextureView = true);
	xD10UnkwonTexture(xD3D10RenderApi* pD10Api);
	virtual ~xD10UnkwonTexture();
};
bool fillLoadInfo(D3DX10_IMAGE_LOAD_INFO& loadInfo , bool lockAble , bool bAsRenderTarget);
END_NAMESPACE_XEVOL3D

#endif
