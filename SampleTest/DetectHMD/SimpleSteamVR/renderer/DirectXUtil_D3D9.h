#pragma once

#include "d3d9.h"
#pragma comment(lib, "d3d9.lib")

#include "d3dx/Include/d3dx9.h"

#ifdef _WIN64
#pragma comment(lib, "d3dx/Lib/x64/d3dx9.lib")
#else
#pragma comment(lib, "d3dx/Lib/x86/d3dx9.lib")
#endif

#ifndef VALIDATE
#define VALIDATE(x, msg) if (!(x)) { MessageBoxA(NULL, (msg), "D3D9 App Utils", MB_ICONERROR | MB_OK); exit(-1); }
#endif


namespace lostvrdx9 {

	// clean up member COM pointers
	template<typename T> void Release(T *&obj)
	{
		if (!obj) return;
		obj->Release();
		obj = nullptr;
	}

	//---------------------------------------------------------------------
	struct DirectX9
	{
		HWND                     Window;
		bool                     Running;
		bool                     Key[256];
		int                      WinSizeW;
		int                      WinSizeH;

		IDirect3D9*				D3D;
		IDirect3DDevice9*		D3DDevice;

		HINSTANCE               hInstance;

		static LRESULT CALLBACK WindowProc(_In_ HWND hWnd, _In_ UINT Msg, _In_ WPARAM wParam, _In_ LPARAM lParam)
		{
			auto p = reinterpret_cast<DirectX9 *>(GetWindowLongPtr(hWnd, 0));
			switch (Msg)
			{
			case WM_KEYDOWN:
				p->Key[wParam] = true;
				break;
			case WM_KEYUP:
				p->Key[wParam] = false;
				break;
			case WM_DESTROY:
				p->Running = false;
				break;
			default:
				return DefWindowProcW(hWnd, Msg, wParam, lParam);
			}
			if ((p->Key['Q'] && p->Key[VK_CONTROL]) || p->Key[VK_ESCAPE])
			{
				p->Running = false;
			}
			return 0;
		}

		DirectX9() :
			D3D(nullptr),
			D3DDevice(nullptr),

			Window(nullptr),
			Running(false),
			WinSizeW(0),
			WinSizeH(0),
			hInstance(nullptr)
		{
			// Clear input
			for (int i = 0; i < sizeof(Key) / sizeof(Key[0]); ++i)
				Key[i] = false;
		}

		~DirectX9()
		{
			ReleaseDevice();
			CloseWindow();
		}

		bool InitWindow(HINSTANCE hinst, LPCWSTR title)
		{
			hInstance = hinst;
			Running = true;

			WNDCLASSW wc;
			memset(&wc, 0, sizeof(wc));
			wc.lpszClassName = L"App";
			wc.style = CS_OWNDC;
			wc.lpfnWndProc = WindowProc;
			wc.cbWndExtra = sizeof(this);
			RegisterClassW(&wc);

			// adjust the window size and show at InitDevice time
			Window = CreateWindowW(wc.lpszClassName, title, WS_OVERLAPPEDWINDOW, 0, 0, 0, 0, 0, 0, hinst, 0);
			if (!Window) return false;

			SetWindowLongPtr(Window, 0, LONG_PTR(this));

			return true;
		}

		void CloseWindow()
		{
			if (Window)
			{
				DestroyWindow(Window);
				Window = nullptr;
				UnregisterClassW(L"App", hInstance);
			}
		}

		bool InitDevice(int vpW, int vpH, const LUID* pLuid, bool windowed = true, int scale = 1)
		{
			WinSizeW = vpW;
			WinSizeH = vpH;

			if (scale == 0)
				scale = 1;

			HMODULE hMod = LoadLibraryA("d3d9.dll");
			VALIDATE((hMod != NULL), "LoadLibraryA(d3d9.dll) failed");

			D3D = Direct3DCreate9(D3D_SDK_VERSION);
			VALIDATE((D3D != nullptr), "Direct3DCreate9 failed");

			D3DDISPLAYMODE mode;
			D3D->GetAdapterDisplayMode(0, &mode);

			D3DPRESENT_PARAMETERS pp;
			ZeroMemory(&pp, sizeof(D3DPRESENT_PARAMETERS));
			pp.BackBufferWidth = WinSizeW;
			pp.BackBufferHeight = WinSizeH;
			pp.BackBufferFormat = mode.Format;
			pp.BackBufferCount = 1;
			pp.SwapEffect = D3DSWAPEFFECT_COPY;
			pp.Windowed = windowed;
			pp.hDeviceWindow = Window;

			Release(D3DDevice);
			VALIDATE((SUCCEEDED(D3D->CreateDevice(D3DADAPTER_DEFAULT, true ? D3DDEVTYPE_HAL : D3DDEVTYPE_REF,
				Window, D3DCREATE_HARDWARE_VERTEXPROCESSING, &pp, &D3DDevice))), "CreateDevice failed");

			return true;
		}

		bool HandleMessages(void)
		{
			MSG msg;
			while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			// This is to provide a means to terminate after a maximum number of frames
			// to facilitate automated testing
#ifdef MAX_FRAMES_ACTIVE 
			if (maxFrames > 0)
			{
				if (--maxFrames <= 0)
					Running = false;
			}
#endif
			return Running;
		}

		void Run(bool(*MainLoop)(bool retryCreate))
		{
			// false => just fail on any error
			VALIDATE(MainLoop(false), "Oculus Rift not detected.");
			while (HandleMessages())
			{
				// true => we'll attempt to retry for ovrError_DisplayLost
				if (!MainLoop(true))
					break;
				// Sleep a bit before retrying to reduce CPU load while the HMD is disconnected
				Sleep(10);
			}
		}

		void ReleaseDevice()
		{
			Release(D3D);
			Release(D3DDevice);
		}
	};

	static struct DirectX9 SDX9INST;

	typedef int EnumEyeID;

	template<typename ComObjectType>
	inline void ReleaseComObjectRef(ComObjectType* Obj)
	{
		if (Obj != nullptr)
		{
			Obj->Release();
			Obj = nullptr;
		}
	}

	class TextureProjector
	{
	public:
		TextureProjector();
		~TextureProjector();

		bool Init(ID3D11Device* InDevice);
		void Fini();
		bool OnPresent(const ID3D11Resource* Source);

		ID3D11Texture2D* GetFinalBuffer(EnumEyeID Eye);
		void SetEyePose(EnumEyeID Eye, const LVMatrix& EyeView, const LVMatrix& Proj);

		// if InSource is nullptr or equal to SourceRef, this call will do nothing
		bool SetSourceRef(ID3D11Texture2D* InSource);

		// it should be called before Init/SetSourceRef/OnPresent, because it will not check or recreate the render target
		void SetRenderTargetSize(uint32 InWidth, uint32 InHeight)
		{
			BufferWidth = InWidth;
			BufferHeight = InHeight;
		}

		static TextureProjector* Get()
		{
			static TextureProjector Inst;
			return &Inst;
		}

		const float NearPlane;
		const float FarPlane;

		LVMatrix DebugWorld;

	protected:
		void Update();

		ID3DXEffect*    Effect;       // D3DX effect interface

		DXGI_FORMAT		BufferFormat;
		UINT			BufferWidth;
		UINT			BufferHeight;

		FrameBufferWVP	EyePose[2];
	};

	class TextureProjector_D3D9
	{
	public:
		TextureProjector_D3D9();
		~TextureProjector_D3D9();


	};
}
