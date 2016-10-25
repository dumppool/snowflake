#include "HookCorePCH.h"

#include "MainEntry.h"
//#include "MainEntry_D3D9.h"

using namespace lostvr;

//-------------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hinst, HINSTANCE, LPSTR, int)
{
	MainApp App;
	if (App.Init(hinst, TEXT("Simple SteamVR")))
		App.Run();

	//WCHAR* path = nullptr;
	//size_t sz = 0;
	//_wdupenv_s(&path, &sz, L"SystemRoot");

	//WCHAR* envs = GetEnvironmentStrings();
	system("pause");
	
	return 0;
}