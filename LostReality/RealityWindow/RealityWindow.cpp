// RealityWindow.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "RealityWindow.h"

#include "TestCase.h"
using namespace LostCore;

#define MAX_LOADSTRING 100

// ȫ�ֱ���: 
HINSTANCE hInst;                                // ��ǰʵ��
WCHAR szTitle[MAX_LOADSTRING];                  // �������ı�
WCHAR szWindowClass[MAX_LOADSTRING];            // ����������

// �˴���ģ���а����ĺ�����ǰ������: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: �ڴ˷��ô��롣

    // ��ʼ��ȫ���ַ���
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_REALITYWINDOW, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // ִ��Ӧ�ó����ʼ��: 
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_REALITYWINDOW));

	MSG msg = {0};

    // ����Ϣѭ��: 
    while (msg.message != WM_QUIT)
    {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			TESTCASE::GetRenderSampleInstance()->Tick();
		}
    }

	TESTCASE::GetRenderSampleInstance()->Fini();
    return (int) msg.wParam;
}



//
//  ����: MyRegisterClass()
//
//  Ŀ��: ע�ᴰ���ࡣ
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_REALITYWINDOW));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_REALITYWINDOW);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ����: InitInstance(HINSTANCE, int)
//
//   Ŀ��: ����ʵ�����������������
//
//   ע��: 
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   //assert(TESTCASE::TestCase_LostCore_VirtualCall(hWnd));

   TESTCASE::GetRenderSampleInstance()->Init(hWnd, true, 1280, 720);

   return TRUE;
}

//
//  ����: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��:    ���������ڵ���Ϣ��
//
//  WM_COMMAND  - ����Ӧ�ó���˵�
//  WM_PAINT    - ����������
//  WM_DESTROY  - �����˳���Ϣ������
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static bool bMouseDown = false;

	static const float s_invalidPos = INFINITY;
	static const float s_factor = 0.1f;
	static const float s_step = 1.1f;

	static bool s_moving = false;
	static float s_startPosX = s_invalidPos;
	static float s_startPosY = s_invalidPos;


    switch (message)
    {
	case WM_MOUSEMOVE:
	{
		if (!s_moving)
		{
			break;
		}

		bool bMove = false;
		float moveX, moveY;
		moveX = moveY = 0.f;

		float posX = LOWORD(lParam);
		float posY = HIWORD(lParam);
		//if (!LostCore::IsEqual(s_startPosX, s_invalidPos))
		if (s_moving)
		{
			moveX = posX - s_startPosX;
			bMove = true;
		}

		//if (!LostCore::IsEqual(s_startPosY, s_invalidPos))
		if (s_moving)
		{
			moveY = posY - s_startPosY;
			bMove = true;
		}

		if (bMove)
		{
			FFloat3 rotate;
			rotate.Yaw = moveX * s_factor;
			rotate.Pitch = moveY * s_factor;
			TESTCASE::GetRenderSampleInstance()->GetCamera()->AddEulerWorld(rotate);
		}

		s_startPosX = posX;
		s_startPosY = posY;

		break;
	}
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // �����˵�ѡ��: 
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
        }
        break;
	case WM_RBUTTONDOWN:
	{
		s_moving = true;
		float posX = LOWORD(lParam);
		float posY = HIWORD(lParam);
		s_startPosX = posX;
		s_startPosY = posY;
	}
	break;
	case WM_RBUTTONUP:
		//s_startPosX = s_startPosY = s_invalidPos;
		s_moving = false;
		break;
	case WM_KEYUP:
	{
		int key = LOWORD(wParam);
		switch (key)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		default:
			break;
		}

		break;
	}
	case WM_KEYDOWN:
	{
		int key = LOWORD(wParam);
		switch (key)
		{
		case 'W':
			TESTCASE::GetRenderSampleInstance()->GetCamera()->AddPositionWorld(FFloat3(0.f, 0.f, s_step));
			break;
		case 'S':
			TESTCASE::GetRenderSampleInstance()->GetCamera()->AddPositionWorld(FFloat3(0.f, 0.f, -s_step));
			break;
		case 'A':
			TESTCASE::GetRenderSampleInstance()->GetCamera()->AddPositionWorld(FFloat3(-s_step, 0.f, 0.f));
			break;
		case 'D':
			TESTCASE::GetRenderSampleInstance()->GetCamera()->AddPositionWorld(FFloat3(s_step, 0.f, 0.f));
			break;
		case 'Q':
			TESTCASE::GetRenderSampleInstance()->GetCamera()->AddPositionWorld(FFloat3(0.f, -s_step, 0.f));
			break;
		case 'E':
			TESTCASE::GetRenderSampleInstance()->GetCamera()->AddPositionWorld(FFloat3(0.f, s_step, 0.f));
			break;
		default:
			break;
		}

		break;
	}

    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: �ڴ˴����ʹ�� hdc ���κλ�ͼ����...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// �����ڡ������Ϣ�������
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
