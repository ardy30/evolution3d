#include "stdafx.h"
#include "xD10LockableTexture.h"
#include "xD10ConstLexer.h"
#include "xDirect3D10API.h"
#include <BaseLib/xI18N.h>
#include <BaseLib/xBaseMath.h>
#include <OperationSys/xOperationSys.h>
#include "Image/PixelFormatAssit.h"
BEGIN_NAMESPACE_XEVOL3D

IMPL_BASE_OBJECT_CLASSID(xD10RenderTarget   , xD10BaseRenderTarget);
IMPL_BASE_OBJECT_CLASSID(xD10RenderTexture  , xD10UnkwonTexture);

IMPL_BASE_OBJECT_CLASSID(xD10DepthBuffer   , xD10BaseRenderTarget);
IMPL_BASE_OBJECT_CLASSID(xD10DepthTexture  , xD10UnkwonTexture);
xD10RenderTarget::xD10RenderTarget(xD3D10RenderApi* pAPI , int arraySlice , int mipmapLevel):xD10BaseRenderTarget(pAPI , arraySlice , mipmapLevel)
{
	m_RefCount = 1;
	m_pRenderTargetView = NULL;
}
int xD10RenderTarget::RefCount()
{
	if(m_pBaseTexture)
		return m_pBaseTexture->RefCount();
	return xD10BaseRenderTarget::RefCount();
}

int xD10RenderTarget::AddRef()
{
	if(m_pBaseTexture)
		return m_pBaseTexture->AddRef();
	return xD10BaseRenderTarget::AddRef();
}

int xD10RenderTarget::ReleaseObject()
{
	if(m_pBaseTexture)
		return m_pBaseTexture->ReleaseObject();
	return xD10BaseRenderTarget::ReleaseObject();
}

int xD10RenderTarget::KillObject()
{
	if(m_pBaseTexture)
		return m_pBaseTexture->KillObject();
	return xD10BaseRenderTarget::KillObject();
}

xD10RenderTarget::~xD10RenderTarget()
{
	destory();   
}

bool xD10RenderTarget::destory()
{
	XSAFE_RELEASE(m_pRenderTargetView);  
	return true;
}

bool xD10RenderTarget::saveToFile(const wchar_t* fileName)
{
	ID3D10Resource* pSource = NULL;

	m_pRenderTargetView->GetResource(&pSource);
	if(pSource == NULL)
		return false;
	if(saveResourceToFile(fileName , pSource) == true)
	{
		pSource->Release();
		return true;
	}
	pSource->Release();
	return false;
}

bool xD10RenderTarget::create(ID3D10Resource* pTexture , xD10TexInfo& TexInfo, DXGI_SAMPLE_DESC& sampleDesc ,int iSlice , int mipLevel )
{
	if(m_pRenderTargetView && m_arraySlice == iSlice && m_mipmapLevel == mipLevel )
		return true;

	destory();
	m_arraySlice    = iSlice;
	m_mipmapLevel   = mipLevel;

	XSAFE_RELEASE(m_pRenderTargetView);   
	m_pRenderTargetView = NULL;

	D3D10_RENDER_TARGET_VIEW_DESC rtDesc;
	rtDesc.Format = TexInfo.m_RTViewFmt;

    if(TexInfo.m_TexDepth == 1)
    {
        if(TexInfo.m_ArraySize > 1)
        {
            rtDesc.Texture2DArray.MipSlice = m_mipmapLevel;
            rtDesc.Texture2DArray.FirstArraySlice = 0;
            rtDesc.Texture2DArray.ArraySize       = (UINT)TexInfo.m_ArraySize;
            rtDesc.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2DARRAY;
        }
        else if(TexInfo.m_ArraySize == 1)
        {
            if(sampleDesc.Count > 1)
                rtDesc.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2DMS;
            else
                rtDesc.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2D;
            rtDesc.Texture2D.MipSlice = m_mipmapLevel;
        }
    }
    else
    {
        rtDesc.Texture3D.MipSlice = m_mipmapLevel;
        rtDesc.Texture3D.FirstWSlice = iSlice ;
        rtDesc.Texture3D.WSize = (UINT)(TexInfo.m_TexDepth - iSlice);
        rtDesc.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE3D;
    }


	m_pD10Api->d10Device()->CreateRenderTargetView( pTexture, &rtDesc, &m_pRenderTargetView );
	return m_pRenderTargetView != NULL;
}

