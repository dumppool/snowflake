/*
* file FLostMediaDecoder.cpp
*
* author luoxw
* date 2016/12/8
*
*
*/
#include "stdafx.h"
//#include "LostMediaPrivatePCH.h"

#include "LostMediaDecoder.h"

FLostMediaDecoder::FLostMediaDecoder() : YUVBuffer(nullptr)
, UrlToPlay("")
, VideoFormat(nullptr)
, VideoContext(nullptr)
, VideoDecoder(nullptr)
, VideoStream(nullptr)
, VideoFrame(nullptr)
, VideoStreamIndex(-1)
, State(EDecodeState::PreOpened)
, bQuit(false)
, bRunning(true)
, PlayPos(0)
{
	BackgroundTask = std::thread([=]() {this->Update_DecodeThread(); });
}

FLostMediaDecoder::~FLostMediaDecoder()
{
	bQuit = true;

	// Waiting for decode thread
	while (bRunning)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	BackgroundTask.join();
}

bool FLostMediaDecoder::OpenUrl(const char * url)
{
	// only one media is allowed
	if (!UrlToPlay.empty())
	{
		return false;
	}

	std::string urlString(url);
	if (urlString.find("file://") == 0)
	{
		UrlToPlay = urlString.substr(7, urlString.length() - 7).c_str();
	}
	else
	{
		UrlToPlay = url;
	}

	EnqueueCommand(EDecodeCommand::Play);
	return true;
}

void FLostMediaDecoder::EnqueueCommand(EDecodeCommand cmd)
{
	if (CommandLock.try_lock())
	{
		Commands.push_front(cmd);
		CommandLock.unlock();
	}
}

EDecodeCommand FLostMediaDecoder::DequeueCommand()
{
	EDecodeCommand cmd = EDecodeCommand::UnDefined;
	if (Commands.size() > 0 && CommandLock.try_lock())
	{
		cmd = Commands.back();
		Commands.pop_back();
		CommandLock.unlock();
	}

	return cmd;
}

void FLostMediaDecoder::SetCallback(IDecodeCallbackWeakPtr callback)
{
	DecodeCallback = callback;
}

void FLostMediaDecoder::SetRate(float rate)
{
	rate = std::fmax(rate, 0.05f);
	FrameTime = 1.0f / (rate * MediaInfo.VideoFrameRate);
}

float FLostMediaDecoder::GetRate() const
{
	return 1.0f / (MediaInfo.VideoFrameRate * FrameTime);
}

void FLostMediaDecoder::SetLooping(bool looping)
{
	bLooping = looping;
}

bool FLostMediaDecoder::GetLooping() const
{
	return bLooping;
}

void FLostMediaDecoder::Seek(double pos)
{
	PlayPos = pos;
	EnqueueCommand(EDecodeCommand::Seek);
}

double FLostMediaDecoder::GetPos() const
{
	return PlayPos;
}

EDecodeState FLostMediaDecoder::GetState() const
{
	return State;
}

const FLostMediaInfo * FLostMediaDecoder::GetMediaInfo() const
{
	return &MediaInfo;
}

