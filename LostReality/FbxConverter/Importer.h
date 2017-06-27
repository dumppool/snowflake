/*
* file Importer.h
*
* author luoxw
* date 2017/03/10
*
*
*/

#pragma once

namespace Importer {

	inline float SNUM(float val)
	{
		return abs(val) < LostCore::SSmallFloat2 ? 0.0f : val;
	}

	inline double SNUM(double val)
	{
		return abs(val) < (double)LostCore::SSmallFloat2 ? 0.0 : val;
	}

	static void WriteRGB(FJson& output, const FbxColor& color)
	{
		if (color.IsValid())
		{
			vector<double> rgb;
			rgb.push_back(color.mRed);
			rgb.push_back(color.mGreen);
			rgb.push_back(color.mBlue);
			output = rgb;
		}
	}

	static void WriteRGBA(FJson& output, const FbxColor& color)
	{
		if (color.IsValid())
		{
			vector<double> rgba;
			rgba.push_back(color.mRed);
			rgba.push_back(color.mGreen);
			rgba.push_back(color.mBlue);
			rgba.push_back(color.mAlpha);
			output = rgba;
		}
	}

	static void WriteFloat2(FJson& output, const FbxDouble2& value)
	{
		vector<double> f2;
		f2.push_back(value[0]);
		f2.push_back(value[1]);
		output = f2;
	}

	static void WriteFloat3_FromUnreal(FJson& output, const FbxDouble3& value)
	{
		vector<double> f3;
		f3.push_back(value[1]);
		f3.push_back(value[2]);
		f3.push_back(value[0]);
		output = f3;
	}

	static void WriteFloat3(FJson& output, const FbxDouble3& value)
	{
		WriteFloat3_FromUnreal(output, value);
		return;

		vector<double> f3;
		f3.push_back(value[0]);
		f3.push_back(value[1]);
		f3.push_back(value[2]);
		output = f3;
	}

	static void WriteFloat3_FromUnreal(FJson& output, const FbxVector4& value, bool isCoordinate = false)
	{
		vector<double> f3;
		f3.push_back(isCoordinate ? SNUM(value[1]) * 0.01 : SNUM(value[1]));
		f3.push_back(isCoordinate ? SNUM(value[2]) * 0.01 : SNUM(value[2]));
		f3.push_back(isCoordinate ? SNUM(value[0]) * 0.01 : SNUM(value[0]));
		output = f3;
	}

	static void WriteFloat3(FJson& output, const FbxVector4& value, bool isCoordinate = false)
	{
		WriteFloat3_FromUnreal(output, value, isCoordinate);
		return;

		vector<double> f3;
		f3.push_back(value[0]);
		f3.push_back(value[1]);
		f3.push_back(value[2]);
		output = f3;
	}

	static void WriteFloat4x4_FromUnreal(FJson& output, const FbxMatrix& matrix)
	{
		vector<double> f4x4;
		for (int row = 0; row < 4; ++row)
		{
			FbxVector4 vec = matrix.GetRow(row);
			if (row < 3)
			{
				for (int col = 0; col < 4; ++col)
				{
					f4x4.push_back(abs(vec[col]) < LostCore::SSmallFloat2 ? 0.0f : vec[col]);
				}
			}
			else
			{
				f4x4.push_back(SNUM(vec[1]) * 0.01);
				f4x4.push_back(SNUM(vec[2]) * 0.01);
				f4x4.push_back(SNUM(vec[0]) * 0.01);
				f4x4.push_back(SNUM(vec[3]));
			}
		}

		output = f4x4;
	}

	static void WriteFloat4x4(FJson& output, const FbxMatrix& matrix)
	{
		WriteFloat4x4_FromUnreal(output, matrix);
		return;

		vector<double> f4x4;
		for (int row = 0; row < 4; ++row)
		{
			FbxVector4 vec = matrix.GetRow(row);
			for (int col = 0; col < 4; ++col)
			{
				f4x4.push_back(SNUM(vec[col]));
			}
		}

		output = f4x4;
	}

	extern bool ImportScene(const string& importSrc, const string& convertDst, bool outputBinary);
	extern bool ImportSceneMeshes(const string& importSrc, const string& convertDst, bool outputBinary, bool exportAnimation);
}