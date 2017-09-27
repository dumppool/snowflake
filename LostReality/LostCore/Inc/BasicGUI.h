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

		void Clear();

		//************************************
		// Method:    Draw ����
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

		FRectParameter Param;

		// ��ȣ�ԽСԽ��ǰ
		float Depth;

		// �����ָ��
		FRect* Parent;

		// ������ȴ�ǰ���󣬻���ʱ��Ӻ�ǰ��
		vector<FRect*> Children;

		IPrimitiveGroup* RectPrimitive;
		FFloat2 RectPrimitiveSize;
		IMaterial* RectMaterial;
		IConstantBuffer* RectBuffer;
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

		void Destroy();

	private:
		FRect Root;
		IFontInterface* Font;
	};
}