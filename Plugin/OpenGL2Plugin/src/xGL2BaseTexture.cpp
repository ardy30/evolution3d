#include "xOpenGL2API.h"
#include "xGL2BaseTexture.h"
#include "BaseLib/xBaseMath.h"
BEGIN_NAMESPACE_XEVOL3D


xGL2TextureBuffer::xGL2TextureBuffer()
{
	m_pitch = 0;
	m_pixel  = NULL;
	m_dataLen = 0;
	m_destBuffer = 0;
}
xGL2TextureBuffer::~xGL2TextureBuffer()
{
	XSAFE_DELETE_ARRAY(m_pixel);
}

void  xGL2TextureBuffer::setDestBuffer(void* destBuffer)
{
	m_destBuffer = destBuffer;
}

bool  xGL2TextureBuffer::create(int h , int pitch)
{
	m_dataLen = h * pitch;
	m_pitch = pitch;
	m_pixel = new char[m_dataLen];
    memset(m_pixel , 0 , m_dataLen);
	return true;
}

bool  xGL2TextureBuffer::unload()
{
	XSAFE_DELETE_ARRAY(m_pixel);
	m_pitch = 0;
	m_dataLen = 0;
	m_destBuffer = 0;
	return true;
}

bool  xGL2TextureBuffer::flush(int h , int dest_pitch , int src_pitch )
{
	if(src_pitch == 0) src_pitch = m_pitch;
	memcpy(m_destBuffer , m_pixel , dest_pitch * h);
	m_destBuffer = NULL;
	return true;
}


xGL2BaseTexture::xGL2BaseTexture(xGL2RenderApi* pD10Api):IBaseTexture(pD10Api) 
{
	m_RefCount = 1;
	m_pGL2Api = pD10Api ; 
}


xGL2BaseTexture::~xGL2BaseTexture()
{

}


unsigned long xGL2BaseTexture::memUsage()
{
	return m_TexInfo.m_MemSize ; 
}

eResourceType xGL2BaseTexture::res_type()
{
	return m_TexInfo.m_Type ; 
}

 bool xGL2BaseTexture::desc(xTextureDesc& _desc)
 {
	 _desc.m_depth      = (int)m_TexInfo.m_TexDepth;
	 _desc.m_fmt        =      m_TexInfo.m_fmt;
	 _desc.m_nArraySize = (int)m_TexInfo.m_ArraySize;
	 _desc.m_width      = (int)m_TexInfo.m_TexWidth;
	 _desc.m_height     = (int)m_TexInfo.m_TexHeight;
	 return true;
 }
