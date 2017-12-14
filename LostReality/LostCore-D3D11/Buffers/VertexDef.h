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

		vector<D3D11_INPUT_ELEMENT_DESC> GetDesc(uint32 flags)
		{
			const char* head = "FInputElementDescMap";
			auto it = Descriptions.find(flags);
			if (it == Descriptions.end())
			{
				vector<D3D11_INPUT_ELEMENT_DESC> item;

				// ISNTANCE_TRANSFORM数据, INSTANCE_TEXTILE, mesh数据有各自的vertex buffer.
				// 在更多的应用需求之前，这个不必要的假设会一直存在.
				uint32 offset = 0, offset2 = 0, offset3 = 0;
				string semantics("");
				uint32 bit32 = sizeof(float);
				uint32 vbIndex = 0;
				if (HAS_FLAGS(VERTEX_COORDINATE2D, flags))
				{
					item.push_back({
						SEMANTICS_POSITION, 
						0, 
						DXGI_FORMAT_R32G32_FLOAT, 
						0, 
						offset, 
						D3D11_INPUT_PER_VERTEX_DATA, 
						0 });

					offset += bit32 * 2;
					semantics.append("|").append(SEMANTICS_POSITION);
				}
				else
				if (HAS_FLAGS(VERTEX_COORDINATE3D, flags))
				{
					item.push_back({ 
						SEMANTICS_POSITION, 
						0, 
						DXGI_FORMAT_R32G32B32_FLOAT, 
						0,
						offset,
						D3D11_INPUT_PER_VERTEX_DATA, 
						0 });

					offset += bit32 * 3;
					semantics.append("|").append(SEMANTICS_POSITION);
				}

				if (HAS_FLAGS(VERTEX_TEXCOORD0, flags))
				{
					item.push_back({ 
						SEMANTICS_TEXCOORD,
						0, 
						DXGI_FORMAT_R32G32_FLOAT,
						0, 
						offset, 
						D3D11_INPUT_PER_VERTEX_DATA,
						0 });

					offset += bit32 * 2;
					semantics.append("|").append(SEMANTICS_TEXCOORD).append("0");
				}

				if (HAS_FLAGS(VERTEX_TEXCOORD1, flags))
				{
					item.push_back({ 
						SEMANTICS_TEXCOORD,
						1, 
						DXGI_FORMAT_R32G32_FLOAT,
						0, 
						offset, 
						D3D11_INPUT_PER_VERTEX_DATA, 
						0 });

					offset += bit32 * 2;
					semantics.append("|").append(SEMANTICS_TEXCOORD).append("1");
				}

				if (HAS_FLAGS(VERTEX_NORMAL, flags))
				{
					item.push_back({ 
						SEMANTICS_NORMAL, 
						0, 
						DXGI_FORMAT_R32G32B32_FLOAT,
						0, 
						offset,
						D3D11_INPUT_PER_VERTEX_DATA, 
						0 });

					offset += bit32 * 3;
					semantics.append("|").append(SEMANTICS_NORMAL);
				}

				if (HAS_FLAGS(VERTEX_TANGENT, flags))
				{
					item.push_back({ 
						SEMANTICS_TANGENT, 
						0, 
						DXGI_FORMAT_R32G32B32_FLOAT,
						0, 
						offset, 
						D3D11_INPUT_PER_VERTEX_DATA, 
						0 });

					offset += bit32 * 3;
					semantics.append("|").append(SEMANTICS_TANGENT);

					item.push_back({
						SEMANTICS_BINORMAL, 
						0, 
						DXGI_FORMAT_R32G32B32_FLOAT, 
						0, 
						offset, 
						D3D11_INPUT_PER_VERTEX_DATA, 
						0 });

					offset += bit32 * 3;
					semantics.append("|").append(SEMANTICS_BINORMAL);
				}

				if (HAS_FLAGS(VERTEX_COLOR, flags))
				{
					item.push_back({ 
						SEMANTICS_COLOR,
						0, 
						DXGI_FORMAT_R32G32B32A32_FLOAT, 
						0, 
						offset,
						D3D11_INPUT_PER_VERTEX_DATA,
						0 });

					offset += bit32 * 4;
					semantics.append("|").append(SEMANTICS_COLOR);
				}

				if (HAS_FLAGS(VERTEX_SKIN, flags))
				{
					item.push_back({ 
						SEMANTICS_WEIGHTS,
						0,
						DXGI_FORMAT_R32G32B32A32_FLOAT, 
						0,
						offset,
						D3D11_INPUT_PER_VERTEX_DATA, 
						0 });

					offset += bit32 * 4;
					semantics.append("|").append(SEMANTICS_WEIGHTS);

					item.push_back({ 
						SEMANTICS_INDICES, 
						0, 
						DXGI_FORMAT_R32G32B32A32_SINT,
						0,
						offset, 
						D3D11_INPUT_PER_VERTEX_DATA, 
						0 });

					offset += bit32 * 4;
					semantics.append("|").append(SEMANTICS_INDICES);
				}

				if (HAS_FLAGS(INSTANCE_TRANSFORM2D, flags))
				{
					vbIndex += 1;
					uint32 stepRate = 1;

					item.push_back({ SEMANTICS_RECTOFFSET, 0, DXGI_FORMAT_R32G32_FLOAT, vbIndex, offset2, D3D11_INPUT_PER_INSTANCE_DATA, stepRate });
					offset2 += bit32 * 2;
					item.push_back({ SEMANTICS_RECTSIZE, 0, DXGI_FORMAT_R32G32_FLOAT, vbIndex, offset2, D3D11_INPUT_PER_INSTANCE_DATA, stepRate });
					offset2 += bit32 * 2;
					item.push_back({ SEMANTICS_RECTSCALE, 0, DXGI_FORMAT_R32G32_FLOAT, vbIndex, offset2, D3D11_INPUT_PER_INSTANCE_DATA, stepRate });
					offset2 += bit32 * 2;

					semantics.append("|").append(SEMANTICS_RECTOFFSET);
					semantics.append("|").append(SEMANTICS_RECTSIZE);
					semantics.append("|").append(SEMANTICS_RECTSCALE);
				}
				else
				if (HAS_FLAGS(INSTANCE_TRANSFORM3D, flags))
				{
					vbIndex += 1;
					uint32 stepRate = 1;

					item.push_back({ SEMANTICS_TRANSFORM, 0, DXGI_FORMAT_R32G32B32A32_FLOAT, vbIndex, offset2, D3D11_INPUT_PER_INSTANCE_DATA, stepRate });
					offset2 += bit32 * 4;
					item.push_back({ SEMANTICS_TRANSFORM, 1, DXGI_FORMAT_R32G32B32A32_FLOAT, vbIndex, offset2, D3D11_INPUT_PER_INSTANCE_DATA, stepRate });
					offset2 += bit32 * 4;
					item.push_back({ SEMANTICS_TRANSFORM, 2, DXGI_FORMAT_R32G32B32A32_FLOAT, vbIndex, offset2, D3D11_INPUT_PER_INSTANCE_DATA, stepRate });
					offset2 += bit32 * 4;
					item.push_back({ SEMANTICS_TRANSFORM, 3, DXGI_FORMAT_R32G32B32A32_FLOAT, vbIndex, offset2, D3D11_INPUT_PER_INSTANCE_DATA, stepRate });
					offset2 += bit32 * 4;

					semantics.append("|").append(SEMANTICS_TRANSFORM);
				}

				if (HAS_FLAGS(INSTANCE_TEXTILE, flags))
				{
					vbIndex += 1;
					uint32 stepRate = 1;
					item.push_back({ SEMANTICS_TILEOFFSET, 0, DXGI_FORMAT_R32G32_FLOAT, vbIndex, offset3, D3D11_INPUT_PER_INSTANCE_DATA, stepRate });
					offset3 += bit32 * 2;
					item.push_back({ SEMANTICS_TILESCALE, 0, DXGI_FORMAT_R32G32_FLOAT, vbIndex, offset3, D3D11_INPUT_PER_INSTANCE_DATA, stepRate });
					offset3 += bit32 * 2;

					semantics.append("|").append(SEMANTICS_TILEOFFSET);
					semantics.append("|").append(SEMANTICS_TILESCALE);
				}

				Descriptions[flags] = item;

				assert((offset) == LostCore::GetVertexDetails(flags).Stride);
				LVMSG(head, "register: %s, stride: %d", semantics.c_str(), (offset));
				return item;
			}

			return it->second;
		}
	};
}