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
	struct FInputElementDescMap
	{
		map<uint32, vector<D3D11_INPUT_ELEMENT_DESC>> Descriptions;

		static FInputElementDescMap* Get()
		{
			static FInputElementDescMap Inst;
			return &Inst;
		}

		FORCEINLINE vector<D3D11_INPUT_ELEMENT_DESC> GetDesc(uint32 flags)
		{
			const char* head = "FInputElementDescMap";
			auto it = Descriptions.find(flags);
			if (it == Descriptions.end())
			{
				vector<D3D11_INPUT_ELEMENT_DESC> item;
				uint32 offset = 0;
				string semantics("");
				if (HAS_FLAGS(VERTEX_COORDINATE2D, flags))
				{
					item.push_back({ SEMANTICS_POSITION, 0, DXGI_FORMAT_R32G32_FLOAT, 0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0 });
					offset += sizeof(float) * 2;
					semantics.append("|").append(SEMANTICS_POSITION);
				}
				else
				{
					item.push_back({ SEMANTICS_POSITION, 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0 });
					offset += sizeof(float) * 3;
					semantics.append("|").append(SEMANTICS_POSITION);
				}

				if (HAS_FLAGS(VERTEX_TEXCOORD0, flags))
				{
					item.push_back({ SEMANTICS_TEXCOORD, 0, DXGI_FORMAT_R32G32_FLOAT, 0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0 });
					offset += sizeof(float) * 2;
					semantics.append("|").append(SEMANTICS_TEXCOORD).append("0");
				}

				if (HAS_FLAGS(VERTEX_TEXCOORD1, flags))
				{
					item.push_back({ SEMANTICS_TEXCOORD, 1, DXGI_FORMAT_R32G32_FLOAT, 0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0 });
					offset += sizeof(float) * 2;
					semantics.append("|").append(SEMANTICS_TEXCOORD).append("1");
				}

				if (HAS_FLAGS(VERTEX_NORMAL, flags))
				{
					item.push_back({ SEMANTICS_NORMAL, 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0 });
					offset += sizeof(float) * 3;
					semantics.append("|").append(SEMANTICS_NORMAL);
				}

				if (HAS_FLAGS(VERTEX_TANGENT, flags))
				{
					item.push_back({ SEMANTICS_TANGENT, 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0 });
					offset += sizeof(float) * 3;
					semantics.append("|").append(SEMANTICS_TANGENT);

					item.push_back({ SEMANTICS_BINORMAL, 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0 });
					offset += sizeof(float) * 3;
					semantics.append("|").append(SEMANTICS_BINORMAL);
				}

				if (HAS_FLAGS(VERTEX_COLOR, flags))
				{
					item.push_back({ SEMANTICS_COLOR, 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0 });
					offset += sizeof(float) * 4;
					semantics.append("|").append(SEMANTICS_COLOR);
				}

				if (HAS_FLAGS(VERTEX_SKIN, flags))
				{
					item.push_back({ SEMANTICS_WEIGHTS, 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0 });
					offset += sizeof(float) * 4;
					semantics.append("|").append(SEMANTICS_WEIGHTS);

					item.push_back({ SEMANTICS_INDICES, 0, DXGI_FORMAT_R32G32B32A32_SINT, 0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0 });
					offset += sizeof(float) * 4;
					semantics.append("|").append(SEMANTICS_INDICES);
				}

				Descriptions[flags] = item;

				assert(offset == LostCore::GetVertexDetails(flags).Stride);
				LVMSG(head, "register: %s, stride: %d", semantics.c_str(), offset);
				return item;
			}

			return it->second;
		}
	};
}