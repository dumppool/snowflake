#pragma once

#include <map>

namespace lostvr
{
	typedef int EEye;

	class IRenderer
	{
	public:
		virtual bool Init() = 0;
		virtual void Fini() = 0;
		virtual bool OnPresent() = 0;
	};

	class ICapture
	{
	public:
		virtual bool Init() = 0;
		virtual void Fini() = 0;
	};

	class ITextureProjector
	{
	public:
		virtual bool Init() = 0;
		virtual void Fini() = 0;
		virtual bool SetSourceRef(void* InBuf) = 0;
		virtual bool OnPresent() = 0;
		virtual void* GetFinalBuffer(EEye InEye) = 0;
	};

	enum class EInitResult : uint8
	{
		Success,
	};

	class IVRDevice
	{
	public:
		virtual bool Startup() = 0;
		virtual void Shutdown() = 0;
		virtual bool OnPresent(void* InBuf) = 0;
		virtual bool IsConnected() = 0;

		virtual bool GetDeviceRecommendSize(uint32& Width, uint32& Height) = 0;
	};

	class LostVR
	{
	public:
		static LostVR* Get()
		{
			static LostVR Inst;
			return &Inst;
		}

		LostVR();
		~LostVR();

		void RegisterVRDevice(const std::string& InKey, IVRDevice* InInst);
		//void RegisterProjector(const CHAR* InKey, const IRenderer* InInst);

		IVRDevice* GetDevice() const;
		IVRDevice* GetDevice(const std::string& InKey) const;

	protected:
		typedef std::map<const std::string, IVRDevice*> KeyDeviceMap;
		KeyDeviceMap VRDevices;

		//typedef std::map<const CHAR*, const IRenderer*> KeyProjectorMap;
		//KeyProjectorMap Projectors;
	};
}