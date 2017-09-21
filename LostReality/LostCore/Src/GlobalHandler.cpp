// LostCore.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "GlobalHandler.h"

using namespace LostCore;

LostCore::FGlobalHandler::FGlobalHandler()
	: RenderContextPP(nullptr)
	, DisplayNormalLength(5.0f)
	, MoveCameraCallback(nullptr)
	, RotateCameraCallback(nullptr)
	, AnimateRate(1.0f)
	, LoggingFunc(nullptr)
	, InitializeWindowCallback(nullptr)
	, LoadFBXCallback(nullptr)
	, ClearSceneCallback(nullptr)
	, PickingCallback(nullptr)
	, ShutdownCallback(nullptr)
	, UpdateFlagAndNameFunc(nullptr)
	, UpdatePosAndRotFunc(nullptr)
	, AssetOperateCallback(nullptr)
{
}

EReturnCode LostCore::FGlobalHandler::SetDisplayNormalLength(float value)
{
	DisplayNormalLength = value;
	return SSuccess;
}

float LostCore::FGlobalHandler::GetDisplayNormalLength() const
{
	return DisplayNormalLength;
}

EReturnCode LostCore::FGlobalHandler::MoveCamera(float x, float y, float z)
{
	if (MoveCameraCallback != nullptr)
	{
		MoveCameraCallback(x, y, z);
		return SSuccess;
	}
	else
	{
		return SErrorNotImplemented;
	}
}

EReturnCode LostCore::FGlobalHandler::RotateCamera(float p, float y, float r)
{
	if (RotateCameraCallback != nullptr)
	{
		RotateCameraCallback(p, y, r);
		return SSuccess;
	}
	else
	{
		return SErrorNotImplemented;
	}
}

EReturnCode LostCore::FGlobalHandler::SetAnimateRate(float rate)
{
	AnimateRate = rate;

	return SSuccess;
}

float LostCore::FGlobalHandler::GetAnimateRate() const
{
	return AnimateRate;
}

EReturnCode LostCore::FGlobalHandler::SetDisplayFlags(uint32 flags)
{
	DisplayFlags = flags;

	return SSuccess;
}

uint32 LostCore::FGlobalHandler::GetDisplayFlags() const
{
	return DisplayFlags;
}

EReturnCode LostCore::FGlobalHandler::SetUpdateFlagAndString(PFN_UpdateFlagAndString animUpdate)
{
	UpdateFlagAndNameFunc = animUpdate;

	return SSuccess;
}

EReturnCode LostCore::FGlobalHandler::PlayAnimation(const char * anim)
{
	if (PlayAnimationCallback != nullptr)
	{
		PlayAnimationCallback(anim);
		return SSuccess;
	}
	else
	{
		return SErrorNotImplemented;
	}
}

EReturnCode LostCore::FGlobalHandler::SetLogger(PFN_Logger logger)
{
	LoggingFunc = logger;

	return SSuccess;
}

EReturnCode LostCore::FGlobalHandler::InitializeWindow(HWND wnd, bool windowed, int32 width, int32 height)
{
	if (InitializeWindowCallback != nullptr)
	{
		InitializeWindowCallback(wnd, windowed, width, height);
		return SSuccess;
	}
	else
	{
		return SErrorNotImplemented;
	}
}

EReturnCode LostCore::FGlobalHandler::LoadFBX(const char * url,
	const char * primitiveOutput,
	const char * animationOutput,
	bool clearScene, 
	bool importTexCoord,
	bool importAnimation, 
	bool importVertexColor,
	bool mergeNormalTangentAll,
	bool importNormal,
	bool forceRegenerateNormal, 
	bool generateNormalIfNotFound, 
	bool importTangent,
	bool forceRegenerateTangent, 
	bool generateTangentIfNotFound)
{
	if (LoadFBXCallback != nullptr)
	{
		LoadFBXCallback(url,
			primitiveOutput,
			animationOutput,
			clearScene,
			importTexCoord,
			importAnimation,
			importVertexColor,
			mergeNormalTangentAll,
			importNormal,
			forceRegenerateNormal,
			generateNormalIfNotFound,
			importTangent,
			forceRegenerateTangent,
			generateTangentIfNotFound);
		return SSuccess;
	}
	else
	{
		return SErrorNotImplemented;
	}
}

EReturnCode LostCore::FGlobalHandler::ClearScene()
{
	if (ClearSceneCallback != nullptr)
	{
		ClearSceneCallback();
		return SSuccess;
	}
	else
	{
		return SErrorNotImplemented;
	}
}

EReturnCode LostCore::FGlobalHandler::Shutdown()
{
	if (ShutdownCallback != nullptr)
	{
		ShutdownCallback();
		return SSuccess;
	}
	else
	{
		return SErrorNotImplemented;
	}
}

EReturnCode LostCore::FGlobalHandler::SetUpdatePosAndRot(PFN_UpdatePosAndRot pfn)
{
	UpdatePosAndRotFunc = pfn;
	return SSuccess;
}

EReturnCode LostCore::FGlobalHandler::OnHover(int32 x, int32 y)
{
	if (HoverCallback != nullptr)
	{
		HoverCallback(x, y);
		return SSuccess;
	}
	else
	{
		return SErrorNotImplemented;
	}
}

EReturnCode LostCore::FGlobalHandler::OnClick(int32 x, int32 y)
{
	if (ClickCallback != nullptr)
	{
		ClickCallback(x, y);
		return SSuccess;
	}
	else
	{
		return SErrorNotImplemented;
	}
}

