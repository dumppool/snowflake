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
	class IFont;
	class IPrimitive;
	class IMaterial;

	__declspec(align(16)) struct FRectVertex 
	{
		static uint32 GetDefaultSize()
		{
			return sizeof(FRectVertex) * 4;
		}

		static const void* GetDefaultVertices(const FColor128& col)
		{
			static FRectVertex SVertices[] =
			{
				{ FFloat2(0.f, 0.f),	FFloat2(0.f, 0.f), FColor128(0xffffffff) },		// top left
				{ FFloat2(1.f, 0.f),	FFloat2(1.f, 0.f), FColor128(0xffffffff) },		// top right
				{ FFloat2(0.f, 1.f),	FFloat2(0.f, 1.f), FColor128(0xffffffff) },		// bottom left
				{ FFloat2(1.f, 1.f),	FFloat2(1.f, 1.f), FColor128(0xffffffff) },		// bottom right
			};

			for (int32 index = 0; index < ARRAYSIZE(SVertices); index++)
			{
				SVertices[index].Color = col;
			}

			return SVertices;
		}

		static uint32 GetVertexElement()
		{
			return VERTEX_COORDINATE2D | VERTEX_COLOR | VERTEX_TEXCOORD0;
		}

		static FBuf* GetDefaultIndices()
		{
			static FBuf SIndices;
			if (SIndices.empty())
			{
				const int16 indices[] = { 0, 1, 2, 1, 3, 2 };
				SIndices.resize(sizeof(indices));
				memcpy(SIndices.data(), indices, sizeof(indices));
			}

			return &SIndices;
		}

		FFloat2 XY;
		FFloat2 TexCoord; 
		FColor128 Color; 
	};

	class FRect
	{
	public:
		FRect();
		virtual ~FRect();

		FRect* GetRoot();

		void SetOffsetLocal(const FFloat2& origin);
		FFloat2 GetOffsetGlobal() const;

		void SetScaleLocal(const FFloat2& val);
		FFloat2 GetScaleGlobal() const;

		void SetSize(const FFloat2& size);
		FFloat2 GetSize() const;

		void SetAutoUpdateWidth(bool val);
		bool GetAutoUpdateWidth() const;

		void SetAutoUpdateHeight(bool val);
		bool GetAutoUpdateHeight() const;

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
		void AddChild(FRect* child);

		//************************************
		// Method:    DelChild ɾ�������
		// FullName:  lostvr::FRect::DelChild
		// Access:    public 
		// Parameter: FRect* child ����壬Ϊ�ջ��߸���岻Ϊthis�޷��ɹ�ɾ��
		void DelChild(FRect* child);
		void PopChild(const function<void(FRect*)>& dealloc);

		void Detach();
		void ClearChildren(const function<void(FRect*)>& dealloc);
		int32 NumChildren() const;
		FRect* GetChild(int32 index);

		virtual void Update();
		virtual void Commit();
		//void SetTexture(ITextureSet* tex);
		void ConstructPrimitive(const FBuf& buf, int32 stride);
		void HasGeometry(bool val);

	protected:
		virtual void UpdateAndCommitRectBuffer();

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

		IPrimitive* RectPrimitive;
		bool bConstructed;
		bool bHasGeometry;
		bool bAutoUpdateWidth;
		bool bAutoUpdateHeight;
		//ITextureSet* RectTexture;
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