/*
* file BasicCamera.h
*
* author luoxw
* date 2017/02/09
*
*
*/

#pragma once

#include "BasicInterface.h"

namespace LostCore
{
	class IRenderContext;

	enum class ECameraType : uint8
	{
		Default,
	};

	class FBasicCamera
	{
	public:
		FBasicCamera();
		virtual ~FBasicCamera();

		virtual void Init(int32 width, int32 height);
		virtual bool Config(const FJson& config);
		virtual FJson Save();
		virtual void Tick();

		void AddPositionWorld(const FFloat3& pos);
		void AddEulerWorld(const FFloat3& euler);
		void AddPositionLocal(const FFloat3& pos);
		void AddEulerLocal(const FFloat3& euler);

		FFloat3& GetViewPosition();
		FFloat3& GetViewEuler();

		void SetNearPlane(float value);
		float GetNearPlane() const;

		void SetFarPlane(float value);
		float GetFarPlane() const;

		void SetFov(float fov);
		float GetFov() const;

		void SetAspectRatio(float ratio);
		float GetAspectRatio() const;
		
		FFloat4x4 GetViewMatrix() const;
		FFloat4x4 GetProjectMatrix() const;
		FFloat4x4 GetViewProjectMatrix() const;

		// Return a ray in world space.
		// left: -Width/2, right: Width/2, bottom: -Height/2, top: Height/2
		FRay ScreenCastRay(int32 x, int32 y, float depth);

		FFloat3 ScreenToWorld(int32 x, int32 y, float depth);

	private:
		int32 ScreenWidth;
		int32 ScreenHeight;
		float RcpScreenWidth;
		float RcpScreenHeight;

		float			NearPlane;
		float			FarPlane;
		float			Fov;
		float			AspectRatio;
		FFloat3			ViewEuler;
		FFloat3			ViewPosition;
	};
}