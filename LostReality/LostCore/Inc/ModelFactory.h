/*
* file ModelFactory.h
*
* author luoxw
* date 2017/09/05
*
*
*/

#pragma once

namespace LostCore
{
	class FModelFactory
	{
	public:
		static FBasicModel* NewModel(const FJson& config);
	};
}

