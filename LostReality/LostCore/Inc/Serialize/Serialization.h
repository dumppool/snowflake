/*
* file Serialization.h
*
* author luoxw
* date 2017/05/25
*
*
*/

#pragma once

namespace LostCore
{
	class FBinaryIO;
	FBinaryIO& Serialize(FBinaryIO& stream, const uint8* buf, uint32 sz);
	FBinaryIO& Deserialize(FBinaryIO& stream, uint8* buf, uint32 sz);

	class FBinaryIO
	{
	protected:
		uint8 * Begin;
		uint8 * Write;
		uint8 * Read;
		uint8 * End;

	public:
		FBinaryIO();
		explicit FBinaryIO(uint32 size);
		FBinaryIO(uint8* begin, uint32 sz);
		~FBinaryIO();

		const void * Retrieve(uint32 bytes);
		void * Reserve(uint32 bytes);
		uint32 Size() const;
		uint32 RemainingSize() const;
		void * Data();

		void WriteToFile(const std::string& filePath);
		void ReadFromFile(const std::string& filePath);
	};

	inline FBinaryIO::FBinaryIO()
	{
		uint32 sz = 64;
		Begin = new uint8[sz];
		Read = Begin;
		Write = Begin;
		End = Begin + sz;
	}

	inline FBinaryIO::FBinaryIO(uint32 size)
	{
		uint32 sz = size;
		Begin = new uint8[sz];
		Read = Begin;
		Write = Begin;
		End = Begin + sz;
	}

	inline FBinaryIO::FBinaryIO(uint8* begin, uint32 sz)
	{
		assert(sz > 0);
		Begin = new uint8[sz];
		Read = Begin;
		Write = Begin;
		End = Begin + sz;
		Serialize(*this, begin, sz);
	}

	inline FBinaryIO::~FBinaryIO()
	{
		delete[] Begin;
		Begin = nullptr;
		Read = nullptr;
		Write = nullptr;
		End = nullptr;
	}

	inline const void * FBinaryIO::Retrieve(uint32 bytes)
	{
		uint8 * oldRead = Read;
		if (Read + bytes <= Write)
		{
			Read += bytes;
			return oldRead;
		}
		else
		{
			assert(0 && "FBinaryIO::Retrieve asked too much bytes");
			return nullptr;
		}
	}

	inline void * FBinaryIO::Reserve(uint32 bytes)
	{
		uint8 * oldWrite = Write;
		Write += bytes;
		if (Write > End)
		{
			uint32 multiplier = (Write - Begin) / (End - Begin) + 1;
			uint32 sz = multiplier * (End - Begin);
			uint8 * newBegin = new uint8[sz];
			memcpy(newBegin, Begin, oldWrite - Begin);
			Write = newBegin + (Write - Begin);
			oldWrite = newBegin + (oldWrite - Begin);
			Read = newBegin + (Read - Begin);
			delete[] Begin;
			Begin = newBegin;
			End = Begin + sz;
		}

		return oldWrite;
	}

	inline uint32 FBinaryIO::Size() const
	{
		return End - Begin;
	}

	inline uint32 FBinaryIO::RemainingSize() const
	{
		return Write - Read;
	}

	inline void * FBinaryIO::Data()
	{
		return Begin;
	}

	inline void FBinaryIO::WriteToFile(const std::string& filePath)
	{
		ofstream file;
		file.open(filePath, ios::out|ios::binary);
		file.write((const char*)Data(), Size());
		file.close();
	}

	inline void FBinaryIO::ReadFromFile(const std::string& filePath)
	{
		ifstream file;
		file.open(filePath, ios::in | ios::binary | ios::ate);
		auto sz = file.tellg();
		Reserve((uint32)sz);
		file.seekg(0);
		file.read((char*)Begin, sz);
		file.close();
	}

	/************************************************************
	Serialize implementation
	************************************************************/
	template<typename T>
	FBinaryIO& operator<<(FBinaryIO& stream, const T& data)
	{
		memcpy(stream.Reserve(sizeof(data)), &data, sizeof(data));
		return stream;
	}

	template<typename T>
	FBinaryIO& operator >> (FBinaryIO& stream, T& data)
	{
		memcpy(&data, stream.Retrieve(sizeof(data)), sizeof(data));
		return stream;
	}

	template<>
	inline FBinaryIO& operator<<(FBinaryIO& stream, const std::string& data)
	{
		auto sz = data.length();
		stream << sz;
		Serialize(stream, (const uint8*)&(data[0]), sz);
		return stream;
	}

	template<>
	inline FBinaryIO& operator >> (FBinaryIO& stream, std::string& data)
	{
		uint32 sz;
		stream >> sz;
		data.resize(sz);
		Deserialize(stream, (uint8*)&(data[0]), sz);
		return stream;
	}

	inline FBinaryIO& Serialize(FBinaryIO& stream, const uint8* buf, uint32 sz)
	{
		memcpy(stream.Reserve(sz), buf, sz);
		return stream;
	}

	inline FBinaryIO& Deserialize(FBinaryIO& stream, uint8* buf, uint32 sz)
	{
		memcpy(buf, stream.Retrieve(sz), sz);
		return stream;
	}
}