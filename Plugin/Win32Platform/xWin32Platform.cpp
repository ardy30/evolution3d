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
#include "xStdPch.h"
#pragma warning (disable:4311)
#pragma warning (disable:4312)
#pragma warning (disable:4244)
#ifdef _WIN32
#define _WIN32_WINNT 0x0400
#include <windows.h>
#include <direct.h>
#include <stdlib.h>
#include <stdio.h>
#include "xWin32Platform.h"
#include <RenderApi/xRenderApiManager.h>

#define new XNEW_OP

#pragma  comment(lib,"winmm.lib")
extern HANDLE Global_hModuleHandle;

#define WIN32PLATFORM  L"Win32"

BEGIN_NAMESPACE_XEVOL3D

PlatformWin32* g_pPlatFormWin32 = NULL;
IMPL_BASE_OBJECT_CLASSID(PlatformWin32 , IPlatform)
bool     PlatformWin32::m_bKeyPressed[256]={0};



bool ConvertToXEvolMsg(xWindowMsg& WinMsg , HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);



/*-------------------------------------------------------------
工厂类．创建Platform
/--------------------------------------------------------------*/
	
const wchar_t* PlatformWin32Factory::name()
{
    return WIN32PLATFORM;
}

IPlatform* PlatformWin32Factory::createInstance()
{
    if(g_pPlatFormWin32 == NULL)
        g_pPlatFormWin32 = new PlatformWin32;
    return g_pPlatFormWin32;
}

PlatformWin32Factory::PlatformWin32Factory()
{
    PlatformWin32Factory::g_pInstance = this;
    IPlatformFactory::registe(this);
}

PlatformWin32Factory* PlatformWin32Factory::g_pInstance = NULL;




PlatformWin32::PlatformWin32()
{
    m_RefCount = 1;
	m_pWindowInCreating = NULL;
	m_startTickt = getTicktCount();
	m_FrameTime  = 0;
	m_pEventTranslator = NULL;
	m_pCurrentApplication = NULL;
	memset(m_bKeyPressed,0,256); 
	//得到命令行参数．
	CoInitialize(NULL);
	//得到当前工作路径
	m_hInstance = (HINSTANCE)Global_hModuleHandle;

}

PlatformWin32::~PlatformWin32()
{
}


//-------------------------------------------------------------------
//
//
//___________________________________________________________________
bool  PlatformWin32::open(int /*argc*/ , wchar_t** /*argv*/)
{


	return true;
}

//-------------------------------------------------------------------
//
//
//___________________________________________________________________
bool PlatformWin32::close()
{
	WindowMap::iterator pos = m_Windows.begin();
	WindowMap::iterator thispos;
	size_t len = m_Windows.size();
	while(len !=0)
	{ 

		thispos = pos ;
		pos ++;
		closeWindow(thispos->second);
		len = m_Windows.size();
	}
	m_pCurrentApplication = NULL;
    HINSTANCE hExeInstance = GetModuleHandle(NULL);
    for(size_t i = 0 ; i < m_vRegistedClass.size() ; i ++)
    {
        UnregisterClassW(m_vRegistedClass[i].c_str() , hExeInstance );
    }
	CoUninitialize();
	return true;
}

bool PlatformWin32::_registerWindow(WIN_CREATE_STRUCT& createStruct)
{
    HINSTANCE hExeInstance = GetModuleHandle(NULL);
	WNDCLASSEX  wndclass ;
	wndclass.cbSize        = sizeof (wndclass) ;

	wndclass.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wndclass.lpfnWndProc   = PlatformWin32::windowProc;
	wndclass.cbClsExtra    = 0 ;
	wndclass.cbWndExtra    = 0 ;
	wndclass.hInstance     = hExeInstance;//hInstance ;
	wndclass.hIcon         = LoadIcon(NULL,IDI_APPLICATION) ;// (wndclass.hInstance, MAKEINTRESOURCE(IDI_DGE) ) ;
	wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
	LOGBRUSH longBrush =
	{
		BS_SOLID,RGB(createStruct.m_BackGrdColor.r,createStruct.m_BackGrdColor.g,createStruct.m_BackGrdColor.b),0
	}
	;
	wndclass.hbrBackground = CreateBrushIndirect(&longBrush);//(HBRUSH) GetStockObject(BLACK_BRUSH) ;

	wndclass.lpszMenuName  = createStruct.m_WinName;
	wndclass.lpszClassName = createStruct.m_WinName;
	wndclass.hIconSm       = LoadIcon(NULL,IDI_APPLICATION);//(wndclass.hInstance, MAKEINTRESOURCE(IDI_DGE) ) ;

	// Register the class
	RegisterClassEx(&wndclass);
    m_vRegistedClass.push_back(wndclass.lpszClassName);
	return true;
}