int xD10DynamicRenderTarget::ReleaseObject()
{
    int RefCount = xD10BaseRenderTarget::ReleaseObject();
    if( RefCount == 0  )
    {
        XSAFE_RELEASEOBJECT(m_pBaseTexture);
    }
    return RefCount;
}

int xD10DynamicRenderTarget::KillObject()
{
    XSAFE_RELEASEOBJECT(m_pBaseTexture);
    return xD10BaseRenderTarget::KillObject();
}


//用于Depth Buffer的 DepthTexture;
xD10DepthBuffer::xD10DepthBuffer(xD3D10RenderApi* pAPI , int arraySlice , int mipmapLevel) : xD10BaseRenderTarget(pAPI , arraySlice , mipmapLevel) 
{ 
	m_pDepthView = NULL ; 
	m_RefCount = 1;
}

xD10DepthBuffer::~xD10DepthBuffer() 
{
	destory() ;
}
bool xD10DepthBuffer::destory()
{
	XSAFE_RELEASE(m_pDepthView);  
	return true;
}

int xD10DepthBuffer::RefCount()
{
	if(m_pBaseTexture)
		return m_pBaseTexture->RefCount();
	return xD10BaseRenderTarget::RefCount();
}

int xD10DepthBuffer::AddRef()
{
	if(m_pBaseTexture)
		return m_pBaseTexture->AddRef();
	return xD10BaseRenderTarget::AddRef();
}

int xD10DepthBuffer::ReleaseObject()
{
	if(m_pBaseTexture)
		return m_pBaseTexture->ReleaseObject();
	return xD10BaseRenderTarget::ReleaseObject();
}

int xD10DepthBuffer::KillObject()
{
	if(m_pBaseTexture)
		return m_pBaseTexture->KillObject();
	return xD10BaseRenderTarget::KillObject();
}
bool xD10DepthBuffer::saveToFile(const wchar_t* fileName)
{
	ID3D10Resource* pSource = NULL;

	m_pDepthView->GetResource(&pSource);
	if(pSource == NULL)
		return false;
	if(saveResourceToFile(fileName , pSource) == true)
	{
		pSource->Release();
		return true;
	}
	pSource->Release();
	return false;
}
bool xD10DepthBuffer::create(ID3D10Resource* pTexture , xD10TexInfo& TexInfo , DXGI_SAMPLE_DESC& sampleDesc)
{
    destory();
	ID3D10Device* pDevice = m_pD10Api->d10Device();
	//创建深度缓冲
	D3D10_DEPTH_STENCIL_VIEW_DESC descDSV;
	memset(&descDSV , 0 , sizeof(D3D10_DEPTH_STENCIL_VIEW_DESC) );
	descDSV.Format = TexInfo.m_RTViewFmt;
	if(sampleDesc.Count > 1)
		descDSV.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2DMS;
	else
	    descDSV.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	HRESULT hr = pDevice->CreateDepthStencilView( pTexture, &descDSV, &m_pDepthView );
	return !FAILED(hr);
}


//=======可渲染的Texture=============================================
xD10RenderTexture::xD10RenderTexture(bool lockAble ,  bool canUseTexture , xD3D10RenderApi* pRenderApi, DXGI_SAMPLE_DESC SampleDesc)
:xD10UnkwonTexture(pRenderApi) , m_RenderTarget(pRenderApi , 0 , 0 )
{
	 m_bLockable = lockAble;
	 m_bCanUseTexture = canUseTexture;
	 m_pSysTexture   = NULL;
	 m_RenderTarget.setTexture(this);
	 m_SampleDesc = SampleDesc;
}
bool xD10RenderTexture::unload()
{
    m_RenderTarget.destory();
    XSAFE_RELEASE(m_pSysTexture);
    return xD10UnkwonTexture::unload();
}
xD10RenderTexture::~xD10RenderTexture()
{
	m_RenderTarget.destory();
    unload();
}

bool  xD10RenderTexture::update(void* data  , int dateLen ,int rowPitch , int depthPicth , int mipmapLevel , int arraySlice)
{
    UINT lockResource = D3D10CalcSubresource((UINT)mipmapLevel , (UINT)arraySlice, (UINT)m_TexInfo.m_MipmapLevel);
    m_pD10Api->d10Device()->UpdateSubresource(m_pTexture , lockResource , NULL , data ,rowPitch , depthPicth);   
    return true;
}

