/*
* file SamplerStateDef.h
*
* author luoxw
* date 2017/04/25
*
*
*/

#pragma once


namespace D3D11
{
	struct FSamplerState_0
	{
		INLINE static std::string GetName()
		{
			static std::string SName = "DEFAULT";
			return SName;
		}

		INLINE static TRefCountPtr<ID3D11SamplerState> GetState(
			const TRefCountPtr<ID3D11Device>& device = nullptr, bool bDestroy = false)
		{
			static TRefCountPtr<ID3D11SamplerState> SState;
			static bool SCreated = false;

			if (device.IsValid() && !SCreated)
			{
				SCreated = true;
				CD3D11_SAMPLER_DESC desc(D3D11_DEFAULT);
				HRESULT hr = device->CreateSamplerState(&desc, SState.GetInitReference());
				if (FAILED(hr))
				{
					LVERR("FSamplerState_0::GetState", "create sampler state failed: 0x%08x(%d)", hr, hr);
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

	struct FSamplerStateMap
	{
		static FSamplerStateMap* Get()
		{
			static FSamplerStateMap Inst;
			return &Inst;
		}

		std::map<std::string, TRefCountPtr<ID3D11SamplerState>> StateMap;

		void Initialize(const TRefCountPtr<ID3D11Device>& device)
		{
			const char* head = "FSamplerStateMap::Initialize";
			StateMap.insert(std::make_pair(FSamplerState_0::GetName(), FSamplerState_0::GetState(device)));
			LVMSG(head, "insert SamplerState[%s]", FSamplerState_0::GetName().c_str());
		}

		void ReleaseComObjects()
		{
			bool bDestroy = true;
			FSamplerState_0::GetState(nullptr, bDestroy);

			for (auto& element : StateMap)
			{
				element.second = nullptr;
			}

			StateMap.clear();
		}

		INLINE TRefCountPtr<ID3D11SamplerState> GetState(const std::string& key)
		{
			for (auto& element : StateMap)
			{
				if (element.first.compare(key) == 0)
				{
					return element.second;
				}
			}

			return TRefCountPtr<ID3D11SamplerState>();
		}
	};
}