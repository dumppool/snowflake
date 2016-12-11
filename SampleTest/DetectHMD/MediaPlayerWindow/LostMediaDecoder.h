/*
* file LostMediaDecoder.h
*
* author luoxw
* date 2016/12/8
*
*
*/

#pragma once

#ifdef LOSTMEDIA
#define LOSTMEDIA_API __declspec(dllexport)
#else
#define LOSTMEDIA_API __declspec(dllimport)
#endif

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
}

#ifdef LOSTMEDIA
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "avfilter.lib")
#pragma comment(lib, "swscale.lib")
#pragma comment(lib, "swresample.lib")
#endif

#include <thread>
#include <functional>
#include <chrono>
#include <string>
#include <deque>
#include <mutex>

enum class EDecodeCommand : uint8
{
	UnDefined,
	Play,
	Seek,
	Pause,
	Stop,
};

enum class EDecodeState : uint8
{
	UnDefined,
	PreOpened,
	Opening,
	Waiting,
	Playing,
	Seeking,
	Stopped,
};

enum class EDecodeEvent : uint8
{
	UnDefined,
	Opened,
	OpenFailed,
	Playing,
	Seeking,
	Paused,
	Closed,
	DroppedOneFrame,
};

class FLostMediaInfo
{
public:
	uint32 AudioChannels;
	uint32 AudioSampleRate;
	uint32 VideoFrameWidth;
	uint32 VideoFrameHeight;
	uint32 VideoBitRate;
	float VideoFrameRate;

	float Duration;
	char Url[256];

	const char* ToString() const
	{
		static char SInfo[1024];
		snprintf(SInfo, 1023,
			"\nurl:\t\t%s\n" \
			"duration:\t\t%.2f(min)\n" \
			"frame width:\t%d\n" \
			"frame height:\t%d\n" \
			"frame rate:\t\t%.1f\n" \
			"video bitrate:\t%d(k)\n",
			&Url[0], Duration/(60*1000000), VideoFrameWidth, VideoFrameHeight, VideoFrameRate, VideoBitRate/(1<<10));
		return &SInfo[0];
	}
};

class IDecodeCallback
{
public:
	virtual bool NeedNewFrame(uint32 texWidth, uint32 texHeight, int32 format) = 0;
	virtual void ProcessVideoFrame(const uint8* buf, uint32 sz) = 0;
	virtual void OnEvent(EDecodeEvent event) = 0;
};

//typedef std::shared_ptr<IDecodeCallback> IDecodeCallbackPtr;
//typedef std::weak_ptr<IDecodeCallback> IDecodeCallbackWeakPtr;

typedef IDecodeCallback* IDecodeCallbackPtr;
typedef IDecodeCallback* IDecodeCallbackWeakPtr;

inline IDecodeCallback* ConvertPtr(IDecodeCallbackWeakPtr ptr)
{
	return ptr;
}

class FLostMediaDecoder
{
public:
	FLostMediaDecoder();
	~FLostMediaDecoder();

	bool OpenUrl(const char* url);

	void EnqueueCommand(EDecodeCommand cmd);
	EDecodeCommand DequeueCommand();
	void SetCallback(IDecodeCallbackWeakPtr callback);

	void SetRate(float rate);
	float GetRate() const;

	void SetLooping(bool looping);
	bool GetLooping() const;

	void Seek(double pos);
	double GetPos() const;
	EDecodeState GetState() const;

	const FLostMediaInfo* GetMediaInfo() const;

protected:
	void Update_DecodeThread();
	bool Open_DecodeThread(const char* url);
	void Close_DecodeThread();

	// return true if a video frame has been decoded
	bool Decode_DecodeThread(bool bDrop = false);

	AVFormatContext*	VideoFormat;
	AVCodec*			VideoDecoder;
	AVCodecContext*		VideoContext;
	AVStream*			VideoStream;
	AVFrame*			VideoFrame;
	int32				VideoStreamIndex;

	uint8*				YUVBuffer;

	std::string			UrlToPlay;

	FLostMediaInfo		MediaInfo;
	IDecodeCallbackWeakPtr DecodeCallback;

	std::deque<EDecodeCommand> Commands;
	std::mutex			CommandLock;

	EDecodeState		State;
	bool				bLooping;

	bool				bQuit;
	bool				bRunning;

	double				PlayPos;
	double				FrameTime;

	std::thread			BackgroundTask;
};

typedef std::shared_ptr<FLostMediaDecoder> DecoderHandle;
	
LOSTMEDIA_API DecoderHandle DecodeMedia(IDecodeCallbackWeakPtr callback, const char* url);
LOSTMEDIA_API void ReleaseDecoder(DecoderHandle* handle);