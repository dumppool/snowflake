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

	class FBasicCamera : public IBasicInterface
	{
	public:
		FBasicCamera();
		virtual ~FBasicCamera() override;

		virtual bool Load(IRenderContext * rc, const char* url) override;
		virtual void Fini() override;
		virtual void Tick(float sec) override;
		virtual void Draw(IRenderContext * rc, float sec) override;

		virtual void AddPositionWorld(const FVec3& pos);
		virtual void AddEulerWorld(const FVec3& euler);
		virtual void AddPositionLocal(const FVec3& pos);
		virtual void AddEulerLocal(const FVec3& euler);

		virtual void SetNearPlane(float value);
		virtual float GetNearPlane() const;

		virtual void SetFarPlane(float value);
		virtual float GetFarPlane() const;

		virtual void SetFov(float fov);
		virtual float GetFov() const;

		virtual void SetAspectRatio(float ratio);
		virtual float GetAspectRatio() const;
		
		virtual FMatrix GetViewMatrix() const;
		virtual FMatrix GetProjectMatrix() const;
		virtual FMatrix GetViewProjectMatrix() const;

	private:
		float			NearPlane;
		float			FarPlane;
		float			Fov;
		float			AspectRatio;
		FVec3			ViewEuler;
		FVec3			ViewPosition;
	};
}