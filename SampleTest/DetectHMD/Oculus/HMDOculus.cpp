#include "OculusPch.h"

struct UsingBasicVR : BasicVR
{
	UsingBasicVR(HINSTANCE hinst) : BasicVR(hinst, L"Using BasicVR") {}

	void MainLoop()
	{
		Layer[0] = new VRLayer(Session);

		while (HandleMessages())
		{
			ActionFromInput();
			Layer[0]->GetEyePoses();

			for (int eye = 0; eye < 2; ++eye)
			{
				Layer[0]->RenderSceneToEyeBuffer(MainCam, RoomScene, eye);
			}

			Layer[0]->PrepareLayerHeader();
			DistortAndPresent(1);
		}
	}
};

//-------------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hinst, HINSTANCE, LPSTR, int)
{
	UsingBasicVR app(hinst);
	return app.Run();
}