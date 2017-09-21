/*
* file RasterizerStateDef.h
*
* author luoxw
* date 2017/03/02
*
*
*/

#pragma once

namespace D3D11
{
	struct FRasterizerState_Solid
	{
		FORCEINLINE static std::string GetName()
		{
			static std::string SName = "SOLID";
			return SName;
		}

		FORCEINLINE static TRefCountPtr<ID3D11RasterizerState> GetState(const TRefCountPtr<ID3D11Device>& device)
		{
			TRefCountPtr<ID3D11RasterizerState> state;

			if (device.IsValid())
			{
				D3D11_RASTERIZER_DESC desc;
				ZeroMemory(&desc, sizeof(desc));
				desc.AntialiasedLineEnable = FALSE;
				//desc.CullMode = D3D11_CULL_NONE;
				desc.CullMode = D3D11_CULL_BACK;
				desc.DepthBias = 0;
				desc.DepthBiasClamp = 0.0f;
				desc.DepthClipEnable = TRUE;
				desc.FillMode = D3D11_FILL_SOLID;
				desc.FrontCounterClockwise = FALSE;
				desc.MultisampleEnable = FALSE;
				desc.ScissorEnable = FALSE;
				desc.SlopeScaledDepthBias = 0.0f;
				HRESULT hr = device->CreateRasterizerState(&desc, state.GetInitReference());
				if (FAILED(hr))
				{
					LVERR("FRasterizerState_Solid::GetState", "create rasterizer state failed: 0x%08x(%d)", hr, hr);
				}
			}

			return state;
		}
	};

	struct FRasterizerState_Wireframe
	{
		FORCEINLINE static std::string GetName()
		{
			static std::string SName = "WIREFRAME";
			return SName;
		}

		FORCEINLINE static TRefCountPtr<ID3D11RasterizerState> GetState(const TRefCountPtr<ID3D11Device>& device)
		{
			TRefCountPtr<ID3D11RasterizerState> state;

			if (device.IsValid())
			{
				D3D11_RASTERIZER_DESC desc;
				ZeroMemory(&desc, sizeof(desc));
				desc.AntialiasedLineEnable = FALSE;
				desc.CullMode = D3D11_CULL_NONE;
				desc.DepthBias = 0;
				desc.DepthBiasClamp = 0.0f;
				desc.DepthClipEnable = FALSE;
				desc.FillMode = D3D11_FILL_WIREFRAME;
				desc.FrontCounterClockwise = FALSE;
				desc.MultisampleEnable = FALSE;
				desc.ScissorEnable = FALSE;
				desc.SlopeScaledDepthBias = 0.0f;
				HRESULT hr = device->CreateRasterizerState(&desc, state.GetInitReference());
				if (FAILED(hr))
				{
					LVERR("FRasterizerState_Wireframe::GetState", "create rasterizer state failed: 0x%08x(%d)", hr, hr);
				}
			}

			return state;
		}
	};

	struct FRasterizerStateMap
	{
		static FRasterizerStateMap* Get()
		{
			static FRasterizerStateMap Inst;
			return &Inst;
		}

		bool bInitialized;
		std::map<std::string, TRefCountPtr<ID3D11RasterizerState>> StateMap;

		FRasterizerStateMap() : bInitialized(false)
		{
		}

		~FRasterizerStateMap()
		{
			ReleaseComObjects();
		}

		void Initialize(const TRefCountPtr<ID3D11Device>& device)
		{
			if (bInitialized)
			{
				return;
			}

			StateMap.insert(std::make_pair(FRasterizerState_Solid::GetName(), FRasterizerState_Solid::GetState(device)));
			StateMap.insert(std::make_pair(FRasterizerState_Wireframe::GetName(), FRasterizerState_Wireframe::GetState(device)));
			bInitialized = true;
		}

		void ReleaseComObjects()
		{
			if (!bInitialized)
			{
				return;
			}

			for (auto& elem : StateMap)
			{
				elem.second = nullptr;
			}

			StateMap.clear();
			bInitialized = false;
		}

		FORCEINLINE TRefCountPtr<ID3D11RasterizerState> GetState(const std::string& key)
		{
			if (bInitialized)
			{
				for (auto element : StateMap)
				{
					if (element.first.compare(key) == 0)
					{
						return element.second;
					}
				}
			}

			return TRefCountPtr<ID3D11RasterizerState>();
		}
	};
}