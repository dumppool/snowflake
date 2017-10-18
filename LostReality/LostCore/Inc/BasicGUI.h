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

	__declspec(align(16)) struct FRectVertex 
	{
		static const void* GetDefaultVertices()
		{
			static const FRectVertex vertices[] =
			{
				{ FFloat2(0.f, 0.f),	FFloat2(0.f, 0.f), FColor128(0xffffff) },		// top left
				{ FFloat2(1.f, 0.f),	FFloat2(1.f, 0.f), FColor128(0xffffff) },		// top right
				{ FFloat2(0.f, 1.f),	FFloat2(0.f, 1.f), FColor128(0xffffff) },		// bottom left
				{ FFloat2(1.f, 1.f),	FFloat2(1.f, 1.f), FColor128(0xffffff) },		// bottom right
			};

			return vertices;
		}

		static uint32 GetVertexElement()
		{
			return VERTEX_COORDINATE2D | VERTEX_COLOR | VERTEX_TEXCOORD0;
		}

		static const void* GetDefaultIndices()
		{
			static const int16 indices[] = { 0, 1, 2, 1, 3, 2 };
			return indices;
		}

		FFloat2 XY;
		FFloat2 TexCoord; 
		FColor128 RGB; 
	};

	class FRect
	{
	public:
		FRect();
		~FRect();

		void SetOriginLocal(const FFloat2& origin);
		FFloat2 GetOriginGlobal() const;

		void SetScaleLocal(float val);
		float GetScaleGlobal() const;

		void SetSize(const FFloat2& size);
		FFloat2 GetSize() const;

		void SetTexCoords(const array<FFloat2, 4>& texCoords);

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
		virtual void AddChild(FRect* child);

		//************************************
		// Method:    DelChild 删除子面板
		// FullName:  lostvr::FRect::DelChild
		// Access:    public 
		// Parameter: FRect* child 子面板，为空或者父面板不为this无法成功删除
		virtual void DelChild(FRect* child);

		void Detach();
		void ClearChildren(bool dealloc = false);

		virtual void Commit();
		void SetTexture(ITexture* tex);
		void ConstructPrimitive(const void* buf, int32 sz, int32 stride);
		void HasGeometry(bool val);

	protected:
		bool HitTestPrivate(const FFloat2& ppos, FRect** result) const;
		void GetLocalPosition(const FFloat2& ppos, FFloat2& cpos) const;
		void CommitPrivate();
		void Destroy();

		FRectParameter Param;

		// 深度，越小越靠前
		float Depth;

		// 父面板指针
		FRect* Parent;

		// 次序：深度从前到后，绘制时需从后到前。
		vector<FRect*> Children;

		IPrimitiveGroup* RectPrimitive;
		bool bConstructed;
		bool bHasGeometry;
		ITexture* RectTexture;
		IConstantBuffer* RectBuffer;
	};

	class FBasicGUI
	{
	public:
		FBasicGUI();
		~FBasicGUI();

		bool Initialize(const FFloat2& size);
		void Destroy();

		void Tick();

		FRect* GetRoot()
		{
			return Root;
		}

	private:
		FRect* Root;
	};
}