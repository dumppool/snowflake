/*
* file InstancingDataInterface.h
*
* author luoxw
* date 2017/12/03
*
*
*/

#pragma once

namespace LostCore
{

	class IInstancingData
	{
	public:
		virtual ~IInstancingData() {}

		virtual void SetVertexElement(uint32 flags) = 0;
		virtual void Commit() = 0;
		virtual void Update(const void* buf, uint32 sz, uint32 numInstances) = 0;
	};
}