/*
* file FBXEditor.cpp
*
* author luoxw
* date 2017/08/02
*
*
*/

#include "stdafx.h"
#include "RenderCore/Console/StackCounterConsole.h"
#include "RenderCore/UserInterface/FontProvider.h"

#include "LostCore-D3D11.h"
using namespace D3D11;
using namespace LostCore;

class FFBXEditor : public ITickTask
{
	typedef function<void()> FCmd;

public:
	FFBXEditor();
	~FFBXEditor();

	// Inherited via ITickTask
	virtual bool Initialize() override;
	virtual void Tick() override;
	virtual void Destroy() override;

	// 启动editor.
	void InitializeWindow(HWND wnd, bool windowed, int32 width, int32 height);

	// 关闭editor
	void Shutdown();

	void PushCommand(const FCmd& cmd);
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
	void ConfigModel(const FJson& config);
	void LoadAnimation(const string& url);
	void ClearScene();
	void CloseScene();
	void LoadScene(const string& url);
	void SaveScene(const string& url);

	void OnPicking(int32 x, int32 y);
	void OnClick(int32 x, int32 y);
	void OnDragging(int32 x, int32 y);
	void OnEndDrag();

	void Pick(FBasicModel* model);
	void UnPick();
	void Hover(FBasicModel* model);
	void UnHover();

private:
	void InitializeEnvironment();
	void InitializeCallback();
	void InitializeScene();
	void FlushNotifies();

	void Log(ELogFlag level, const char* fmt, ...);

	// TODO: 需要说明RayTest的目的,返回更精确的测试结果.
	FBasicModel* ScreenRayTest(int32 x, int32 y);

	IRenderContext*			RC;
	FBasicCamera*			Camera;
	FBasicScene*			Scene;

	FBasicModel*			CurrSelectedModel;
	FBasicModel*			CurrHoveredModel;

	string OutputDir;

	FCommandQueue<FCmd> TickCommands;

	// Editor tools
	FGizmoOperator* GizmoOp;

	int32 ScreenWidth;
	int32 ScreenHeight;

	FBasicGUI* GUI;
	FStackCounterConsole* Console;

	FTickThread* Thread;

	static const char * const SConverterExe;
	static const char * const SConverterOutput;

};

const char * const FFBXEditor::SConverterExe = "FbxConverter.exe";
const char * const FFBXEditor::SConverterOutput = "FBXEditor/";

FFBXEditor::FFBXEditor()
	: OutputDir("")
	, RC(nullptr)
	, Camera(nullptr)
	, TickCommands(true)
	, CurrSelectedModel(nullptr)
	, CurrHoveredModel(nullptr)
	, GizmoOp(nullptr)
	, GUI(nullptr)
	, Console(nullptr)
	, Thread(nullptr)
{
	InitializeEnvironment();
	InitializeCallback();
}

FFBXEditor::~FFBXEditor()
{
	Destroy();

	assert(RC == nullptr);
	assert(Camera == nullptr);
	assert(Scene == nullptr);
}

void FFBXEditor::InitializeScene()
{
}

void FFBXEditor::Shutdown()
{
	SAFE_DELETE(Thread);
}

void FFBXEditor::PushCommand(const FCmd & cmd)
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
			auto result = FDirectoryHelper::Get()->GetAnimationRelativePath(fileAbs, fileRel);
			assert(result);
			Log(ELogFlag::LogInfo, "Output anim: %s, [%s]", fileRel.c_str(), fileAbs.c_str());

			LoadAnimation(fileRel);
		}

		clearScene |= !importAnimation;

		if (clearScene)
		{
			ClearScene();
		}

		if (!importAnimation)
		{
			for (auto& elem : j[K_MESH])
			{
				string path = elem.value(K_PATH, "");
				ReplaceChar(path, "/", "\\");
				string fileAbs(CopyFileTo(primitiveOutputAbs, path)), fileRel;
				auto result = FDirectoryHelper::Get()->GetPrimitiveRelativePath(fileAbs, fileRel);
				assert(result);
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
			}
		}
	}
}

void FFBXEditor::LoadModel(const string & url)
{
	if (Scene == nullptr)
	{
		return;
	}

	auto model = FModelFactory::NewModel(url);
	if (model != nullptr)
	{
		Scene->AddModel(model);
	}
}

void FFBXEditor::ConfigModel(const FJson & config)
{
}

void FFBXEditor::LoadAnimation(const string & url)
{
	string anim;
	if (FAnimationLibrary::Get()->Load(url, anim))
	{
		FGlobalHandler::Get()->UpdateFlagAndName(EUpdateFlag::UpdateAnimAdd, anim);
	}
	else
	{
		LVERR("", "%s", url);
	}
}

