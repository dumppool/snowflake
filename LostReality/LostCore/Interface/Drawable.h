#pragma once

namespace LostCore
{
	class IRenderContext;

	class IDrawable
	{
	public:
		virtual ~IDrawable() {}

		virtual void Draw(IRenderContext* rc, float sec) = 0;
	};
}