/*
* file GlobalHandler.h
*
* author luoxw
* date 2017/08/14
*
*
*/

#pragma once

#define FLAG_SKEL_1 (1<<0)
#define FLAG_SKEL_2 (1<<1)

#define FLAG_ANIM_CLEAR (1<<0)
#define FLAG_ANIM_ADD (1<<1)

namespace LostCore
{
	typedef void(WINAPI *PFN_AnimUpdate)(int32 flag, const char* anim);

	class FGlobalHandler
	{

	public:
		static FGlobalHandler* Get()
		{
			static FGlobalHandler Inst;
			return &Inst;
		}

	private:
		bool bDisplayNormal;
		bool bDisplayTangent;

		float DisplayNormalLength;

		typedef function<void(float, float, float)> Callback_FFF;
		Callback_FFF MoveCameraCallback;
		Callback_FFF RotateCameraCallback;

		float AnimateRate;

		uint32 FlagDisplaySkeleton;

		typedef function<void(const char*)> Callback_S;
		Callback_S PlayAnimationCallback;

		PFN_AnimUpdate AnimUpdateFunc;

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

		void SetAnimateRate(float rate);
		float GetAnimateRate() const;
		
		void SetDisplaySkeleton(uint32 flag);
		uint32 GetDisplaySkeleton() const;

		void SetAnimUpdater(PFN_AnimUpdate animUpdate);
		void PlayAnimation(const char* anim);

	public:
		bool IsDisplayNormal(uint32 flags) const;

		// 显示切空间坐标
		// 显示切空间坐标同时不单独显示法线.
		bool IsDisplayTangent(uint32 flags) const;

		void SetMoveCameraCallback(Callback_FFF callback);
		void SetRotateCameraCallback(Callback_FFF callback);

		bool IsDisplaySkeleton(uint32 flag) const;

		void ClearAnimationList();
		void AddAnimation(const char* anim);
		void SetCallbackPlayAnimation(Callback_S playAnim);
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

	LOSTCORE_API void SetAnimateRate(float rate);
	static TExportFuncWrapper<float> WrappedSetAnimateRate("SetAnimateRate", GetModule_LostCore);

	LOSTCORE_API void SetDisplaySkeleton(bool enable);
	static TExportFuncWrapper<bool> WrappedSetDisplaySkeleton("SetDisplaySkeleton", GetModule_LostCore);

	LOSTCORE_API void SetAnimUpdater(PFN_AnimUpdate animUpdate);
	static TExportFuncWrapper<PFN_AnimUpdate> WrappedSetAnimUpdater("SetAnimUpdater", GetModule_LostCore);

	LOSTCORE_API void PlayAnimation(const char* anim);
	static TExportFuncWrapper<const char*> WrappedPlayAnimation("PlayAnimation", GetModule_LostCore);

}