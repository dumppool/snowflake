/*
* file FLMediaDecoderPrivate.h
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
#include <libavutil/time.h>
#include <SDL.h>
#include <SDL_thread.h>
}

#include <thread>
#include <functional>
#include <chrono>
#include <string>
#include <deque>
#include <mutex>

#ifdef LOSTMEDIA
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "avfilter.lib")
#pragma comment(lib, "swscale.lib")
#pragma comment(lib, "swresample.lib")
#pragma comment(lib, "SDL2.lib")
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

	struct FImage
	{
		int32 Width;
		int32 Height;
		uint8* Data;
		int32 Pitches[3];
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
		FImage Bmp;
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
		int32 Finished;
		bool bPacketPending;
		SDL_cond* EmptyQueueCond;
		int64 StartPts;
		AVRational StartPtsTb;
		int64 NextPts;
		AVRational NextPtsTb;
		std::thread DecoderTID;
	};

	enum EShowMode
	{
		SHOW_MODE_NONE = -1,
		SHOW_MODE_VIDEO = 0,
		SHOW_MODE_WAVES,
		SHOW_MODE_RDFT,
		SHOW_MODE_NB
	};

	struct FVideoState
	{
		std::thread ReadTID;
		AVInputFormat* InputFormat;
		AVFormatContext* Ic;

		bool bAbort;
		bool bForceRefresh;
		bool bPaused;
		bool bLastPaused;
		bool bQueueAttachmentsReq;
		bool bSeekReq;
		bool bRealtime;
		int32 ReadPauseReturn;
		int32 SeekFlags;
		int64 SeekPos;
		int64 SeekRel;


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

		int32 AudioStreamIndex;

		int32 AvSyncType;

		double AudioClockValue;
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

		int32 LastVideoStreamIndex;
		int32 LastAudioStreamIndex;
		int32 LastSubtitleStreamIndex;

		SDL_cond* ContinueReadThread;
	};

	enum {
		AV_SYNC_AUDIO_MASTER, /* default choice */
		AV_SYNC_VIDEO_MASTER,
		AV_SYNC_EXTERNAL_CLOCK, /* synchronize to an external clock */
	};

	class FLMediaDecoderPrivate
	{
	private:
		uint32 SwsFlags;
		AVDictionary *SwsDict;
		AVDictionary *SwrOpts;
		AVDictionary *FmtOpts;
		AVDictionary *CodecOpts;
		AVDictionary *ResampleOpts;

		AVPacket FlushPacket;
		const char* InputFileName;
		const char* WindowTitle;
		int32 FSScreenWidth;
		int32 FSScreenHeight;
		int32 DefaultWidth;
		int32 DefaultHeight;
		int32 ScreenWidth;
		int32 ScreenHeight;
		bool bAudioDisable;
		bool bVideoDisable;
		bool bSubtitleDisable;
		bool bDisplayDisable;
		bool bFast;
		bool bLowres;
		bool bAutoexit;
		bool bSeekByBytes;
		bool bShowStatus;
		const char* WantedStreamSpec[AVMEDIA_TYPE_NB];
		int32 AvSyncType;
		int64 StartTime;
		int64 Duration;
		int32 DecoderReorderPts;
		int32 Loop;
		int32 Framedrop;
		int32 InfiniteBuffer;
		EShowMode ShowMode;
		const char* AudioCodecName;
		const char* SubtitleCodecName;
		const char* VideoCodecName;
		double RdftSpeed;
		int64 CursorLastShown;
		int64 AudioCallbackTime;

	private:
		int32 PacketQueuePutPrivate(FPacketQueue* q, AVPacket* pkt);
		int32 PacketQueuePut(FPacketQueue* q, AVPacket* pkt);
		int32 PacketQueuePutNull(FPacketQueue* q, int32 streamIndex);
		int32 PacketQueueInit(FPacketQueue* q);
		void PacketQueueFlush(FPacketQueue* q);
		void PacketQueueDestroy(FPacketQueue* q);
		void PacketQueueAbort(FPacketQueue* q);
		void PacketQueueStart(FPacketQueue* q);
		int32 PacketQueueGet(FPacketQueue* q, AVPacket* pkt, bool block, int32* serial);

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
		FFrame* FrameQueuePeekWritable(FFrameQueue* f);
		FFrame* FrameQueuePeekReadable(FFrameQueue* f);
		void FrameQueuePush(FFrameQueue* f);
		void FrameQueueNext(FFrameQueue* f);
		int32 FrameQueuePrev(FFrameQueue* f);
		int32 FrameQueueNBRemaining(FFrameQueue* f);
		int64 FrameQueueLastPos(FFrameQueue* f);

		void DecoderAbort(FDecoder* d, FFrameQueue* fq);

		//fill_rectangle
		//fill_border
		//ablend_subrect
		//calculate_display_rect
		void AllocPicture(FVideoState *is);
		void FreePicture(FFrame* vp);

		int32 ComputeMod(int32 a, int32 b)
		{
			return a < 0 ? a % b + b : a % b;
		}

		void VideoImageDisplay(FVideoState* is);
		void VideoAudioDisplay(FVideoState* is);
		void VideoDisplay(FVideoState* is);

		void StreamComponentClose(FVideoState* is, int32 streamIndex);
		void StreamClose(FVideoState* is);
		void DoExit(FVideoState* is);

		int32 VideoOpen(FVideoState* is, bool bForceSetVideoMode, FFrame* vp);

		double GetClock(FClock* c);
		void SetClockAt(FClock* c, double pts, int32 serial, double time);
		void SetClock(FClock* c, double pts, int32 serial);
		void SetClockSpeed(FClock* c, double speed);
		void InitClock(FClock* c, int32* queueSerial);
		void SyncClockToSlave(FClock* c, FClock* slave);
		int32 GetMasterSyncType(FVideoState* is);
		double GetMasterClock(FVideoState* is);
		void CheckExternalClockSpeed(FVideoState* is);

		void StreamSeek(FVideoState* is, int64 pos, int64 rel, bool seekByBytes);
		void StreamTogglePause(FVideoState* is);
		void TogglePause(FVideoState* is);
		void ToggleMute(FVideoState* is);

		void StepToNextFrame(FVideoState* is);
		double ComputeTargetDelay(double delay, FVideoState* is);
		double VpDuration(FVideoState* is, FFrame* vp, FFrame* nextVp);

		void UpdateVideoPts(FVideoState* is, double pts, int64 pos, int32 serial);
		void VideoRefresh(void* opaque, double* remainingTime);

		int32 QueuePicture(FVideoState* is, AVFrame* srcFrame, double pts, double duration, int64 pos, int32 serial);
		int32 GetVideoFrame(FVideoState* is, AVFrame* frame);

		int32 AudioThread(void* arg);
		int32 DecoderStart(FDecoder* d, std::function<void()> func);
		int32 VideoThread(void* arg);
		int32 SubtitleThread(void* arg);
		void UpdateSampleDisplay(FVideoState* is, short* samples, int32 samplesSize);
		int32 SynchronizeAudio(FVideoState* is, int32 nbSamples);
		int32 AudioDecodeFrame(FVideoState* is);

		void XdlAudioCallback(void* opaque, uint8* stream, int32 len);

		int32 AudioOpen(void* opaque, int64 wantedChannelLayout, int32 wantedNbChannels, int32 wantedSampleRate, FAudioAttribute* audioHwParams);
		int32 StreamComponentOpen(FVideoState* is, int32 streamIndex);
		int32 DecodeInterruptCb(void* ctx);
		bool IsRealtime(AVFormatContext* s);

		int32 ReadThread(void* arg);
		FVideoState* StreamOpen(const char* filename, AVInputFormat* iformat);
		void StreamCycleChannel(FVideoState* is, int32 codecType);
		void ToggleAudioDisplay(FVideoState* is);

		void SeekChapter(FVideoState* is, int32 incr);
		void DoSeek(FVideoState* is, int32 incr);

	public:
		FLMediaDecoderPrivate();
		~FLMediaDecoderPrivate();
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

typedef IDecodeCallback* IDecodeCallbackPtr;
typedef IDecodeCallback* IDecodeCallbackWeakPtr;

inline IDecodeCallback* ConvertPtr(IDecodeCallbackWeakPtr ptr)
{
	return ptr;
}

class FLMediaDecoder
{
public:
	FLMediaDecoder();
	~FLMediaDecoder();

	bool OpenUrl(const char* url);

	void EnqueueCommand(EDecodeCommand cmd);
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
	void Update();

	std::string			UrlToPlay;

	FLostMediaInfo		MediaInfo;
	IDecodeCallbackWeakPtr DecodeCallback;

	EDecodeState		State;
	float Rate;

	LDecoder::FLMediaDecoderPrivate* PrivateDecoder;
};

typedef std::shared_ptr<FLMediaDecoder> DecoderHandle;

LOSTMEDIA_API DecoderHandle DecodeMedia(IDecodeCallbackWeakPtr callback, const char* url);
LOSTMEDIA_API void ReleaseDecoder(DecoderHandle* handle);