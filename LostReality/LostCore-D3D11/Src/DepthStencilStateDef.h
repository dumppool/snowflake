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
		INLINE static std::string GetName()
		{
			static std::string SName = "Z_ENABLE_WRITE";
			return SName;
		}

		INLINE static TRefCountPtr<ID3D11DepthStencilState> GetState(
			const TRefCountPtr<ID3D11Device>& device = nullptr, bool bDestroy = false)
		{
			static TRefCountPtr<ID3D11DepthStencilState> SState;
			static bool SCreated = false;

			if (device.IsValid() && !SCreated)
			{
				SCreated = true;
				CD3D11_DEPTH_STENCIL_DESC desc(D3D11_DEFAULT);
				HRESULT hr = device->CreateDepthStencilState(&desc, SState.GetInitReference());
				if (FAILED(hr))
				{
					LVERR("FDepthStencilState_0::GetState", "create depth stencil state failed: 0x%08x(%d)", hr, hr);
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

	struct FDepthStencilState_1
	{
		INLINE static std::string GetName()
		{
			static std::string SName = "ALWAYS";
			return SName;
		}

		INLINE static TRefCountPtr<ID3D11DepthStencilState> GetState(
			const TRefCountPtr<ID3D11Device>& device = nullptr, bool bDestroy = false)
		{
			static TRefCountPtr<ID3D11DepthStencilState> SState;
			static bool SCreated = false;

			if (device.IsValid() && !SCreated)
			{
				SCreated = true;
				CD3D11_DEPTH_STENCIL_DESC desc(D3D11_DEFAULT);
				//desc.StencilReadMask = 0;
				//desc.StencilWriteMask = 0;
				HRESULT hr = device->CreateDepthStencilState(&desc, SState.GetInitReference());
				if (FAILED(hr))
				{
					LVERR("FDepthStencilState_0::GetState", "create depth stencil state failed: 0x%08x(%d)", hr, hr);
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

	struct FDepthStencilStateMap
	{
		static FDepthStencilStateMap* Get()
		{
			static FDepthStencilStateMap Inst;
			return &Inst;
		}

		std::map<std::string, TRefCountPtr<ID3D11DepthStencilState>> StateMap;

		void Initialize(const TRefCountPtr<ID3D11Device>& device)
		{
			StateMap.insert(std::make_pair(FDepthStencilState_0::GetName(), FDepthStencilState_0::GetState(device)));
		}

		void ReleaseComObjects()
		{
			bool bDestroy = true;
			FDepthStencilState_0::GetState(nullptr, bDestroy);

			StateMap.clear();
		}

		INLINE TRefCountPtr<ID3D11DepthStencilState> GetState(const std::string& key)
		{
			for (auto element : StateMap)
			{
				if (element.first.compare(key) == 0)
				{
					return element.second;
				}
			}

			return TRefCountPtr<ID3D11DepthStencilState>();
		}
	};
}