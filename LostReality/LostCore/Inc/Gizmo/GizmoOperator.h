/*
* file GizmoOperator.h
*
* author luoxw
* date 2017/09/12
*
*
*/

#pragma once


namespace LostCore
{
	class IPrimitiveGroup;
	class IMaterial;
	class FBasicModel;

	class FGizmoOperator
	{
	public:
		FGizmoOperator();
		~FGizmoOperator();

		bool Load(const string& url);
		bool Config(const FJson& config);

		// 世界射线相交测试
		bool RayTest(FRay & ray, bool active = false);
		void MoveGizmo(const FRay& ray);
		void StopMove();

		void Tick();
		void Draw();

		void SetTarget(FBasicModel* target);
		FBasicModel* GetTarget();

	private:
		void Fini();

		enum class EOp : uint8
		{
			PlacerX = 0,
			PlacerY,
			PlacerZ,
			RotatorX,
			RotatorY,
			RotatorZ,
			End,
			Null = 127,
		};

		struct FComp
		{
			EOp Id;
			FFloat4x4 Local;
			FPlane Plane;
			FBasicModel* Model;

			FComp();
			void Reset();
			bool operator==(const FComp& rhs) const;
		};


		vector<FFloat4x4> PlacerLocals;
		vector<FBasicModel*> PlacerProxies;
		vector<FFloat4x4> RotatorLocals;
		vector<FBasicModel*> RotatorProxies;

		FFloat4x4 World;
		vector<FComp> Components;
		FBasicModel* Target;
		float BaseOffset;
		FComp ActivedComponent;
	};
}

