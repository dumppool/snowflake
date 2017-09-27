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
	struct FRasterizerStateMap
	{
		static FRasterizerStateMap* Get()
		{
			static FRasterizerStateMap Inst;
			return &Inst;
		}

		bool bInitialized;
		map<uint32, TRefCountPtr<ID3D11RasterizerState>> StateMap;

		FRasterizerStateMap();
		~FRasterizerStateMap();

		void InitializeState(uint32 flags);
		void Initialize();
		void ReleaseComObjects();
		TRefCountPtr<ID3D11RasterizerState> GetState(uint32 flags);
	};
}