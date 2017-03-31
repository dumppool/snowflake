/*
* file FLMediaDecoder.h
*
* author luoxw
* date 2017/3/31
*
*
*/

#pragma once

#ifdef LOSTMEDIA
#define LOSTMEDIA_API __declspec(dllexport)
#else
#define LOSTMEDIA_API __declspec(dllimport)
#endif

#define USE_AUDIOBUFFER2

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavcodec/avfft.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
#include <SDL.h>
#include <SDL_thread.h>
}

#ifdef LOSTMEDIA
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "avfilter.lib")
#pragma comment(lib, "swscale.lib")
#pragma comment(lib, "swresample.lib")
#pragma comment(lib, "SDL.lib")
#endif

//#include <thread>
//#include <functional>
//#include <chrono>
//#include <string>
//#include <deque>
//#include <mutex>

namespace LDecoder
{
	static const int32 SVideoPictureQueueSize = 3;
	static const int32 SSubPictureQueueSize = 16;
	static const int32 SSampleQueueSize = 9;
	static const int32 SFrameQueueSize = 16;
	static const int32 SSampleArraySize = 8 * 65536;

	struct FAVPacketList
	{
		AVPacket Packet;
		FAVPacketList* Next;
		int32 Serial;
	};

	struct FPacketQueue
	{
		FAVPacketList* First;
		FAVPacketList* Last;
		int32 Num;
		int32 Size;
		int32 Serial;
		bool bAbort;
		SDL_mutex* Mutex;
		SDL_cond* Cond;
	};

	struct FAudioAttribute
	{
		int32 Freq;
		int32 Channels;
		int64 ChannelLayout;
		AVSampleFormat Format;
		int32 FrameSize;
		int32 BytesPerSec;
	};

	struct FClock
	{
		double Pts;
		double PtsDrift;
		double LastUpdated;
		double Speed;
		int32 Serial;
		bool bPaused;
		int32* QueueSerial;
	};

	struct FFrame
	{
		AVFrame* Frame;
		AVSubtitle Sub;
		AVSubtitleRect** SubRects;
		int32 Serial;
		double Pts;
		double Duration;
		int64 Pos;
		//SDL_Overlay* Bmp;
		bool bAllocated;
		bool bReallocate;
		int32 Width;
		int32 Height;
		AVRational Sar;
	};

	struct FFrameQueue
	{
		FFrame Queue[SFrameQueueSize];
		int32 ReadIndex;
		int32 WriteIndex;
		int32 Index;
		int32 Size;
		int32 MaxSize;
		bool bKeepLast;
		int32 ReadIndexShown;
		SDL_mutex* Mutex;
		SDL_cond* Cond;
		FPacketQueue* PacketQueue;
	};

	struct FDecoder
	{
		AVPacket Packet;
		AVPacket PacketTemp;
		FPacketQueue* Queue;
		AVCodecContext* AvCtx;
		int32 PacketSerial;
		bool bFinished;
		bool bPacketPending;
		SDL_cond* EmptyQueueCond;
		int64 StartPts;
		AVRational StartPtsTb;
		int64 NextPts;
		AVRational NextPtsTb;
		SDL_Thread* DecoderTID;
	};

	struct FVideoState
	{
		SDL_Thread* ReadTID;
		AVInputFormat* InputFormat;
		bool bAbort;
		bool bForceRefresh;
		bool bPaused;
		bool bLastPaused;
		bool bQueueAttachmentsReq;
		bool bSeekReq;
		int32 SeekFlags;
		int64 SeekPos;
		int64 SeekRel;
		bool bRealtime;

		FClock AudioClock;
		FClock VideoClock;
		FClock ExternClock;

		FFrameQueue PicQueue;
		FFrameQueue SubQueue;
		FFrameQueue SamQueue;

		FDecoder AudioDec;
		FDecoder VideoDec;
		FDecoder SubDec;

		int32 VideoDecWidth;
		int32 VideoDecHeight;

		int32 AudioStream;

		int32 AvSyncType;

		double AudioClock;
		int32 AudioClockSerial;
		double AudioDiffCum;
		double AudioDiffAvgCoef;
		double AudioDiffThreshold;
		int32 AudioDiffAvgCount;
		AVStream* AudioStream;
		FPacketQueue AudioQueue;
		int32 AudioHwBufSize;
		uint8 SilenceBuf[512];
		uint8* AudioBuf;
		uint8* AudioBuf1;
		uint32 AudioBufSize;
		uint32 AudioBuf1Size;
		int32 AudioBufIndex;
		int32 AudioWriteBufSize;
		int32 AudioVolume;
		bool bMuted;
		FAudioAttribute AudioSrc;
		FAudioAttribute AudioTgt;
		SwrContext* SwrCtx;
		int32 FrameDropsEarly;
		int32 FrameDropsLate;

