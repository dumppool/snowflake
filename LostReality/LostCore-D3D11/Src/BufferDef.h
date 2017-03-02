/*
* file BufferDef.h
*
* author luoxw
* date 2017/01/22
*
*
*/

#pragma once


#define ALIGNED_LOSTVR(x) __declspec(align(x))

// Vertex data for a colored cube.
ALIGNED_LOSTVR(16) struct MeshVertex
{
	LostCore::FVec3 Position;
	LostCore::FVec2 Texcoord;
	MeshVertex() = default;
	MeshVertex(LostCore::FVec3 p, LostCore::FVec2 t) : Position(p), Texcoord(t) { }
};

ALIGNED_LOSTVR(16) struct FrameBufferWVP
{
	LVMatrix W;
	LVMatrix V;
	LVMatrix P;
	FrameBufferWVP() = default;
	FrameBufferWVP(const LVMatrix& w, const LVMatrix& v, const LVMatrix& p)
		: W(w), V(v), P(p) {}
};

namespace D3D11
{
	ALIGNED_LOSTVR(16) struct FVertex_XYZUV
	{
		LostCore::FVec3 XYZ;
		LostCore::FVec2 UV;

		INLINE static std::string GetName()
		{
			static std::string SName = "XYZUV";
			return SName;
		}

		INLINE static std::pair<D3D11_INPUT_ELEMENT_DESC*, int32> GetDesc()
		{
			static D3D11_INPUT_ELEMENT_DESC SDesc[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(FVertex_XYZUV, XYZ), D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, offsetof(FVertex_XYZUV, UV),  D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};

			return std::make_pair(SDesc, ARRAYSIZE(SDesc));
		}
	};

	struct FInputElementDescMap
	{
		static FInputElementDescMap* Get()
		{
			static FInputElementDescMap Inst;
			return &Inst;
		}

		std::map<std::string, std::pair<D3D11_INPUT_ELEMENT_DESC*, int32>> DescMap;

		FInputElementDescMap()
		{
			DescMap.insert(std::make_pair(FVertex_XYZUV::GetName(), FVertex_XYZUV::GetDesc()));
		}

		INLINE std::pair<D3D11_INPUT_ELEMENT_DESC*, int32> GetDesc(const std::string& key)
		{
			for (auto element : DescMap)
			{
				if (element.first.compare(key) == 0)
				{
					return element.second;
				}
			}

			return std::make_pair(nullptr, 0);
		}
	};
}