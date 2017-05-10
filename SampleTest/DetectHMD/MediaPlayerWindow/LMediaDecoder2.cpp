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

#include "LMediaDecoder2.h"

static vector<int32> s_int32arr;
static vector<int64> s_int64arr;
static vector<int64> s_int64arr2;
static vector<float> s_float32arr;
static vector<double> s_float64arr;
static vector<double> s_float64arr2;
static vector<AVStream*> s_videoStreamArr;
static chrono::time_point<chrono::steady_clock> s_start_pt;

FLostMediaDecoder::FLostMediaDecoder() : YUVBuffer(nullptr)
, AudioBuffer(nullptr)
, AudioBufferSize(1)
, UrlToPlay("")
, MediaFormat(nullptr)
, VideoContext(nullptr)
, VideoDecoder(nullptr)
, VideoStream(nullptr)
, VideoFrame(nullptr)
, VideoStreamIndex(-1)
, AudioDecoder(nullptr)
, AudioContext(nullptr)
, AudioStream(nullptr)
, AudioFrame(nullptr)
, AudioStreamIndex(-1)
, AudioResampleCxt(nullptr)
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
	//if (CommandLock.try_lock())
	CommandLock.lock();

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
	rate = std::fmax(rate, 0.01f);
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

FLostMediaInfo * FLostMediaDecoder::GetMediaInfo()
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
			av_seek_frame(MediaFormat, -1, PlayPos * 1000000, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_ANY);
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
				double decodeTime = 0.0;
				while (elapsed > 1.5 * FrameTime)
				{
					Decode_DecodeThread(decodeTime, true);
					elapsed -= FrameTime;
					IDecodeCallback* p = ConvertPtr(DecodeCallback);
					if (p != nullptr)
					{
						p->OnEvent(EDecodeEvent::DroppedOneFrame);
					}
				}

				Decode_DecodeThread(decodeTime);
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
	//IDecodeCallback* p = ConvertPtr(DecodeCallback);
	//if (p != nullptr)
	//{
	//	p->OnEvent(EDecodeEvent::Closed);
	//}
	//else
	//{
	//	printf("");
	//}
}