void FFBXEditor::ClearScene()
{
	UnPick();
	UnHover();
	if (Scene != nullptr)
	{
		Scene->ClearModels();
	}
}

void FFBXEditor::CloseScene()
{
	ClearScene();
	SAFE_DELETE(Scene);
}

void FFBXEditor::LoadScene(const string & url)
{
	if (Scene != nullptr)
	{
		SAFE_DELETE(Scene);
	}

	Scene = new FBasicScene;
	Scene->Load(url);

	Camera = Scene->GetCamera();
	Camera->Init(ScreenWidth, ScreenHeight);
}

void FFBXEditor::SaveScene(const string & url)
{
	if (Scene != nullptr)
	{
		Scene->Save(url);
	}
}

void FFBXEditor::OnPicking(int32 x, int32 y)
{
	UnHover();
	Hover(ScreenRayTest(x, y));
}

void FFBXEditor::OnClick(int32 x, int32 y)
{
	UnPick();
	Pick(ScreenRayTest(x, y));
}

void FFBXEditor::OnDragging(int32 x, int32 y)
{
	if (Camera != nullptr && GizmoOp != nullptr)
	{
		GizmoOp->OnDragging(x, y, Camera);
	}
}

void FFBXEditor::OnEndDrag()
{
	if (GizmoOp != nullptr)
	{
		GizmoOp->EndDrag();
	}
}

void FFBXEditor::Pick(FBasicModel * model)
{
	CurrSelectedModel = model;
	if (CurrSelectedModel != nullptr)
	{
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
	if (CurrHoveredModel != nullptr)
	{
		CurrHoveredModel->GetBoundingBox()->bVisible = true;
	}
}

void FFBXEditor::UnHover()
{
	if (CurrHoveredModel != nullptr)
	{
		CurrHoveredModel->GetBoundingBox()->bVisible = false;
		CurrHoveredModel = nullptr;
	}
}

bool FFBXEditor::Initialize()
{
	return true;
}

void FFBXEditor::Tick()
{
	static auto SLastStamp = chrono::system_clock::now();
	auto now = chrono::system_clock::now();
	auto sec = chrono::duration<float>(now - SLastStamp);
	SLastStamp = now;

	FGlobalHandler::Get()->SetFrameTime(sec.count());

	FCmd cmd;
	while (TickCommands.Pop(cmd))
	{
		cmd();
	}

	if (RC != nullptr)
	{
		FlushNotifies();

		RC->FirstCommit();

		if (Camera != nullptr)
		{
			Camera->Tick();
		}

		if (Scene != nullptr)
		{
			Scene->Tick();
		}

		if (GizmoOp != nullptr)
		{
			GizmoOp->Tick();
		}

		if (Console != nullptr)
		{
			Console->FinishCounting();
		}

		if (GUI != nullptr)
		{
			GUI->Tick();
		}

		RC->FinishCommit();

		FFontProvider::Get()->UpdateRes();

		if (Console != nullptr)
		{
			Console->FinishDisplay();
		}
	}
}

void FFBXEditor::Destroy()
{
	FGlobalHandler::Get()->SetMoveCameraCallback(nullptr);
	FGlobalHandler::Get()->SetRotateCameraCallback(nullptr);

	SAFE_DELETE(Console);
	SAFE_DELETE(GUI);

	FFontProvider::Get()->Destroy();

	SAFE_DELETE(GizmoOp);

	CurrSelectedModel = nullptr;
	CurrHoveredModel = nullptr;

	SAFE_DELETE(Scene);
	SAFE_DELETE(Camera);
	SAFE_DELETE(RC);

	FGlobalHandler::Get()->SetRenderContextPP(nullptr);
}

void FFBXEditor::InitializeEnvironment()
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
}

