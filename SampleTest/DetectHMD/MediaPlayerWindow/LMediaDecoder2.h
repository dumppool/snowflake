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
#include <libavresample/avresample.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
}

#ifdef LOSTMEDIA
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "avfilter.lib")
#pragma comment(lib, "swscale.lib")
#pragma comment(lib, "avresample.lib")
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
	DroppedOneFrame,
	InfoUpdated,

	// This decoder is designed to work for only one media, if user
	// want to open another media, should create a new one. So there is
	// no point to send back the 'Closed' event.
	Closed,
};

class FAudioParams
{
public:
	AVSampleFormat		Format;
	uint32				Channels;
	uint32				SampleRate;
	int64				ChannelLayout;
	int32				LineSize;
	int32				FrameSize;
	int32				BytesPerSec;

	char				Info[1024];

	const char* ToString();
};

class FLostMediaInfo
{
public:
	int32				AudioCodecID;
	char				AudioCodecShortName[256];
	char				AudioCodecLongName[1024];

	FAudioParams		AudioParamsSrc;
	FAudioParams		AudioParamsDst;

	uint32				VideoFrameWidth;
	uint32				VideoFrameHeight;
	uint32				VideoBitRate;
	float				VideoFrameRate;
	int32				VideoCodecID;
	char				VideoCodecShortName[256];
	char				VideoCodecLongName[256];

	float				Duration;
	char				Url[256];

	char				Info[1024];

	void UpdateLongName();

	const char* ToString();
};

enum class EDecodeMsgLv : uint8
{
	UnDefined,
	Info,
	Error,
};

class IDecodeCallback
{
public:
	virtual bool NeedNewFrame(uint32 texWidth, uint32 texHeight, int32 format) = 0;
	virtual void ProcessVideoFrame(const uint8* buf, uint32 sz) = 0;
	virtual void ProcessAudioFrame(const uint8* buf, uint32 sz, int32 rate) = 0;
	virtual void OnEvent(EDecodeEvent event) = 0;
	virtual void OnMessage(EDecodeMsgLv level, const CHAR* buf, uint32 sz) = 0;
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

	FLostMediaInfo* GetMediaInfo();

protected:
	void Update_DecodeThread();
	bool Open_DecodeThread(const char* url);
	void Close_DecodeThread();

	// return true if a video frame has been decoded
	bool Decode_DecodeThread(double& decodeTime, bool bDrop = false);

	int32 AudioResample_DecodeThread(AVFrame* aframe);

	AVFormatContext*	MediaFormat;

	AVCodec*			VideoDecoder;
	AVCodecContext*		VideoContext;
	AVStream*			VideoStream;
	AVFrame*			VideoFrame;
	int32				VideoStreamIndex;

	AVCodec*			AudioDecoder;
	AVCodecContext*		AudioContext;
	AVStream*			AudioStream;
	AVFrame*			AudioFrame;
	int32				AudioStreamIndex;

	AVAudioResampleContext* AudioResampleCxt;

	uint8*				YUVBuffer;
	uint8*				AudioBuffer;
	uint32				AudioBufferSize;

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