		enum EShowMode
		{
			SHOW_MODE_NONE = -1,
			SHOW_MODE_VIDEO = 0,
			SHOW_MODE_WAVES,
			SHOW_MODE_RDFT,
			SHOW_MODE_NB
		};

		EShowMode ShowMode;
		int16 SampleArray[SSampleArraySize];
		int32 SampleArrayIndex;
		int32 LastIStart;
		RDFTContext* Rdft;
		int32 RdftBits;
		FFTSample* RdftData;
		int32 Xpos;
		double LastVisTime;

		int32 SubtitleStreamIndex;
		AVStream* SubtitleStream;
		FPacketQueue SubtitleQueue;

		double FrameTimer;
		double FrameLastReturnedTime;
		double FrameLastFilterDelay;
		int32 VideoStreamIndex;
		AVStream* VideoStream;
		FPacketQueue VideoQueue;
		double MaxFrameDuration;
		SwsContext* SubConvertCtx;
		int32 Eof;

		char* Filename;
		int32 Width;
		int32 Height;
		int32 XLeft;
		int32 YTop;
		int32 Step;

		int32 LastVideoStream;
		int32 LastAudioStream;
		int32 LastSubtitleStream;

		SDL_cond* ContinueReadThread;
	};

	class FLMediaDecoder
	{
		AVPacket FlushPacket;

		int32 PacketQueuePutPrivate(FPacketQueue* q, AVPacket* pkt);
		int32 PacketQueuePut(FPacketQueue* q, AVPacket* pkt);
		int32 PacketQueuePutNull(FPacketQueue* q, int32 streamIndex);
		int32 PacketQueueInit(FPacketQueue* q);
		void PacketQueueFlush(FPacketQueue* q);
		void PacketQueueDestroy(FPacketQueue* q);
		void PacketQueueAbort(FPacketQueue* q);
		void PacketQueueStart(FPacketQueue* q);
		int32 PacketQueueGet(FPacketQueue* q, AVPacket* pkt, int32 block, int32* serial);

		void DecoderInit(FDecoder* d, AVCodecContext* avctx, FPacketQueue* queue, SDL_cond* emptyQueueCond);
		int32 DecoderDecodeFrame(FDecoder* d, AVFrame* frame, AVSubtitle* sub);
		void DecoderDestroy(FDecoder* d);

		void FrameQueueUnrefItem(FFrame* vp);
		int32 FrameQueueInit(FFrameQueue* f, FPacketQueue* pktq, int32 maxSize, int32 keepLast);
		void FrameQueueDestroy(FFrameQueue* f);
		void FrameQueueSignal(FFrameQueue* f);
		FFrame* FrameQueuePeek(FFrameQueue* f);
		FFrame* FrameQueuePeekNext(FFrameQueue* f);
		FFrame* FrameQueuePeekLast(FFrameQueue* f);
		FFrame* FrameQueuePeekWriteable(FFrameQueue* f);
		FFrame* FrameQueuePeekReadable(FFrameQueue* f);
		void FrameQueuePush(FFrameQueue* f);
		void FrameQueueNext(FFrameQueue* f);
		int32 FrameQueuePrev(FFrameQueue* f);
		int32 FrameQueueNBRemaining(FFrameQueue* f);
		int64 FrameQueueLastPos(FFrameQueue* f);

		void DecoderAbort(FDecoder* d, FFrameQueue* fq);
	};
}

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
	bool Decode_DecodeThread(double& secondsFromLastFrame, bool bDrop = false);

	int32 AudioResample_DecodeThread(AVFrame* aframe);

#ifdef USE_AUDIOBUFFER2
	int32 AudioResample_DecodeThread2(AVFrame* aframe);
#endif

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
	SwrContext*			Swr;

	uint8*				YUVBuffer;
	uint8*				AudioBuffer;
	uint32				AudioBufferSize;

#ifdef USE_AUDIOBUFFER2
	uint8**				AudioBuffer2;
	uint32				AudioBufferSize2;
#endif

	std::string			UrlToPlay;

	FLostMediaInfo		MediaInfo;
	IDecodeCallbackWeakPtr DecodeCallback;

	//std::deque<EDecodeCommand> Commands;
	//std::mutex			CommandLock;

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