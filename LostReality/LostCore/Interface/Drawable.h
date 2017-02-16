#pragma once

namespace LostCore
{
	class IRenderContext;

	class IDrawable
	{
	public:
		virtual ~IDrawable() {}

		virtual void Draw(float sec, IRenderContext* rc) = 0;
	};
}