bool  xD10RenderTexture::grabRenderTagetData(void* pData, int x , int y , int w , int h , int arraySlice , int mipmapLevel)
{
    if(m_TexInfo.m_TexDepth == 1)
        return grabRenderTagetDataTexture2D(pData , x , y , w , h , arraySlice , mipmapLevel);
    return false;   
}

bool  xD10RenderTexture::grabRenderTagetDataTexture2D(void* pData, int x , int y , int w , int h , int arraySlice , int mipmapLevel)
{
	if(m_bLockable == false || m_pSysTexture == NULL)
		return false;

	ID3D10Device* pDevice = m_pD10Api->d10Device();
	pDevice->CopyResource(m_pSysTexture , m_pTexture);

    ID3D10Texture2D* pSys2DTexture = (ID3D10Texture2D*)m_pSysTexture;

	xTextureLockArea lockInfo;
    D3D10_MAPPED_TEXTURE2D  mappedTex;
    mappedTex.pData = NULL;
    mappedTex.RowPitch = 0;
    D3D10_MAP  mapType = D3D10_MAP_READ;
    pDevice->CopyResource(pSys2DTexture , m_pTexture);
    UINT lockResource = D3D10CalcSubresource((UINT)0 , (UINT)0, m_TexInfo.m_MipmapLevel);
    lockInfo.m_lockResource = (long)lockResource;

    pSys2DTexture->Map(lockResource , mapType , 0 , &mappedTex);
    if(mappedTex.pData == NULL)
        return false;
    lockInfo.m_width       = (int)m_TexInfo.m_TexWidth;
    lockInfo.m_height      = (int)m_TexInfo.m_TexHeight;
    lockInfo.m_depth       = 1;
    lockInfo.m_picth       = (int)mappedTex.RowPitch ;
    lockInfo.m_slice_pitch = (int)mappedTex.RowPitch * m_TexInfo.m_TexWidth;
    lockInfo.m_pixels      = (char*)mappedTex.pData;
    //---------Lock ok--------------------


	const char* pSrcLine = (const char*)lockInfo.m_pixels + x * m_TexInfo.m_nBytePerPixel;
	pSrcLine += lockInfo.m_picth * y;
	char*       pDstLine = (char*)pData;
	int dstPitch = m_TexInfo.m_nBytePerPixel * w;
	int line_len =  dstPitch;
	int max_len  =  lockInfo.m_picth - x * m_TexInfo.m_nBytePerPixel;
	if(dstPitch > max_len ) line_len = max_len;

	if(h > 0)
	{
		for(int _y = 0 ; _y < (int)h ; _y ++ )
		{
			memcpy(pDstLine , pSrcLine , line_len );
			pDstLine += dstPitch;
			pSrcLine += lockInfo.m_picth;
		}

	}
	else
	{
		for(int _y = 0 ; _y < -(int)h ; _y ++ )
		{
			memcpy(pDstLine , pSrcLine , line_len );
			pDstLine += dstPitch;
			pSrcLine -= lockInfo.m_picth;
		}

	}
	
    pSys2DTexture->Unmap(lockInfo.m_lockResource );
	return true;
}

bool xD10RenderTexture::lock(eLockPolicy lockPolicy, xTextureLockArea& lockInfo, int mipmapLevel, int arraySlice)
{
	if(m_bLockable == false || m_pSysTexture == NULL)
		return false;

	if(lockPolicy != eLock_Read )
		return false;

    return false;
}

bool xD10RenderTexture::unlock(xTextureLockArea& lockInfo)
{
	if(m_bLockable == false || m_pSysTexture == NULL)
		return false;

	return false;
}

