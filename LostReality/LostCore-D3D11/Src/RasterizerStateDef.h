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
		INLINE static std::string GetName()
		{
			static std::string SName = "Solid";
			return SName;
		}

		INLINE static TRefCountPtr<ID3D11RasterizerState> GetDesc(const TRefCountPtr<ID3D11Device>& device = nullptr, bool bDestroy = false)
		{
			static TRefCountPtr<ID3D11RasterizerState> SState;
			static bool SCreated = false;

			if (device.IsValid() && !SCreated)
			{
				SCreated = true;
				D3D11_RASTERIZER_DESC desc;
				ZeroMemory(&desc, sizeof(desc));
				desc.AntialiasedLineEnable = FALSE;
				desc.CullMode = D3D11_CULL_NONE;
				desc.DepthBias = 0;
				desc.DepthBiasClamp = 0.0f;
				desc.DepthClipEnable = FALSE;
				desc.FillMode = D3D11_FILL_SOLID;
				desc.FrontCounterClockwise = FALSE;
				desc.MultisampleEnable = FALSE;
				desc.ScissorEnable = FALSE;
				desc.SlopeScaledDepthBias = 0.0f;
				HRESULT hr = device->CreateRasterizerState(&desc, SState.GetInitReference());
				if (FAILED(hr))
				{
					LVERR("FRasterizerState_Solid::GetDesc", "create rasterizer state failed: 0x%08x(%d)", hr, hr);
				}
			}

			if (bDestroy)
			{
				SState = nullptr;
				SCreated = false;
			}

			return SState;
		}
	};

	struct FRasterizerState_Wireframe
	{
		INLINE static std::string GetName()
		{
			static std::string SName = "Solid";
			return SName;
		}

		INLINE static TRefCountPtr<ID3D11RasterizerState> GetDesc(const TRefCountPtr<ID3D11Device>& device = nullptr, bool bDestroy = false)
		{
			static TRefCountPtr<ID3D11RasterizerState> SState;
			static bool SCreated = false;

			if (device.IsValid() && !SCreated)
			{
				SCreated = true;
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
				HRESULT hr = device->CreateRasterizerState(&desc, SState.GetInitReference());
				if (FAILED(hr))
				{
					LVERR("FRasterizerState_Wireframe::GetDesc", "create rasterizer state failed: 0x%08x(%d)", hr, hr);
				}
			}

			if (bDestroy)
			{
				SState = nullptr;
				SCreated = false;
			}

			return SState;
		}
	};

	struct FRasterizerStateMap
	{
		static FRasterizerStateMap* Get()
		{
			static FRasterizerStateMap Inst;
			return &Inst;
		}

		std::map<std::string, TRefCountPtr<ID3D11RasterizerState>> StateMap;

		void Initialize(const TRefCountPtr<ID3D11Device>& device)
		{
			StateMap.insert(std::make_pair(FRasterizerState_Solid::GetName(), FRasterizerState_Solid::GetDesc(device)));
			StateMap.insert(std::make_pair(FRasterizerState_Wireframe::GetName(), FRasterizerState_Wireframe::GetDesc(device)));
		}

		void ReleaseComObjects()
		{
			bool bDestroy = true;
			FRasterizerState_Solid::GetDesc(nullptr, bDestroy);
			FRasterizerState_Wireframe::GetDesc(nullptr, bDestroy);

			StateMap.clear();
		}

		INLINE TRefCountPtr<ID3D11RasterizerState> GetDesc(const std::string& key)
		{
			for (auto element : StateMap)
			{
				if (element.first.compare(key) == 0)
				{
					return element.second;
				}
			}

			return TRefCountPtr<ID3D11RasterizerState>();
		}
	};
}