void FLostMediaDecoder::Update_DecodeThread()
{
	auto last = std::chrono::steady_clock::now();
	double elapsed = 0;

	while (!bQuit)
	{
		EDecodeCommand cmd = DequeueCommand();

		// Process command
		if (State == EDecodeState::PreOpened && cmd == EDecodeCommand::Play)
		{
			if (!UrlToPlay.empty())
			{
				Open_DecodeThread(UrlToPlay.c_str());
			}

			if (State == EDecodeState::Waiting)
			{
				State = EDecodeState::Playing;
				IDecodeCallback* p = ConvertPtr(DecodeCallback);
				if (p != nullptr)
				{
					p->OnEvent(EDecodeEvent::Playing);
				}
			}
		}
		else if (State == EDecodeState::Waiting && cmd == EDecodeCommand::Play)
		{
			State = EDecodeState::Playing;
			IDecodeCallback* p = ConvertPtr(DecodeCallback);
			if (p != nullptr)
			{
				p->OnEvent(EDecodeEvent::Playing);
			}
		}
		else if (State == EDecodeState::Playing && cmd == EDecodeCommand::Pause)
		{
			State = EDecodeState::Waiting;
			IDecodeCallback* p = ConvertPtr(DecodeCallback);
			if (p != nullptr)
			{
				p->OnEvent(EDecodeEvent::Paused);
			}
		}
		else if ((State == EDecodeState::Waiting || State == EDecodeState::Playing) && cmd == EDecodeCommand::Seek)
		{
			EDecodeState old = State;
			State = EDecodeState::Seeking;
			IDecodeCallback* p = ConvertPtr(DecodeCallback);
			if (p != nullptr)
			{
				p->OnEvent(EDecodeEvent::Seeking);
			}
			av_seek_frame(VideoFormat, -1, PlayPos * 1000000, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_ANY);
			avcodec_flush_buffers(VideoContext);
			State = old;
			p = ConvertPtr(DecodeCallback);
			if (p != nullptr)
			{
				p->OnEvent(State == EDecodeState::Playing ? EDecodeEvent::Playing : EDecodeEvent::Paused);
			}

			//if (State == EDecodeState::Waiting)
			//{
			//	Decode_DecodeThread();
			//	elapsed -= FrameTime;
			//}
		}
		else if (cmd == EDecodeCommand::Stop)
		{
			bQuit = true;
		}

		auto curr = std::chrono::steady_clock::now();
		auto dt = std::chrono::duration<float>(curr - last).count();
		last = curr;
		if (State == EDecodeState::Playing)
		{
			elapsed += dt;
			if (elapsed > FrameTime)
			{
				while (elapsed > 1.5 * FrameTime)
				{
					Decode_DecodeThread(true);
					elapsed -= FrameTime;
					IDecodeCallback* p = ConvertPtr(DecodeCallback);
					if (p != nullptr)
					{
						p->OnEvent(EDecodeEvent::DroppedOneFrame);
					}
				}

				Decode_DecodeThread();
				elapsed -= FrameTime;
			}
			else
			{
				auto sleeptime = std::chrono::milliseconds(int32((FrameTime - elapsed) * 1000));
				//std::this_thread::sleep_for(std::chrono::milliseconds(0));
				std::this_thread::sleep_for(sleeptime);
			}
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(int32(FrameTime * 1000)));
		}
	}

	Close_DecodeThread();
	bRunning = false;
	IDecodeCallback* p = ConvertPtr(DecodeCallback);
	if (p != nullptr)
	{
		p->OnEvent(EDecodeEvent::Closed);
	}
	else
	{
		printf("");
	}
}

bool FLostMediaDecoder::Open_DecodeThread(const char * url)
{
	const CHAR* head = "FLostMediaDecoder::Open_DecodeThread";

	State = EDecodeState::Opening;

	int ret = 0;

	av_register_all();
	if ((ret = avformat_open_input(&VideoFormat, url, nullptr, nullptr)) < 0)
	{
		return false;
	}

	if ((ret = avformat_find_stream_info(VideoFormat, nullptr)) < 0)
	{
		return false;
	}

	VideoStreamIndex = av_find_best_stream(VideoFormat, AVMEDIA_TYPE_VIDEO, -1, -1, &VideoDecoder, 0);
	if (VideoStreamIndex < 0)
	{
		return false;
	}

	VideoStream = VideoFormat->streams[VideoStreamIndex];
	VideoContext = VideoStream->codec;
	if ((ret = avcodec_open2(VideoContext, VideoDecoder, nullptr)) == 0)
	{
		int w = VideoContext->width;
		int h = VideoContext->height;
		VideoFrame = av_frame_alloc();
		VideoFrame->format = VideoContext->pix_fmt;
		VideoFrame->width = w;
		VideoFrame->height = h;
		av_frame_get_buffer(VideoFrame, 0);
		avcodec_default_get_buffer2(VideoContext, VideoFrame, AV_GET_BUFFER_FLAG_REF);
		YUVBuffer = new uint8[w * (h + h / 2)];

	}

	snprintf(MediaInfo.Url, 255, "%s", url);
	if (ret == 0)
	{
		MediaInfo.AudioChannels = 0;
		MediaInfo.AudioSampleRate = 0;
		MediaInfo.VideoBitRate = VideoContext->bit_rate;
		MediaInfo.VideoFrameWidth = VideoContext->width;
		MediaInfo.VideoFrameHeight = VideoContext->height;
		if (VideoContext->framerate.num == 0)
		{
			MediaInfo.VideoFrameRate = VideoStream->avg_frame_rate.num / VideoStream->avg_frame_rate.den;
		}
		else
		{
			MediaInfo.VideoFrameRate = VideoContext->framerate.num / VideoContext->framerate.den;
		}

		MediaInfo.Duration = VideoFormat->duration;

		FrameTime = 1.0 / MediaInfo.VideoFrameRate;

		State = EDecodeState::Waiting;
		IDecodeCallback* p = ConvertPtr(DecodeCallback);
		if (p != nullptr)
		{
			p->OnEvent(EDecodeEvent::Opened);
		}

		return true;
	}
	else
	{
		State = EDecodeState::PreOpened;
		IDecodeCallback* p = ConvertPtr(DecodeCallback);
		if (p != nullptr)
		{
			p->OnEvent(EDecodeEvent::OpenFailed);
		}

		UrlToPlay.clear();
		return false;
	}
}

