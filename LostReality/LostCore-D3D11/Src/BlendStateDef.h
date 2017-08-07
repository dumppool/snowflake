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
		INLINE static std::string GetName()
		{
			static std::string SName = "SOLID";
			return SName;
		}

		INLINE static TRefCountPtr<ID3D11BlendState> GetState(const TRefCountPtr<ID3D11Device>& device)
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
		INLINE static std::string GetName()
		{
			static std::string SName = "ADD";
			return SName;
		}

		INLINE static TRefCountPtr<ID3D11BlendState> GetState(const TRefCountPtr<ID3D11Device>& device)
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

		std::map<std::string, TRefCountPtr<ID3D11BlendState>> StateMap;

		void Initialize(const TRefCountPtr<ID3D11Device>& device)
		{
			const char* head = "FBlendStateMap::Initialize";

			StateMap.insert(std::make_pair(FBlendState_Solid::GetName(), FBlendState_Solid::GetState(device)));
			LVMSG(head, "Insert blend state[%s]", FBlendState_Solid::GetName().c_str());

			StateMap.insert(std::make_pair(FBlendState_Add::GetName(), FBlendState_Add::GetState(device)));
			LVMSG(head, "Insert blend state[%s]", FBlendState_Add::GetName().c_str());
		}

		void ReleaseComObjects()
		{
			bool bDestroy = true;

			for (auto& elem : StateMap)
			{
				elem.second = nullptr;
			}

			StateMap.clear();
		}

		INLINE TRefCountPtr<ID3D11BlendState> GetState(const std::string& key)
		{
			for (auto element : StateMap)
			{
				if (element.first.compare(key) == 0)
				{
					return element.second;
				}
			}

			return TRefCountPtr<ID3D11BlendState>();
		}
	};
}