bool PlatformWin32::releaseWindowEvent(IWindow* pWnd)
{
	Win32WindowImp* pWin32Win = (Win32WindowImp*)pWnd;
	HWND hWnd = pWin32Win->getHWND();
	WNDPROC wProc = (WNDPROC)GetWindowLong(hWnd , GWL_WNDPROC);
	if(wProc == PlatformWin32::windowProc && pWin32Win->m_wProc != NULL)
	{
		SetWindowLong(hWnd,GWL_WNDPROC,(int)pWin32Win->m_wProc );
		pWin32Win->m_wProc = NULL;
	}
	return true;
}

bool  PlatformWin32::grabWindowEvent(IWindow* pWnd)
{
	Win32WindowImp* pWin32Win = (Win32WindowImp*)pWnd;
	HWND hWnd = pWin32Win->getHWND();
	WNDPROC wProc = (WNDPROC)GetWindowLong(hWnd , GWL_WNDPROC);
	if(wProc != PlatformWin32::windowProc)
	{
		pWin32Win->m_wProc = wProc;
		SetWindowLong(hWnd,GWL_WNDPROC,(int)PlatformWin32::windowProc );
	}
	return true;

}
//-------------------------------------------------------------------
//
//IWindow*        createWindow( WIN_CREATE_STRUCT createStruct)
//___________________________________________________________________
IWindow*        PlatformWin32::createWindow(WIN_CREATE_STRUCT& createStruct  , const xXmlNode* params)
{
	if(createStruct.m_isFullScreen == true && (createStruct.m_WinRect.w == 0 || createStruct.m_WinRect.h == 0) )
	{
		createStruct.m_WinRect.x = 0;
		createStruct.m_WinRect.w =	GetSystemMetrics(SM_CXSCREEN); // x size
		createStruct.m_WinRect.y = 0;
		createStruct.m_WinRect.h =  GetSystemMetrics(SM_CYSCREEN); // y size
	}

	if( params ==NULL || !params->value(L"externWindow"))
	{
		HRESULT hr= GetLastError();
		_registerWindow(createStruct);
		hr= GetLastError();		
		hr= GetLastError();
	}

	Win32WindowImp* pWindow =  new Win32WindowImp;// pRendererCreator->createWindow(createStruct,params);

	m_pWindowInCreating = pWindow;
	if(false == pWindow->__createWindow(createStruct,params))
	{
		m_pWindowInCreating = NULL;
		delete pWindow;
		return NULL;
	}
	m_pWindowInCreating = NULL;
	wcsncpy( ((Win32WindowImp*)pWindow)->m_name , createStruct.m_WinName, 64);
	xWindowMsg msg;
	msg.MsgID = WIN_CREATE;
	pWindow->dispatchMsg(msg);
	((Win32WindowImp*)pWindow)->saveWindowMsgHandler();
	return pWindow;
}

IRenderApiCreator* GetRenderApiCreate(const wchar_t* renderApiName)
{
	if(renderApiName == NULL ) renderApiName = L"Auto";
	if(renderApiName == std::wstring(L"Auto"))
	{
		OSVERSIONINFOW _version;
		ZeroMemory(&_version , sizeof(_version) );
		_version.dwOSVersionInfoSize = sizeof(_version);
		GetVersionExW(&_version);
		if(_version.dwMajorVersion >=6 && _version.dwMinorVersion >= 1) //Win7
		{
			renderApiName = L"Direct3D11";
		}
		else if(_version.dwMajorVersion ==6 && _version.dwMinorVersion == 0 ) //Win Vista
		{
			renderApiName = L"Direct3D10";
		}
		else 
		{
			renderApiName = L"Direct3D9";
		}
	}
	return  xRendererAPIManager::findAPICreator(renderApiName);
}



IRenderApi*   PlatformWin32::createRenderApi(int iWnd , const wchar_t* RenderApi , const xXmlNode* params)
{
	IRenderApiCreator * pCreator = GetRenderApiCreate(RenderApi);
	
	if(pCreator == NULL)
	{
		return NULL;
	}

	HWND hWnd = (HWND)iWnd;
	if(IsWindow(hWnd) == false )
	{
		return NULL;
	}
	
	int w = 0;//params?params->int_value(L"Backbuffer.Width")  : 0;
	int h = 0;//params?params->int_value(L"Backbuffer.Height") : 0;
	IRenderApi* pOutRenderApi = pCreator->createRenderApi(iWnd , NULL , w , h , params );

	if(pOutRenderApi == NULL && RenderApi != std::wstring(L"Direct3D9"))
		return createRenderApi(iWnd , L"Direct3D9" , params);

	return pOutRenderApi;
}

