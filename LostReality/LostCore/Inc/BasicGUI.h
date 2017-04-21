/*
* file BasicGUI.h
*
* author luoxw
* date 2017/04/17
*
*
*/

#pragma once

#include "BasicInterface.h"

namespace LostCore
{
	class IPrimitiveGroup;
	class IMaterial;

	class FRect
	{
		struct FParam
		{
			// 相对父面板的位置，单位，像素
			FVec2 Origin;

			// 相对父面板的缩放，(0.f, 1.f]
			float Scale;

			float Unused;

			FParam() : Origin(0.f, 0.f), Scale(1.f) { assert(sizeof(FParam) == 4 * sizeof(float)); }
		};

	public:
		FRect();

		void SetOrigin(const FVec2& origin);
		FVec2 GetOrigin() const;

		void SetScale(float val);
		float GetScale() const;

		void SetSize(const FVec2& size);
		FVec2 GetSize() const;

		//************************************
		// Method:    HitTest 点击测试
		// FullName:  lostvr::FRect::HitTest
		// Access:    public 
		// Returns:   bool 点击成功返回true，否则false
		// Parameter: const FVec2& ppos 测试目标位置，父面板空间坐标
		// Parameter: FRect** result 如果不为空，测试成功后写入被成功点击的面板
		//************************************
		bool HitTest(const FVec2& ppos, FRect** result) const;

		//************************************
		// Method:    AddChild 添加子面板，然后根据深度重新排序
		// FullName:  lostvr::FRect::AddChild
		// Access:    public 
		// Parameter: FRect* child 子面板，为空或者父面板不为空无法成功添加
		void AddChild(FRect* child);

		//************************************
		// Method:    DelChild 删除子面板
		// FullName:  lostvr::FRect::DelChild
		// Access:    public 
		// Parameter: FRect* child 子面板，为空或者父面板不为this无法成功删除
		void DelChild(FRect* child);

		void Clear();

		//************************************
		// Method:    DelChild 删除子面板
		// FullName:  lostvr::FRect::DelChild
		// Access:    public 
		// Parameter: FRect* child 子面板，为空或者父面板不为this无法成功删除
		void Draw(IRenderContext * rc, float sec);

		virtual void ReconstructPrimitive(IRenderContext * rc);

	private:
		bool HitTestPrivate(const FVec2& ppos, FRect** result) const;
		void GetLocalPosition(const FVec2& ppos, FVec2& cpos) const;
		void DrawPrivate(IRenderContext* rc, float sec);

		FParam Param;

		// 绝对尺寸，单位，像素
		FVec2 Size;

		// 深度，越小越靠前
		float Depth;

		// 父面板指针
		FRect* Parent;

		// 次序：深度从前到后，绘制时需从后到前。
		std::vector<FRect*> Children;

		IPrimitiveGroup* RectPrimitive;
		FVec2 RectPrimitiveSize;
		IMaterial* RectMaterial;
	};

	//class FBasicGUI : public IBasicInterface
	//{
	//public:
	//	FBasicGUI();
	//	virtual ~FBasicGUI() override;

	//	virtual void Tick(float sec) override;
	//	virtual void Draw(IRenderContext * rc, float sec) override;
	//	virtual bool Init(const char* name, IRenderContext * rc) override;
	//	virtual void Fini() override;
	//};
}