void FFBXEditor::InitializeCallback()
{
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

	FGlobalHandler::Get()->SetPlayAnimationCallback([&]
	(const string& anim)
	{
		if (reinterpret_cast<FSkeletalModel*>(CurrSelectedModel) != nullptr)
		{
			reinterpret_cast<FSkeletalModel*>(CurrSelectedModel)->PlayAnimation(anim);
		}
	});

	FGlobalHandler::Get()->SetInitializeWindowCallback([&]
	(HWND wnd, bool windowed, int32 width, int32 height)
	{
		InitializeWindow(wnd, windowed, width, height);
	});


	FGlobalHandler::Get()->SetAssetOperateCallback([&]
	(int32 op, const char* url)
	{
		string localUrl(url);
		if ((EAssetOperation)op == EAssetOperation::LoadModel)
		{
			PushCommand([=]()
			{
				LoadModel(localUrl);
			});
		}
		else if ((EAssetOperation)op == EAssetOperation::LoadAnimation)
		{
			PushCommand([=]()
			{
				LoadAnimation(localUrl);
			});
		}
		else if ((EAssetOperation)op == EAssetOperation::LoadScene)
		{
			PushCommand([=]()
			{
				CloseScene();
				LoadScene(localUrl);
			});
		}
		else if ((EAssetOperation)op == EAssetOperation::SaveScene)
		{
			PushCommand([=]()
			{
				SaveScene(localUrl);
			});
		}
	});

	FGlobalHandler::Get()->SetLoadFBXCallback([&]
	(const char * file,
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

	FGlobalHandler::Get()->SetClearSceneCallback([&]
	()
	{
		PushCommand([=]()
		{
			ClearScene();
		});
	});

	FGlobalHandler::Get()->SetHoverCallback([&]
	(int32 x, int32 y)
	{
		this->PushCommand([=]()
		{
			this->OnPicking(x, y);
		});
	});

	FGlobalHandler::Get()->SetClickCallback([&]
	(int32 x, int32 y)
	{
		this->PushCommand([=]()
		{
			this->OnClick(x, y);
		});
	});

	FGlobalHandler::Get()->SetDraggingCallback([&]
	(int32 x, int32 y)
	{
		this->PushCommand([=]()
		{
			this->OnDragging(x, y);
		});
	});

	FGlobalHandler::Get()->SetEndDragCallback([&]()
	{
		this->PushCommand([=]()
		{
			this->OnEndDrag();
		});
	});

	FGlobalHandler::Get()->SetShutdownCallback([&]
	()
	{
		Shutdown();
	});

	// TODO: FGlobalHandler应该是进程全局唯一单例，而FStackCounterManager是线程本地单例
	FGlobalHandler::Get()->SetRecordProfileCallback([=]
	()
	{
		this->PushCommand([=]()
		{
			if (Console != nullptr)
			{
				Console->Record();
			}
		});
	});
}

void FFBXEditor::InitializeWindow(HWND wnd, bool windowed, int32 width, int32 height)
{
	if (Thread != nullptr)
	{
		return;
	}

	auto ret = WrappedCreateRenderContext(&RC);
	RC->InitializeDevice(EContextID::D3D11_DXGI0, wnd, windowed, width, height);
	FFontProvider::Get()->Initialize();

	ScreenWidth = width;
	ScreenHeight = height;

	GizmoOp = new FGizmoOperator;
	if (!GizmoOp->Load("axis.json"))
	{
		Log(ELogFlag::LogError, "failed to load gizmo config: %s", "axis.json");
	}


	GUI = new FBasicGUI;
	GUI->Initialize(FFloat2(width, height));

	Console = new FStackCounterConsole;
	Console->Initialize(GUI->GetRoot());

	Thread = new FTickThread(this, "EDitorTick");
}

void FFBXEditor::FlushNotifies()
{
	static FStackCounterRequest SCounter("FFBXEditor::FlushNotifies");
	FScopedStackCounterRequest req(SCounter);

	string name;
	FFloat3 pos, rot;
	if (CurrSelectedModel != nullptr)
	{
		name = CurrSelectedModel->GetPrimitiveData()->Name;
		pos = CurrSelectedModel->GetWorldMatrix().GetOrigin();
		rot = CurrSelectedModel->GetWorldMatrix().GetOrientation().Euler();
	}
	else if (Camera != nullptr)
	{
		name = "Camera";
		pos = Camera->GetViewMatrix().Invert().GetOrigin();
		rot = Camera->GetViewMatrix().Invert().GetOrientation().Euler();
	}

	FGlobalHandler::Get()->UpdateFlagAndName(EUpdateFlag::UpdateMonitorName, name);
	FGlobalHandler::Get()->UpdatePosAndRot(pos, rot);
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

FBasicModel * FFBXEditor::ScreenRayTest(int32 x, int32 y)
{
	//static FStackCounterRequest SCounter("FFBXEditor::ScreenRayTest");
	//FScopedStackCounterRequest req(SCounter);

	if (Camera == nullptr || Scene == nullptr)
	{
		return nullptr;
	}

	// 初始化世界空间射线.
	float minDist = 10000.0f;
	FRay worldRay = Camera->ScreenCastRay(x, y, 1.0f);
	worldRay.Distance = minDist;

	// Gizmo优先测试.
	if (GizmoOp != nullptr && GizmoOp->RayTest(worldRay, true))
	{
		return nullptr;
	}

	auto result = Scene->RayTest(worldRay, minDist);
	FGlobalHandler::Get()->UpdateFlagAnd32Bit(EUpdateFlag::UpdateRayTestDistance, *(uint32*)&minDist);
	return result;
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	FFBXEditor* editor = nullptr;
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		editor = new FFBXEditor;
		break;
	}
	case DLL_PROCESS_DETACH:
	{
		SAFE_DELETE(editor);
		break;
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}