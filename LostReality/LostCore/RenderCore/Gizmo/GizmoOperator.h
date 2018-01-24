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
	class IPrimitive;
	class IMaterial;
	class FBasicModel;
	class FBasicCamera;

	class FGizmoOperator
	{
	public:
		FGizmoOperator();
		~FGizmoOperator();

		bool Load(const string& url);
		bool Config(const FJson& config);

		// 世界射线相交测试
		bool RayTest(FRay & ray, bool active = false);
		void OnDragging(int32 x, int32 y, FBasicCamera* camera);
		void EndDrag();

		void Tick();

		void SetTarget(FBasicModel* target);
		FBasicModel* GetTarget();

	private:
		void Destroy();

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
		FFloat3 LastDragEnd;
		FComp* ActivedComponent;
	};
}