EReturnCode LostCore::FGlobalHandler::OnDragging(int32 x, int32 y)
{
	if (DraggingCallback != nullptr)
	{
		DraggingCallback(x, y);
		return SSuccess;
	}
	else
	{
		return SErrorNotImplemented;
	}
}

EReturnCode LostCore::FGlobalHandler::OnEndDrag()
{
	if (EndDragCallback != nullptr)
	{
		EndDragCallback();
		return SSuccess;
	}
	else
	{
		return SErrorNotImplemented;
	}
}

EReturnCode LostCore::FGlobalHandler::AssetOperate(int32 op, const char * url)
{
	if (AssetOperateCallback != nullptr)
	{
		AssetOperateCallback(op, url);
		return SSuccess;
	}
	else
	{
		return SErrorNotImplemented;
	}
}

void LostCore::FGlobalHandler::SetRenderContextPP(IRenderContext ** rc)
{
	RenderContextPP = rc;
}

IRenderContext * LostCore::FGlobalHandler::GetRenderContext() const
{
	if (RenderContextPP != nullptr)
	{
		return *RenderContextPP;
	}
	else
	{
		return nullptr;
	}
}

void LostCore::FGlobalHandler::SetFrameTime(float sec)
{
	FrameTime = sec;
}

float LostCore::FGlobalHandler::GetFrameTime() const
{
	return FrameTime;
}

void LostCore::FGlobalHandler::SetMoveCameraCallback(Callback_FFF callback)
{
	MoveCameraCallback = callback;
}

void LostCore::FGlobalHandler::SetRotateCameraCallback(Callback_FFF callback)
{
	RotateCameraCallback = callback;
}

bool LostCore::FGlobalHandler::IsDisplay(uint32 flag) const
{
	return (DisplayFlags & flag) == flag;
}

void LostCore::FGlobalHandler::SetPlayAnimationCallback(Callback_S callback)
{
	PlayAnimationCallback = callback;
}

void LostCore::FGlobalHandler::Logging(int32 level, const string & msg)
{
	if (LoggingFunc != nullptr)
	{
		LoggingFunc(level, msg.c_str());
	}
}

void LostCore::FGlobalHandler::SetInitializeWindowCallback(Callback_HBII callback)
{
	InitializeWindowCallback = callback;
}

void LostCore::FGlobalHandler::SetLoadFBXCallback(Callback_X callback)
{
	LoadFBXCallback = callback;
}

void LostCore::FGlobalHandler::SetClearSceneCallback(Callback_V callback)
{
	ClearSceneCallback = callback;
}

void LostCore::FGlobalHandler::SetShutdownCallback(Callback_V callback)
{
	ShutdownCallback = callback;
}

void LostCore::FGlobalHandler::SetHoverCallback(Callback_II callback)
{
	HoverCallback = callback;
}

void LostCore::FGlobalHandler::SetClickCallback(Callback_II callback)
{
	ClickCallback = callback;
}

void LostCore::FGlobalHandler::SetDraggingCallback(Callback_II callback)
{
	DraggingCallback = callback;
}

void LostCore::FGlobalHandler::SetEndDragCallback(Callback_V callback)
{
	EndDragCallback = callback;
}

void LostCore::FGlobalHandler::SetAssetOperateCallback(Callback_IS callback)
{
	AssetOperateCallback = callback;
}

void LostCore::FGlobalHandler::UpdateFlagAndName(EUpdateFlag flag, const string & name)
{
	if (UpdateFlagAndNameFunc != nullptr)
	{
		UpdateFlagAndNameFunc((uint32)flag, name.c_str());
	}
}

void LostCore::FGlobalHandler::UpdatePosAndRot(const FFloat3 & pos, const FFloat3 & rot)
{
	if (UpdatePosAndRotFunc != nullptr)
	{
		UpdatePosAndRotFunc(pos.X, pos.Y, pos.Z, rot.Pitch, rot.Yaw, rot.Roll);
	}
}

#define EXPORTER_PTR (LostCore::FGlobalHandler::Get())

EXPORT_WRAP_1_DEF(SetDisplayNormalLength, float);
EXPORT_WRAP_3_DEF(MoveCamera, float, float, float); // x, y, z
EXPORT_WRAP_3_DEF(RotateCamera, float, float, float); // pitch, yaw, roll
EXPORT_WRAP_1_DEF(SetAnimateRate, float);
EXPORT_WRAP_1_DEF(SetDisplayFlags, uint32);
EXPORT_WRAP_1_DEF(SetUpdateFlagAndString, LostCore::PFN_UpdateFlagAndString);
EXPORT_WRAP_1_DEF(PlayAnimation, const char*);
EXPORT_WRAP_1_DEF(SetLogger, LostCore::PFN_Logger);
EXPORT_WRAP_4_DEF(InitializeWindow, HWND, bool, int32, int32);
EXPORT_WRAP_14_DEF(LoadFBX, const char*, const char*, const char*, bool, bool, bool, bool, bool, bool, bool, bool, bool, bool, bool);
EXPORT_WRAP_0_DEF(ClearScene);
EXPORT_WRAP_0_DEF(Shutdown);
EXPORT_WRAP_1_DEF(SetUpdatePosAndRot, LostCore::PFN_UpdatePosAndRot);
EXPORT_WRAP_2_DEF(OnHover, int32, int32);
EXPORT_WRAP_2_DEF(OnClick, int32, int32);
EXPORT_WRAP_2_DEF(OnDragging, int32, int32);
EXPORT_WRAP_0_DEF(OnEndDrag);
EXPORT_WRAP_2_DEF(AssetOperate, int32, const char*);

#undef EXPORTER_PTR