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
//#include "FontInterface.h"

namespace LostCore
{
	class IFontInterface;
	class IPrimitiveGroup;
	class IMaterial;
	class ITexture;

	class FRect
	{
		struct FParam
		{
			// 大小尺寸，像素
			FFloat2 Size;

			// 相对父面板的位置，单位，像素
			FFloat2 Origin;

			// 相对父面板的缩放，(0.f, 1.f]
			float Scale;

			FFloat3 Unused;

			FParam() : Size(0.f, 0.f), Origin(0.f, 0.f), Scale(1.f) { assert(sizeof(FParam) == 8 * sizeof(float)); }
		};

	public:
		FRect();
		~FRect();

		void SetOrigin(const FFloat2& origin);
		FFloat2 GetOrigin() const;

		void SetScale(float val);
		float GetScale() const;

		void SetSize(const FFloat2& size);
		FFloat2 GetSize() const;

		//************************************
		// Method:    HitTest 点击测试
		// FullName:  lostvr::FRect::HitTest
		// Access:    public 
		// Returns:   bool 点击成功返回true，否则false
		// Parameter: const FFloat2& ppos 测试目标位置，父面板空间坐标
		// Parameter: FRect** result 如果不为空，测试成功后写入被成功点击的面板
		//************************************
		bool HitTest(const FFloat2& ppos, FRect** result) const;

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
		// Method:    Draw 绘制
		// FullName:  lostvr::FRect::Draw
		// Access:    public 
		// Parameter: IRenderContext * rc
		// Parameter:  
		void Draw();

		void SetColorTexture(ITexture* tex);

		virtual void ReconstructPrimitive();

	private:
		bool HitTestPrivate(const FFloat2& ppos, FRect** result) const;
		void GetLocalPosition(const FFloat2& ppos, FFloat2& cpos) const;
		void DrawPrivate();

		FParam Param;

		// 深度，越小越靠前
		float Depth;

		// 父面板指针
		FRect* Parent;

		// 次序：深度从前到后，绘制时需从后到前。
		vector<FRect*> Children;

		IPrimitiveGroup* RectPrimitive;
		FFloat2 RectPrimitiveSize;
		IMaterial* RectMaterial;
	};

	class FBasicGUI : public IBasicInterface
	{
	public:
		FBasicGUI();
		virtual ~FBasicGUI() override;

		virtual void Tick() override;
		virtual void Draw() override;
		virtual bool Config(const FJson& config) override;
		virtual bool Load(const char* url) override;

		void Fini();

	private:
		FRect Root;
		IFontInterface* Font;
	};
}