bool xD10RenderTexture::__createSysTexture()
{
    if(m_TexInfo.m_TexDepth == 1)
    {
        D3D10_TEXTURE2D_DESC desc;
        ZeroMemory( &desc, sizeof(desc) );
        desc.Width            = (UINT)m_TexInfo.m_TexWidth;
        desc.Height           = (UINT)m_TexInfo.m_TexHeight;
        desc.MipLevels        = (UINT)m_TexInfo.m_MipmapLevel;
        desc.ArraySize        = (UINT)m_TexInfo.m_ArraySize;
        desc.Format           = m_TexInfo.m_ResFmt ; //DXGI_FORMAT_R32G32B32A32_FLOAT;
        desc.SampleDesc       = m_SampleDesc;
        desc.BindFlags        =  0; 
        desc.Usage            = D3D10_USAGE_STAGING;
        desc.CPUAccessFlags   = D3D10_CPU_ACCESS_READ | D3D10_CPU_ACCESS_WRITE;
        ID3D10Texture2D* pTexture = NULL;
        m_pD10Api->d10Device()->CreateTexture2D( &desc, NULL, &pTexture );
        m_pSysTexture= pTexture;
    }
    else
    {
        D3D10_TEXTURE3D_DESC desc;
        ZeroMemory( &desc, sizeof(desc) );
        desc.Width            = (UINT)m_TexInfo.m_TexWidth;
        desc.Height           = (UINT)m_TexInfo.m_TexHeight;
        desc.MipLevels        = (UINT)m_TexInfo.m_MipmapLevel;
        desc.Depth            = (UINT)m_TexInfo.m_TexDepth;
        desc.Format           = m_TexInfo.m_ResFmt ; //DXGI_FORMAT_R32G32B32A32_FLOAT;
        desc.BindFlags        =  0; 
        desc.Usage            = D3D10_USAGE_STAGING;
        desc.CPUAccessFlags   = D3D10_CPU_ACCESS_READ | D3D10_CPU_ACCESS_WRITE;
        ID3D10Texture3D* pTexture = NULL;
        m_pD10Api->d10Device()->CreateTexture3D( &desc, NULL, &pTexture );
        m_pSysTexture= pTexture;
    }
    return m_pSysTexture != NULL;
}

bool xD10RenderTexture::create(const  xTextureInitDesc& initDesc , xTextureInitData* pInitData, int nInitData)
{
    unload();
    int w             = initDesc.m_TextureDesc.m_width;
    int h             = initDesc.m_TextureDesc.m_height;
    int mipMapLevels  = initDesc.m_nMipmap;
    int arraySize     = initDesc.m_TextureDesc.m_nArraySize;
    ePIXEL_FORMAT fmt = initDesc.m_TextureDesc.m_fmt;
    int depth         = initDesc.m_TextureDesc.m_depth;


    xD10GIFormatInfo* pFmtInfo = xD10ConstLexer::singleton()->GetPixelFormat(fmt);
    m_TexInfo.m_RTViewFmt = pFmtInfo->m_dxfmt;
    m_TexInfo.m_ResFmt    = pFmtInfo->m_dxfmt;
    m_TexInfo.m_ShaderViewFmt= pFmtInfo->m_dxfmt;

	if(depth == 1)
    {
        D3D10_TEXTURE2D_DESC desc;
        ZeroMemory( &desc, sizeof(desc) );
        desc.Width            = (UINT)w;
        desc.Height           = (UINT)h;
        desc.MipLevels        = (UINT)mipMapLevels;
        desc.ArraySize        = (UINT)arraySize;
        desc.Format           = m_TexInfo.m_ResFmt ; //DXGI_FORMAT_R32G32B32A32_FLOAT;
        desc.SampleDesc       = m_SampleDesc;
        desc.BindFlags        =  D3D10_BIND_RENDER_TARGET; 
        if(m_bCanUseTexture)
            desc.BindFlags |= D3D10_BIND_SHADER_RESOURCE;
        desc.Usage            = D3D10_USAGE_DEFAULT;
        desc.CPUAccessFlags   = 0;
        m_TexInfo.m_Usage     = desc.Usage;

        ID3D10Texture2D* pTexture = NULL;
        m_pD10Api->d10Device()->CreateTexture2D( &desc, NULL, &pTexture );
        if(pTexture == NULL)
            return false;
        m_pTextureView = NULL;
        _load(pTexture , m_bCanUseTexture );
        m_TexInfo.m_xfmt = fmt;

        if(m_bLockable)
        {
            return __createSysTexture();
        }
    }
    else
    {
        D3D10_TEXTURE3D_DESC desc;
        ZeroMemory( &desc, sizeof(desc) );
        desc.Width            = (UINT)w;
        desc.Height           = (UINT)h;
        desc.MipLevels        = (UINT)mipMapLevels;
        desc.Depth            = (UINT)depth;
        desc.Format           = m_TexInfo.m_ResFmt ; //DXGI_FORMAT_R32G32B32A32_FLOAT;
        desc.BindFlags        = D3D10_BIND_RENDER_TARGET; 
        if(m_bCanUseTexture)
            desc.BindFlags |= D3D10_BIND_SHADER_RESOURCE;
        desc.Usage            = D3D10_USAGE_DEFAULT;
        desc.CPUAccessFlags   = 0;
        m_TexInfo.m_Usage     = desc.Usage;

        ID3D10Texture3D* pTexture = NULL;
        m_pD10Api->d10Device()->CreateTexture3D( &desc, NULL, &pTexture );
        if(pTexture == NULL)
            return false;
        m_pTextureView = NULL;
        _load(pTexture , m_bCanUseTexture);
        m_TexInfo.m_xfmt = fmt;
        if(m_bLockable)
        {
            return __createSysTexture();
        }
    }

	return true;
}
bool xD10RenderTexture::isSameInstance(IRenderTarget* pRenderTarget)
{
    if(&m_RenderTarget == pRenderTarget)
        return true;
    XEVOL_WARNNING_NOT_IMPLEMENT;
    return false;
}

