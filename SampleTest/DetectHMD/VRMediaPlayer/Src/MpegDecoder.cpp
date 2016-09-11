#include "stdafx.h"
#include "MpegDecoder.h"

VideoPlayer::VideoPlayer()
{
	format = nullptr;

	video_decoder = nullptr;
	video_context = nullptr;
	video_stream = nullptr;
	video_convert_ctx = nullptr;
	video_frame = nullptr;
	video_stream_index = -1;
	//video_tex = 0;
	rgb_size = 0;
	width = 0;
	height = 0;
	video_current_pbo = 0;
	//memset(video_pbos, 0, sizeof(video_pbos));

	audio_decoder = nullptr;
	audio_context = nullptr;
	audio_stream = nullptr;
	audio_frame = nullptr;
	audio_resample = nullptr;
	audio_stream_index = -1;
	audio_source = 0;
	audio_play = false;

	frameTime = 10000.0f;
}


VideoPlayer::~VideoPlayer()
{
	bActive = false;
	if (DecodeThreadHandle.joinable())
	{
		DecodeThreadHandle.join();
	}

	close();
}


bool VideoPlayer::load(const char* filename, bool ignore_audio)
{
	av_register_all();
	path = filename;

	// Open file and read streams info
	if (avformat_open_input(&format, filename, nullptr, nullptr) < 0)
		return false;

	int ret = avformat_find_stream_info(format, nullptr);
	if (ret <= 0)
		return false;

	// Open video stream
	video_stream_index = av_find_best_stream(format,
		AVMEDIA_TYPE_VIDEO, -1, -1, &video_decoder, 0);
	if (video_stream_index >= 0)
	{
		video_stream = format->streams[video_stream_index];
		video_context = video_stream->codec;
		if (avcodec_open2(video_context, video_decoder, nullptr) == 0)
		{
			width = video_context->width;
			width2 = width / 2;
			height = video_context->height;
			height2 = height / 2;
			bBottomUp = width == height;

			// Setup video buffers
			video_frame = av_frame_alloc();
			video_frame->format = video_context->pix_fmt;
			video_frame->width = width;
			video_frame->height = height;
			av_frame_get_buffer(video_frame, 0);
			avcodec_default_get_buffer2(video_context, video_frame, AV_GET_BUFFER_FLAG_REF);

			rgb_size = width * (height + height2);
		}
		else
		{
			printf("Unable to open the video codec.\n");
			video_stream_index = -1;
			return 0;
		}
	}
	else
	{
		printf("Video stream not found.\n");
		video_stream_index = -1;
		return 0;
	}

	frameTime = (double)video_stream->avg_frame_rate.den / (double)video_stream->avg_frame_rate.num;

	// Open audio stream
	audio_stream_index = av_find_best_stream(format,
		AVMEDIA_TYPE_AUDIO, -1, -1, &audio_decoder, 0);
	if (audio_stream_index >= 0 && !VP_IGNORE_AUDIO && !ignore_audio)
	{
		audio_stream = format->streams[audio_stream_index];
		audio_context = audio_stream->codec;
		if (avcodec_open2(audio_context, audio_decoder, nullptr) == 0)
		{
			// Setup buffer
			audio_frame = av_frame_alloc();

			// Setup audio resampler
			audio_resample = avresample_alloc_context();
			av_opt_set_int(audio_resample, "in_channel_layout", audio_context->channel_layout, 0);
			av_opt_set_int(audio_resample, "in_sample_rate", audio_context->sample_rate, 0);
			av_opt_set_int(audio_resample, "in_sample_fmt", audio_context->sample_fmt, 0);
			av_opt_set_int(audio_resample, "out_channel_layout", audio_context->channel_layout, 0);
			av_opt_set_int(audio_resample, "out_sample_rate", audio_context->sample_rate, 0);
			av_opt_set_int(audio_resample, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);
			if (avresample_open(audio_resample) != 0)
			{
				printf("Cannot open audio resampler.\n");
				audio_stream_index = -1;
			}

			// Create OpenAL buffers
			alGenSources(1, &audio_source);
		}
		else
		{
			printf("Unable to open the audio codec.\n");
			audio_stream_index = -1;
		}
	}
	else
	{
		printf("No audio track found.\n");
		audio_stream_index = -1;
	}

	if (onLoad)
		onLoad();

	return true;
}

