#pragma once

namespace lostvr
{
	enum class EDirect3D : uint8
	{
		DeviceRef,
		DXGI0,
		DXGI1,
		DXGI2,
		DXGI3,
	};

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
		IDXGISwapChain*			SwapChain;

		void DestroyRHI()
		{
			SAFE_RELEASE(Buffer);
			SAFE_RELEASE(SwapChain);
			SAFE_RELEASE(Context);
			SAFE_RELEASE(Device);
		}

	public:
		explicit Direct3D11Helper(EDirect3D InVer);


		//************************************
		// Method:    Direct3D11Helper 根据提供的Direct9的设置初始化渲染器
		// FullName:  lostvr::Direct3D11Helper::Direct3D11Helper
		// Access:    public 
		// Returns:   
		// Qualifier:
		// Parameter: EDirect3D ver
		// Parameter: IDirect3DDevice9 * device
		//************************************
		Direct3D11Helper(EDirect3D ver, IDirect3DDevice9* device);

		~Direct3D11Helper()
		{
			DestroyRHI();
		}

		bool UpdateRHIWithSwapChain(IDXGISwapChain* swapChain);
		bool UpdateBuffer_Direct3D9(IDirect3DDevice9* device);

		bool InitializeRHI(UINT width, UINT height);

		bool OutputBuffer_Texture2D(ID3D11Texture2D* dst);

		ID3D11Device* GetDevice() 
		{
			return Device;
		}

		ID3D11DeviceContext* GetContext()
		{
			return Context;
		}

		IDXGISwapChain* GetSwapChain()
		{
			return SwapChain;
		}

		EDirect3D GetGraphicsInterfaceVersion()
		{
			return GIVer;
		}

		IDirect3DDevice9* GetTemporaryDirect9Device() const;
	};
}