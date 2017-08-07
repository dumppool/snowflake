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

		virtual bool Config(IRenderContext * rc, const FJson& config) = 0;
		virtual bool Load(IRenderContext * rc, const char* url) = 0;
		virtual void Tick(float sec) = 0;
		virtual void Draw(IRenderContext * rc, float sec) = 0;
	};
}