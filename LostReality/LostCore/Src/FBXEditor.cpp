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

	// FBasicWorld overridew 
	virtual bool Config(const FJson& config) override;
	virtual bool Load(const char* url) override;
	virtual void Tick() override;
	virtual void DrawPostScene() override;

	virtual bool InitializeWindow(HWND wnd, bool windowed, int32 width, int32 height) override;
	virtual FBasicCamera* GetCamera() override;
	virtual FBasicScene* GetScene() override;

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
	void Picking(int32 x, int32 y, bool clicked);
	void Pick(FBasicModel* model);
	void UnPick();
	void Hover(FBasicModel* model);
	void UnHover();

private:
	void Fini();
	void InitializeScene();
	void StartRenderLoop();
	void StartTickLoop();

	void Log(ELogFlag level, const char* fmt, ...);

	IRenderContext*			RC;
	FBasicCamera*			Camera;
	FBasicScene*			Scene;

	FBasicModel*			CurrSelectedModel;
	FBasicModel*			CurrHoveredModel;

	string OutputDir;

	int32 ScreenWidth;
	int32 ScreenHeight;
	float RcpScreenWidth;
	float RcpScreenHeight;

	FCommandQueue<CommandType> RenderCommands;
	bool bKeepRendering;
	thread RenderThread;

	// 暂tick也在render loop里.
	FCommandQueue<CommandType> TickCommands;
	bool bKeepTicking;
	thread TickThread;

	// TODO: 需要更有效率的场景树
	vector<FBasicModel*> Ms;

	// Editor tools
	FGizmoOperator* GizmoOp;

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
	, CurrHoveredModel(nullptr)
	, GizmoOp(nullptr)
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
	FGlobalHandler::Get()->SetMoveCameraCallback([&]
	(float x, float y, float z)
	{
		if (Camera != nullptr)
		{
			Camera->AddPositionLocal(FFloat3(x, y, z));
		}
	});

	FGlobalHandler::Get()->SetRotateCameraCallback([&]
	(float p, float y, float r)
	{
		if (Camera != nullptr)
		{
			Camera->AddEulerWorld(FFloat3(p, y, r));
		}
	});

	FGlobalHandler::Get()->SetCallbackPlayAnimation([&]
	(const string& anim)
	{
		if (reinterpret_cast<FSkeletalModel*>(CurrSelectedModel) != nullptr)
		{
			reinterpret_cast<FSkeletalModel*>(CurrSelectedModel)->PlayAnimation(anim);
		}
	});

	FGlobalHandler::Get()->SetCallbackLoadModel([&]
	(const char* url) 
	{
		string localUrl(url);
		PushCommand([=]
		()
		{
			LoadModel(localUrl);
		});
	});

	FGlobalHandler::Get()->SetCallbackInitializeWindow([&]
	(HWND wnd, bool windowed, int32 width, int32 height)
	{
		InitializeWindow(wnd, windowed, width, height);
	});

	FGlobalHandler::Get()->SetCallbackLoadAnimation([&]
	(const char* url)
	{
		string localUrl(url);
		PushCommand([=]
		()
		{
			LoadAnimation(localUrl);
		});
	});

	FGlobalHandler::Get()->SetCallbackLoadFBX([&]
	(	const char * file,
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

	FGlobalHandler::Get()->SetCallbackClearScene([&]
	()
	{
		ClearEditorScene();
	});

	FGlobalHandler::Get()->SetCallbackPicking([&]
	(int32 x, int32 y, bool clicked)
	{
		this->PushCommand([=]() 
		{
			this->Picking(x, y, clicked);
		});
	});

	FGlobalHandler::Get()->SetCallbackShutdown([&]
	()
	{
		this->Fini();
	});
}

FFBXEditor::~FFBXEditor()
{
	Fini();

	assert(RC == nullptr);
	assert(Camera == nullptr);
	assert(Scene == nullptr);
}

void FFBXEditor::InitializeScene()
{
	assert(Camera == nullptr && Scene == nullptr);

	GizmoOp = new FGizmoOperator;
	if (!GizmoOp->Load("axis.json"))
	{
		Log(ELogFlag::LogError, "failed to load gizmo config: %s", "axis.json");
	}

	FBasicWorld::Load("");
	Camera = new FBasicCamera;
	if (!Camera->Load(""))
	{
		return;
	}

	Scene = new FBasicScene;
	if (Scene->Config(FJson()))
	{
		Log(ELogFlag::LogInfo, "InitializeScene finished.");
		return;
	}
	else
	{
		Log(ELogFlag::LogError, "InitializeScene failed.");
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
		Log(ELogFlag::LogError, "Invalid input fbx file path[%s].", file);
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
		Log(ELogFlag::LogError, "Process could not be loaded: cmd[%s].", cmd.c_str());
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		memset(&pi, 0, sizeof(pi));
		return;
	}
	else
	{
		Log(ELogFlag::LogInfo, "Converting fbx[%s].", file);

		WaitForSingleObject(pi.hProcess, INFINITE);

		string primitiveOutputAbs(""), animationOutputAbs("");
		FDirectoryHelper::Get()->GetPrimitiveAbsolutePath(primitiveOutput, primitiveOutputAbs);
		FDirectoryHelper::Get()->GetAnimationAbsolutePath(animationOutput, animationOutputAbs);

		Log(ELogFlag::LogInfo, "Convert fbx[%s] finished, output file should be saved[%s].",
			file, outputFile.c_str());

		ifstream stream(outputFile, ios::in);
		if (stream.fail())
		{
			Log(ELogFlag::LogError, "Output file notfound: %s", outputFile.c_str());
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
			Log(ELogFlag::LogInfo, "Output anim: %s, [%s]", fileRel.c_str(), fileAbs.c_str());

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
				Log(ELogFlag::LogInfo, "Output primitive: %s, [%s]", fileRel.c_str(), fileAbs.c_str());

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
	if (model != nullptr)
	{
		Scene->AddModel(model);
		Ms.push_back(model);
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
		Ms.clear();
		Scene->ClearModels();
	});
}

void FFBXEditor::Picking(int32 x, int32 y, bool clicked)
{
	UnHover();
	if (clicked)
	{
		UnPick();
	}

	if (Camera == nullptr)
	{
		return;
	}

	// Clip space
	auto proj = Camera->GetProjectMatrix();
	FFloat3 rayDir((2 * x * RcpScreenWidth - 1.0f) / proj.M[0][0],
		(1.0f - 2 * y * RcpScreenHeight) / proj.M[1][1], 1.0f);

	// World space
	auto invView = Camera->GetViewMatrix().Invert();
	rayDir = invView.ApplyVector(rayDir);
	auto rayP0 = invView.GetOrigin();

	float minDist = 10000.0f;
	FRay worldRay(rayP0, rayDir, minDist);
	FBasicModel* nearest = nullptr;
	for (auto m : Ms)
	{
		if (m == nullptr || m->GetBoundingBox() == nullptr)
		{
			continue;
		}

		if (RayBoxIntersect(worldRay, *m->GetBoundingBox(), m->GetWorldMatrix().Invert(), minDist))
		{
			nearest = m;
			worldRay.Distance = minDist;
		}
	}

	if (nearest)
	{
		Hover(nearest);
		if (clicked)
		{
			Pick(nearest);
		}
	}
}

void FFBXEditor::Pick(FBasicModel * model)
{
	if (model != nullptr)
	{
		CurrSelectedModel = model;
		GizmoOp->SetTarget(CurrSelectedModel);
	}
}

void FFBXEditor::UnPick()
{
	if (CurrSelectedModel != nullptr)
	{
		GizmoOp->SetTarget(nullptr);
		CurrSelectedModel = nullptr;
	}
}

void FFBXEditor::Hover(FBasicModel * model)
{
	CurrHoveredModel = model;
	CurrHoveredModel->GetBoundingBox()->bVisible = true;
}

void FFBXEditor::UnHover()
{
	if (CurrHoveredModel != nullptr)
	{
		CurrHoveredModel->GetBoundingBox()->bVisible = false;
		CurrHoveredModel = nullptr;
	}
}

bool FFBXEditor::Config(const FJson & config)
{
	return false;
}

bool FFBXEditor::Load(const char * url)
{
	return false;
}

void FFBXEditor::Tick()
{
	FBasicWorld::Tick();
	if (GizmoOp != nullptr)
	{
		GizmoOp->Tick();
	}
}

void FFBXEditor::DrawPostScene()
{
	if (GizmoOp != nullptr)
	{
		GizmoOp->Draw();
	}

	FBasicWorld::DrawPostScene();
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

	SAFE_DELETE(GizmoOp);

	CurrSelectedModel = nullptr;
	CurrHoveredModel = nullptr;

	SAFE_DELETE(RC);
	SAFE_DELETE(Scene);
	SAFE_DELETE(Camera);

	FGlobalHandler::Get()->SetRenderContextPP(nullptr);
}

bool FFBXEditor::InitializeWindow(HWND wnd, bool windowed, int32 width, int32 height)
{
	auto ret = WrappedCreateRenderContext(EContextID::D3D11_DXGI0, &RC);
	if (RC != nullptr && RC->Init(wnd, windowed, width, height))
	{
		ScreenWidth = width;
		ScreenHeight = height;
		RcpScreenWidth = 1.f / width;
		RcpScreenHeight = 1.f / height;
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

FBasicCamera * FFBXEditor::GetCamera()
{
	return Camera;
}

FBasicScene * FFBXEditor::GetScene()
{
	return Scene;
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

void FFBXEditor::Log(ELogFlag level, const char * fmt, ...)
{
	char msg[1024];
	msg[1023] = 0;

	va_list args;
	va_start(args, fmt);
	vsnprintf(msg, 1023, fmt, args);
	va_end(args);

	FGlobalHandler::Get()->Logging((int32)level, msg);
}