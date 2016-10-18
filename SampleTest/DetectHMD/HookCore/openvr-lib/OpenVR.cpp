#include "HookCorePCH.h"
#include "OpenVR.h"

using namespace lostvr;

void OpenVRRenderer::GetEyeViewProject(vr::Hmd_Eye Eye, float fNearZ, float fFarZ, vr::EGraphicsAPIConvention eProjType, LVMatrix& EyeView, LVMatrix& Proj) const
{
	bool bCustom = false;

	if (bCustom)
	{
		float L, R, B, T;
		Sys->GetProjectionRaw(Eye, &L, &R, &T, &B);
		L *= 1.f;
		R *= 1.f;
		B *= -1.f;
		T *= -1.f;

		Proj.r[0].m128_f32[0] = 2.f / (R - L);
		Proj.r[0].m128_f32[1] = 0.f;
		Proj.r[0].m128_f32[2] = 0.f;
		Proj.r[0].m128_f32[3] = 0.f;

		Proj.r[1].m128_f32[0] = 0.f;
		Proj.r[1].m128_f32[1] = 2.f / (T - B);
		Proj.r[1].m128_f32[2] = 0.f;
		Proj.r[1].m128_f32[3] = 0.f;

		Proj.r[2].m128_f32[0] = (R + L) / (R - L);
		Proj.r[2].m128_f32[1] = (T + B) / (T - B);
		Proj.r[2].m128_f32[2] = 0.f;
		Proj.r[2].m128_f32[3] = 1.f;

		Proj.r[3].m128_f32[0] = 0.f;
		Proj.r[3].m128_f32[1] = 0.f;
		Proj.r[3].m128_f32[2] = 0.1f;
		Proj.r[3].m128_f32[3] = 0.f;
	}
	else
	{
		vr::HmdMatrix44_t HmdProj = Sys->GetProjectionMatrix(Eye, fNearZ, fFarZ, eProjType);
		memcpy(&Proj, &HmdProj, sizeof(HmdProj));
		Proj = DirectX::XMMatrixTranspose(Proj);
	}

	vr::HmdMatrix34_t HmdView = Sys->GetEyeToHeadTransform(Eye);

	if (bCustom)
	{
		EyeView = DirectX::XMMatrixTranslation(HmdView.m[0][3], HmdView.m[1][3], HmdView.m[2][3]);
		EyeView = DirectX::XMMatrixInverse(nullptr, EyeView);
	}
	else
	{
		memcpy(&EyeView, &HmdView, sizeof(HmdView));
		EyeView.r[3].m128_f32[0] = 0.f;
		EyeView.r[3].m128_f32[1] = 0.f;
		EyeView.r[3].m128_f32[2] = 0.f;
		EyeView.r[3].m128_f32[3] = 1.f;
		EyeView = DirectX::XMMatrixTranspose(EyeView);
		EyeView = DirectX::XMMatrixInverse(nullptr, EyeView);
	}
}