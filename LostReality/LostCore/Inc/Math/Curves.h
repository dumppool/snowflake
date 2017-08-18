/*
* file Curves.h
*
* author luoxw
* date 2017/08/16
*
*
*/

#pragma once

namespace LostCore
{
	template <typename XType, typename YType>
	class TCurve
	{
	public:
		template <typename XType, typename YType>
		friend FBinaryIO& operator<<(FBinaryIO& stream, const TCurve<XType, YType>& data);		
		
		template <typename XType, typename YType>
		friend FBinaryIO& operator>>(FBinaryIO& stream, TCurve<XType, YType>& data);

		typedef map<XType, YType> FKeyFrameMap;
		typedef typename FKeyFrameMap::iterator FKeyFrameMapIter;
		typedef typename FKeyFrameMap::const_iterator FKeyFrameMapConstIter;

		enum class EWrap : uint8
		{
			Clamp,
			Wrap,
			//Mirror,
		};

		enum class EInterpolation : uint8
		{
			Constant,
			Linear,
			CatmullRom,				// Cubic with default tangent weight/velocity (for fbx).
		};

		TCurve();
		~TCurve();

		void SetMode(EWrap wrap, EInterpolation interp);
		void AddKey(const XType& keyTime, const YType& value);
		YType GetValue(const XType& keyTime) const;
		XType GetRangeMin() const;
		XType GetRangeMax() const;
		XType GetRange() const;
		uint32 GetNumKeys() const;

	protected:
		typename FKeyFrameMapConstIter Get(int32 index) const;

		FKeyFrameMap Keys;

		EWrap WrapMode;
		EInterpolation InterpolationMode;
	};

	template<typename XType, typename YType>
	FORCEINLINE TCurve<XType, YType>::TCurve()
	{
	}

	template<typename XType, typename YType>
	FORCEINLINE TCurve<XType, YType>::~TCurve()
	{
	}

	template<typename XType, typename YType>
	FORCEINLINE void TCurve<XType, YType>::SetMode(EWrap wrap, EInterpolation interp)
	{
		WrapMode = wrap;
		InterpolationMode = interp;
	}

	template<typename XType, typename YType>
	FORCEINLINE void TCurve<XType, YType>::AddKey(const XType & keyTime, const YType & value)
	{
		Keys[keyTime] = value;
	}

	template<typename XType, typename YType>
	FORCEINLINE YType TCurve<XType, YType>::GetValue(const XType & keyTime) const
	{
		assert(GetNumKeys() > 1);

		YType result;
		auto validKeyTime = keyTime;

		if (WrapMode == EWrap::Wrap)
		{
			validKeyTime = InRange(keyTime, GetRangeMin(), GetRangeMax());
		}
		else if (WrapMode == EWrap::Clamp && keyTime < (*Keys.begin()).first)
		{
			validKeyTime = (*Keys.begin()).first;
		}

		int32 index = 0;
		for (auto it = Keys.begin(); it != Keys.end(); ++it, ++index)
		{
			if (validKeyTime < (*it).first)
			{
				break;
			}
		}

		if (InterpolationMode == EInterpolation::CatmullRom)
		{
			auto it0 = Get(index - 2);
			auto it1 = Get(index - 1);
			auto it2 = Get(index);
			auto it3 = Get(index + 1);

			auto m1 = ((*it2).second - (*it0).second) * ((XType)1 / ((*it2).first - (*it0).first));
			auto m2 = ((*it3).second - (*it1).second) * ((XType)1 / ((*it3).first - (*it1).first));
			auto p1 = (*it1).second;
			auto p2 = (*it2).second;
			auto t = (validKeyTime - (*it1).first) / ((*it2).first - (*it1).first);

			auto t2 = t * t;
			auto t3 = t2 * t;

			result = (2 * t3 - 3 * t2 + 1) * p1 + (t3 - 2 * t2 + t)*m1 + (-2 * t3 + 3 * t2)*p2 + (t3 - t2)*m2;
		}
		else if (InterpolationMode == EInterpolation::Constant)
		{
			auto it1 = Get(index - 1);
			result = (*it1).second;
		}
		else if (InterpolationMode == EInterpolation::Linear)
		{
			auto it1 = Get(index - 1);
			auto it2 = Get(index);
			auto t = (validKeyTime - (*it1).first) / ((*it2).first - (*it1).first);
			result = (*it1).second + ((*it2).second - (*it1).second) * t;
		}
		else
		{
			assert(0);
		}

		return result;
	}

	template<typename XType, typename YType>
	FORCEINLINE XType TCurve<XType, YType>::GetRangeMin() const
	{
		assert(GetNumKeys() > 0);
		return (Keys.begin())->first;
	}

	template<typename XType, typename YType>
	FORCEINLINE XType TCurve<XType, YType>::GetRangeMax() const
	{
		assert(GetNumKeys() > 0);
		return (*(Keys.end() - 1)).second;
	}

	template<typename XType, typename YType>
	FORCEINLINE XType TCurve<XType, YType>::GetRange() const
	{
		if (Keys.size() == 0)
		{
			return static_cast<XType>(0);
		}

		return (Keys.end() - 1)->first - Keys.begin()->first;
	}

	template<typename XType, typename YType>
	FORCEINLINE uint32 TCurve<XType, YType>::GetNumKeys() const
	{
		return Keys.size();
	}

	template<typename XType, typename YType>
	FORCEINLINE typename TCurve<XType, YType>::FKeyFrameMapConstIter LostCore::TCurve<XType, YType>::Get(int32 index) const
	{
		assert(GetNumKeys() > 0);
		assert(InterpolationMode != EInterpolation::Constant && GetNumKeys() > 1);

		int32 numKeys = GetNumKeys();
		int32 maxIndex = numKeys - 1;
		int32 validIndex = 0;
		if (index < 0)
		{
			if (WrapMode == EWrap::Clamp)
			{
				validIndex = 0;
			}
			else if (WrapMode == EWrap::Wrap)
			{
				validIndex = InCircle(index, numKeys);
			}
			else
			{
				assert(0);
			}
		}
		else if (maxIndex < index)
		{
			if (WrapMode == EWrap::Clamp)
			{
				validIndex = maxIndex;
			}
			else if (WrapMode == EWrap::Wrap)
			{
				validIndex = InCircle(index, numKeys);
			}
			else
			{
				assert(0);
			}
		}
		else
		{
			validIndex = index;
		}

		return Keys.begin() + validIndex;
	}

	// 曲线的序列化/反序列化
	template <typename XType, typename YType>
	FORCEINLINE FBinaryIO& operator<<(FBinaryIO& stream, const TCurve<XType, YType>& data)
	{
		stream << (uint32)data.WrapMode << (uint32)data.InterpolationMode << data.Keys;
		return stream;
	}

	template <typename XType, typename YType>
	FORCEINLINE FBinaryIO& operator >> (FBinaryIO& stream, TCurve<XType, YType>& data)
	{
		uint32 wrap, interp;
		stream >> wrap >> interp >> data.Keys;
		data.WrapMode = (TCurve<XType, YType>::EWrap)wrap;
		data.InterpolationMode = (TCurve<XType, YType>::EInterpolation)interp;
		return stream;
	}

	typedef TCurve<float, float> FRealCurve;
	typedef TCurve<float, FFloat3> FVec3Curve;
}