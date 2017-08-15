/*
* file GlobalHandler.h
*
* author luoxw
* date 2017/08/14
*
*
*/

#pragma once

namespace LostCore
{
	class FGlobalHandler
	{
		bool bDisplayNormal;
		bool bDisplayTangent;

		float DisplayNormalLength;

		typedef function<void(float, float, float)> FCameraCallback;
		FCameraCallback MoveCameraCallback;
		FCameraCallback RotateCameraCallback;

	public:
		static FGlobalHandler* Get()
		{
			static FGlobalHandler Inst;
			return &Inst;
		}

	public:
		FGlobalHandler();
		
		void SetDisplayNormal(bool enable);
		bool GetDisplayNormal() const;

		void SetDisplayTangent(bool enable);
		bool GetDisplayTangent() const;

		void SetDisplayNormalLength(float value);
		float GetDisplayNormalLength() const;

		void MoveCamera(float x, float y, float z);
		void RotateCamera(float p, float y, float r);

	public:
		bool IsDisplayNormal(uint32 flags) const;

		// 显示切空间坐标
		// 显示切空间坐标同时不单独显示法线.
		bool IsDisplayTangent(uint32 flags) const;

		void SetMoveCameraCallback(FCameraCallback callback);
		void SetRotateCameraCallback(FCameraCallback callback);
	};

	LOSTCORE_API void SetDisplayNormal(bool enable);
	static TExportFuncWrapper<bool> WrappedSetDisplayNormal("SetDisplayNormal", GetModule_LostCore);

	LOSTCORE_API void SetDisplayTangent(bool enable);
	static TExportFuncWrapper<bool> WrappedSetDisplayTangent("SetDisplayTangent", GetModule_LostCore);

	LOSTCORE_API void SetDisplayNormalLength(float value);
	static TExportFuncWrapper<float> WrappedSetDisplayNormalLength("SetDisplayNormalLength", GetModule_LostCore);

	LOSTCORE_API void MoveCamera(float x, float y, float z);
	static TExportFuncWrapper<float, float, float> WrappedMoveCamera("MoveCamera", GetModule_LostCore);

	LOSTCORE_API void RotateCamera(float p, float y, float r);
	static TExportFuncWrapper<float, float, float> WrappedRotateCamera("RotateCamera", GetModule_LostCore);

}