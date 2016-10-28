#pragma once

namespace lostvr
{
	enum class EDirect3D : uint8
	{
		Undefined = 0,
		DeviceRef,
		DXGI0,
		DXGI1,
		DXGI2,
		DXGI3,
		Num,
	};

	static bool GetNextGIVersion(EDirect3D ver, EDirect3D& nextVer)
	{
		nextVer = EDirect3D(((int)ver + 1) % (int)EDirect3D::Num);
		switch (ver)
		{
		case EDirect3D::Undefined:
		case EDirect3D::DeviceRef:
		case EDirect3D::DXGI2:
		case EDirect3D::DXGI3:
			return false;
		}

		return true;
	}

	static const CHAR* GetEDirect3DString(EDirect3D Ver)
	{
		if (Ver == EDirect3D::DeviceRef)
		{
			return "Device reference";
		}
		else if (Ver == EDirect3D::DXGI0)
		{
			return "DXGI 1.0";
		}
		else if (Ver == EDirect3D::DXGI1)
		{
			return "DXGI 1.1";
		}
		else if (Ver == EDirect3D::DXGI2)
		{
			return "DXGI 1.2";
		}
		else
		{
			return "unknown graphics interface";
		}
	}

	class Direct3D11Helper
	{
		EDirect3D				GIVer;

		ID3D11Device*			Device;
		ID3D11DeviceContext*	Context;
		ID3D11Texture2D*		Buffer;
		ID3D11Texture2D*		Buffer_Direct9Copy;
		IDXGISwapChain*			SwapChain;

		void DestroyRHI()
		{
			SAFE_RELEASE(Buffer);
			SAFE_RELEASE(Buffer_Direct9Copy);
			SAFE_RELEASE(SwapChain);
			SAFE_RELEASE(Context);
			SAFE_RELEASE(Device);
		}

		bool InitializeRHI(UINT width = 0, UINT height = 0, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);

	public:
		explicit Direct3D11Helper(EDirect3D InVer);


		//************************************
		// Method:    Direct3D11Helper 根据提供的Direct9的设置初始化swap chain
		// FullName:  lostvr::Direct3D11Helper::Direct3D11Helper
		// Access:    public 
		// Returns:   
		// Qualifier:
		// Parameter: EDirect3D ver
		// Parameter: IDirect3DDevice9 * device
		//************************************
		Direct3D11Helper(EDirect3D ver, IDirect3DDevice9* device);

		//************************************
		// Method:    Direct3D11Helper 提供更具体的初始化信息
		// FullName:  lostvr::Direct3D11Helper::Direct3D11Helper
		// Access:    public 
		// Returns:   
		// Qualifier:
		// Parameter: EDirect3D ver
		// Parameter: UINT width
		// Parameter: UINT height
		// Parameter: DXGI_FORMAT format
		//************************************
		Direct3D11Helper(EDirect3D ver, UINT width, UINT height, DXGI_FORMAT format);

		~Direct3D11Helper()
		{
			DestroyRHI();
		}

		bool UpdateRHIWithSwapChain(IDXGISwapChain* swapChain);
		bool OutputBuffer_Texture2D(ID3D11Texture2D* dst);
		bool UpdateBuffer_Direct3D9(IDirect3DDevice9* device);
		bool OutputBuffer_Texture2D_Direct9(ID3D11Texture2D* dst);

		ID3D11Device* GetDevice();
		ID3D11DeviceContext* GetContext();
		IDXGISwapChain* GetSwapChain();
		bool GetSwapChainData(UINT& width, UINT& height, DXGI_FORMAT& format);
		EDirect3D GetGraphicsInterfaceVersion();

		//************************************
		// Method:    GetTemporaryDirect9Device 创建临时Direct9设备对象
		// FullName:  lostvr::Direct3D11Helper::GetTemporaryDirect9Device
		// Access:    public 
		// Returns:   IDirect3DDevice9* Direct9设备对象，使用完务必调用释放引用接口
		// Qualifier: const
		//************************************
		IDirect3DDevice9* GetTemporaryDirect9Device() const;
		bool EnsureDirect9CopyValid(D3DSURFACE_DESC surfaceDesc);

		//************************************
		// Method:    GetDirect9FormatMatch 获取和Direct9资源格式匹配的DXGI格式
		// FullName:  lostvr::Direct3D11Helper::GetDirect9FormatMatch
		// Access:    public 
		// Returns:   DXGI_FORMAT
		// Qualifier:
		// Parameter: D3DFORMAT format
		//************************************
		DXGI_FORMAT GetDirect9FormatMatch(D3DFORMAT format);
	};
}