FrameTexture VideoPlayer::decodeFrame(ID3D11Device* d3d_device, uint8_t* textureBuffer)
{
	static uint8_t* buffer = new uint8_t[width * (height + height2)];
	static AVPacket packet;
	static int rewinded = 0;
	int decoded_video = 0;
	int decoded_audio = 0;

	FrameTexture texture;

	if (!format)
		return texture;

	int64_t cur_video_dst = 0;
	int64_t last_audio_dst = 0;

	int audio_queue_size = 10;
	while (!decoded_video || (audio_stream_index >= 0 && (audio_queue_size < 5/* || last_audio_dst <= cur_video_dst*/)))
	{
		av_init_packet(&packet);
		av_read_frame(format, &packet);
		if (packet.stream_index == video_stream_index)
		{
			int ret = avcodec_decode_video2(video_context, video_frame, &decoded_video, &packet);
			if (ret <= 0 && !decoded_video && !rewinded)
			{
				if (onComplete)
					onComplete();
				rewind();
				rewinded = 1;
				av_free_packet(&packet);
				break;
			}
			if (decoded_video)
			{
				rewinded = 0;
				//printf("video frame decoded\n"); 

				if (!cur_video_dst)
					cur_video_dst = video_frame->pkt_dts;

				if (textureBuffer)
				{
					uint8_t* ptr = textureBuffer;
					if (ptr)
					{
						// Copy Y plane
						for (int i = 0; i < height; i++)
							memcpy(ptr + i*width, video_frame->data[0] + i * video_frame->linesize[0], width);
						// Copy U and V plane
						for (int i = 0; i < height2; i++)
						{
							memcpy(ptr + (i + height) * width, video_frame->data[1] + i * video_frame->linesize[1], width2);
							memcpy(ptr + (i + height) * width + width2, video_frame->data[2] + i * video_frame->linesize[2], width2);
						}
					}
				}
			}
		}

		av_free_packet(&packet);
	}
	return texture;
}

void VideoPlayer::close()
{
	if (format)
	{
		if (video_stream_index >= 0)
		{
			sws_freeContext(video_convert_ctx);
			av_frame_free(&video_frame);
			avcodec_close(video_context);
		}
		if (audio_stream_index >= 0)
		{
			avresample_free(&audio_resample);
			av_frame_free(&audio_frame);
			avcodec_close(audio_context);
		}
		avformat_close_input(&format);
	}
	format = nullptr;
	Now = 0.0f;
}

void VideoPlayer::rewind()
{
	if (format) {
		//av_seek_frame(format, video_stream_index, 0, AVSEEK_FLAG_BACKWARD);
		close();
		load(path.c_str(), disable_audio);
	}
}

void VideoPlayer::getNextFrame()
{
	//decodeFrame(TODO, TODO);
}

void VideoPlayer::InitDecodeThread(ID3D11Device* InDevice, ID3D11DeviceContext* InContext)
{
	bActive = true;
	ImageData = nullptr;
	bPlaying = true;
	bFrameReady = false;
	FrameIndex = 0;

	HALDevice = InDevice;
	HALContext = InContext;
	void(VideoPlayer::*memfunc)(void) = &VideoPlayer::RunDecode;
	DecodeThreadHandle = std::thread(&VideoPlayer::RunDecode, this);

	//std::function<void(void)> f;
	//f.;
	//DecodeThreadHandle = std::function<void(void)>()(this, &Video::RunDecode);
}

void VideoPlayer::RunDecode()
{
	if (!HALDevice || !HALContext)
		return;

	auto start = std::chrono::steady_clock::now();
	double elapsed{ 0 };
	bool firstFrame = true;

	D3D11_QUERY_DESC qdesc{ D3D11_QUERY_EVENT, 0 };
	ID3D11Query* d3d_query;
	HRESULT res = HALDevice->CreateQuery(&qdesc, &d3d_query);
	assert(SUCCEEDED(res));

	while (bActive)
	{
		auto stop = std::chrono::steady_clock::now();
		auto dt = std::chrono::duration<float>(stop - start).count();
		start = stop;

		if (bPlaying)
		{
			elapsed += dt;
			Now += dt;
		}

		if (firstFrame || (bPlaying && elapsed > frameTime))
		{
			if (bNewDestFrame)
			{
				av_seek_frame(format, -1, DestFrameUS, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_ANY);
				avcodec_flush_buffers(video_context);
				OutputDebugStringA("avcodec flush buffers.\n");
				bNewDestFrame = false;
			}

			while (elapsed > frameTime * 1.5f)
			{
				decodeFrame(nullptr);
				elapsed -= frameTime;
				OutputDebugStringA("frame discarded.\n");
				//printf("frame discarded\n");
			}

			//while (DestFrame >video_context->frame_number)

			// wait GPU idle
			BOOL gpu_done = FALSE;
			while (!gpu_done && !FAILED(HALContext->GetData(d3d_query, &gpu_done, sizeof(gpu_done), 0)))
			{
				printf("wait GPU\n");
			}

			if (!bFrameReady && ImageData)
			{
				//char Buf[256];
				//sprintf_s(&Buf[0], 256, "frame: %Id, decode buff adress: %I64x.\n", FrameIndex, (unsigned long long)ImageData);
				//OutputDebugStringA(&Buf[0]);

				decodeFrame(nullptr, ImageData);
				//ImageData = nullptr;
				bFrameReady = true;
				elapsed = 0;
			}

			firstFrame = false;

		}
		else
		{
		    int milli = (frameTime - elapsed) * 1000;
		    std::this_thread::sleep_for(std::chrono::milliseconds(milli));
		}

	}
}