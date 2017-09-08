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

		virtual bool Config(const FJson& config) = 0;
		virtual bool Load(const char* url) = 0;
		virtual void Tick() = 0;
		virtual void Draw() = 0;
	};
}