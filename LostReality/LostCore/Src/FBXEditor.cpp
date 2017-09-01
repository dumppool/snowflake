/*
* file FBXEditor.cpp
*
* author luoxw
* date 2017/08/02
*
*
*/

#include "stdafx.h"
#include "FBXEditor.h"
#include "BasicWorld.h"

#include "LostCore-D3D11.h"
using namespace D3D11;
using namespace LostCore;

class FFBXEditor : public FBasicWorld
{
	typedef function<void()> CommandType;

public:
	FFBXEditor();
	~FFBXEditor();

	void PushCommands(const CommandType& cmd);
	void SetLogger(PFN_Logger logger);
	void SetOutputDirectory(const char* output);
	void LoadFBX(
		const char * file,
		bool clearScene,
		bool importTexCoord,
		bool importAnimation,
		bool importVertexColor,
		bool mergeNormalTangentAll,
		bool importNormal,
		bool forceRegenerateNormal,
		bool generateNormalIfNotFound,
		bool importTangent,
		bool forceRegenerateTangent,
		bool generateTangentIfNotFound);

	// FBasicWorld overridew 
	virtual bool Config(IRenderContext * rc, const FJson& config) override;
	bool Load(IRenderContext * rc, const char* url) override;

	virtual bool InitializeWindow(const char* name, HWND wnd, bool windowed, int32 width, int32 height) override;
	virtual IRenderContext* GetRenderContext() override;
	virtual FBasicCamera* GetCamera() override;


private:
	void Fini();
	void InitializeScene();
	void StartRenderLoop();
	void StartTickLoop();

	void Log(int32 level, const char* fmt, ...);

	IRenderContext*			RC;
	FBasicCamera*			Camera;
	FBasicScene*			Scene;

	FBasicModel*			CurrSelectedModel;

	string OutputDir;
	PFN_Logger Logger;

	FCommandQueue<CommandType> RenderCommands;
	bool bKeepRendering;
	thread RenderThread;

	// 暂tick也在render loop里.
	FCommandQueue<CommandType> TickCommands;
	bool bKeepTicking;
	thread TickThread;

	static const int32 SInfo = 0;
	static const int32 SWarning = 1;
	static const int32 SError = 2;

	static const char * const SConverterExe;
	static const char * const SConverterOutput;
};

const char * const FFBXEditor::SConverterExe = "FbxConverter.exe";
const char * const FFBXEditor::SConverterOutput = "FBXEditor/";

static FFBXEditor SEditor;

FFBXEditor::FFBXEditor()
	: OutputDir("")
	, Logger(nullptr)
	, RC(nullptr)
	, Camera(nullptr)
	, TickCommands(true)
	, RenderCommands(true)
	, CurrSelectedModel(nullptr)
{
	FDirectoryHelper::Get()->GetPrimitiveAbsolutePath(SConverterOutput, OutputDir);
	FGlobalHandler::Get()->SetMoveCameraCallback([&](float x, float y, float z) {
		if (Camera != nullptr)
		{
			Camera->AddPositionLocal(FFloat3(x, y, z));
		}
	});

	FGlobalHandler::Get()->SetRotateCameraCallback([&](float p, float y, float r) {
		if (Camera != nullptr)
		{
			Camera->AddEulerWorld(FFloat3(p, y, r));
		}
	});

	FGlobalHandler::Get()->SetCallbackPlayAnimation([&](const string& anim) {
		if (reinterpret_cast<FSkeletalModel*>(CurrSelectedModel) != nullptr)
		{
			reinterpret_cast<FSkeletalModel*>(CurrSelectedModel)->PlayAnimation(anim);
		}
	});
}

FFBXEditor::~FFBXEditor()
{
	Fini();

	assert(RC == nullptr);
	assert(Camera == nullptr);
	assert(Scene == nullptr);
	assert(CurrSelectedModel == nullptr);
}

void FFBXEditor::InitializeScene()
{
	assert(Camera == nullptr && Scene == nullptr);

	FBasicWorld::Load(RC, "");
	Camera = new FBasicCamera;
	if (!Camera->Load(RC, ""))
	{
		return;
	}

	Scene = new FBasicScene;
	if (Scene->Config(RC, FJson()))
	{
		AddScene(Scene);
		Log(SInfo, "InitializeScene finished.");
		return;
	}
	else
	{
		Log(SError, "InitializeScene failed.");
		return;
	}
}

void FFBXEditor::PushCommands(const CommandType & cmd)
{
	TickCommands.Push(cmd);
}

void FFBXEditor::SetLogger(PFN_Logger logger)
{
	Logger = logger;
}