IRenderTarget* xD10RenderTexture::toRenderTarget(size_t iSlice , size_t iMipMapLevel )
{
    if(iSlice == 0 && iMipMapLevel == 0)
	{
		m_RenderTarget.create(m_pTexture,m_TexInfo ,m_SampleDesc);
        AddRef();
		return &m_RenderTarget;
	}
	else
	{
		xD10DynamicRenderTarget* pRenderTarget = new xD10DynamicRenderTarget(m_pD10Api , (int)iSlice , (int)iMipMapLevel);
		pRenderTarget->setTexture(this);
		pRenderTarget->create(m_pTexture,m_TexInfo  , m_SampleDesc , (int)iSlice , (int)iMipMapLevel);
        AddRef();
		return pRenderTarget;
	}
    return NULL;
}

//===========可渲染的深度纹理
xD10DepthTexture::xD10DepthTexture(bool lockAble ,  bool canUseTexture , xD3D10RenderApi* pRenderApi, DXGI_SAMPLE_DESC SampleDes) 
:xD10UnkwonTexture(pRenderApi) , m_RenderTarget(pRenderApi , 0 , 0)
{
	m_bLockable     = lockAble;
	m_bCanUseTexute = canUseTexture;
	m_RenderTarget.setTexture(this);
	m_SampleDesc = SampleDes;
}

bool xD10DepthTexture::unload()
{
    m_RenderTarget.destory();
    return xD10UnkwonTexture::unload();
}

xD10DepthTexture::~xD10DepthTexture()
{
    unload();
	m_RenderTarget.destory();
    XSAFE_RELEASE(m_pTexture);
}


