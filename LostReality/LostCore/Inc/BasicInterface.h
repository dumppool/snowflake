/*
* file BasicInterface.h
*
* author luoxw
* date 2017/02/13
*
*
*/

#pragma once

namespace LostCore
{
	class IRenderContext;

	class IBasicInterface
	{
	public:
		virtual ~IBasicInterface() {}

		virtual bool Init(const char* name, IRenderContext * rc) = 0;
		virtual void Fini() = 0;
		virtual void Tick(float sec) = 0;
		virtual void Draw(IRenderContext * rc, float sec) = 0;
	};
}