void FFBXEditor::SetOutputDirectory(const char * output)
{
	OutputDir = output;
}

void FFBXEditor::LoadFBX(
	const char * file, 
	bool clearScene,
	bool importTexCoord,
	bool importAnimation,
	bool importVertexColor,
	bool mergeNormalTangentAll,
	bool importNormal,
	bool forceRegenerateNormal,
	bool generateNormalIfNotFound,
	bool importTangent,
	bool forceRegenerateTangent,
	bool generateTangentIfNotFound)
{
	if (file == nullptr)
	{
		Log(SError, "Invalid input fbx file path[%s].", file);
		return;
	}

	string fileName, fileExt, outputFile;
	GetFileName(fileName, fileExt, file);
	outputFile = OutputDir + fileName + ".json";
	
	string cmd;
	cmd = string("\"").append(GetCurrentWorkingPath()).append(SConverterExe).append("\" ")
		.append(K_INPUT_PATH).append("=").append(file).append(" ")
		.append(K_OUTPUT_PATH).append("=").append(outputFile);

	if (importTexCoord)
	{
		cmd.append(" ").append(K_IMP_TEXCOORD);
	}

	if (importAnimation)
	{
		cmd.append(" ").append(K_IMP_ANIM);
	}

	if (importVertexColor)
	{
		cmd.append(" ").append(K_IMP_VERTEXCOLOR);
	}

	if (mergeNormalTangentAll)
	{
		cmd.append(" ").append(K_MERGE_NORMAL_TANGENT_ALL);
	}

	if (importNormal)
	{
		cmd.append(" ").append(K_IMP_NORMAL);
	}

	if (forceRegenerateNormal)
	{
		cmd.append(" ").append(K_FORCE_GEN_NORMAL);
	}

	if (generateNormalIfNotFound)
	{
		cmd.append(" ").append(K_GEN_NORMAL_IF_NOT_FOUND);
	}

	if (importTangent)
	{
		cmd.append(" ").append(K_IMP_TANGENT);
	}

	if (forceRegenerateTangent)
	{
		cmd.append(" ").append(K_FORCE_GEN_TANGENT);
	}

	if (generateTangentIfNotFound)
	{
		cmd.append(" ").append(K_GEN_TANGENT_IF_NOT_FOUND);
	}

	PROCESS_INFORMATION pi;
	STARTUPINFOA si;
	SECURITY_ATTRIBUTES psec;
	SECURITY_ATTRIBUTES tsec;

	ZeroMemory(&pi, sizeof(pi));
	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&psec, sizeof(psec));
	ZeroMemory(&tsec, sizeof(tsec));

	psec.nLength = sizeof(psec);
	tsec.nLength = sizeof(tsec);

	if (!CreateProcessA(NULL, const_cast<char*>(cmd.c_str()), 
		&psec, &tsec, false, 0, NULL, GetCurrentWorkingPath().c_str(), &si, &pi))
	{
		Log(SError, "Process could not be loaded: cmd[%s].", cmd.c_str());
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		memset(&pi, 0, sizeof(pi));
		return;
	}
	else
	{
		Log(SInfo, "Converting fbx[%s].", file);

		WaitForSingleObject(pi.hProcess, INFINITE);

		Log(SInfo, "Convert fbx[%s] finished, output file should be saved[%s].",
			file, outputFile.c_str());

		ifstream stream(outputFile, ios::in);
		if (stream.fail())
		{
			Log(SError, "Output file notfound: %s", outputFile.c_str());
			return;
		}

		FJson j;
		stream >> j;
		stream.close();

		FJson config;
		for (auto& elem : j[K_MESH])
		{
			Log(SInfo, "Output mesh: %s, %s", elem.value(K_PATH, "").c_str(), 
				GetVertexDetails(elem[K_VERTEX_ELEMENT]).Name.c_str());
		}

		for (auto& elem : j[K_ANIMATION])
		{
			auto path = elem[K_PATH];
			Log(SInfo, "Output anim: %s", elem.value(K_PATH, "").c_str());

			vector<string> anims;
			string animName;
			if (FAnimationLibrary::Get()->Load(path, animName))
			{
				anims.push_back(animName);
			}

			for (auto& anim : anims)
			{
				FGlobalHandler::Get()->AddAnimation(anim.c_str());
			}
		}

		clearScene |= !importAnimation;

		if (clearScene)
		{
			// RemoveModel只是从场景移除model，不释放内存.
			Scene->RemoveModel(CurrSelectedModel);
			SAFE_DELETE(CurrSelectedModel);
		}

		if (!importAnimation)
		{
			for (auto& elem : j[K_MESH])
			{
				string path = elem[K_PATH];
				uint32 flag = elem[K_VERTEX_ELEMENT];
				config["type"] = (uint32)EPrimitiveType::PrimitiveFile;
				config["primitive"] = path;
				config["material_prefix"] = "default";
				FBasicModel* model = nullptr;
				if ((flag & EVertexElement::Skin) == EVertexElement::Skin)
				{
					model = new FSkeletalModel;
				}
				else
				{
					model = new FStaticModel;
				}

				assert(CurrSelectedModel == nullptr);
				CurrSelectedModel = model;
				if (model->Config(RC, config))
				{
					Scene->AddModel(model);
				}
			}
		}
	}
}

