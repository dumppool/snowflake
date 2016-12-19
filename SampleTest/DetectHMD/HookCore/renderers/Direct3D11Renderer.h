/*
 * file Direct3D11Renderer.h
 *
 * author luoxw
 * date 2016/11/10
 *
 * 
 */
#pragma once

//struct IDXGIDevice;
//struct IDXGIDevice1;
//struct IDXGIDevice2;
//struct IDXGIDevice3;

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

	extern bool GetNextGIVersion(EDirect3D ver, EDirect3D& nextVer);

	extern const CHAR* GetEDirect3DString(EDirect3D Ver);

	extern EDirect3D GetInterfaceVersionFromSwapChain(IDXGISwapChain* swapChain);

	extern std::string GetDescriptionFromSwapChain(IDXGISwapChain* swapChain);

	extern std::string GetDescriptionFromDevice(IDirect3DDevice9* device);

	extern void ContextCopyResource(ID3D11Texture2D* dst, ID3D11Texture2D* src, const CHAR* msgHead = "", bool bCheckContext = false);

	class Direct3D11Helper
	{
		EDirect3D				GIVer;

		ID3D11Device*			Device;
		ID3D11DeviceContext*	Context;
		ID3D11Texture2D*		Buffer;
		ID3D11Texture2D*		Buffer_Direct9Copy;
		IDXGISwapChain*			SwapChain;

		ID3D11RasterizerState*	DefaultRasterizer;
		ID3D11VertexShader*		DefaultVS;
		ID3D11PixelShader*		DefaultPS;
		ID3D11InputLayout*		DefaultInputLayout;

		ID3D11Buffer*			CursorVB;
		ID3D11Buffer*			CursorIB;
		ID3D11Buffer*			CursorCB;
		ID3D11PixelShader*		CursorPS;

		ID3D11BlendState*		BlendState_Add_RGB;
		ID3D11BlendState*		BlendState_Blend_RGB;

		void ZeroRHI()
		{
			Buffer = nullptr;
			Buffer_Direct9Copy = nullptr;
			SwapChain = nullptr;
			Context = nullptr;
			Device = nullptr;

			DefaultRasterizer = nullptr;
			DefaultVS = nullptr;
			DefaultPS = nullptr;
			DefaultInputLayout = nullptr;

			CursorVB = nullptr;
			CursorIB = nullptr;
			CursorCB = nullptr;
			CursorPS = nullptr;
			BlendState_Add_RGB = nullptr;
			BlendState_Blend_RGB = nullptr;
		}

		void DestroyRHI()
		{
			SAFE_RELEASE(Buffer);
			SAFE_RELEASE(Buffer_Direct9Copy);
			SAFE_RELEASE(SwapChain);
			SAFE_RELEASE(Context);
			SAFE_RELEASE(Device);

			SAFE_RELEASE(DefaultRasterizer);
			SAFE_RELEASE(DefaultVS);
			SAFE_RELEASE(DefaultPS);
			SAFE_RELEASE(DefaultInputLayout);

			SAFE_RELEASE(CursorVB);
			SAFE_RELEASE(CursorIB);
			SAFE_RELEASE(CursorCB);
			SAFE_RELEASE(CursorPS);
			SAFE_RELEASE(BlendState_Add_RGB);
			SAFE_RELEASE(BlendState_Blend_RGB);
		}

		bool InitializeRHI(UINT width = 0, UINT height = 0, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);

	public:
		bool bRestoreState;
		bool bEnableDebugWindow;

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

		bool UpdateRHIWithDevice(ID3D11Device* device);
		bool UpdateRHIWithSwapChain(IDXGISwapChain* swapChain);
		bool UpdateBuffer_Direct3D9(IDirect3DDevice9* device);
		bool OutputBuffer_Texture2D_Direct3D9(ID3D11Texture2D* dst);

		ID3D11Device* GetDevice();
		ID3D11DeviceContext* GetContext();
		IDXGISwapChain* GetSwapChain();
		ID3D11Texture2D* GetSwapChainBuffer();
		ID3D11Texture2D* GetBufferDirect3D9Copy();
		bool GetSwapChainData(UINT& width, UINT& height, DXGI_FORMAT& format);
		EDirect3D GetGraphicsInterfaceVersion();

		//************************************
		// Method:    GetTemporaryDirect9Device 创建临时Direct9设备对象
		// FullName:  lostvr::Direct3D11Helper::GetTemporaryDirect9Device
		// Access:    public 
		// Returns:   IDirect3DDevice9* Direct9设备对象，已AddRef
		// Qualifier: const
		//************************************
		IDirect3DDevice9* GetTemporaryDirect9Device() const;
		IDirect3DDevice9Ex* GetTemporaryDirect9DeviceEx() const;
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

		void GetDescriptionTemplate_DefaultTexture2D(D3D11_TEXTURE2D_DESC& desc);
		void GetDescriptionTemplate_DefaultBuffer(D3D11_BUFFER_DESC& desc);

		//************************************
		// Method:    CreateTexture2D 根据描述(desc)创建纹理资源
		// FullName:  lostvr::Direct3D11Helper::CreateTexture2D
		// Access:    public 
		// Returns:   bool 创建成功返回true，否则返回false
		// Qualifier:
		// Parameter: ID3D11Texture2D * * ppTex 不为null则指向创建的纹理Texture2D对象，已AddRef
		// Parameter: ID3D11ShaderResourceView * * ppSRV 不为null则指向创建的纹理的SRV对象，已AddRef
		// Parameter: D3D11_TEXTURE2D_DESC * pDesc 创建纹理的描述，参考GetDescriptionTemplate_DefaultTexture2D
		// Parameter: ID3D11Device * deviceOverride null则使用内部device对象创建，否则使用传入的device对象
		//************************************
		bool CreateTexture2D(ID3D11Texture2D** ppTex, ID3D11ShaderResourceView** ppSRV, D3D11_TEXTURE2D_DESC * pDesc, ID3D11Device* deviceOverride = nullptr);
		
		//************************************
		// Method:    CreateShaderResourceView 在纹理的Texture2D对象基础上创建SRV对象；和一般类似的接口都是用内部device创建资源不同，
		// 这个接口用传入tex的device创建资源。
		// FullName:  lostvr::Direct3D11Helper::CreateShaderResourceView
		// Access:    public 
		// Returns:   bool 创建成功返回true，否则返回false
		// Qualifier:
		// Parameter: ID3D11Texture2D * tex 必须指向有意义的纹理Texture2D对象
		// Parameter: ID3D11ShaderResourceView * * ppSRV 必须指向一个SRV指针，已AddRef
		//************************************
		bool CreateShaderResourceView(ID3D11Texture2D* tex, ID3D11ShaderResourceView** ppSRV);

		//************************************
		// Method:    CreateRenderTargetView 在纹理的Texture2D上创建RTV
		// 这个接口用传入tex的device创建资源。
		// FullName:  lostvr::Direct3D11Helper::CreateShaderResourceView
		// Access:    public 
		// Returns:   bool 创建成功返回true，否则返回false
		// Qualifier:
		// Parameter: ID3D11Texture2D * tex 纹理Texture2D对象，若为null则使用内部swap chain的Texture2D对象
		// Parameter: ID3D11RenderTargetView * * rtv 必须指向一个SRV指针，已AddRef
		//************************************
		bool CreateRenderTargetView(ID3D11Texture2D* tex, ID3D11RenderTargetView** rtv);

		//************************************
		// Method:    CreateMesh_Rect 创建一个rect的vertex/index buffer
		// FullName:  lostvr::Direct3D11Helper::CreateMesh_Rect
		// Access:    public 
		// Returns:   bool 创建成功返回true，否则返回false
		// Qualifier:
		// Parameter: float width rect宽度
		// Parameter: float height rect高度
		// Parameter: UINT vertexSizeInBytes size of single vertex data
		// Parameter: ID3D11Buffer * * vb 不为null则指向创建的vertex buffer，已AddRef
		// Parameter: ID3D11Buffer * * ib 不为null则指向创建index buffer，已AddRef
		//************************************
		bool CreateMesh_Rect(float width, float height, UINT vertexSizeInBytes, ID3D11Buffer** vb, ID3D11Buffer** ib);

		//************************************
		// Method:    CreateBuffer 创建一个ConstantBuffer
		// FullName:  lostvr::Direct3D11Helper::CreateBuffer
		// Access:    public 
		// Returns:   bool 创建成功返回true，否则返回false
		// Qualifier:
		// Parameter: D3D11_BUFFER_DESC & desc 参考GetDescriptionTemplate_DefaultBuffer
		// Parameter: ID3D11Buffer * * ppBuffer 必须指向一个buffer指针，已AddRef
		//************************************
		bool CreateBuffer(D3D11_BUFFER_DESC& desc, ID3D11Buffer** ppBuffer);

		void UpdateCursor(const LVVec3& areaSize, float scale, const LVMatrix& matView, const LVMatrix& matProj, bool bVisible);

		bool GetDefaultShader(ID3D11VertexShader** vs, ID3D11PixelShader** ps, ID3D11InputLayout** layout);
		bool CompileShader(LPCWSTR file, LPCSTR entry, LPCSTR target, ID3DBlob** blob);

		bool GetDefaultRasterizer(ID3D11RasterizerState** rs);

	protected:
		bool CreateBlendStates();
	};

	struct RenderStateCacheNull
	{
		RenderStateCacheNull(ID3D11DeviceContext* context) {}
		~RenderStateCacheNull() {}

		void Capture() {}
		void Restore() {}
	};

	struct RenderStateCache
	{
		UINT NumViewports;
		D3D11_VIEWPORT Viewport;
		ID3D11RasterizerState* RS;
		ID3D11Buffer *CB, *VB, *IB;
		UINT VBStride, VBOffset;
		DXGI_FORMAT IBFormat;
		UINT IBOffset;
		D3D11_PRIMITIVE_TOPOLOGY Topology;
		ID3D11InputLayout* InputLayout;
		ID3D11RenderTargetView* RTV;
		ID3D11DepthStencilView* DSV;
		ID3D11VertexShader* VS;
		ID3D11PixelShader* PS;

		ID3D11DeviceContext* ContextRef;

		RenderStateCache(ID3D11DeviceContext* context);
		~RenderStateCache();

		void Capture();
		void Restore();
	};
}