bool xD10DepthTexture::create(const  xTextureInitDesc& initDesc , xTextureInitData* pInitData , int nInitData)
{
	unload();
    int w             = initDesc.m_TextureDesc.m_width;
    int h             = initDesc.m_TextureDesc.m_height;
    int mipMapLevels  = initDesc.m_nMipmap;
    int arraySize     = initDesc.m_TextureDesc.m_nArraySize;
    ePIXEL_FORMAT fmt = initDesc.m_TextureDesc.m_fmt;

	xD10GIFormatInfo* pFmtInfo = xD10ConstLexer::singleton()->GetPixelFormat(fmt);
	D3D10_TEXTURE2D_DESC desc;
	ZeroMemory(&desc , sizeof(desc) );
    m_TexInfo.m_RTViewFmt = pFmtInfo->m_dxfmt;
    m_TexInfo.m_ResFmt    = pFmtInfo->m_dxfmt;
    m_TexInfo.m_ShaderViewFmt= pFmtInfo->m_dxfmt;
    if(m_bCanUseTexute == true)
    {
        if(m_TexInfo.m_ResFmt == DXGI_FORMAT_D32_FLOAT ) 
        { 
            m_TexInfo.m_ResFmt     = DXGI_FORMAT_R32_TYPELESS; 
            m_TexInfo.m_ShaderViewFmt = DXGI_FORMAT_R32_FLOAT; 
        } 
        else if(m_TexInfo.m_ResFmt == DXGI_FORMAT_D16_UNORM ) 
        { 
            m_TexInfo.m_ResFmt     = DXGI_FORMAT_R16_TYPELESS; 
            m_TexInfo.m_ShaderViewFmt = DXGI_FORMAT_R16_UNORM; 
        } 
        else
        {
            const wchar_t* _name  = xPixelFormatAssit::singleton()->GetPixelFormatName(fmt);
            XEVOL_LOG(eXL_ERROR_FALT , L"Depthbuffer format not support bind as texture [fmt=%s]\n" , _name );
        }

    }
	ZeroMemory( &desc, sizeof(desc) );
	desc.Width            = (UINT)w;
	desc.Height           = (UINT)h;
	desc.MipLevels        = (UINT)mipMapLevels;
	desc.ArraySize        = (UINT)arraySize;
	desc.Format           = m_TexInfo.m_ResFmt; //DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc.SampleDesc       = m_SampleDesc;
	desc.BindFlags        = D3D10_BIND_DEPTH_STENCIL;

	if(m_bCanUseTexute)
		desc.BindFlags |= D3D10_BIND_SHADER_RESOURCE;

	desc.Usage            = D3D10_USAGE_DEFAULT;
	desc.CPUAccessFlags   = 0;
	m_TexInfo.m_Usage     = desc.Usage;

	ID3D10Texture2D* pTexture = NULL;
	m_pD10Api->d10Device()->CreateTexture2D( &desc, NULL, &pTexture );
	if(pTexture == NULL)
		return false;

	m_pTexture = pTexture;

    XSAFE_RELEASE(m_pTextureView);
	D3D10_RESOURCE_DIMENSION type;
	m_pTexture->GetType( &type );
	if( type ==  D3D10_RESOURCE_DIMENSION_TEXTURE2D)
	{
		ZeroMemory(&desc , sizeof(desc) );
		ID3D10Texture2D *pTexture2D = (ID3D10Texture2D*)m_pTexture;
		pTexture2D->GetDesc( &desc );

		m_pTextureView = NULL;


        pTexture2D->GetDesc( &desc );
        if(m_bCanUseTexute)
        {
            D3D10_SHADER_RESOURCE_VIEW_DESC srvDesc;
            srvDesc.Format = m_TexInfo.m_ShaderViewFmt;
            srvDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MostDetailedMip = 0;
            srvDesc.Texture2D.MipLevels       = desc.MipLevels;
            ID3D10ShaderResourceView *pSRView = NULL;
            m_pD10Api->d10Device()->CreateShaderResourceView( m_pTexture, &srvDesc, &pSRView );
            if(pSRView != NULL)
            {
                m_pTextureView = pSRView;
            }
        }      

        //填充
		m_TexInfo.m_ArraySize   = desc.ArraySize;
		m_TexInfo.m_TexWidth    = desc.Width;
		m_TexInfo.m_TexHeight   = desc.Height;
		m_TexInfo.m_TexDepth    = 1;
		m_TexInfo.m_MipmapLevel = desc.MipLevels;
		m_TexInfo.m_Type        = RESOURCE_DEPTHBUFFER;
		//===================================
		xD10GIFormatInfo* pFormat = xD10ConstLexer::singleton()->GetPixelFormat( m_TexInfo.m_ShaderViewFmt );
		m_TexInfo.m_nBytePerPixel = pFormat->m_compont * pFormat->m_bytePerComponent;
		m_TexInfo.m_xfmt = pFormat->m_fmt;
		m_TexInfo.m_Pitch = xFloorToPower2( (unsigned int)(m_TexInfo.m_TexWidth  * pFormat->m_compont * pFormat->m_bytePerComponent) );
		m_TexInfo.m_MemSize = int( m_TexInfo.m_Pitch * m_TexInfo.m_ArraySize * m_TexInfo.m_TexDepth * m_TexInfo.m_TexHeight );
		return true;
	}
	return false;
}
bool xD10DepthTexture::isSameInstance(IRenderTarget* pRenderTarget)
{
    if(&m_RenderTarget == pRenderTarget)
        return true;
    return false;
}
IRenderTarget* xD10DepthTexture::toRenderTarget(size_t iSlice , size_t iMipMapLevel)
{
	if(iSlice == 0 && iMipMapLevel == 0)
	{
		m_RenderTarget.create(m_pTexture,m_TexInfo , m_SampleDesc);
        AddRef();
		return &m_RenderTarget;
	}
	return NULL;
}

END_NAMESPACE_XEVOL3D