/*
* file DepthStencilStateDef.h
*
* author luoxw
* date 2017/03/06
*
*
*/

#pragma once

namespace D3D11
{
	struct FDepthStencilState_0
	{
		FORCEINLINE static std::string GetName()
		{
			static std::string SName = K_DEPTH_STENCIL_Z_WRITE;
			return SName;
		}

		FORCEINLINE static TRefCountPtr<ID3D11DepthStencilState> GetState(const TRefCountPtr<ID3D11Device>& device)
		{
			TRefCountPtr<ID3D11DepthStencilState> state;

			if (device.IsValid())
			{
				CD3D11_DEPTH_STENCIL_DESC desc(D3D11_DEFAULT);
				HRESULT hr = device->CreateDepthStencilState(&desc, state.GetInitReference());
				if (FAILED(hr))
				{
					LVERR("FDepthStencilState_0::GetState", "create depth stencil state failed: 0x%08x(%d)", hr, hr);
				}
			}

			return state;
		}
	};

	struct FDepthStencilState_1
	{
		FORCEINLINE static std::string GetName()
		{
			static std::string SName = K_DEPTH_STENCIL_ALWAYS;
			return SName;
		}

		FORCEINLINE static TRefCountPtr<ID3D11DepthStencilState> GetState(const TRefCountPtr<ID3D11Device>& device)
		{
			TRefCountPtr<ID3D11DepthStencilState> state;

			if (device.IsValid())
			{
				CD3D11_DEPTH_STENCIL_DESC desc(
					false,
					D3D11_DEPTH_WRITE_MASK_ZERO,
					D3D11_COMPARISON_NEVER,
					false,
					0,
					0,
					D3D11_STENCIL_OP_KEEP,
					D3D11_STENCIL_OP_KEEP,
					D3D11_STENCIL_OP_KEEP,
					D3D11_COMPARISON_NEVER,
					D3D11_STENCIL_OP_KEEP,
					D3D11_STENCIL_OP_KEEP,
					D3D11_STENCIL_OP_KEEP,
					D3D11_COMPARISON_NEVER
				);

				HRESULT hr = device->CreateDepthStencilState(&desc, state.GetInitReference());
				if (FAILED(hr))
				{
					LVERR("FDepthStencilState_1::GetState", "create depth stencil state failed: 0x%08x(%d)", hr, hr);
				}
			}

			return state;
		}
	};

	struct FDepthStencilStateMap
	{
		static FDepthStencilStateMap* Get()
		{
			static FDepthStencilStateMap Inst;
			return &Inst;
		}

		bool bInitialized;
		std::map<std::string, TRefCountPtr<ID3D11DepthStencilState>> StateMap;

		FDepthStencilStateMap() : bInitialized(false)
		{
		}

		~FDepthStencilStateMap()
		{
			ReleaseComObjects();
		}

		void Initialize(const TRefCountPtr<ID3D11Device>& device)
		{
			if (bInitialized)
			{
				return;
			}

			StateMap.insert(std::make_pair(FDepthStencilState_0::GetName(), FDepthStencilState_0::GetState(device)));
			StateMap.insert(std::make_pair(FDepthStencilState_1::GetName(), FDepthStencilState_1::GetState(device)));
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

		FORCEINLINE TRefCountPtr<ID3D11DepthStencilState> GetState(const std::string& key)
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

			return TRefCountPtr<ID3D11DepthStencilState>();
		}
	};
}