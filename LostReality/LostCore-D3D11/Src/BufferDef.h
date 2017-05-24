/*
* file BufferDef.h
*
* author luoxw
* date 2017/01/22
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
		LostCore::FVec3 XYZ;
		LostCore::FVec2 UV;

		INLINE static LostCore::FVertexTypes::Details GetDetails()
		{
			return LostCore::FVertexTypes::GetVertexType3DString(true, false, false, false, false);
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
		LostCore::FVec2 UV;
		LostCore::FVec3 N;

		INLINE static LostCore::FVertexTypes::Details GetDetails()
		{
			return LostCore::FVertexTypes::GetVertexType3DString(true, true, false, false, false);
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
		LostCore::FVec3 XYZ;
		LostCore::FVec2 UV;
		LostCore::FVec3 N;
		LostCore::FVec3 RGB;

		INLINE static LostCore::FVertexTypes::Details GetDetails()
		{
			return LostCore::FVertexTypes::GetVertexType3DString(true, true, false, false, true);
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

	ALIGNED_LR(16) struct FVertex_3
	{
		LostCore::FVec2 XY;
		LostCore::FVec4 Color;

		INLINE static LostCore::FVertexTypes::Details GetDetails()
		{
			return LostCore::FVertexTypes::GetVertexType2DString(false, true);
		}

		INLINE static std::pair<D3D11_INPUT_ELEMENT_DESC*, int32> GetDesc()
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
		LostCore::FVec2 XY;
		LostCore::FVec2 UV;
		LostCore::FVec4 Color;

		INLINE static LostCore::FVertexTypes::Details GetDetails()
		{
			return LostCore::FVertexTypes::GetVertexType2DString(true, true);
		}

		INLINE static std::pair<D3D11_INPUT_ELEMENT_DESC*, int32> GetDesc()
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

	/********************************************************
	Constant buffer
	*/

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
		int32 GetByteWidth() const
		{
			return ByteWidth;
		}

		TRefCountPtr<ID3D11Buffer> GetBufferRHI()
		{
			return Buffer;
		}

	private:
		int32		ByteWidth;
		bool		bDynamic;
		int32		Slot;
		TRefCountPtr<ID3D11Buffer> Buffer;
	};

	/*
	cbuffer ViewProject : register(b0)
	{
		float ScreenWidth;
		float ScreenHeight;
		float ScreenWidthRcp;
		float ScreenHeightRcp;
		float4x4 ViewProject;
	};
	*/
	struct FConstantBufferRegister0 : public FConstantBuffer
	{
		struct FParam
		{
			float ScreenWidth;
			float ScreenHeight;
			float ScreenWidthRcp;
			float ScreenHeightRcp;
			LostCore::FMatrix ViewProject;
		};

		FConstantBufferRegister0() : FConstantBuffer(sizeof(FParam), false, 0)
		{
		}

		void UpdateBuffer(const TRefCountPtr<ID3D11DeviceContext>& cxt, const void* buf, int32 sz);
	};

	struct FConstantBufferMatrix : public FConstantBuffer
	{
		struct FParam
		{
			LostCore::FMatrix Mat;
		};

		FConstantBufferMatrix() : FConstantBuffer(sizeof(FParam), false, 1)
		{
		}

		FConstantBufferMatrix(bool dynamic, int32 slot) : FConstantBuffer(sizeof(FParam), dynamic, slot)
		{
		}

		void UpdateBuffer(const TRefCountPtr<ID3D11DeviceContext>& cxt, const void* buf, int32 sz);
	};

	struct FConstantBufferUIRect : public FConstantBuffer
	{
		FConstantBufferUIRect() : FConstantBuffer(sizeof(float) * 8, false, 1)
		{
		}

		FConstantBufferUIRect(bool dynamic, int32 slot) : FConstantBuffer(sizeof(float) * 8, dynamic, slot)
		{
		}
	};
}