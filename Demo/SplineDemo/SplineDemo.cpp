// SplineDemo.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "SplineDemo.h"
#include "XMathLib/xMathLib.h"
#include "XMathLib/xGeomLib.h"
#include "XMathLib/xspline.h"
using namespace xMathLib;
using namespace xGeomLib;
#define MAX_LOADSTRING 100


#include <vector>  // Include STL vector class.

// a polygon/contour and a series of triangles.



// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       LPTSTR    lpCmdLine,
                       int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.
    MSG msg;
    HACCEL hAccelTable;

    // Initialize global strings
    LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadString(hInstance, IDC_SPLINEDEMO, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SPLINEDEMO));

    // Main message loop:
    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style			= CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc	= WndProc;
    wcex.cbClsExtra		= 0;
    wcex.cbWndExtra		= 0;
    wcex.hInstance		= hInstance;
    wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SPLINEDEMO));
    wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_SPLINEDEMO);
    wcex.lpszClassName	= szWindowClass;
    wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    HWND hWnd;

    hInst = hInstance; // Store instance handle in our global variable

    hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

    if (!hWnd)
    {
        return FALSE;
    }

    xvec2 ptList[8];
    ptList[0] = xvec2( 3.0f ,  0.0f );
    ptList[1] = xvec2( 1.0f ,  1.0f );

    ptList[2] = xvec2( 0.0f ,  3.0f );
    ptList[3] = xvec2(-1.0f ,  1.0f );

    ptList[4] = xvec2(-3.0f ,  0.0f );
    ptList[5] = xvec2(-1.0f , -1.0f );

    ptList[6] = xvec2( 0.0f , -3.0f );
    ptList[7] = xvec2( 1.0f , -1.0f );


    ds_vector(xMathLib::xvec3i) triList;
    XM_ToTriangles(ptList , 8 , triList);


    //点到三角形的投影
    {
        xMathLib::xvec3 _v0(1 , 0 , 0 );
        xMathLib::xvec3 _v1(0 , 1 , 0 );
        xMathLib::xvec3 _v2(0 , 0 , 1 );
        xtriangle _tri(_v0 , _v1 , _v2);
        xMathLib::xvec3 pt(100.0f , 30.0f , 10.0f);
        xMathLib::xvec3 _ret ;
        float t1 = 1.0f , t2 = -1.0f;
        XM_Project(pt , _tri , _ret , t1 , t2);
        xMathLib::xvec3 _dir = pt -  _ret;
        int i = 0;
    }

    
    //点到平面的投影
    {
        xMathLib::xvec3 p0(1.0f , 1.0f , 1.0f);
        xMathLib::xvec3 n(1.0f , 1.0f , 1.0f);
        xGeomLib::xplane pl(p0 , n );
        xMathLib::xvec3 pt(100.0f , 30.0f , 10.0f);

        xMathLib::xvec3 _ret ;
        XM_Project(pt , pl , _ret);
        xMathLib::xvec3 _dir = pt -  _ret;
        int i = 0;

    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
void drawBezier(HDC hDC)
{
    //int nRank = 5;
    //   xBezier<xvec2> myBezier(nRank);
    //myBezier[0] = xvec2(30.0f,840.0f);
    //myBezier[1] = xvec2(360.0f,120.0f);
    //myBezier[2] = xvec2(790.0f,780.0f);
    //myBezier[3] = xvec2(1240.0f,980.0f);
    //myBezier[4] = xvec2(1600.0f,780.0f);

    //POINT pt;
    //MoveToEx(hDC , myBezier[0].x , myBezier[0].y , &pt);
    //for(int i = 1 ; i < nRank ; i ++)
    //{
    //	xvec2 p = myBezier[i];
    //	LineTo(hDC   , p.x , p.y );
    //}

    //MoveToEx(hDC , myBezier[0].x , myBezier[0].y , &pt);
    //for(int i = 0 ; i <= 3000 ; i ++)
    //{
    //       float t = i/3000.0f;
    //	xvec2 p = myBezier(t);
    //	LineTo(hDC   , p.x , p.y );
    //}


    xBezierSpline<float> spline;
    int nPt = 4;
    spline.init(nPt);
    spline.m_vPoints[0] = xvec3(130.0f ,440.0f , 0.0f );
    spline.m_vPoints[1] = xvec3(380.0f ,540.0f , 0.0f );
    spline.m_vPoints[2] = xvec3(480.0f ,290.0f , 0.0f );
    spline.m_vPoints[3] = xvec3(680.0f ,690.0f , 0.0f );

    spline.m_vNormals[0] = xvec3(-1.0f ,1.0f , 0.0f ) ; spline.m_vNormals[0].normalize();
    spline.m_vNormals[1] = xvec3(1.0f  ,0.0f , 0.0f ) ; spline.m_vNormals[1].normalize();
    spline.m_vNormals[2] = xvec3(-0.2f ,0.5f , 0.0f ) ; spline.m_vNormals[2].normalize();
    spline.m_vNormals[3] = xvec3(-0.5f ,0.0f , 0.0f ) ; spline.m_vNormals[3].normalize();
    spline.setup();

    HPEN hNormalPen = CreatePen(1 , 1 , 0xff00ff);
    HPEN hLinePen   = CreatePen(1 , 1 , 0x0000ff);
    HPEN oldPen     = (HPEN)SelectObject(hDC , hNormalPen);

    POINT pt;
    for(int i = 0 ; i < nPt-1 ; i ++)
    {
        xBezierSpline<float>::bezier& bz = spline.m_vBiziers[i];

        xvec3 p  = spline.m_vPoints[i];
        xvec3 p2 = spline.m_vPoints[i + 1];
        xvec3 n  = spline.m_vNormals[i];
        xvec3 p1 = bz.m_ControlPts[1];// p + n * 10.0f;
        SelectObject(hDC , hLinePen);
        MoveToEx(hDC , p.x  , p.y , &pt);
        LineTo(hDC   , p1.x , p1.y );
        LineTo(hDC   , p2.x , p2.y );

    }

    for(int i = 0 ; i < nPt ; i ++)
    {
        int iBizer  = i ;
        if(i == nPt - 1) iBizer = nPt - 2;
        xBezierSpline<float>::bezier& bz = spline.m_vBiziers[iBizer];

        xvec3 p  = spline.m_vPoints[i];
        xvec3 n  = spline.m_vNormals[i];

        SelectObject(hDC , hNormalPen);
        xvec3 p2 =  p + n * 50.0f;
        MoveToEx(hDC , p.x  , p.y , &pt);
        LineTo(hDC   , p2.x , p2.y );

    }

    SelectObject(hDC , oldPen);
    MoveToEx(hDC , spline.m_vPoints[0].x , spline.m_vPoints[0].y , &pt);
    for(int iBezier = 0 ; iBezier < nPt -1 ; iBezier ++)
    {
        for(int i = 0 ; i <= 3000 ; i ++)
        {
            float t = i/3000.0f;
            xvec3 p = spline(iBezier , t);
            LineTo(hDC   , p.x , p.y );
        }
    }



    xCubicSpline<float > cubicSpline;
    nPt = 3;
    cubicSpline.init(nPt);
    cubicSpline.m_vNodes[0].m_Position = xvec4(830.0f ,440.0f , 0.0f , 1.0f);
    cubicSpline.m_vNodes[1].m_Position = xvec4(980.0f ,540.0f , 0.0f , 1.0f);
    cubicSpline.m_vNodes[2].m_Position = xvec4(1380.0f ,640.0f , 0.0f , 1.0f);

    cubicSpline.m_vNodes[0].m_Tangent = xvec4(300.0f   ,-200.0f, 0.0f , 1.0f) ;
    cubicSpline.m_vNodes[1].m_Tangent = xvec4(-300.0f  ,300.0f , 0.0f , 1.0f) ;
    cubicSpline.m_vNodes[2].m_Tangent = xvec4(400.0f   ,-400.0f, 0.0f , 1.0f) ;
    cubicSpline.setup();

    for(int i = 0 ; i < nPt ; i ++)
    {
        xCubicSpline<float>::xPointNode& node = cubicSpline.m_vNodes[i];

        xvec4 p  = node.m_Position;
        xvec4 d  = node.m_Tangent;
        xvec4 p1 = p + d * 0.2;;

        SelectObject(hDC , hNormalPen);
        MoveToEx(hDC , p.x  , p.y , &pt);
        LineTo(hDC   , p1.x , p1.y );

    }

    SelectObject(hDC , oldPen);
    MoveToEx(hDC , cubicSpline.m_vNodes[0].m_Position.x , cubicSpline.m_vNodes[0].m_Position.y , &pt);
    for(int i = 0 ; i <= 3000 ; i ++)
    {
        float t = i/3000.0f;
        xvec4 p = cubicSpline(t);
        LineTo(hDC   , p.x , p.y );
    }

}

std::vector<xvec2>  ptList;
ds_vector(xvec3i)   triList;
ds_vector(xtriangle) tessTriangles;

int bDrawType = 1;
void drawLine(HDC hDC)
{
    if(ptList.size() <= 1)
        return ;

    HPEN hNormalPen = CreatePen(PS_SOLID , 3 , 0xff00ff);
    HPEN hLinePen   = CreatePen(PS_SOLID , 3 , 0x0000ff);
    HPEN oldPen     = (HPEN)SelectObject(hDC , hNormalPen);

    POINT pt;
    MoveToEx(hDC , ptList[0].x  , ptList[0].y , &pt);
    SelectObject(hDC , hNormalPen);
    for(int i =  1 ; i < ptList.size() ; i ++)
    {
        LineTo(hDC   , ptList[i].x , ptList[i].y );
    }
    LineTo(hDC   , ptList[0].x , ptList[0].y );
    SelectObject(hDC , oldPen);
    DeleteObject(hNormalPen);
    DeleteObject(hLinePen);
}


void drawTriangle(HDC hDC)
{
    if(triList.size() ==0 )
        return ;

    drawLine(hDC);
    HPEN hNormalPen = CreatePen(1 , 1 , 0xff00ff);
    HPEN hLinePen   = CreatePen(1 , 1 , 0x0000ff);
    HPEN oldPen     = (HPEN)SelectObject(hDC , hNormalPen);

    POINT pt;  

    SelectObject(hDC , hNormalPen);
    for(int i =  0 ; i < triList.size() ; i ++)
    {
        xvec3i _tr1 = triList[i];
        MoveToEx(hDC , ptList[_tr1.v[0]].x  , ptList[_tr1.v[0]].y , &pt);
        LineTo(hDC   , ptList[_tr1.v[1]].x  , ptList[_tr1.v[1]].y );
        LineTo(hDC   , ptList[_tr1.v[2]].x  , ptList[_tr1.v[2]].y );
        LineTo(hDC   , ptList[_tr1.v[0]].x  , ptList[_tr1.v[0]].y );
    }

	SelectObject(hDC , hLinePen);
	for(int i =  0 ; i < tessTriangles.size() ; i ++)
	{
		xtriangle _tr = tessTriangles[i];
		MoveToEx(hDC , _tr.m_points[0].x  , _tr.m_points[0].y , &pt);
		LineTo(hDC   , _tr.m_points[1].x  , _tr.m_points[1].y );
		LineTo(hDC   , _tr.m_points[2].x  , _tr.m_points[2].y );
		LineTo(hDC   , _tr.m_points[0].x  , _tr.m_points[0].y );
	}

    SelectObject(hDC , oldPen);
    DeleteObject(hNormalPen);
    DeleteObject(hLinePen);
}

#define LOSHORT(l)           ((short)(((DWORD_PTR)(l)) & 0xffff))
#define HISHORT(l)           ((short)((((DWORD_PTR)(l)) >> 16) & 0xffff))

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int wmId, wmEvent;
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
    case WM_LBUTTONUP:
        {
            int x, y;
            y=HISHORT(lParam);
            x=LOSHORT(lParam);
            xvec2 v( (float)x , y);
            ptList.push_back(v);
            ::InvalidateRect(hWnd , NULL , TRUE);
        };
        break;
    case WM_RBUTTONUP:
        {
            if(bDrawType == 1 )
            {
                triList.clear();
				tessTriangles.clear();
                XM_ToTriangles( &ptList[0] , ptList.size() , triList );

				xtriangle* ptriList = new xtriangle[triList.size()];
				for(int i =  0 ; i < triList.size() ; i ++)
				{
					xtriangle& _tr = ptriList[i];
					 xvec3i _tr1 = triList[i];
					_tr.m_points[0] = xvec3( ptList[_tr1.v[0]].x  , ptList[_tr1.v[0]].y   , 0.0f);
					_tr.m_points[1] = xvec3( ptList[_tr1.v[1]].x  , ptList[_tr1.v[1]].y   , 0.0f);
					_tr.m_points[2] = xvec3( ptList[_tr1.v[2]].x  , ptList[_tr1.v[2]].y   , 0.0f);
				}
				XM_Tesselation(ptriList , triList.size() , 3 , tessTriangles);
				delete [] ptriList;
            }
            bDrawType ++;
            bDrawType %= 3;
            ::InvalidateRect(hWnd , NULL , TRUE);

        };
        break;
    case WM_MBUTTONUP:
        {
            if(bDrawType != 0)
            {
                ptList.clear();
                triList.clear();
                ::InvalidateRect(hWnd , NULL , TRUE);
            }


        };
        break;
    case WM_COMMAND:
        wmId    = LOWORD(wParam);
        wmEvent = HIWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        if(bDrawType == 0 ) drawBezier (hdc) ;
        if(bDrawType == 1 ) drawLine(hdc);
        if(bDrawType == 2 ) drawTriangle(hdc);
        //drawBezier(hdc);
        // TODO: Add any drawing code here...
        EndPaint(hWnd, &ps);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