bool FLostMediaDecoder::Open_DecodeThread(const char * url)
{
	const CHAR* head = "FLostMediaDecoder::Open_DecodeThread";
	snprintf(MediaInfo.Url, 255, "%s", url);

	State = EDecodeState::Opening;

	int ret = 0;
	//avformat_seek_file(ic, -1, INT64_MIN, timestamp, INT64_MAX, 0);
	av_register_all();

	/****************************************/
	AVDictionary* opt = nullptr;
	av_dict_set(&opt, "scan_all_pmts", "1", AV_DICT_DONT_OVERWRITE);
	/****************************************/

	if ((ret = avformat_open_input(&MediaFormat, url, nullptr, nullptr)) < 0)
	{
		goto onfailed;
	}

	if ((ret = avformat_find_stream_info(MediaFormat, nullptr)) < 0)
	{
		goto onfailed;
	}

	// Open video streams
	VideoStreamIndex = av_find_best_stream(MediaFormat, AVMEDIA_TYPE_VIDEO, -1, -1, &VideoDecoder, 0);
	if (VideoStreamIndex < 0)
	{
		goto onfailed;
	}

	VideoStream = MediaFormat->streams[VideoStreamIndex];
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

		MediaInfo.VideoCodecID = VideoDecoder->id;
		snprintf(MediaInfo.VideoCodecShortName, 255, "%s", VideoDecoder->name);
		snprintf(MediaInfo.VideoCodecLongName, 255, "%s", VideoDecoder->long_name);
		MediaInfo.VideoBitRate = VideoContext->bit_rate;
		MediaInfo.VideoFrameWidth = VideoContext->width;
		MediaInfo.VideoFrameHeight = VideoContext->height;
		{
			MediaInfo.VideoFrameRate = VideoStream->avg_frame_rate.num / VideoStream->avg_frame_rate.den;
		}

		FrameTime = 1.0 / MediaInfo.VideoFrameRate;
	}

	// Open audio streams
	AudioStreamIndex = av_find_best_stream(MediaFormat, AVMEDIA_TYPE_AUDIO, -1, 
		VideoStreamIndex,
		//-1,
		&AudioDecoder, 0);
	if (AudioStreamIndex < 0 || AudioDecoder == nullptr)
	{
		goto onfailed;
	}

	MediaInfo.Duration = MediaFormat->duration;
	AudioStream = MediaFormat->streams[AudioStreamIndex];
	AudioContext = AudioStream->codec;
	if ((ret = avcodec_open2(AudioContext, AudioDecoder, nullptr)) == 0)
	{
		MediaInfo.AudioParamsDst.Format = AudioContext->sample_fmt;
		MediaInfo.AudioCodecID = AudioDecoder->id;
		snprintf(MediaInfo.AudioCodecShortName, 255, "%s", AudioDecoder->name);
		//snprintf(MediaInfo.AudioCodecLongName, 255, "%s", AudioDecoder->name);
		MediaInfo.AudioParamsDst.Channels = AudioContext->channels;
		MediaInfo.AudioParamsDst.SampleRate = AudioContext->sample_rate;
		MediaInfo.AudioParamsDst.ChannelLayout = AudioContext->channel_layout;
		MediaInfo.AudioParamsDst.FrameSize = av_samples_get_buffer_size(&MediaInfo.AudioParamsDst.LineSize, MediaInfo.AudioParamsDst.Channels,
			1, MediaInfo.AudioParamsDst.Format, 1);
		MediaInfo.AudioParamsDst.BytesPerSec = av_samples_get_buffer_size(nullptr, MediaInfo.AudioParamsDst.Channels,
			MediaInfo.AudioParamsDst.SampleRate, MediaInfo.AudioParamsDst.Format, 1);
		if (!MediaInfo.AudioParamsDst.ChannelLayout ||
			MediaInfo.AudioParamsDst.Channels != av_get_channel_layout_nb_channels(MediaInfo.AudioParamsDst.ChannelLayout))
		{
			MediaInfo.AudioParamsDst.ChannelLayout = av_get_default_channel_layout(MediaInfo.AudioParamsDst.ChannelLayout);
			MediaInfo.AudioParamsDst.ChannelLayout &= AV_CH_LAYOUT_STEREO_DOWNMIX;
		}

		MediaInfo.AudioParamsSrc = MediaInfo.AudioParamsDst;
		MediaInfo.AudioParamsDst.Format = AV_SAMPLE_FMT_S16P;
		AudioFrame = av_frame_alloc();

		// Setup audio resampler
		AudioResampleCxt = avresample_alloc_context();
		av_opt_set_int(AudioResampleCxt, "in_channel_layout", MediaInfo.AudioParamsSrc.ChannelLayout, 0);
		av_opt_set_int(AudioResampleCxt, "in_sample_rate", MediaInfo.AudioParamsSrc.SampleRate, 0);
		av_opt_set_int(AudioResampleCxt, "in_sample_fmt", MediaInfo.AudioParamsSrc.Format, 0);
		av_opt_set_int(AudioResampleCxt, "out_channel_layout", MediaInfo.AudioParamsDst.ChannelLayout, 0);
		av_opt_set_int(AudioResampleCxt, "out_sample_rate", MediaInfo.AudioParamsDst.SampleRate, 0);
		av_opt_set_int(AudioResampleCxt, "out_sample_fmt", MediaInfo.AudioParamsDst.Format, 0);
		if (avresample_open(AudioResampleCxt) != 0)
		{
			assert(0&&"Cannot open audio resampler.\n");
		}
	}

	MediaInfo.UpdateLongName();
	if (ret == 0)
	{
		State = EDecodeState::Waiting;
		IDecodeCallback* p = ConvertPtr(DecodeCallback);
		if (p != nullptr)
		{
			p->OnEvent(EDecodeEvent::Opened);
			p->OnEvent(EDecodeEvent::InfoUpdated);
		}

		return true;
	}
	else
	{
	onfailed:
		State = EDecodeState::PreOpened;
		IDecodeCallback* p = ConvertPtr(DecodeCallback);
		if (p != nullptr)
		{
			p->OnEvent(EDecodeEvent::OpenFailed);
			p->OnEvent(EDecodeEvent::InfoUpdated);
		}

		UrlToPlay.clear();
		return false;
	}
}

void FLostMediaDecoder::Close_DecodeThread()
{
	if (MediaFormat != nullptr)
	{
		if (VideoStreamIndex >= 0)
		{
			av_frame_free(&VideoFrame);
			avcodec_close(VideoContext);
		}

		avformat_close_input(&MediaFormat);
	}

	if (YUVBuffer != nullptr)
	{
		delete[] YUVBuffer;
		YUVBuffer = nullptr;
	}

	if (AudioBuffer != nullptr)
	{
		av_freep(&AudioBuffer);
		AudioBuffer = nullptr;
	}

	if (AudioResampleCxt != nullptr)
	{
		avresample_free(&AudioResampleCxt);
		AudioResampleCxt = nullptr;

		av_frame_free(&AudioFrame);
		AudioFrame = nullptr;

		//avcodec_close(AudioContext);
		//AudioContext = nullptr;
	}
}

