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

	void PushCommand(const CommandType& cmd);
	void LoadFBX(
		const char * file,
		const char* primitiveOutput,
		const char* animationOutput,
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

	void LoadModel(const string& url);
	void LoadModel(const FJson& config);
	void LoadAnimation(const string& url);
	void ClearEditorScene();

	// FBasicWorld overridew 
	virtual bool Config(const FJson& config) override;
	bool Load(const char* url) override;

	virtual bool InitializeWindow(HWND wnd, bool windowed, int32 width, int32 height) override;
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

	FCommandQueue<CommandType> RenderCommands;
	bool bKeepRendering;
	thread RenderThread;

	// ‘›tick“≤‘⁄render loop¿Ô.
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
	, RC(nullptr)
	, Camera(nullptr)
	, TickCommands(true)
	, RenderCommands(true)
	, CurrSelectedModel(nullptr)
{
	char* temp = nullptr;
	size_t sz = 0;
	if (!_dupenv_s(&temp, &sz, "APPDATA") == 0 || temp == nullptr)
	{
		::MessageBoxA(0, "FFBXEditor", "failed to get environment path", 0);
		return;
	}

	OutputDir = string(temp).append("\\_LostReality\\");
	free(temp);

	FGlobalHandler::Get()->SetRenderContextPP(&RC);

	//FDirectoryHelper::Get()->GetPrimitiveAbsolutePath(SConverterOutput, OutputDir);
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

	FGlobalHandler::Get()->SetCallbackLoadModel([&](const char* url) {
		string localUrl(url);
		PushCommand([=]() {
			LoadModel(localUrl);
		});
	});

	FGlobalHandler::Get()->SetCallbackInitializeWindow([&](HWND wnd, bool windowed, int32 width, int32 height) {
		InitializeWindow(wnd, windowed, width, height);
	});

	FGlobalHandler::Get()->SetCallbackLoadAnimation([&](const char* url) {
		string localUrl(url);
		PushCommand([=]() {
			LoadAnimation(localUrl);
		});
	});

	FGlobalHandler::Get()->SetCallbackLoadFBX([&](
		const char * file,
		const char* primitiveOutput,
		const char* animationOutput,
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
		bool generateTangentIfNotFound) {
		string filePath(file), primPath(primitiveOutput), animPath(animationOutput);
		this->PushCommand([=]()
		{
			LoadFBX(
				filePath.c_str(),
				primPath.c_str(),
				animPath.c_str(),
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
	});

	FGlobalHandler::Get()->SetCallbackClearScene([&]() {
		ClearEditorScene();
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

	FBasicWorld::Load("");
	Camera = new FBasicCamera;
	if (!Camera->Load(""))
	{
		return;
	}

	Scene = new FBasicScene;
	if (Scene->Config(FJson()))
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

void FFBXEditor::PushCommand(const CommandType & cmd)
{
	TickCommands.Push(cmd);
}

void FFBXEditor::LoadFBX(
	const char * file,
	const char* primitiveOutput,
	const char* animationOutput,
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

		string primitiveOutputAbs(""), animationOutputAbs("");
		FDirectoryHelper::Get()->GetPrimitiveAbsolutePath(primitiveOutput, primitiveOutputAbs);
		FDirectoryHelper::Get()->GetAnimationAbsolutePath(animationOutput, animationOutputAbs);

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

		for (auto& elem : j[K_ANIMATION])
		{
			string path = elem.value(K_PATH, "");
			ReplaceChar(path, "/", "\\");
			string fileAbs(CopyFileTo(animationOutputAbs, path)), fileRel;
			assert(FDirectoryHelper::Get()->GetAnimationRelativePath(fileAbs, fileRel));
			Log(SInfo, "Output anim: %s, [%s]", fileRel.c_str(), fileAbs.c_str());

			LoadAnimation(fileRel);
		}

		clearScene |= !importAnimation;

		if (clearScene)
		{
			ClearEditorScene();
		}

		if (!importAnimation)
		{
			for (auto& elem : j[K_MESH])
			{
				string path = elem.value(K_PATH, "");
				ReplaceChar(path, "/", "\\");
				string fileAbs(CopyFileTo(primitiveOutputAbs, path)), fileRel;
				assert(FDirectoryHelper::Get()->GetPrimitiveRelativePath(fileAbs, fileRel));
				Log(SInfo, "Output primitive: %s, [%s]", fileRel.c_str(), fileAbs.c_str());

				FJson config;
				config[K_VERTEX_ELEMENT] = (uint32)elem[K_VERTEX_ELEMENT];
				config["primitive"] = fileRel;
				config[K_AUTO] = "default";

				string modelFileAbs, modelFileName;
				GetDirectory(modelFileAbs, fileRel);
				GetFileName(modelFileName, string(), fileRel);
				modelFileAbs.append(modelFileName).append(".json");
				FDirectoryHelper::Get()->GetModelAbsolutePath(modelFileAbs, modelFileAbs);
				SaveJson(config, modelFileAbs);

				LoadModel(config);
			}
		}
	}
}

void FFBXEditor::LoadModel(const string & url)
{
	FJson config;
	if (FDirectoryHelper::Get()->GetModelJson(url, config))
	{
		LoadModel(config);
	}
}

void FFBXEditor::LoadModel(const FJson & config)
{
	auto model = FModelFactory::NewModel(config);
	CurrSelectedModel = model;
	if (model != nullptr)
	{
		Scene->AddModel(model);
	}
}

void FFBXEditor::LoadAnimation(const string & url)
{
	string anim;
	if (FAnimationLibrary::Get()->Load(url, anim))
	{
		FGlobalHandler::Get()->AddAnimation(anim);
	}
	else
	{
		LVERR("", "%s", url);
	}
}

void FFBXEditor::ClearEditorScene()
{
	PushCommand([=]() {
		Scene->ClearModels();
	});
}

bool FFBXEditor::Config(const FJson & config)
{
	return false;
}

bool FFBXEditor::Load(const char * url)
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

	FGlobalHandler::Get()->SetRenderContextPP(nullptr);
}

bool FFBXEditor::InitializeWindow(HWND wnd, bool windowed, int32 width, int32 height)
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

		FGlobalHandler::Get()->SetFrameTime(sec.count());

		RenderCommands.Swap(TickCommands);
		CommandType cmd;
		while (RenderCommands.Pop(cmd))
		{
			cmd();
		}

		
		Tick();
		Draw();

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

		Tick();
	}
}

void FFBXEditor::Log(int32 level, const char * fmt, ...)
{
	char msg[1024];
	msg[1023] = 0;

	va_list args;
	va_start(args, fmt);
	vsnprintf(msg, 1023, fmt, args);
	va_end(args);

	FGlobalHandler::Get()->Logging(level, msg);
}