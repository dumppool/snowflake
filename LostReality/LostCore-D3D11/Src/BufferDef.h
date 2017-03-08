/*
* file BufferDef.h
*
* author luoxw
* date 2017/01/22
*
*
*/

#pragma once

// Vertex data for a colored cube.
ALIGNED_LR(16) struct MeshVertex
{
	LostCore::FVec3 Position;
	LostCore::FVec2 Texcoord;
	MeshVertex() = default;
	MeshVertex(LostCore::FVec3 p, LostCore::FVec2 t) : Position(p), Texcoord(t) { }
};

ALIGNED_LR(16) struct FrameBufferWVP
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
	ALIGNED_LR(16) struct FVertex_0
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
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(FVertex_0, XYZ), D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, offsetof(FVertex_0, UV),  D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};

			return std::make_pair(SDesc, ARRAYSIZE(SDesc));
		}
	};

	ALIGNED_LR(16) struct FVertex_1
	{
		LostCore::FVec3 XYZ;
		LostCore::FVec3 N;
		LostCore::FVec2 UV;

		INLINE static std::string GetName()
		{
			static std::string SName = "XYZNUV";
			return SName;
		}

		INLINE static std::pair<D3D11_INPUT_ELEMENT_DESC*, int32> GetDesc()
		{
			static D3D11_INPUT_ELEMENT_DESC SDesc[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(FVertex_1, XYZ), D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(FVertex_1, N), D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, offsetof(FVertex_1, UV),  D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};

			return std::make_pair(SDesc, ARRAYSIZE(SDesc));
		}
	};

	ALIGNED_LR(16) struct FVertex_2
	{
		LostCore::FVec3 RGB;
		LostCore::FVec3 XYZ;
		LostCore::FVec3 N;
		LostCore::FVec2 UV;

		INLINE static std::string GetName()
		{
			static std::string SName = "RGBXYZNUV";
			return SName;
		}

		INLINE static std::pair<D3D11_INPUT_ELEMENT_DESC*, int32> GetDesc()
		{
			static D3D11_INPUT_ELEMENT_DESC SDesc[] =
			{
				{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(FVertex_2, RGB), D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(FVertex_2, XYZ), D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(FVertex_2, N), D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, offsetof(FVertex_2, UV),  D3D11_INPUT_PER_VERTEX_DATA, 0 },
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
			DescMap.insert(std::make_pair(FVertex_0::GetName(), FVertex_0::GetDesc()));
			DescMap.insert(std::make_pair(FVertex_1::GetName(), FVertex_1::GetDesc()));
			DescMap.insert(std::make_pair(FVertex_2::GetName(), FVertex_2::GetDesc()));
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

	struct FConstantBuffer
	{
		FConstantBuffer(int32 byteWidth, bool dynamic, int32 slot)
			: ByteWidth(byteWidth)
			, bDynamic(dynamic)
			, Slot(slot)
			, Buffer(nullptr)
		{
		}

		~FConstantBuffer()
		{
			Buffer = nullptr;
		}

		bool Initialize(const TRefCountPtr<ID3D11Device>& device);
		void UpdateBuffer(const TRefCountPtr<ID3D11DeviceContext>& cxt, const void* buf, int32 sz);
		void Bind(const TRefCountPtr<ID3D11DeviceContext>& cxt);

	private:
		int32		ByteWidth;
		bool		bDynamic;
		int32		Slot;
		TRefCountPtr<ID3D11Buffer> Buffer;
	};

	struct FConstantBufferOneMatrix : public FConstantBuffer
	{
		FConstantBufferOneMatrix(bool dynamic, int32 slot) : FConstantBuffer(sizeof(LostCore::FMatrix), dynamic, slot)
		{
		}
	};
}