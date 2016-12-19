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
		// Method:    Direct3D11Helper �����ṩ��Direct9�����ó�ʼ��swap chain
		// FullName:  lostvr::Direct3D11Helper::Direct3D11Helper
		// Access:    public 
		// Returns:   
		// Qualifier:
		// Parameter: EDirect3D ver
		// Parameter: IDirect3DDevice9 * device
		//************************************
		Direct3D11Helper(EDirect3D ver, IDirect3DDevice9* device);

		//************************************
		// Method:    Direct3D11Helper �ṩ������ĳ�ʼ����Ϣ
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
		// Method:    GetTemporaryDirect9Device ������ʱDirect9�豸����
		// FullName:  lostvr::Direct3D11Helper::GetTemporaryDirect9Device
		// Access:    public 
		// Returns:   IDirect3DDevice9* Direct9�豸������AddRef
		// Qualifier: const
		//************************************
		IDirect3DDevice9* GetTemporaryDirect9Device() const;
		IDirect3DDevice9Ex* GetTemporaryDirect9DeviceEx() const;
		bool EnsureDirect9CopyValid(D3DSURFACE_DESC surfaceDesc);

		//************************************
		// Method:    GetDirect9FormatMatch ��ȡ��Direct9��Դ��ʽƥ���DXGI��ʽ
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
		// Method:    CreateTexture2D ��������(desc)����������Դ
		// FullName:  lostvr::Direct3D11Helper::CreateTexture2D
		// Access:    public 
		// Returns:   bool �����ɹ�����true�����򷵻�false
		// Qualifier:
		// Parameter: ID3D11Texture2D * * ppTex ��Ϊnull��ָ�򴴽�������Texture2D������AddRef
		// Parameter: ID3D11ShaderResourceView * * ppSRV ��Ϊnull��ָ�򴴽��������SRV������AddRef
		// Parameter: D3D11_TEXTURE2D_DESC * pDesc ����������������ο�GetDescriptionTemplate_DefaultTexture2D
		// Parameter: ID3D11Device * deviceOverride null��ʹ���ڲ�device���󴴽�������ʹ�ô����device����
		//************************************
		bool CreateTexture2D(ID3D11Texture2D** ppTex, ID3D11ShaderResourceView** ppSRV, D3D11_TEXTURE2D_DESC * pDesc, ID3D11Device* deviceOverride = nullptr);
		
		//************************************
		// Method:    CreateShaderResourceView �������Texture2D��������ϴ���SRV���󣻺�һ�����ƵĽӿڶ������ڲ�device������Դ��ͬ��
		// ����ӿ��ô���tex��device������Դ��
		// FullName:  lostvr::Direct3D11Helper::CreateShaderResourceView
		// Access:    public 
		// Returns:   bool �����ɹ�����true�����򷵻�false
		// Qualifier:
		// Parameter: ID3D11Texture2D * tex ����ָ�������������Texture2D����
		// Parameter: ID3D11ShaderResourceView * * ppSRV ����ָ��һ��SRVָ�룬��AddRef
		//************************************
		bool CreateShaderResourceView(ID3D11Texture2D* tex, ID3D11ShaderResourceView** ppSRV);

		//************************************
		// Method:    CreateRenderTargetView �������Texture2D�ϴ���RTV
		// ����ӿ��ô���tex��device������Դ��
		// FullName:  lostvr::Direct3D11Helper::CreateShaderResourceView
		// Access:    public 
		// Returns:   bool �����ɹ�����true�����򷵻�false
		// Qualifier:
		// Parameter: ID3D11Texture2D * tex ����Texture2D������Ϊnull��ʹ���ڲ�swap chain��Texture2D����
		// Parameter: ID3D11RenderTargetView * * rtv ����ָ��һ��SRVָ�룬��AddRef
		//************************************
		bool CreateRenderTargetView(ID3D11Texture2D* tex, ID3D11RenderTargetView** rtv);

		//************************************
		// Method:    CreateMesh_Rect ����һ��rect��vertex/index buffer
		// FullName:  lostvr::Direct3D11Helper::CreateMesh_Rect
		// Access:    public 
		// Returns:   bool �����ɹ�����true�����򷵻�false
		// Qualifier:
		// Parameter: float width rect���
		// Parameter: float height rect�߶�
		// Parameter: UINT vertexSizeInBytes size of single vertex data
		// Parameter: ID3D11Buffer * * vb ��Ϊnull��ָ�򴴽���vertex buffer����AddRef
		// Parameter: ID3D11Buffer * * ib ��Ϊnull��ָ�򴴽�index buffer����AddRef
		//************************************
		bool CreateMesh_Rect(float width, float height, UINT vertexSizeInBytes, ID3D11Buffer** vb, ID3D11Buffer** ib);

		//************************************
		// Method:    CreateBuffer ����һ��ConstantBuffer
		// FullName:  lostvr::Direct3D11Helper::CreateBuffer
		// Access:    public 
		// Returns:   bool �����ɹ�����true�����򷵻�false
		// Qualifier:
		// Parameter: D3D11_BUFFER_DESC & desc �ο�GetDescriptionTemplate_DefaultBuffer
		// Parameter: ID3D11Buffer * * ppBuffer ����ָ��һ��bufferָ�룬��AddRef
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