IWindow*   PlatformWin32::createRenderWindow(IRenderApi** pOutRenderApi ,  WIN_CREATE_STRUCT& createStruct, const wchar_t* RenderApi , const xXmlNode* params)
{
     IWindow* pWindow = createWindow(createStruct , params);
	 if(pWindow == NULL) return NULL;
	 IRenderApiCreator * pCreator = GetRenderApiCreate(RenderApi);
	 if(pCreator == NULL)
	 {
		 *pOutRenderApi = NULL;
		 return pWindow;
	 }


	 int w = params?params->int_value(L"Backbuffer.Width")  : 0;
	 int h = params?params->int_value(L"Backbuffer.Height") : 0;

	 IRenderApi* pNewRenderApi =  pCreator->createRenderApi(pWindow , NULL , w , h , params);
	 if(pNewRenderApi == NULL && RenderApi != std::wstring(L"Direct3D9"))
		 return createRenderWindow(pOutRenderApi , createStruct , L"Direct3D9" , params);

	 *pOutRenderApi = pNewRenderApi;
	 return pWindow;
}
//-------------------------------------------------------------------
//关闭一个Window．
//并把它从当前的窗口列表中清除掉．释放IWindow
//___________________________________________________________________
bool  PlatformWin32::closeWindow(IWindow* pWnd)
{
	Win32WindowImp* pWin32Wind = (Win32WindowImp*)pWnd;
	m_Windows.erase(pWin32Wind->getHWND());
	pWin32Wind->close(); 
	pWin32Wind->ReleaseObject();
	return true;
}

void PlatformWin32::managerWindow(IWindow* pWindow)
{
	HWND hWnd  =  (HWND)pWindow->getHandle();
	m_Windows.insert(WindowMap::value_type(hWnd,pWindow));
}

void PlatformWin32::unmanagerWindow(IWindow* pWindow)
{
	HWND hWnd  =  (HWND)pWindow->getHandle();
	m_Windows.erase(hWnd);
}


//-------------------------------------------------------------------
//得到Tickt的时间值
//
//___________________________________________________________________
int PlatformWin32::getTicktCount()
{
	return timeGetTime();
}
int PlatformWin32::getFrameTime()
{
	return m_FrameTime;
}

int PlatformWin32::getElapsTime()
{
	int elapsTime =  getTicktCount() - m_startTickt;  
	if(elapsTime > 1000)
	{
		XEVOL_LOG(eXL_DEBUG_NORMAL,L"elapsTime = %dms , is too long clamp to 20ms\n",elapsTime);
		return 20;
	}
	return elapsTime;
}

//-------------------------------------------------------------------
//启动消息循环．
//应用程序开始进行运行．
//___________________________________________________________________
bool PlatformWin32::messageLoop()
{
	return dealMessage();
}

