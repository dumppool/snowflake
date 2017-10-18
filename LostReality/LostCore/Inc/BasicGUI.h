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
		// Method:    HitTest �������
		// FullName:  lostvr::FRect::HitTest
		// Access:    public 
		// Returns:   bool ����ɹ�����true������false
		// Parameter: const FFloat2& ppos ����Ŀ��λ�ã������ռ�����
		// Parameter: FRect** result �����Ϊ�գ����Գɹ���д�뱻�ɹ���������
		//************************************
		bool HitTest(const FFloat2& ppos, FRect** result) const;

		//************************************
		// Method:    AddChild �������壬Ȼ����������������
		// FullName:  lostvr::FRect::AddChild
		// Access:    public 
		// Parameter: FRect* child ����壬Ϊ�ջ��߸���岻Ϊ���޷��ɹ����
		virtual void AddChild(FRect* child);

		//************************************
		// Method:    DelChild ɾ�������
		// FullName:  lostvr::FRect::DelChild
		// Access:    public 
		// Parameter: FRect* child ����壬Ϊ�ջ��߸���岻Ϊthis�޷��ɹ�ɾ��
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

		// ��ȣ�ԽСԽ��ǰ
		float Depth;

		// �����ָ��
		FRect* Parent;

		// ������ȴ�ǰ���󣬻���ʱ��Ӻ�ǰ��
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