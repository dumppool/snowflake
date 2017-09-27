/*
* file BlendStateDef.h
*
* author luoxw
* date 2017/03/02
*
*
*/

#pragma once

namespace D3D11
{
	struct FBlendState_Solid
	{
		FORCEINLINE static std::string GetName()
		{
			static std::string SName = "SOLID";
			return SName;
		}

		FORCEINLINE static TRefCountPtr<ID3D11BlendState> GetState(const TRefCountPtr<ID3D11Device>& device)
		{
			TRefCountPtr<ID3D11BlendState> state;

			if (device.IsValid())
			{
				D3D11_BLEND_DESC desc;
				ZeroMemory(&desc, sizeof(D3D11_BLEND_DESC));
				desc.AlphaToCoverageEnable = FALSE;
				desc.IndependentBlendEnable = FALSE;
				desc.RenderTarget[0].BlendEnable = FALSE;
				desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
				HRESULT hr = device->CreateBlendState(&desc, state.GetInitReference());
				if (FAILED(hr))
				{
					LVERR("FBlendState_Solid::GetState", "create blend state failed: 0x%08x(%d)", hr, hr);
				}
			}

			return state;
		}
	};

	struct FBlendState_Add
	{
		FORCEINLINE static std::string GetName()
		{
			static std::string SName = "ADD";
			return SName;
		}

		FORCEINLINE static TRefCountPtr<ID3D11BlendState> GetState(const TRefCountPtr<ID3D11Device>& device)
		{
			TRefCountPtr<ID3D11BlendState> state;

			if (device.IsValid())
			{
				D3D11_BLEND_DESC desc;
				ZeroMemory(&desc, sizeof(D3D11_BLEND_DESC));
				desc.AlphaToCoverageEnable = TRUE;
				desc.IndependentBlendEnable = FALSE;
				desc.RenderTarget[0].BlendEnable = TRUE;
				desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
				desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
				desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
				desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
				desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
				desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
				desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
				HRESULT hr = device->CreateBlendState(&desc, state.GetInitReference());
				if (FAILED(hr))
				{
					LVERR("FBlendState_Add::GetState", "create blend state failed: 0x%08x(%d)", hr, hr);
				}
			}

			return state;
		}
	};

	struct FBlendStateMap
	{
		static FBlendStateMap* Get()
		{
			static FBlendStateMap Inst;
			return &Inst;
		}

		bool bInitialized;
		std::map<std::string, TRefCountPtr<ID3D11BlendState>> StateMap;

		FBlendStateMap();
		~FBlendStateMap();

		void InitializeState(uint32 overallFlags, const array<uint32, 8>& flags);
		void Initialize();
		void ReleaseComObjects();
		FORCEINLINE TRefCountPtr<ID3D11BlendState> GetState(uint32 flags);
	};
}