const wchar_t* PlatformWin32::name()
{
    return PlatformWin32Factory::g_pInstance->name();
}
bool PlatformWin32::dealMessage()
{
	MSG msg;
	//long  thisTick;
	long  lastTick;
	lastTick=getTicktCount();
	while( 1 )
	{  
		if( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
		{
			if( !GetMessage( &msg, NULL, 0, 0 ) )//Remove The message From the Message Queue
			{
				if(m_pCurrentApplication)
				{
					m_pCurrentApplication->exitAppliction();
				}				
				return msg.wParam != 0;
			}
			//if ( !TranslateAccelerator( g_AppData.hWnd, g_AppData.hAccel, &msg ) ) 
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else if(m_pCurrentApplication && m_pCurrentApplication->isRun())
		{
			long thisTickt = getTicktCount();
			long passedTime = thisTickt - lastTick;
			m_FrameTime = passedTime;
			lastTick = thisTickt;
			if(m_pCurrentApplication)
			{
				m_pCurrentApplication->prevUpdateFrame(passedTime);
				m_pCurrentApplication->updateFrame(passedTime);
				m_pCurrentApplication->postUpdateFrame(passedTime);
			}
		}
		else
		{   
			WaitMessage();
		}
	}
	return true;
}
//-------------------------------------------------------------------
//退出当前的程序，返回到系统
//___________________________________________________________________
bool PlatformWin32::quit()
{
	PostQuitMessage(0);
	return true;
}

//-------------------------------------------------------------------
//处理消息．
//首先让应用程序处理，如果要继续处理．则交给窗口处理．否则不处理
//___________________________________________________________________

bool  PlatformWin32::dispatchMsg(xWindowMsg & msg)
{
	return IPlatform::dispatchMsg(msg);
}

bool PlatformWin32::setMessageTranslator(ISysEventTranslator* pTranslator)
{
	m_pEventTranslator = pTranslator;
	return pTranslator != NULL;
}
//-------------------------------------------------------------------
//私有函数．
//将一个Window放到Window Map里．当收到WM_CREATE消息的时候，
//表示有一个Window要创建了．
//___________________________________________________________________
bool  PlatformWin32::insertWindow(IWindow* pWnd)
{
	if(pWnd == NULL)
		return false;

	Win32WindowImp * pWin32Win = (Win32WindowImp *)pWnd;
	m_Windows.insert(WindowMap::value_type(pWin32Win->m_hWindow,pWnd));
	return true;
}

bool PlatformWin32::insertWindow(IWindow* pWnd, HWND hWnd)
{
	if(pWnd == NULL)
		return false;

	Win32WindowImp * pWin32Win = (Win32WindowImp *)pWnd;
	m_Windows.insert(WindowMap::value_type(hWnd,pWnd));
	return true;
}

void PlatformWin32::getMousePosition(x2DPoint& point)
{
	::GetCursorPos((POINT*)&point);
	return ;
}

//-------------------------------------------------------------------
//真正的消息处理函数．
//当窗口创建的时候，这个函数为消息处理．
//在WM_CREATE的时候．保留IWindow对象和HWND之间的关系．
//所有的消息．都被转化成DGE的消息．交给Platform去统一分流
//___________________________________________________________________

LONG CALLBACK PlatformWin32::windowProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	xWindowMsg WinMsg;

	IWindow* pWnd = NULL;
	WindowMap::iterator pos = g_pPlatFormWin32->m_Windows.find(hWnd);
	if( (WM_GETMINMAXINFO ==  msg || WM_NCCALCSIZE == msg || WM_NCCREATE == msg)
		&& g_pPlatFormWin32->m_pWindowInCreating != NULL )
	{
		pWnd = g_pPlatFormWin32->m_pWindowInCreating;
		g_pPlatFormWin32->m_pWindowInCreating = NULL;
		g_pPlatFormWin32->insertWindow(pWnd,hWnd);
		goto WIND_MSG_PROC;
	}

	if(pos == g_pPlatFormWin32->m_Windows.end() )
	{
		XEVOL_LOG(eXL_ERROR_FALT,L"Window Message belong to a unregisted window : HWWD=%d,MsgID=%d\n",(int)hWnd,msg);
		return (LONG)::DefWindowProc(hWnd,msg,wParam,lParam);
	}
	pWnd = pos->second;

WIND_MSG_PROC:


	switch(msg)
	{
	case WM_KEYDOWN:
		m_bKeyPressed[(unsigned char)wParam] = true;
		break ;
	case WM_KEYUP:
		m_bKeyPressed[(unsigned char)wParam] = false;
		break ;
	}


	
	if(ConvertToXEvolMsg(WinMsg , hWnd , msg , wParam , lParam) )
	{
		WinMsg.m_pWndReciver = pWnd;
		g_pPlatFormWin32->dispatchMsg(WinMsg);
	}
	else
	{
		return (LONG)::DefWindowProc(hWnd,msg,wParam,lParam); ;
	}

	return 0;
}



bool PlatformWin32::setDisplayMode(int w, int h, int cl_bits,int display_fre)
{
	DEVMODE dmSettings = {0};	
	if(!EnumDisplaySettings(NULL,ENUM_CURRENT_SETTINGS,&dmSettings))
	{
		MessageBox(NULL, L"Could Not Enum Display Settings", L"Error", MB_OK);
		return FALSE;
	}

	dmSettings.dmPelsWidth	= w;	
	dmSettings.dmPelsHeight	= h;
	dmSettings.dmBitsPerPel = cl_bits;
	dmSettings.dmDisplayFrequency = display_fre;
	dmSettings.dmFields     = DM_DISPLAYFREQUENCY| DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

	int result = ChangeDisplaySettings(&dmSettings,CDS_FULLSCREEN);	
	// Check if we didn't recieved a good return message From the function
	if(result != DISP_CHANGE_SUCCESSFUL)
	{
		// Display the error message and quit the program
		MessageBox(NULL, L"Display Mode Not Compatible", L"Error", MB_OK);
		return false;
	}
	return true;
}



END_NAMESPACE_XEVOL3D

#endif



