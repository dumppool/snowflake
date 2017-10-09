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
	struct FBlendStateMap
	{
		static FBlendStateMap* Get()
		{
			static FBlendStateMap Inst;
			return &Inst;
		}

		bool bInitialized;
		std::map<uint32, TRefCountPtr<ID3D11BlendState>> StateMap;

		FBlendStateMap();
		~FBlendStateMap();

		void InitializeState(uint32 flags);
		void Initialize();
		void ReleaseComObjects();
		TRefCountPtr<ID3D11BlendState> GetState(uint32 flags);
	};
}