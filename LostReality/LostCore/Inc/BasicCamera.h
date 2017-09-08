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

		virtual bool Config(const FJson& config) override;
		virtual bool Load(const char* url) override;
		virtual void Tick() override;
		virtual void Draw() override;

		virtual void AddPositionWorld(const FFloat3& pos);
		virtual void AddEulerWorld(const FFloat3& euler);
		virtual void AddPositionLocal(const FFloat3& pos);
		virtual void AddEulerLocal(const FFloat3& euler);

		virtual void SetNearPlane(float value);
		virtual float GetNearPlane() const;

		virtual void SetFarPlane(float value);
		virtual float GetFarPlane() const;

		virtual void SetFov(float fov);
		virtual float GetFov() const;

		virtual void SetAspectRatio(float ratio);
		virtual float GetAspectRatio() const;
		
		virtual FFloat4x4 GetViewMatrix() const;
		virtual FFloat4x4 GetProjectMatrix() const;
		virtual FFloat4x4 GetViewProjectMatrix() const;

	private:
		float			NearPlane;
		float			FarPlane;
		float			Fov;
		float			AspectRatio;
		FFloat3			ViewEuler;
		FFloat3			ViewPosition;
	};
}