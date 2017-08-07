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
public:
	FFBXEditor();
	~FFBXEditor();

	void InitializeScene();
	void Fini();
	void SetLogger(PFN_Logger logger);
	void SetOutputDirectory(const char* output);
	void LoadFBX(const char* file, bool clearScene);

	// FBasicWorld overridew 
	virtual bool Config(IRenderContext * rc, const FJson& config) override;
	bool Load(IRenderContext * rc, const char* url) override;

	virtual bool InitializeWindow(const char* name, HWND wnd, bool windowed, int32 width, int32 height) override;
	virtual IRenderContext* GetRenderContext() override;
	virtual FBasicCamera* GetCamera() override;


private:
	void Log(int32 level, const char* fmt, ...);

	IRenderContext*			RC;
	FBasicCamera*			Camera;
	FBasicScene*			Scene;

	string OutputDir;
	PFN_Logger Logger;

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
{
	FDirectoryHelper::Get()->GetPrimitiveAbsolutePath(SConverterOutput, OutputDir);
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

void FFBXEditor::SetLogger(PFN_Logger logger)
{
	Logger = logger;
}

void FFBXEditor::SetOutputDirectory(const char * output)
{
	OutputDir = output;
}

void FFBXEditor::LoadFBX(const char * file, bool clearScene)
{
	if (file == nullptr)
	{
		Log(SError, "Invalid input fbx file path[%s].", file);
		return;
	}

	string fileName, fileExt, fileAbsPath;
	GetFileName(fileName, fileExt, file);
	fileAbsPath = OutputDir + fileName + "." + fileExt;

	
	string cmd;
	cmd = "\"" + GetCurrentWorkingPath() + string(SConverterExe) + "\" src=" + string(file) + " dst=" + fileAbsPath;

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

	if (!CreateProcessA(NULL, const_cast<char*>(cmd.c_str()), &psec, &tsec, false, 0, NULL, GetCurrentWorkingPath().c_str(), &si, &pi))
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

		int32 retry = 6000;
		DWORD code = STILL_ACTIVE;
		while (retry-- > 0 && code == STILL_ACTIVE)
		{
			GetExitCodeProcess(pi.hProcess, &code);
		}

		Log(SInfo, "Convert fbx[%s] finished, output file should be saved[%s].", file, fileAbsPath.c_str());
		Log(SInfo, "Loading fbx[%s].", fileAbsPath.c_str());

		if (clearScene)
		{
			// Clear scene
		}

		FJson modelJson;
		modelJson["type"] = (uint32)EPrimitiveType::PrimitiveFile;
		modelJson["primitive"] = fileAbsPath;
		modelJson["material_prefix"] = "default";

		auto model = new FSkeletalModel;

		// Fbx converter需要输出转换导出信息。
		model->SetPrimitiveVertexFlags(EVertexElement::Coordinate | EVertexElement::Normal | EVertexElement::Skin);

		if (model->Config(RC, modelJson))
		{
			Scene->AddModel(model);
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

LOSTCORE_API void LostCore::SetOutputDirectory(const char * output)
{
	SEditor.SetOutputDirectory(output);
}

LOSTCORE_API void LostCore::InitializeWindow(HWND wnd, bool windowed, int32 width, int32 height)
{
	SEditor.InitializeWindow("", wnd, windowed, width, height);
}

LOSTCORE_API void LostCore::LoadFBX(const char * file, bool clearScene)
{
	SEditor.LoadFBX(file, clearScene);
}

LOSTCORE_API void LostCore::Tick()
{
	SEditor.Tick(0.f);
	SEditor.Draw(nullptr, 0.f);
}