bool xGL2BaseTexture::unload()
{
    return true;
}
 //////////////////////////////////////////////////////////////////////////
 xGL2UnkwonTexture::xGL2UnkwonTexture(xGL2RenderApi* pD10Api) : xGL2BaseTexture(pD10Api) 
 {
	 //m_pTextureView = NULL;
	 //m_pTexture  = NULL;
	 //memset(&m_TexInfo, 0 ,sizeof(m_TexInfo));
 }
 bool xGL2UnkwonTexture::isLoaded()
 {
	 return true;
	 //return m_pTexture != NULL && m_pTextureView != NULL;
 }

 bool xGL2UnkwonTexture::unload()
 {
	 //XSAFE_RELEASE(m_pTextureView);
	 //XSAFE_RELEASE(m_pTexture);
     xGL2BaseTexture::unload();
	 return !isLoaded();
 }

 xGL2UnkwonTexture::~xGL2UnkwonTexture()
 {
     unload();
 }

 void* xGL2UnkwonTexture::handle()
 {
	 return NULL;
 }

 //bool xGL2UnkwonTexture::_load(ID3D10Resource* pTexture , bool bCreateTextureView )
 //{
	// return false;
	// //m_pTexture = pTexture;
	// //GL2_SHADER_RESOURCE_VIEW_DESC srvDesc;
	// //GL2_RESOURCE_DIMENSION type;
	// //m_pTexture->GetType( &type );

	// //m_TexInfo.m_Pitch = 0;
	// //m_TexInfo.m_SlicePitch = 0;

	// //switch( type )
	// //{
	// //case GL2_RESOURCE_DIMENSION_BUFFER:
	//	// //...
	//	// break;
	// //case GL2_RESOURCE_DIMENSION_TEXTURE1D:
	//	// {
	//	//	 GL2_TEXTURE1D_DESC desc;
	//	//	 ID3D10Texture1D *pTexture1D = (ID3D10Texture1D*)m_pTexture;
	//	//	 pTexture1D->GetDesc( &desc );
 // //           if(m_TexInfo.m_ShaderViewFmt == DXGI_FORMAT_UNKNOWN) m_TexInfo.m_ShaderViewFmt = desc.Format;
 // //           if(m_TexInfo.m_RTViewFmt  == DXGI_FORMAT_UNKNOWN) m_TexInfo.m_RTViewFmt  = desc.Format;
 // //           if(m_TexInfo.m_ResFmt     == DXGI_FORMAT_UNKNOWN) m_TexInfo.m_ResFmt     = desc.Format;
	//	//	 if(bCreateTextureView)
	//	//	 {
 // //      		 srvDesc.Format = m_TexInfo.m_ShaderViewFmt;
	//	//		 srvDesc.ViewDimension = GL2_SRV_DIMENSION_TEXTURE1D;
	//	//		 srvDesc.Texture2D.MostDetailedMip = 0;
	//	//		 srvDesc.Texture2D.MostDetailedMip = desc.MipLevels;
	//	//		 ID3D10ShaderResourceView *pSRView = NULL;
	//	//		 m_pGL2Api->d10Device()->CreateShaderResourceView( m_pTexture, &srvDesc, &pSRView );
	//	//		 if(pSRView == NULL)
	//	//		 {
	//	//			 unload();
	//	//			 return false;
	//	//		 }
	//	//		 m_pTextureView = pSRView;
	//	//	 }			 

	//	//	 //野割
	//	//	 m_TexInfo.m_ArraySize   = desc.ArraySize;
	//	//	 m_TexInfo.m_TexWidth    = desc.Width;
	//	//	 m_TexInfo.m_TexHeight   = 1;
	//	//	 m_TexInfo.m_TexDepth    = 1;
	//	//	 m_TexInfo.m_MipmapLevel = desc.MipLevels;
	//	//	 m_TexInfo.m_Type        = RESOURCE_TEXTURE1D;
	//	// }
	//	// break;
	// //case GL2_RESOURCE_DIMENSION_TEXTURE2D:
	//	// {
	//	//	 GL2_TEXTURE2D_DESC desc;
	//	//	 ID3D10Texture2D *pTexture2D = (ID3D10Texture2D*)m_pTexture;
	//	//	 pTexture2D->GetDesc( &desc );
 // //           if(m_TexInfo.m_ShaderViewFmt == DXGI_FORMAT_UNKNOWN) m_TexInfo.m_ShaderViewFmt = desc.Format;
 // //           if(m_TexInfo.m_RTViewFmt  == DXGI_FORMAT_UNKNOWN) m_TexInfo.m_RTViewFmt  = desc.Format;
 // //           if(m_TexInfo.m_ResFmt     == DXGI_FORMAT_UNKNOWN) m_TexInfo.m_ResFmt     = desc.Format;
	//	//	 if(bCreateTextureView)
	//	//	 {
	//	//		 srvDesc.Format = m_TexInfo.m_ShaderViewFmt;
	//	//		 srvDesc.ViewDimension = GL2_SRV_DIMENSION_TEXTURE2D;
	//	//		 srvDesc.Texture2D.MostDetailedMip = 0;
	//	//		 srvDesc.Texture2D.MipLevels       = desc.MipLevels;
	//	//		 ID3D10ShaderResourceView *pSRView = NULL;
	//	//		 m_pGL2Api->d10Device()->CreateShaderResourceView( m_pTexture, &srvDesc, &pSRView );
	//	//		 if(pSRView == NULL)
	//	//		 {
	//	//			 unload();
	//	//			 return false;
	//	//		 }
	//	//		 m_pTextureView = pSRView;
	//	//	 }

	//	//	 //野割
	//	//	 m_TexInfo.m_ArraySize   = desc.ArraySize;
	//	//	 m_TexInfo.m_TexWidth    = desc.Width;
	//	//	 m_TexInfo.m_TexHeight   = desc.Height;
	//	//	 m_TexInfo.m_TexDepth    = 1;
	//	//	 m_TexInfo.m_MipmapLevel = desc.MipLevels;
	//	//	 m_TexInfo.m_Type        = RESOURCE_TEXTURE2D;
	//	// }
	//	// break;

	// //case GL2_RESOURCE_DIMENSION_TEXTURE3D:
	//	// {
	//	//	 GL2_TEXTURE3D_DESC desc;
	//	//	 ID3D10Texture3D *pTexture3D = (ID3D10Texture3D*)m_pTexture;
	//	//	 pTexture3D->GetDesc( &desc );
 // //           if(m_TexInfo.m_ShaderViewFmt == DXGI_FORMAT_UNKNOWN) m_TexInfo.m_ShaderViewFmt = desc.Format;
 // //           if(m_TexInfo.m_RTViewFmt  == DXGI_FORMAT_UNKNOWN) m_TexInfo.m_RTViewFmt  = desc.Format;
 // //           if(m_TexInfo.m_ResFmt     == DXGI_FORMAT_UNKNOWN) m_TexInfo.m_ResFmt     = desc.Format;
	//	//	 if(bCreateTextureView)
	//	//	 {
	//	//		 srvDesc.Format = m_TexInfo.m_ShaderViewFmt;
	//	//		 srvDesc.ViewDimension = GL2_SRV_DIMENSION_TEXTURE3D;
	//	//		 srvDesc.Texture2D.MostDetailedMip = 0;
	//	//		 srvDesc.Texture2D.MostDetailedMip = desc.MipLevels;
	//	//		 ID3D10ShaderResourceView *pSRView = NULL;
	//	//		 m_pGL2Api->d10Device()->CreateShaderResourceView( m_pTexture, &srvDesc, &pSRView );
	//	//		 if(pSRView == NULL)
	//	//		 {
	//	//			 unload();
	//	//			 return false;
	//	//		 }
	//	//		 m_pTextureView = pSRView;
	//	//	 }

	//	//	 //野割
	//	//	 m_TexInfo.m_ArraySize   = 1;
	//	//	 m_TexInfo.m_TexWidth    = desc.Width;
	//	//	 m_TexInfo.m_TexHeight   = desc.Height;
	//	//	 m_TexInfo.m_TexDepth    = desc.Depth;
	//	//	 m_TexInfo.m_MipmapLevel = desc.MipLevels;
	//	//	 m_TexInfo.m_Type        = RESOURCE_TEXTURE3D;
	//	// }
	//	// break;
	// //default:
	//	// return false;
	//	// break;
	// //}

	// ////===================================
	// //xGL2GIFormatInfo* pFormat = xGL2ConstLexer::singleton()->GetPixelFormat( m_TexInfo.m_ShaderViewFmt );
	// //m_TexInfo.m_nBytePerPixel = pFormat->m_compont * pFormat->m_byte;
	// //m_TexInfo.m_xfmt = pFormat->m_fmt;
	// //unsigned int iPitch = (unsigned int) (m_TexInfo.m_TexWidth  * pFormat->m_compont * pFormat->m_byte );
	// //m_TexInfo.m_Pitch = xFloorToPower2( iPitch );
	// //m_TexInfo.m_SlicePitch = xFloorToPower2(m_TexInfo.m_Pitch * m_TexInfo.m_TexHeight );
	// //m_TexInfo.m_MemSize = int( m_TexInfo.m_Pitch * m_TexInfo.m_ArraySize * m_TexInfo.m_TexDepth * m_TexInfo.m_TexHeight );
	// return true;
 //}
END_NAMESPACE_XEVOL3D


