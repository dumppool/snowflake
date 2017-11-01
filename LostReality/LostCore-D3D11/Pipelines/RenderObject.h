/*
* file RenderObject.h
*
* author luoxw
* date 2017/10/09
*
*
*/

namespace D3D11
{
	struct FRenderObject
	{
		FPrimitiveGroupPtr PrimitiveGroup;
		vector<FConstantBufferPtr> ConstantBuffers;
		vector<FTexture2DPtr> ShaderResources;

		FRenderObject();

		void Reset();
	};
}