bool FLostMediaDecoder::Decode_DecodeThread(double& decodeTime, bool bDrop)
{
	static AVPacket packet;

	decodeTime = 0.0;
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
	if ((ptr != nullptr && !ptr->NeedNewFrame(w, h + h2, 61)))
	{
		return false;
	}

	int32 vdecoded = 0;
	int32 adecoded = 0;
	int32 nloop = 10;

	while (vdecoded <= 0 && nloop--)
	{
		av_init_packet(&packet);
		av_read_frame(MediaFormat, &packet);
		if (VideoStreamIndex == packet.stream_index)
		{
			ret = avcodec_decode_video2(VideoContext, VideoFrame, &vdecoded, &packet);
			if (vdecoded > 0)
			{
				//auto dts = VideoStream->cur_dts;
				//auto pts = VideoStream->pts;
				AVStream* st = (AVStream*)av_malloc(sizeof(AVStream));
				*st = *VideoStream;
				s_videoStreamArr.push_back(st);
				double timeBase = VideoStream->time_base.num / (double)VideoStream->time_base.den;

				//int64 ts = packet.pts == AV_NOPTS_VALUE ? packet.dts : packet.pts;
				//int64 st_start = VideoStream->start_time != AV_NOPTS_VALUE ? VideoStream->start_time : 0;
				//double pos2 = (ts - st_start) * VideoStream->time_base.num / (double)VideoStream->time_base.den;
				//char msg[128];
				//snprintf(msg, 128, "decode: %f, %f\n", pos, pos2);
				//OutputDebugStringA(msg);

				PlayPos += FrameTime;
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
		else if (AudioStreamIndex == packet.stream_index)
		{
			avcodec_decode_audio4(AudioContext, AudioFrame, &adecoded, &packet);
			if (adecoded > 0)
			{
				int32 datasize = 0;
				datasize = AudioResample_DecodeThread(AudioFrame);
				if (datasize > 0)
				{
					ptr = ConvertPtr(DecodeCallback);
					if (ptr != nullptr)
					{
						ptr->ProcessAudioFrame(AudioBuffer, datasize, MediaInfo.AudioParamsDst.SampleRate);
					}
				}

				//delete [] AudioBuffer;
				//AudioBuffer = nullptr;
			}
		}

		av_packet_unref(&packet);
	}

	return vdecoded > 0;
}

int32 FLostMediaDecoder::AudioResample_DecodeThread(AVFrame * aframe)
{
	char err[1024];
	//char warn[1024];
	int32 linesize = 0;
	int32 datasize = 0;
	int32 resampledsize = 0;
	int64 layout;
	int32 channels = AudioContext->channels;
	datasize = av_samples_get_buffer_size(&linesize, channels, aframe->nb_samples, (AVSampleFormat)aframe->format, 1);
	layout = (aframe->channel_layout && channels == av_get_channel_layout_nb_channels(
		aframe->channel_layout)) ? aframe->channel_layout : av_get_default_channel_layout(channels);

	if (aframe->format != MediaInfo.AudioParamsSrc.Format ||
		aframe->sample_rate != MediaInfo.AudioParamsSrc.SampleRate ||
		layout != MediaInfo.AudioParamsSrc.ChannelLayout)
	{
	}

	if (AudioBuffer != nullptr && AudioBufferSize != datasize)
	{
		av_freep(AudioBuffer);
		AudioBuffer = nullptr;
	}

	if (AudioBuffer == nullptr)
	{
		AudioBuffer = (uint8*)av_mallocz(datasize);
		AudioBufferSize = datasize;
	}

	avresample_convert(AudioResampleCxt, &AudioBuffer, linesize, AudioFrame->nb_samples,
		AudioFrame->data, AudioFrame->linesize[0], AudioFrame->nb_samples);

	return datasize;
}

DecoderHandle DecodeMedia(IDecodeCallbackWeakPtr callback, const char * url)
{
	DecoderHandle handle(new FLostMediaDecoder);
	handle->SetCallback(callback);
	handle->OpenUrl(url);
	return handle;
}

void ReleaseDecoder(DecoderHandle* handle)
{
	//handle = nullptr;
	if (handle != nullptr)
	{
		*handle = nullptr;
	}
}

const char * FAudioParams::ToString()
{
	char strlayout[256];
	av_get_channel_layout_string(strlayout, 255, Channels, ChannelLayout);
	memset(Info, 0, sizeof(Info));
	snprintf(Info, 1023, "%s,%s %d,%dHz,%dBytes/frame(%d),%dKBytes/sec",
		av_get_sample_fmt_name(Format), strlayout, Channels, SampleRate, FrameSize, LineSize, BytesPerSec / 1024);
	return &Info[0];
}


void FLostMediaInfo::UpdateLongName()
{
	memset(AudioCodecLongName, 0, sizeof(AudioCodecLongName));
	snprintf(AudioCodecLongName, 1023, "%s in(%s) out(%s)", AudioCodecShortName, AudioParamsSrc.ToString(), AudioParamsDst.ToString());
}

const char* FLostMediaInfo::ToString()
{
	memset(Info, 0, sizeof(Info));
	snprintf(Info, 1023,
		"\nurl:\t\t%s\n" \
		"duration:\t\t%.2f(min)\n" \
		"frame width:\t%d\n" \
		"frame height:\t%d\n" \
		"frame rate:\t\t%.1f\n" \
		"video bitrate:\t%dKB\n"\
		"audio codec: %x(%d), %s\n",
		&Url[0], Duration / (60 * 1000000), VideoFrameWidth, VideoFrameHeight, VideoFrameRate, VideoBitRate / 1024,
		AudioCodecID, AudioCodecID, AudioCodecLongName);

	return &Info[0];
}