void FLostMediaDecoder::Close_DecodeThread()
{
	if (VideoFormat != nullptr)
	{
		if (VideoStreamIndex >= 0)
		{
			av_frame_free(&VideoFrame);
			avcodec_close(VideoContext);
		}

		avformat_close_input(&VideoFormat);
	}
}

bool FLostMediaDecoder::Decode_DecodeThread(bool bDrop)
{
	static AVPacket packet;

	int ret = 0;

	if (YUVBuffer == nullptr)
	{
		return false;
	}

	int32 w = VideoContext->width;
	int32 w2 = w / 2;
	int32 h = VideoContext->height;
	int32 h2 = h / 2;

	IDecodeCallback* ptr = ConvertPtr(DecodeCallback);
	if (bDrop || (ptr != nullptr && !ptr->NeedNewFrame(w, h + h2, 61)))
	{
		return false;
	}

	int32 vdecoded = 0;
	int32 nloop = 10;

	while (vdecoded <= 0 && nloop--)
	{
		av_init_packet(&packet);
		av_read_frame(VideoFormat, &packet);
		if (VideoStreamIndex == packet.stream_index)
		{
			ret = avcodec_decode_video2(VideoContext, VideoFrame, &vdecoded, &packet);
			if (vdecoded > 0)
			{
				PlayPos += 1.0 / MediaInfo.VideoFrameRate;
				if (!bDrop)
				{
					uint8* p = YUVBuffer;
					for (int i = 0; i < h; ++i)
					{
						memcpy(p + i * w, VideoFrame->data[0] + i * VideoFrame->linesize[0], w);
					}

					for (int i = 0; i < h2; ++i)
					{
						memcpy(p + (i + h) * w, VideoFrame->data[1] + i * VideoFrame->linesize[1], w2);
						memcpy(p + (i + h) * w + w2, VideoFrame->data[2] + i * VideoFrame->linesize[2], w2);
					}

					ptr = ConvertPtr(DecodeCallback);
					if (ptr != nullptr)
					{
						ptr->ProcessVideoFrame(p, w * (h + h2));
					}
				}
			}
		}
		else if (0)
		{

		}

		av_packet_unref(&packet);
	}

	return vdecoded > 0;
}

LOSTMEDIA_API DecoderHandle DecodeMedia(IDecodeCallbackWeakPtr callback, const char * url)
{
	DecoderHandle handle(new FLostMediaDecoder);
	handle->SetCallback(callback);
	handle->OpenUrl(url);
	return handle;
}

LOSTMEDIA_API void ReleaseDecoder(DecoderHandle* handle)
{
	//handle = nullptr;
	if (handle != nullptr)
	{
		*handle = nullptr;
	}
}
