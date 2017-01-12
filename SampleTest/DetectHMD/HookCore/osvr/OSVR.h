/*
 * file OSVR.h
 *
 * author luoxw
 * date 2017/01/09
 *
 * There is no c++ & direct3d sample for osvr on Internet, and most of
 * code comes from unreal engine osvr plugin.
 * 
 * Thank unreal engine osvr plugin's authors.
 *
 */
#pragma once

#include <osvr/ClientKit/ClientKitC.h>
//#include "osvr-lib/include/osvr/ClientKit/Display.h"
#include <osvr/ClientKit/DisplayC.h>
#include <osvr/RenderKit/RenderManagerD3D11C.h>
#include "renderers/Projector.h"
#include "renderers/IRenderer.h"

namespace lostvr 
{
	struct FDescriptionData
	{
		LVVec2 DisplaySize[2];
		LVVec2 Fov[2];

		FDescriptionData()
		{
			// Set defaults...
			for (int i = 0; i < 2; ++i)
			{
				DisplaySize[i].x = 960;
				DisplaySize[i].y = 1080;
				Fov[i].x = 90;
				Fov[i].y = 101.25f;
			}
		}
	};

	class FOSVRDescription
	{
	public:
		FOSVRDescription();

		~FOSVRDescription();

		bool Init(OSVR_ClientContext OSVRClientContext, OSVR_DisplayConfig displayConfig);
		bool IsValid() const;
		LVVec2 GetDisplaySize(EnumEyeID eye) const;
		LVVec2 GetFov(EnumEyeID eye) const;
		//LVVec3 GetLocation(EnumEyeID Eye) const;
		LVMatrix GetProjectionMatrix(float left, float right, float bottom, float top, float nearClip, float farClip) const;
		LVMatrix GetProjectionMatrix(EnumEyeID eye, OSVR_DisplayConfig displayConfig, float nearClip, float farClip) const;
		LVMatrix GetViewMatrix(EnumEyeID eye) const;
		bool OSVRViewerFits(OSVR_DisplayConfig displayConfig);

		// Helper function
		// IPD    = ABS(GetLocation(LEFT_EYE).X - GetLocation(RIGHT_EYE).X);
		float GetInterpupillaryDistance() const;

		// Helper function
		// Width  = GetDisplaySize(LEFT_EYE).X + GetDisplaySize(RIGHT_EYE).X;
		// Height = MAX(GetDisplaySize(LEFT_EYE).Y, GetDisplaySize(RIGHT_EYE).Y);
		//LVVec2 GetResolution() const;

	private:
		FOSVRDescription(FOSVRDescription&);
		FOSVRDescription& operator=(FOSVRDescription&);

		bool InitIPD(OSVR_DisplayConfig displayConfig);
		bool InitDisplaySize(OSVR_DisplayConfig displayConfig);
		bool InitFOV(OSVR_DisplayConfig displayConfig);

		float m_ipd;
		bool bValid;
		FDescriptionData* Data;

	};

	class FOSVR : public IVRDevice
	{
		std::vector<HMODULE> Modules;

		std::vector<OSVR_RenderBufferD3D11> RenderBuffers;
		std::vector<OSVR_RenderInfoD3D11>	RenderInfos;
		std::vector<OSVR_ViewportDescription> RenderViewports;

		OSVR_ClientContext					ClientCxt;
		OSVR_DisplayConfig					DisplayConf;
		OSVR_RenderManagerD3D11				RenderManagerD3D11;
		OSVR_RenderManager					RenderManager;
		OSVR_RenderParams					RenderParams;

		FOSVRDescription					Description;

		BaseTextureProjector*				Projector;
		BaseTextureProjector_Direct3D9*		Projector9;

		LVQuat*			PoseOrientation;

		bool			bIsCounting;

	public:

		FOSVR(const std::string& key);
		virtual ~FOSVR();

		virtual bool Startup() override;
		virtual void Shutdown() override;
		virtual bool IsConnected() override;

		virtual bool OnPresent_Direct3D11(IDXGISwapChain* swapChain) override;
		virtual bool OnPresent_Direct3D9(IDirect3DDevice9* device) override;

		virtual const std::string GetDeviceString() const override;

		virtual void AddMovement(EMovement movement) override;

		void ProcessPose();

	protected:

		bool UpdateRenderInfos(ID3D11Device* device, ID3D11DeviceContext* context);
		bool UpdateRenderBuffers();
		bool UpdateRenderViewports();
	};
}