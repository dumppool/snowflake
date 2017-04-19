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

	class FRect
	{
	public:

		FRect();

		FVec2 GetOrigin() const;
		float GetScale() const;

		//************************************
		// Method:    HitTest �������
		// FullName:  lostvr::FRect::HitTest
		// Access:    public 
		// Returns:   bool ����ɹ�����true������false
		// Parameter: const FVec2& ppos ����Ŀ��λ�ã������ռ�����
		// Parameter: FRect** result �����Ϊ�գ����Գɹ���д�뱻�ɹ���������
		//************************************
		bool HitTest(const FVec2& ppos, FRect** result) const;

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
		// Method:    DelChild ɾ�������
		// FullName:  lostvr::FRect::DelChild
		// Access:    public 
		// Parameter: FRect* child ����壬Ϊ�ջ��߸���岻Ϊthis�޷��ɹ�ɾ��
		void Draw(IRenderContext * rc, float sec);

		virtual void ReconstructPrimitive(IRenderContext * rc);

	private:
		bool HitTestPrivate(const FVec2& ppos, FRect** result) const;
		void GetLocalPosition(const FVec2& ppos, FVec2& cpos) const;

		// ��Ը�����λ�ã���λ������
		FVec2 Origin;

		// ���Գߴ磬��λ������
		FVec2 Size;

		// ��Ը��������ţ�(0.f, 1.f]
		float Scale;

		// ��ȣ�ԽСԽ��ǰ
		float Depth;

		// �����ָ��
		FRect* Parent;

		// ������ȴ�ǰ���󣬻���ʱ��Ӻ�ǰ��
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