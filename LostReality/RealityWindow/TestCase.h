#pragma once

namespace TESTCASE {
	bool TestCase_LostCoreD3D11(HWND wnd);
	bool TestCase_LostCore_VirtualCall(HWND wnd);

	class FRenderSample
	{
	public:
		FRenderSample();
		~FRenderSample();

		bool Init(HWND hwnd, bool bWindowed, int32 width, int32 height);
		void Fini();

		void Tick();

	protected:
		LostCore::FBasicWorld* World;
		double LastTick;
	};

	static FRenderSample* GetRenderSampleInstance()
	{
		static TESTCASE::FRenderSample SInst;
		return &SInst;
	}
}