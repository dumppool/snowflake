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
		std::map<uint32, TRefCountPtr<ID3D11SamplerState>> StateMap;

		FSamplerStateMap();
		~FSamplerStateMap();

		void InitializeState(uint32 flags);
		void Initialize();
		void ReleaseComObjects();
		TRefCountPtr<ID3D11SamplerState> GetState(uint32 flags);
	};
}