bool FFBXEditor::Config(IRenderContext * rc, const FJson & config)
{
	return false;
}

bool FFBXEditor::Load(IRenderContext * rc, const char * url)
{
	return false;
}

void FFBXEditor::Fini()
{
	FGlobalHandler::Get()->SetMoveCameraCallback(nullptr);
	FGlobalHandler::Get()->SetRotateCameraCallback(nullptr);

	bKeepRendering = false;
	if (RenderThread.joinable())
	{
		RenderThread.join();
	}

	bKeepTicking = false;
	if (TickThread.joinable())
	{
		TickThread.join();
	}

	CurrSelectedModel = nullptr;

	SAFE_DELETE(RC);
	SAFE_DELETE(Camera);
	SAFE_DELETE(Scene);
}

bool FFBXEditor::InitializeWindow(const char * name, HWND wnd, bool windowed, int32 width, int32 height)
{
	auto ret = WrappedCreateRenderContext(EContextID::D3D11_DXGI0, &RC);
	if (RC != nullptr && RC->Init(wnd, windowed, width, height))
	{
		InitializeScene();
		RenderThread = thread([=]() {this->StartRenderLoop(); });
		//TickThread = thread([=]() {this->StartTickLoop(); });
		return true;
	}
	else
	{
		return false;
	}
}

IRenderContext * FFBXEditor::GetRenderContext()
{
	return RC;
}

FBasicCamera * FFBXEditor::GetCamera()
{
	return Camera;
}

void FFBXEditor::StartRenderLoop()
{
	bKeepRendering = true;
	while (bKeepRendering) 
	{
		static auto SLastStamp = chrono::system_clock::now();
		auto now = chrono::system_clock::now();
		auto sec = chrono::duration<float>(now - SLastStamp);
		SLastStamp = now;

		RenderCommands.Swap(TickCommands);
		CommandType cmd;
		while (RenderCommands.Pop(cmd))
		{
			cmd();
		}

		
		Tick(sec.count());
		Draw(RC, sec.count());

		this_thread::sleep_for(chrono::milliseconds(1));
	}
}

void FFBXEditor::StartTickLoop()
{
	bKeepTicking = true;
	while (bKeepTicking)
	{
		CommandType cmd;
		while (RenderCommands.Pop(cmd))
		{
			cmd();
		}

		Tick(0.f);
	}
}

void FFBXEditor::Log(int32 level, const char * fmt, ...)
{
	if (Logger == nullptr)
	{
		return;
	}

	char msg[1024];
	msg[1023] = 0;

	va_list args;
	va_start(args, fmt);
	vsnprintf(msg, 1023, fmt, args);
	va_end(args);

	Logger(level, msg);
}

LOSTCORE_API void LostCore::SetLogger(PFN_Logger logger)
{
	SEditor.SetLogger(logger);
}

LOSTCORE_API void LostCore::InitializeWindow(HWND wnd, bool windowed, int32 width, int32 height)
{
	SEditor.InitializeWindow("", wnd, windowed, width, height);
}

LOSTCORE_API void LostCore::LoadFBX(
	const char * file, 
	bool clearScene,
	bool importTexCoord,
	bool importAnimation,
	bool importVertexColor,
	bool mergeNormalTangentAll,
	bool importNormal,
	bool forceRegenerateNormal,
	bool generateNormalIfNotFound,
	bool importTangent,
	bool forceRegenerateTangent,
	bool generateTangentIfNotFound)
{
	string filePath(file);
	SEditor.PushCommands([=]()
	{
		SEditor.LoadFBX(
			filePath.c_str(),
			clearScene,
			importTexCoord,
			importAnimation,
			importVertexColor,
			mergeNormalTangentAll,
			importNormal,
			forceRegenerateNormal,
			generateNormalIfNotFound,
			importTangent,
			forceRegenerateTangent,
			forceRegenerateTangent);
	});
}
