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
	struct FDepthStencilStateMap
	{
		static FDepthStencilStateMap* Get()
		{
			static FDepthStencilStateMap Inst;
			return &Inst;
		}

		bool bInitialized;
		map<uint32, TRefCountPtr<ID3D11DepthStencilState>> StateMap;

		FDepthStencilStateMap();
		~FDepthStencilStateMap();

		void InitializeState(uint32 flags);
		void Initialize();
		void ReleaseComObjects();
		TRefCountPtr<ID3D11DepthStencilState> GetState(uint32 flags);
	};
}