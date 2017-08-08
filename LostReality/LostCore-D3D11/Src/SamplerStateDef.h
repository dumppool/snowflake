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
		FORCEINLINE static std::string GetName()
		{
			static std::string SName = "DEFAULT";
			return SName;
		}

		FORCEINLINE static TRefCountPtr<ID3D11SamplerState> GetState(
			const TRefCountPtr<ID3D11Device>& device)
		{
			TRefCountPtr<ID3D11SamplerState> state;

			if (device.IsValid())
			{
				CD3D11_SAMPLER_DESC desc(D3D11_DEFAULT);
				HRESULT hr = device->CreateSamplerState(&desc, state.GetInitReference());
				if (FAILED(hr))
				{
					LVERR("FSamplerState_0::GetState", "create sampler state failed: 0x%08x(%d)", hr, hr);
				}
			}

			return state;
		}
	};

	struct FSamplerStateMap
	{
		static FSamplerStateMap* Get()
		{
			static FSamplerStateMap Inst;
			return &Inst;
		}

		bool bInitialized;
		std::map<std::string, TRefCountPtr<ID3D11SamplerState>> StateMap;

		FSamplerStateMap() : bInitialized(false)
		{
		}

		~FSamplerStateMap()
		{
			ReleaseComObjects();
		}

		void Initialize(const TRefCountPtr<ID3D11Device>& device)
		{
			if (bInitialized)
			{
				return;
			}

			const char* head = "FSamplerStateMap::Initialize";
			StateMap.insert(std::make_pair(FSamplerState_0::GetName(), FSamplerState_0::GetState(device)));
			LVMSG(head, "insert SamplerState[%s]", FSamplerState_0::GetName().c_str());

			bInitialized = true;
		}

		void ReleaseComObjects()
		{
			if (!bInitialized)
			{
				return;
			}

			for (auto& element : StateMap)
			{
				element.second = nullptr;
			}

			StateMap.clear();
			bInitialized = false;
		}

		FORCEINLINE TRefCountPtr<ID3D11SamplerState> GetState(const std::string& key)
		{
			if (bInitialized)
			{
				for (auto& element : StateMap)
				{
					if (element.first.compare(key) == 0)
					{
						return element.second;
					}
				}
			}

			return TRefCountPtr<ID3D11SamplerState>();
		}
	};
}