#pragma once

namespace lostvr
{
	class LostVR
	{
	public:
		void OnPresent(IDXGISwapChain* InSwapChain);

		static LostVR* Get()
		{
			static LostVR Inst;
			return &Inst;
		}
	};
}