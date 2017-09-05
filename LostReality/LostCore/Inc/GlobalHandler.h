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

#define FLAG_LOG_INFO (1<<0)
#define FLAG_LOG_WARN (1<<1)
#define FLAG_LOG_ERROR (1<<2)

#define FLAG_ANIM_CLEAR (1<<0)
#define FLAG_ANIM_ADD (1<<1)

#define FLAG_ASSET_MODEL (1<<0)
#define FLAG_ASSET_ANIMATION (1<<1)

namespace LostCore
{
	typedef void(WINAPI *PFN_AnimUpdate)(int32 flag, const char* anim);
	typedef void(WINAPI *PFN_Logger)(int32 level, const char* msg);

	typedef function<void(float, float, float)> Callback_FFF;
	typedef function<void(const char*)> Callback_S;
	typedef function<void(HWND wnd, bool windowed, int32 width, int32 height)> Callback_HBII;
	typedef function<void(
		const char* file,
		const char* primitiveOutput,
		const char* animationOutput,
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
		bool generateTangentIfNotFound)> Callback_X;

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

		Callback_FFF MoveCameraCallback;
		Callback_FFF RotateCameraCallback;

		float AnimateRate;

		uint32 FlagDisplaySkeleton;

		Callback_S PlayAnimationCallback;

		PFN_AnimUpdate AnimUpdateFunc;

		Callback_S LoadModelCallback;
		Callback_S LoadAnimationCallback;

		PFN_Logger LoggingFunc;
		Callback_HBII InitializeWindowCallback;
		Callback_X LoadFBXCallback;

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

		void LoadAsset(uint32 flag, const char* name);

		void SetLogger(PFN_Logger logger);
		void InitializeWindow(HWND wnd, bool windowed, int32 width, int32 height);
		void LoadFBX(
			const char* file,
			const char* primitiveOutput,
			const char* animationOutput,
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
			bool generateTangentIfNotFound);

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

		void SetCallbackLoadModel(Callback_S loadModel);
		void SetCallbackLoadAnimation(Callback_S loadAnimation);

		void Logging(int32 level, const string& msg);
		void SetCallbackInitializeWindow(Callback_HBII initWin);
		void SetCallbackLoadFBX(Callback_X loadFBX);
	};

}

EXPORT_WRAP_1_DCL(SetDisplayNormal, bool);
EXPORT_WRAP_1_DCL(SetDisplayTangent, bool);
EXPORT_WRAP_1_DCL(SetDisplayNormalLength, float);
EXPORT_WRAP_3_DCL(MoveCamera, float, float, float); // x, y, z
EXPORT_WRAP_3_DCL(RotateCamera, float, float, float); // pitch, yaw, roll
EXPORT_WRAP_1_DCL(SetAnimateRate, float);
EXPORT_WRAP_1_DCL(SetDisplaySkeleton, bool);
EXPORT_WRAP_1_DCL(SetAnimUpdater, LostCore::PFN_AnimUpdate);
EXPORT_WRAP_1_DCL(PlayAnimation, const char*);
EXPORT_WRAP_2_DCL(LoadAsset, uint32, const char*);
EXPORT_WRAP_1_DCL(SetLogger, LostCore::PFN_Logger);
EXPORT_WRAP_4_DCL(InitializeWindow, HWND, bool, int32, int32);
EXPORT_WRAP_14_DCL(LoadFBX, const char*, const char*, const char*, bool, bool, bool, bool, bool, bool, bool, bool, bool, bool, bool);