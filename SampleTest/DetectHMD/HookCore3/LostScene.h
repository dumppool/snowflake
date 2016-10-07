#pragma once

namespace LostVR {
	class LostScene
	{
	public:
		void Init(ID3D11Device* Dev);
		void Update(ID3D11Texture2D* Tex);
	};
}