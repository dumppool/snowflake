/*
* file VertexDef.h
*
* author luoxw
* date 2017/06/13
*
*
*/

#pragma once

namespace D3D11
{
	/********************************************************
	Vertex detail
	*/
	ALIGNED_LR(16) struct FVertex_0
	{
		LostCore::FFloat3 XYZ;
		LostCore::FFloat2 UV;

		FORCEINLINE static LostCore::FVertexTypes::Details GetDetails()
		{
			return LostCore::FVertexTypes::GetVertexDetail3D(true, false, false, false, false);
		}

		FORCEINLINE static std::pair<D3D11_INPUT_ELEMENT_DESC*, int32> GetDesc()
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
		LostCore::FFloat3 XYZ;
		LostCore::FFloat2 UV;
		LostCore::FFloat3 N;

		FORCEINLINE static LostCore::FVertexTypes::Details GetDetails()
		{
			return LostCore::FVertexTypes::GetVertexDetail3D(true, true, false, false, false);
		}

		FORCEINLINE static std::pair<D3D11_INPUT_ELEMENT_DESC*, int32> GetDesc()
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
		LostCore::FFloat3 XYZ;
		LostCore::FFloat2 UV;
		LostCore::FFloat3 N;
		LostCore::FFloat3 RGB;

		FORCEINLINE static LostCore::FVertexTypes::Details GetDetails()
		{
			return LostCore::FVertexTypes::GetVertexDetail3D(true, true, false, true, false);
		}

		FORCEINLINE static std::pair<D3D11_INPUT_ELEMENT_DESC*, int32> GetDesc()
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

	ALIGNED_LR(16) struct FVertex_3
	{
		LostCore::FFloat2 XY;
		LostCore::FFloat4 Color;

		FORCEINLINE static LostCore::FVertexTypes::Details GetDetails()
		{
			return LostCore::FVertexTypes::GetVertexDetail2D(false, true);
		}

		FORCEINLINE static std::pair<D3D11_INPUT_ELEMENT_DESC*, int32> GetDesc()
		{
			static D3D11_INPUT_ELEMENT_DESC SDesc[] =
			{
				{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(FVertex_3, Color), D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(FVertex_3, XY), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};

			return std::make_pair(SDesc, ARRAYSIZE(SDesc));
		}
	};

	ALIGNED_LR(16) struct FVertex_4
	{
		LostCore::FFloat2 XY;
		LostCore::FFloat2 UV;
		LostCore::FFloat4 Color;

		FORCEINLINE static LostCore::FVertexTypes::Details GetDetails()
		{
			return LostCore::FVertexTypes::GetVertexDetail2D(true, true);
		}

		FORCEINLINE static std::pair<D3D11_INPUT_ELEMENT_DESC*, int32> GetDesc()
		{
			static D3D11_INPUT_ELEMENT_DESC SDesc[] =
			{
				{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(FVertex_4, Color), D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(FVertex_4, XY), D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(FVertex_4, UV), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};

			return std::make_pair(SDesc, ARRAYSIZE(SDesc));
		}
	};

	ALIGNED_LR(16) struct FVertex_5
	{
		LostCore::FFloat3 XYZ;
		LostCore::FFloat2 UV;
		LostCore::FFloat3 N;
		LostCore::FFloat3 Color;
		LostCore::FFloat4 BlendWeights;
		LostCore::FSInt4 BlendIndices;

		FORCEINLINE static LostCore::FVertexTypes::Details GetDetails()
		{
			return LostCore::FVertexTypes::GetVertexDetail3D(true, true, false, true, true);
		}

		FORCEINLINE static std::pair<D3D11_INPUT_ELEMENT_DESC*, int32> GetDesc()
		{
			static D3D11_INPUT_ELEMENT_DESC SDesc[] =
			{
				{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(FVertex_5, Color), D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(FVertex_5, XYZ), D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(FVertex_5, N), D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(FVertex_5, UV),  D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "BLENDWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(FVertex_5, BlendWeights),  D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_SINT, 0, offsetof(FVertex_5, BlendIndices),  D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};

			return std::make_pair(SDesc, ARRAYSIZE(SDesc));
		}
	};

	ALIGNED_LR(16) struct FVertex_6
	{
		LostCore::FFloat3 XYZ;
		LostCore::FFloat2 UV;
		LostCore::FFloat3 N;
		LostCore::FFloat4 BlendWeights;
		LostCore::FSInt4 BlendIndices;

		FORCEINLINE static LostCore::FVertexTypes::Details GetDetails()
		{
			return LostCore::FVertexTypes::GetVertexDetail3D(true, true, false, false, true);
		}

		FORCEINLINE static std::pair<D3D11_INPUT_ELEMENT_DESC*, int32> GetDesc()
		{
			static D3D11_INPUT_ELEMENT_DESC SDesc[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(FVertex_6, XYZ), D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(FVertex_6, N), D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(FVertex_6, UV),  D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "BLENDWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(FVertex_6, BlendWeights),  D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_SINT, 0, offsetof(FVertex_6, BlendIndices),  D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};

			return std::make_pair(SDesc, ARRAYSIZE(SDesc));
		}
	};

	ALIGNED_LR(16) struct FVertex_7
	{
		LostCore::FFloat3 XYZ;
		LostCore::FFloat3 N;
		LostCore::FFloat4 BlendWeights;
		LostCore::FSInt4 BlendIndices;

		FORCEINLINE static LostCore::FVertexTypes::Details GetDetails()
		{
			return LostCore::FVertexTypes::GetVertexDetail3D(false, true, false, false, true);
		}

		FORCEINLINE static std::pair<D3D11_INPUT_ELEMENT_DESC*, int32> GetDesc()
		{
			static D3D11_INPUT_ELEMENT_DESC SDesc[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(FVertex_7, XYZ), D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(FVertex_7, N), D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "BLENDWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(FVertex_7, BlendWeights),  D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_SINT, 0, offsetof(FVertex_7, BlendIndices),  D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};

			return std::make_pair(SDesc, ARRAYSIZE(SDesc));
		}
	};

	ALIGNED_LR(16) struct FVertex_8
	{
		LostCore::FFloat3 XYZ;
		LostCore::FColor96 COLOR;

		FORCEINLINE static LostCore::FVertexTypes::Details GetDetails()
		{
			return LostCore::FVertexTypes::GetVertexDetail3D(false, false, false, true, false);
		}

		FORCEINLINE static std::pair<D3D11_INPUT_ELEMENT_DESC*, int32> GetDesc()
		{
			static D3D11_INPUT_ELEMENT_DESC SDesc[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(FVertex_8, XYZ), D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(FVertex_8, COLOR), D3D11_INPUT_PER_VERTEX_DATA, 0 },
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
			const char* head = "FInputElementDescMap";

			DescMap.insert(std::make_pair(FVertex_0::GetDetails().Name, FVertex_0::GetDesc()));
			LVMSG(head, "register: %s, stride: %d, aligned stride: %d", FVertex_0::GetDetails().Name.c_str(), FVertex_0::GetDetails().Stride, sizeof(FVertex_0));

			DescMap.insert(std::make_pair(FVertex_1::GetDetails().Name, FVertex_1::GetDesc()));
			LVMSG(head, "register: %s, stride: %d, aligned stride: %d", FVertex_1::GetDetails().Name.c_str(), FVertex_1::GetDetails().Stride, sizeof(FVertex_1));

			DescMap.insert(std::make_pair(FVertex_2::GetDetails().Name, FVertex_2::GetDesc()));
			LVMSG(head, "register: %s, stride: %d, aligned stride: %d", FVertex_2::GetDetails().Name.c_str(), FVertex_2::GetDetails().Stride, sizeof(FVertex_2));

			DescMap.insert(std::make_pair(FVertex_3::GetDetails().Name, FVertex_3::GetDesc()));
			LVMSG(head, "register: %s, stride: %d, aligned stride: %d", FVertex_3::GetDetails().Name.c_str(), FVertex_3::GetDetails().Stride, sizeof(FVertex_3));

			DescMap.insert(std::make_pair(FVertex_4::GetDetails().Name, FVertex_4::GetDesc()));
			LVMSG(head, "register: %s, stride: %d, aligned stride: %d", FVertex_4::GetDetails().Name.c_str(), FVertex_4::GetDetails().Stride, sizeof(FVertex_4));

			DescMap.insert(std::make_pair(FVertex_5::GetDetails().Name, FVertex_5::GetDesc()));
			LVMSG(head, "register: %s, stride: %d, aligned stride: %d", FVertex_5::GetDetails().Name.c_str(), FVertex_5::GetDetails().Stride, sizeof(FVertex_5));

			DescMap.insert(std::make_pair(FVertex_6::GetDetails().Name, FVertex_6::GetDesc()));
			LVMSG(head, "register: %s, stride: %d, aligned stride: %d", FVertex_6::GetDetails().Name.c_str(), FVertex_6::GetDetails().Stride, sizeof(FVertex_6));

			DescMap.insert(std::make_pair(FVertex_7::GetDetails().Name, FVertex_7::GetDesc()));
			LVMSG(head, "register: %s, stride: %d, aligned stride: %d", FVertex_7::GetDetails().Name.c_str(), FVertex_7::GetDetails().Stride, sizeof(FVertex_7));

			DescMap.insert(std::make_pair(FVertex_8::GetDetails().Name, FVertex_8::GetDesc()));
			LVMSG(head, "register: %s, stride: %d, aligned stride: %d", FVertex_8::GetDetails().Name.c_str(), FVertex_8::GetDetails().Stride, sizeof(FVertex_8));
		}

		FORCEINLINE std::pair<D3D11_INPUT_ELEMENT_DESC*, int32> GetDesc(const std::string& key)
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