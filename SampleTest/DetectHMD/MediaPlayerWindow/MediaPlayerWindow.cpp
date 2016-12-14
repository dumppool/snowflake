// MediaPlayerWindow.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "MediaPlayerWindow.h"
#include "LostMediaDecoder.h"

#include <al.h>
#include <alc.h>

#pragma comment(lib, "OpenAL32.lib")

using namespace lostvr;

DecoderHandle SDecoder;
LostMediaPlayer* SPlayer;

#define MAX_LOADSTRING 100

// 全局变量: 
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

class ADecodeCallback : public IDecodeCallback
{
public:

	virtual bool NeedNewFrame(uint32 w, uint32 h, int32 format) override
	{
		return SPlayer->CanWrite(w, h, (DXGI_FORMAT)format);
	}

	virtual void ProcessVideoFrame(const uint8* buf, uint32 sz) override
	{
		SPlayer->ProcessVideoBuffer(buf, sz);
	}

	virtual void ProcessAudioFrame(const uint8* buf, uint32 sz, int32 rate) override
	{
		// module for rendering audio?
		static ALCdevice*  oalDevice = nullptr;
		static ALCcontext* oalContext = nullptr;
		static ALuint audio_source;
		static std::deque<ALuint> audio_buffer_queue;

		if (!oalDevice || !oalContext)
		{
			oalDevice = alcOpenDevice(nullptr);
			if (!oalDevice)
				return;
			oalContext = alcCreateContext(oalDevice, nullptr);
			if (!oalContext)
				return;
			alcMakeContextCurrent(oalContext);
			alGenSources(1, &audio_source);
		}

		ALuint albuffer = 0;
		alGenBuffers(1, &albuffer);
		alBufferData(albuffer, AL_FORMAT_STEREO16, buf, sz, rate);
		alSourceQueueBuffers(audio_source, 1, &albuffer);
		audio_buffer_queue.push_front(albuffer);

		int32 val = -1;
		alGetSourcei(audio_source, AL_BUFFERS_PROCESSED, &val);
		while (val--)
		{
			alSourceUnqueueBuffers(audio_source, 1, &audio_buffer_queue.back());
			alDeleteBuffers(1, &audio_buffer_queue.back());
			audio_buffer_queue.pop_back();
		}

		alGetSourcei(audio_source, AL_SOURCE_STATE, &val);
		if (val != AL_PLAYING)
			alSourcePlay(audio_source);
	}

	virtual void OnEvent(EDecodeEvent event) override
	{
		char ret[1024];
		ret[0] = '\0';
		switch (event)
		{
		case EDecodeEvent::UnDefined:
			break;
		case EDecodeEvent::Opened:
			snprintf(ret, 1023, "opened -\t%s\n", SDecoder->GetMediaInfo()->Url);
			break;
		case EDecodeEvent::OpenFailed:
			snprintf(ret, 1023, "open failed -\t%s\n", SDecoder->GetMediaInfo()->Url);
			break;
		case EDecodeEvent::Playing:
			snprintf(ret, 1023, "playing -\t%f\n", SDecoder->GetPos());
			break;
		case EDecodeEvent::Seeking:
			snprintf(ret, 1023, "seeking -\t%f\n", SDecoder->GetPos());
			break;
		case EDecodeEvent::Paused:
			snprintf(ret, 1023, "paused -\t%f\n", SDecoder->GetPos());
			break;
		case EDecodeEvent::Closed:
			snprintf(ret, 1023, "closed -\t%s\n", SDecoder->GetMediaInfo()->Url);
			break;
		case EDecodeEvent::DroppedOneFrame:
			snprintf(ret, 1023, "dropped one frame -\t%f\n", SDecoder->GetPos());
			break;
		case EDecodeEvent::InfoUpdated:
			snprintf(ret, 1023, "info updated -\t%s\n", SDecoder->GetMediaInfo()->ToString());
			break;
		default:
			break;
		}

		OutputDebugStringA(ret);
	}

	virtual void OnMessage(EDecodeMsgLv level, const CHAR* buf, uint32 sz) override
	{
		OutputDebugStringA(buf);
	}
};

IDecodeCallbackPtr SDecodeCallback;

// 此代码模块中包含的函数的前向声明: 
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

    // TODO: 在此放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MEDIAPLAYERWINDOW, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化: 
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MEDIAPLAYERWINDOW));

    MSG msg;

    // 主消息循环: 
    while (1)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
			if (msg.message == WM_QUIT)
			{
				break;
			}

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
		else
		if (SPlayer != nullptr)
		{
			SPlayer->Tick();
		}
    }

    return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MEDIAPLAYERWINDOW));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MEDIAPLAYERWINDOW);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   static const char* SURLs[] = {
	   "file://f:/User/VRMp4/showcase.mp4",
	   "http://down.video.vronline.com/dev/2c34cf5a90b845c32081a60cf4c66bfb.mp4",
	   "http://down.video.vronline.com/dev/Video3.mp4",
	   "http://down.video.vronline.com/dev/49058022ea364643715d501e3c9657db.mp4",
	   "http://down.video.vronline.com/dev/c66a1ea0a5cf04e74537aa83a92a8065.mp4",
	   "http://down.video.vronline.com/dev/ed72804f371a166d26aecacd0a5ab69c.mp4",
	   "http://down.video.vronline.com/dev/948191cd4b137312348114a1d99c4ee6.mp4",
	   "http://down.video.vronline.com/dev/yaogunxueyuanshishenghaichang.mp4",
	   "http://down.video.vronline.com/dev/hongsejingjiekongzhongjuedou.mp4",
	   "http://down.video.vronline.com/dev/meisaidesibenchi.mp4",
	   "http://down.video.vronline.com/dev/yuehangyuanxunlianrichang.mp4",
	   "http://down.video.vronline.com/dev/Video13.mp4",
	   "http://down.video.vronline.com/dev/Video11.mp4",
	   "http://down.video.vronline.com/dev/nanjidalu.mp4",
	   "http://down.video.vronline.com/dev/meiguicheng.mp4",
   };
   SPlayer = new LostMediaPlayer(hWnd);
   SDecodeCallback = IDecodeCallbackPtr(new ADecodeCallback);
   SDecoder = DecodeMedia((SDecodeCallback), SURLs[9]);

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static bool bSwitch = true;
	static float rate = 1.f;
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 分析菜单选择: 
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
	case WM_KEYUP:
	{
		int id = LOWORD(wParam);
		switch (id)
		{
		case VK_SPACE:
			bSwitch = !bSwitch;
			SDecoder->EnqueueCommand(bSwitch ? EDecodeCommand::Play : EDecodeCommand::Pause);
			break;
		case VK_ESCAPE:
			SDecoder->EnqueueCommand(EDecodeCommand::Stop);
			break;
		case VK_DOWN:
			rate *= 0.7f;
			SDecoder->SetRate(rate);
			break;
		case VK_UP:
			rate *= 1.5f;
			SDecoder->SetRate(rate);
			break;
		case VK_LEFT:
			SDecoder->Seek(SDecoder->GetPos() - 5.0);
			break;
		case VK_RIGHT:
			SDecoder->Seek(SDecoder->GetPos() + 5.0);
			break;
		default:
			break;
		}
	}
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);

		//SDecodeCallback = nullptr;
		SDecoder->SetCallback(IDecodeCallbackPtr(nullptr));

		Sleep(100);

		ReleaseDecoder(&SDecoder);

		delete SPlayer;
		SPlayer = nullptr;
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”框的消息处理程序。
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
