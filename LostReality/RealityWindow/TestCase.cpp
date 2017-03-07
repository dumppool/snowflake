#include "stdafx.h"
#include "TestCase.h"

using namespace LostCore;

//bool TESTCASE::Startup()
//{
//	D3D11::StartupModule("");
//}
//
//void TESTCASE::Shutdown()
//{
//	D3D11::ShutdownModule();
//}

bool TESTCASE::TestCase_LostCoreD3D11(HWND wnd)
{
	const char* head = "TESTCASE::TestCase_LostCoreD3D11";
	//TRefCountPtr<ID3D11Device> device;
	//TRefCountPtr<IDXGISwapChain> swapChain;
	//TRefCountPtr<IDXGISwapChain> swapChain2;
	//D3D11::WrappedCreateDevice(D3D11::EDeviceType::DXGI1, device.GetInitReference(), nullptr);
	//D3D11::WrappedCreateSwapChain(device, wnd, true, 1280, 720, swapChain.GetInitReference());
	//D3D11::WrappedCreateSwapChain(device, wnd, true, 800, 600, swapChain2.GetInitReference());

	//return device.IsValid() && swapChain.IsValid() && swapChain2.IsValid();
	return false;
}

bool TESTCASE::TestCase_LostCore_VirtualCall(HWND wnd)
{
	const char* head = "TESTCASE::TestCase_LostCore_VirtualCall";
	ITestBasic* pHuman = nullptr;
	ITestBasic* pMan = nullptr;
	ITestBasic* pMan2 = nullptr;
	WrappedCreateHuman(&pHuman);
	WrappedCreateMan(&pMan);
	WrappedCreateMan(&pMan2);

	pHuman->MemFunc1(0);
	pHuman->MemFunc2(0.f);

	pMan->MemFunc1(1);
	pMan->MemFunc2(1.f);

	pMan2->MemFunc1(2);
	pMan2->MemFunc2(2.f);

	WrappedDestroyHuman(std::forward<ITestBasic*>(pHuman));
	WrappedDestroyMan(std::forward<ITestBasic*>(pMan));
	WrappedDestroyMan(std::forward<ITestBasic*>(pMan2));

	return true;
}

TESTCASE::FRenderSample::FRenderSample() : World(nullptr), LastTick(0x0badbeef)
{
	WrappedCreateSimpleWorld(&World);
}

TESTCASE::FRenderSample::~FRenderSample()
{
	WrappedDestroySimpleWorld(std::forward<FBasicWorld*>(World));
}

bool TESTCASE::FRenderSample::Init(HWND hwnd, bool bWindowed, int32 width, int32 height)
{
	assert(LastTick == 0x0badbeef);
	return World->InitWindow("", hwnd, bWindowed, width, height);
}

void TESTCASE::FRenderSample::Fini()
{
	World->Fini();
}

void TESTCASE::FRenderSample::Tick()
{
	World->Tick(1);
	World->Draw(nullptr, 1);
}

LostCore::FBasicCamera * TESTCASE::FRenderSample::GetCamera()
{
	return World->GetCamera();
}
