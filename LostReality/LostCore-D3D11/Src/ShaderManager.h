/*
* file ShaderManager.h
*
* author luoxw
* date 2017/09/22
*
*
*/

#pragma once

#define NAME_MAX_BONES				"MAX_BONES"
#define NAME_LIT_MODE				"LIT_MODE"
#define NAME_CUSTOM_BUFFER			"CUSTOM_BUFFER"
#define NAME_VERTEX_FLAGS			"VERTEX_FLAGS"
#define NAME_VERTEX_TEXCOORD0		"VERTEX_TEXCOORD0"
#define NAME_VERTEX_NORMAL			"VERTEX_NORMAL"
#define NAME_VERTEX_TANGENT			"VERTEX_TANGENT"
#define NAME_VERTEX_COLOR			"VERTEX_COLOR"
#define NAME_VERTEX_SKIN			"VERTEX_SKIN"
#define NAME_VERTEX_TEXCOORD1		"VERTEX_TEXCOORD1"
#define NAME_VERTEX_COORDINATE3D	"VERTEX_COORDINATE3D"
#define NAME_VERTEX_COORDINATE2D	"VERTEX_COORDINATE2D"
#define NAME_INSTANCE_TRANSFORM3D	"INSTANCE_TRANSFORM3D"
#define NAME_INSTANCE_TRANSFORM2D	"INSTANCE_TRANSFORM2D"
#define NAME_INSTANCE_TEXTILE		"INSTANCE_TEXTILE"

namespace D3D11
{
	enum class ELightingMode : uint8
	{
		UnLit = 0,
		Phong,
	};

	enum class EShaderID : uint8
	{
		Vertex = 0,
		Hull,
		Domain,
		Geometry,
		Compute,
		Pixel,
	};

	struct FShaderKey
	{
		ELightingMode LitMode;
		uint32 VertexElement;
		int32 CustomBufferVersion;
		int32 CodeVersion;

		FShaderKey();

		//bool operator==(const FShaderKey& rhs) const;
		bool operator<(const FShaderKey& rhs) const;
		string ToString() const;
	};

	FORCEINLINE LostCore::FBinaryIO& operator<<(LostCore::FBinaryIO& stream, const FShaderKey& data)
	{
		stream << data.CodeVersion << (uint8)(data.LitMode) << data.VertexElement << data.CustomBufferVersion;
		return stream;
	}

	FORCEINLINE LostCore::FBinaryIO& operator >> (LostCore::FBinaryIO& stream, FShaderKey& data)
	{
		stream >> data.CodeVersion >> (uint8&)data.LitMode >> data.VertexElement >> data.CustomBufferVersion;
		return stream;
	}

	struct FShaderKeyBlobs
	{
		FShaderKey Key;
		map<EShaderID, FBuf> Blobs;

		FShaderKeyBlobs();
		explicit FShaderKeyBlobs(const FShaderKey& key);

		bool operator<(const FShaderKeyBlobs& rhs) const;
		string ToString() const;
	};

	FORCEINLINE LostCore::FBinaryIO& operator<<(LostCore::FBinaryIO& stream, const FShaderKeyBlobs& data)
	{
		stream << data.Key << data.Blobs;
		return stream;
	}

	FORCEINLINE LostCore::FBinaryIO& operator >> (LostCore::FBinaryIO& stream, FShaderKeyBlobs& data)
	{
		stream >> data.Key >> data.Blobs;
		return stream;
	}

	struct FShaderObject
	{
		MEMORY_ALLOC(FShaderObject);

		TRefCountPtr<ID3D11VertexShader> VS;
		TRefCountPtr<ID3D11PixelShader> PS;
		TRefCountPtr<ID3D11InputLayout> IL;

		FShaderObject()
			: VS(nullptr)
			, PS(nullptr)
			, IL(nullptr)
		{
		}

		~FShaderObject()
		{
			VS = nullptr;
			PS = nullptr;
			IL = nullptr;
		}
	};

	class FShaderManager
	{
	public:
		static FShaderManager* Get()
		{
			static FShaderManager Inst;
			return &Inst;
		}

		FShaderManager();
		~FShaderManager();

		void Save();
		void Load();

		// 调用前确保所有shader object不再被使用.
		void Destroy();

		FShaderObject* GetShader(const FShaderKey& key);
		void Bind(const FShaderKey& key);

	private:
		set<FShaderKeyBlobs>::const_iterator Compile(const FShaderKey& key);
		set<FShaderKeyBlobs>::const_iterator GetBlobs(const FShaderKey& key);
		FShaderObject* CreateShaderObject(set<FShaderKeyBlobs>::const_iterator blobIt);

		TRefCountPtr<ID3D11InputLayout> CreateInputLayout(const FBuf& vsBlob, const FShaderKey& key);

		set<FShaderKeyBlobs> KeyBlobs;
		map<FShaderKey, FShaderObject*> ShaderMap;
	};
}