/*
* file FLMediaDecoderPrivate.cpp
*
* author luoxw
* date 2017/3/31
*
*
*/

#include "stdafx.h"
//#include "LostMediaPrivatePCH.h"

#include "LMediaDecoder.h"

using namespace LDecoder;


#define MAX_QUEUE_SIZE (15 * 1024 * 1024)
#define MIN_FRAMES 25
#define EXTERNAL_CLOCK_MIN_FRAMES 2
#define EXTERNAL_CLOCK_MAX_FRAMES 10

/* external clock speed adjustment constants for realtime sources based on buffer fullness */
#define EXTERNAL_CLOCK_SPEED_MIN  0.900
#define EXTERNAL_CLOCK_SPEED_MAX  1.010
#define EXTERNAL_CLOCK_SPEED_STEP 0.001

/* no AV sync correction is done if below the minimum AV sync threshold */
#define AV_SYNC_THRESHOLD_MIN 0.04
/* AV sync correction is done if above the maximum AV sync threshold */
#define AV_SYNC_THRESHOLD_MAX 0.1
/* If a frame duration is longer than this, it will not be duplicated to compensate AV sync */
#define AV_SYNC_FRAMEDUP_THRESHOLD 0.1
/* no AV correction is done if too big error */
#define AV_NOSYNC_THRESHOLD 10.0

#define SAMPLE_ARRAY_SIZE (8 * 65536)

/* we use about AUDIO_DIFF_AVG_NB A-V differences to make the average */
#define AUDIO_DIFF_AVG_NB   20

/* maximum audio speed change to get correct sync */
#define SAMPLE_CORRECTION_PERCENT_MAX 10

#define VIDEO_PICTURE_QUEUE_SIZE 3
#define SUBPICTURE_QUEUE_SIZE 16
#define SAMPLE_QUEUE_SIZE 9
#define FRAME_QUEUE_SIZE FFMAX(SAMPLE_QUEUE_SIZE, FFMAX(VIDEO_PICTURE_QUEUE_SIZE, SUBPICTURE_QUEUE_SIZE))


int check_stream_specifier(AVFormatContext *s, AVStream *st, const char *spec)
{
	int ret = avformat_match_stream_specifier(s, st, spec);
	if (ret < 0)
		av_log(s, AV_LOG_ERROR, "Invalid stream specifier: %s.\n", spec);
	return ret;
}

AVDictionary *filter_codec_opts(AVDictionary *opts, enum AVCodecID codec_id,
	AVFormatContext *s, AVStream *st, AVCodec *codec)
{
	AVDictionary    *ret = nullptr;
	AVDictionaryEntry *t = nullptr;
	int            flags = s->oformat ? AV_OPT_FLAG_ENCODING_PARAM
		: AV_OPT_FLAG_DECODING_PARAM;
	char          prefix = 0;
	const AVClass    *cc = avcodec_get_class();

	if (!codec)
		codec = s->oformat ? avcodec_find_encoder(codec_id)
		: avcodec_find_decoder(codec_id);

	switch (st->codec->codec_type) {
	case AVMEDIA_TYPE_VIDEO:
		prefix = 'v';
		flags |= AV_OPT_FLAG_VIDEO_PARAM;
		break;
	case AVMEDIA_TYPE_AUDIO:
		prefix = 'a';
		flags |= AV_OPT_FLAG_AUDIO_PARAM;
		break;
	case AVMEDIA_TYPE_SUBTITLE:
		prefix = 's';
		flags |= AV_OPT_FLAG_SUBTITLE_PARAM;
		break;
	}

	while (t = av_dict_get(opts, "", t, AV_DICT_IGNORE_SUFFIX)) {
		char *p = strchr(t->key, ':');

		/* check stream specification in opt name */
		if (p)
			switch (check_stream_specifier(s, st, p + 1)) {
			case  1: *p = 0; break;
			case  0:         continue;
			default:         return nullptr;
			}

		if (av_opt_find(&cc, t->key, NULL, flags, AV_OPT_SEARCH_FAKE_OBJ) ||
			!codec ||
			(codec->priv_class &&
				av_opt_find(&codec->priv_class, t->key, NULL, flags,
					AV_OPT_SEARCH_FAKE_OBJ)))
			av_dict_set(&ret, t->key, t->value, 0);
		else if (t->key[0] == prefix &&
			av_opt_find(&cc, t->key + 1, NULL, flags,
				AV_OPT_SEARCH_FAKE_OBJ))
			av_dict_set(&ret, t->key + 1, t->value, 0);

		if (p)
			*p = ':';
	}
	return ret;
}

AVDictionary **setup_find_stream_info_opts(AVFormatContext *s,
	AVDictionary *codec_opts)
{
	int i;
	AVDictionary **opts;

	if (!s->nb_streams)
		return NULL;
	opts = (AVDictionary **)av_mallocz_array(s->nb_streams, sizeof(*opts));
	if (!opts) {
		av_log(NULL, AV_LOG_ERROR,
			"Could not alloc memory for stream options.\n");
		return NULL;
	}
	for (i = 0; i < s->nb_streams; i++)
		opts[i] = filter_codec_opts(codec_opts, s->streams[i]->codec->codec_id,
			s, s->streams[i], NULL);
	return opts;
}

int is_realtime(AVFormatContext *s)
{
	if (!strcmp(s->iformat->name, "rtp")
		|| !strcmp(s->iformat->name, "rtsp")
		|| !strcmp(s->iformat->name, "sdp")
		)
		return 1;

	if (s->pb && (!strncmp(s->filename, "rtp:", 4)
		|| !strncmp(s->filename, "udp:", 4)
		)
		)
		return 1;
	return 0;
}

int32 LDecoder::FLMediaDecoderPrivate::PacketQueuePutPrivate(FPacketQueue * q, AVPacket * pkt)
{
	if (q->bAbort)
	{
		return -1;
	}

	FAVPacketList* pkt1 = (FAVPacketList*)av_mallocz(sizeof(FAVPacketList));
	if (pkt1 == nullptr)
	{
		return -1;
	}

	pkt1->Packet = *pkt;
	pkt1->Next = nullptr;
	if (pkt == &FlushPacket)
	{
		++q->Serial;
	}

	pkt1->Serial = q->Serial;

	if (q->Last == nullptr)
	{
		q->First = pkt1;
	}
	else
	{
		q->Last->Next = pkt1;
	}

	q->Last = pkt1;
	++q->Num;
	q->Size += pkt1->Packet.size + sizeof(*pkt1);

	SDL_CondSignal(q->Cond);
	return 0;
}

int32 LDecoder::FLMediaDecoderPrivate::PacketQueuePut(FPacketQueue * q, AVPacket * pkt)
{
	int32 ret;

	SDL_LockMutex(q->Mutex);
	ret = PacketQueuePutPrivate(q, pkt);
	SDL_UnlockMutex(q->Mutex);

	if (pkt != &FlushPacket && ret < 0)
	{
		av_packet_unref(pkt);
	}

	return ret;
}

int32 LDecoder::FLMediaDecoderPrivate::PacketQueuePutNull(FPacketQueue * q, int32 streamIndex)
{
	AVPacket pkt;
	av_init_packet(&pkt);
	pkt.data = nullptr;
	pkt.size = 0;
	pkt.stream_index = streamIndex;
	return PacketQueuePut(q, &pkt);
}

int32 LDecoder::FLMediaDecoderPrivate::PacketQueueInit(FPacketQueue * q)
{
	memset(q, 0, sizeof(FPacketQueue));
	q->Mutex = SDL_CreateMutex();
	q->Cond = SDL_CreateCond();
	assert(q->Mutex && q->Cond && "PacketQueueInit: SDL_CreateMutex or SDL_CreateCond failed");
	q->bAbort = true;
	return 0;
}

void LDecoder::FLMediaDecoderPrivate::PacketQueueFlush(FPacketQueue * q)
{
	FAVPacketList *pkt, *pkt1;

	SDL_LockMutex(q->Mutex);
	for (pkt = q->First; pkt != nullptr; pkt = pkt1)
	{
		pkt1 = pkt->Next;
		av_packet_unref(&pkt->Packet);
		av_freep(&pkt);
	}

	q->Last = nullptr;
	q->First = nullptr;
	q->Num = 0;
	q->Size = 0;
	SDL_UnlockMutex(q->Mutex);
}

void LDecoder::FLMediaDecoderPrivate::PacketQueueDestroy(FPacketQueue * q)
{
	PacketQueueFlush(q);
	SDL_DestroyMutex(q->Mutex);
	SDL_DestroyCond(q->Cond);
}

void LDecoder::FLMediaDecoderPrivate::PacketQueueAbort(FPacketQueue * q)
{
	SDL_LockMutex(q->Mutex);
	q->bAbort = true;
	SDL_CondSignal(q->Cond);
	SDL_UnlockMutex(q->Mutex);
}

void LDecoder::FLMediaDecoderPrivate::PacketQueueStart(FPacketQueue * q)
{
	SDL_LockMutex(q->Mutex);
	q->bAbort = false;
	PacketQueuePutPrivate(q, &FlushPacket);
	SDL_UnlockMutex(q->Mutex);
}

int32 LDecoder::FLMediaDecoderPrivate::PacketQueueGet(FPacketQueue * q, AVPacket * pkt, bool block, int32 * serial)
{
	FAVPacketList * pkt1;
	int ret;

	SDL_LockMutex(q->Mutex);

	while (true)
	{
		if (q->bAbort)
		{
			ret = -1;
			break;
		}

		pkt1 = q->First;
		if (pkt1 != nullptr)
		{
			q->First = pkt1->Next;
			if (q->First == nullptr)
			{
				q->Last = nullptr;
			}

			--q->Num;
			q->Size -= pkt1->Packet.size + sizeof(*pkt1);
			*pkt = pkt1->Packet;
			if (serial != nullptr)
			{
				*serial = pkt1->Serial;
			}

			av_free(pkt1);
			ret = 1;
			break;
		}
		else if (!block)
		{
			ret = 0;
			break;
		}
		else
		{
			SDL_CondWait(q->Cond, q->Mutex);
		}
	}

	SDL_UnlockMutex(q->Mutex);
	return ret;
}

void LDecoder::FLMediaDecoderPrivate::DecoderInit(FDecoder * d, AVCodecContext * avctx, FPacketQueue * queue, SDL_cond * emptyQueueCond)
{
	memset(d, 0, sizeof(FDecoder));
	d->AvCtx = avctx;
	d->Queue = queue;
	d->EmptyQueueCond = emptyQueueCond;
	d->StartPts = AV_NOPTS_VALUE;
}

int32 LDecoder::FLMediaDecoderPrivate::DecoderDecodeFrame(FDecoder * d, AVFrame * frame, AVSubtitle * sub)
{
	int got = 0;

	do 
	{
		int ret = -1;
		if (d->Queue->bAbort)
		{
			return -1;
		}

		if (!d->bPacketPending || d->Queue->Serial != d->PacketSerial)
		{
			AVPacket pkt;
			do
			{
				if (d->Queue->Num == 0)
				{
					SDL_CondSignal(d->EmptyQueueCond);
				}

				if (PacketQueueGet(d->Queue, &pkt, true, &d->PacketSerial) < 0)
				{
					return -1;
				}

				if (pkt.data == FlushPacket.data)
				{
					avcodec_flush_buffers(d->AvCtx);
					d->Finished = 0;
					d->NextPts = d->StartPts;
					d->NextPtsTb = d->StartPtsTb;
				}
			} 
			while (pkt.data == FlushPacket.data || d->Queue->Serial != d->PacketSerial);
			av_packet_unref(&d->Packet);
			d->PacketTemp = d->Packet = pkt;
			d->bPacketPending = true;
		}

		switch (d->AvCtx->codec_type)
		{
		case AVMEDIA_TYPE_VIDEO:
			ret = avcodec_decode_video2(d->AvCtx, frame, &got, &d->PacketTemp);
			if (got)
			{
				if (DecoderReorderPts == -1)
				{
					frame->pts = av_frame_get_best_effort_timestamp(frame);
				}
				else if (DecoderReorderPts)
				{
					frame->pts = frame->pkt_pts;
				}
				else
				{
					frame->pts = frame->pkt_dts;
				}
			}

			break;
		case AVMEDIA_TYPE_AUDIO:
			ret = avcodec_decode_audio4(d->AvCtx, frame, &got, &d->PacketTemp);
			if (got)
			{
				AVRational tb {1, frame->sample_rate};
				if (frame->pts != AV_NOPTS_VALUE)
				{
					frame->pts = av_rescale_q(frame->pts, d->AvCtx->time_base, tb);
				}
				else if (frame->pkt_pts != AV_NOPTS_VALUE)
				{
					frame->pts = av_rescale_q(frame->pkt_pts, av_codec_get_pkt_timebase(d->AvCtx), tb);
				}
				else if (d->NextPts != AV_NOPTS_VALUE)
				{
					frame->pts = av_rescale_q(d->NextPts, d->NextPtsTb, tb);
				}
				
				if (frame->pts != AV_NOPTS_VALUE)
				{
					d->NextPts = frame->pts + frame->nb_samples;
					d->NextPtsTb = tb;
				}
			}

			break;
		case AVMEDIA_TYPE_SUBTITLE:
			ret = avcodec_decode_subtitle2(d->AvCtx, sub, &got, &d->PacketTemp);
			break;
		default:
			break;
		}

		if (ret < 0)
		{
			d->bPacketPending = false;
		}
		else
		{
			d->PacketTemp.dts = d->PacketTemp.pts = AV_NOPTS_VALUE;
			if (d->PacketTemp.data)
			{
				if (d->AvCtx->codec_type != AVMEDIA_TYPE_AUDIO)
				{
					ret = d->PacketTemp.size;
				}

				d->PacketTemp.data += ret;
				d->PacketTemp.size -= ret;
				if (d->PacketTemp.size <= 0)
				{
					d->bPacketPending = false;
				}
			}
			else
			{
				if (!got)
				{
					d->bPacketPending = false;
					d->Finished = d->PacketSerial;
				}
			}
		}
	} while (!got && !d->Finished);

	return got;
}

void LDecoder::FLMediaDecoderPrivate::DecoderDestroy(FDecoder * d)
{
	av_packet_unref(&d->Packet);
}

void LDecoder::FLMediaDecoderPrivate::FrameQueueUnrefItem(FFrame * vp)
{
	int32 i;
	for (i = 0; i < vp->Sub.num_rects; ++i)
	{
		av_freep(&vp->SubRects[i]->data[0]);
		av_freep(&vp->SubRects[i]);
	}

	av_freep(&vp->SubRects);
	av_frame_unref(vp->Frame);
	avsubtitle_free(&vp->Sub);
}

int32 LDecoder::FLMediaDecoderPrivate::FrameQueueInit(FFrameQueue * f, FPacketQueue * pktq, int32 maxSize, int32 keepLast)
{
	int32 i;
	memset(f, 0, sizeof(FFrameQueue));
	f->Mutex = SDL_CreateMutex();
	f->Cond = SDL_CreateCond();
	assert(f->Mutex && f->Cond && "FrameQueueInit: SDL_CreateMutex or SDL_CreateCond failed");

	f->PacketQueue = pktq;
	f->MaxSize = FFMIN(maxSize, 16);
	f->bKeepLast = !!keepLast;
	for (i = 0; i < f->MaxSize; ++i)
	{
		if (!(f->Queue[i].Frame = av_frame_alloc()))
		{
			return AVERROR(ENOMEM);
		}
	}

	return 0;
}

void LDecoder::FLMediaDecoderPrivate::FrameQueueDestroy(FFrameQueue * f)
{
	for (int32 i = 0; i < f->MaxSize; ++i)
	{
		FFrame* vp = &f->Queue[i];
		FrameQueueUnrefItem(vp);
		av_frame_free(&vp->Frame);
		FreePicture(vp);
	}

	SDL_DestroyMutex(f->Mutex);
	SDL_DestroyCond(f->Cond);
}

void LDecoder::FLMediaDecoderPrivate::FrameQueueSignal(FFrameQueue * f)
{
	SDL_LockMutex(f->Mutex);
	SDL_CondSignal(f->Cond);
	SDL_UnlockMutex(f->Mutex);
}

FFrame * LDecoder::FLMediaDecoderPrivate::FrameQueuePeek(FFrameQueue * f)
{
	return &f->Queue[(f->ReadIndex + f->ReadIndexShown) % f->MaxSize];
}

FFrame * LDecoder::FLMediaDecoderPrivate::FrameQueuePeekNext(FFrameQueue * f)
{
	return &f->Queue[(f->ReadIndex + f->ReadIndexShown + 1) % f->MaxSize];
}

FFrame * LDecoder::FLMediaDecoderPrivate::FrameQueuePeekLast(FFrameQueue * f)
{
	return &f->Queue[f->ReadIndex];
}

FFrame * LDecoder::FLMediaDecoderPrivate::FrameQueuePeekWritable(FFrameQueue * f)
{
	SDL_LockMutex(f->Mutex);
	while (f->Size >= f->MaxSize && !f->PacketQueue->bAbort)
	{
		SDL_CondWait(f->Cond, f->Mutex);
	}

	SDL_UnlockMutex(f->Mutex);
	if (f->PacketQueue->bAbort)
	{
		return nullptr;
	}

	return &f->Queue[f->WriteIndex];
}

FFrame * LDecoder::FLMediaDecoderPrivate::FrameQueuePeekReadable(FFrameQueue * f)
{
	SDL_LockMutex(f->Mutex);
	while (f->Size - f->ReadIndexShown <= 0 &&
		!f->PacketQueue->bAbort) {
		SDL_CondWait(f->Cond, f->Mutex);
	}
	SDL_UnlockMutex(f->Mutex);

	if (f->PacketQueue->bAbort)
		return nullptr;

	return &f->Queue[(f->ReadIndex + f->ReadIndexShown) % f->MaxSize];
}

void LDecoder::FLMediaDecoderPrivate::FrameQueuePush(FFrameQueue * f)
{
	if (++f->WriteIndex == f->MaxSize)
		f->WriteIndex = 0;
	SDL_LockMutex(f->Mutex);
	f->Size++;
	SDL_CondSignal(f->Cond);
	SDL_UnlockMutex(f->Mutex);
}

void LDecoder::FLMediaDecoderPrivate::FrameQueueNext(FFrameQueue * f)
{
	if (f->bKeepLast && !f->ReadIndexShown) {
		f->ReadIndexShown = 1;
		return;
	}
	FrameQueueUnrefItem(&f->Queue[f->ReadIndex]);
	if (++f->ReadIndex == f->MaxSize)
		f->ReadIndex = 0;
	SDL_LockMutex(f->Mutex);
	f->Size--;
	SDL_CondSignal(f->Cond);
	SDL_UnlockMutex(f->Mutex);
}

int32 LDecoder::FLMediaDecoderPrivate::FrameQueuePrev(FFrameQueue * f)
{
	int ret = f->ReadIndexShown;
	f->ReadIndexShown = 0;
	return ret;
}

int32 LDecoder::FLMediaDecoderPrivate::FrameQueueNBRemaining(FFrameQueue * f)
{
	return f->Size - f->ReadIndexShown;
}

int64 LDecoder::FLMediaDecoderPrivate::FrameQueueLastPos(FFrameQueue * f)
{
	FFrame *fp = &f->Queue[f->ReadIndex];
	if (f->ReadIndexShown && fp->Serial == f->PacketQueue->Serial)
		return fp->Pos;
	else
		return -1;
}

void LDecoder::FLMediaDecoderPrivate::PushCommand(const std::function<void()>& cmd)
{
	std::lock_guard<std::mutex> guard(CmdsLock);
	Cmds.push_front(cmd);
}

void LDecoder::FLMediaDecoderPrivate::DecoderAbort(FDecoder * d, FFrameQueue * fq)
{
	PacketQueueAbort(d->Queue);
	FrameQueueSignal(fq);
	d->DecoderTID.join();
	PacketQueueFlush(d->Queue);
}

void LDecoder::FLMediaDecoderPrivate::AllocPicture(FVideoState *is)
{
	FFrame* vp;
	int64 bufferdiff;
	vp = &is->PicQueue.Queue[is->PicQueue.WriteIndex];
	FreePicture(vp);

	vp->Bmp.Width = is->VideoDecWidth;
	vp->Bmp.Height = is->VideoDecHeight;
	vp->Bmp.Size = vp->Bmp.Width * vp->Bmp.Height * 1.5;
	vp->Bmp.Data = (uint8*)av_mallocz(vp->Bmp.Size);

	SDL_LockMutex(is->PicQueue.Mutex);
	vp->bAllocated = true;
	SDL_CondSignal(is->PicQueue.Cond);
	SDL_UnlockMutex(is->PicQueue.Mutex);
}

void LDecoder::FLMediaDecoderPrivate::FreePicture(FFrame* vp)
{
	if (vp->Bmp.Data != NULL)
		av_freep(vp->Bmp.Data);
}

void LDecoder::FLMediaDecoderPrivate::VideoImageDisplay(FVideoState * is)
{
	FFrame *vp;
	FFrame *sp;
	SDL_Rect rect;
	int i;

	vp = FrameQueuePeek(&is->PicQueue);
	if (vp->Bmp.Data != nullptr) {
		//if (is->subtitle_st) {
		//	if (frame_queue_nb_remaining(&is->subpq) > 0) {
		//		sp = frame_queue_peek(&is->subpq);

		//		if (vp->pts >= sp->pts + ((float)sp->sub.start_display_time / 1000)) {
		//			uint8_t *data[4];
		//			int linesize[4];

		//			SDL_LockYUVOverlay(vp->bmp);

		//			data[0] = vp->bmp->pixels[0];
		//			data[1] = vp->bmp->pixels[2];
		//			data[2] = vp->bmp->pixels[1];

		//			linesize[0] = vp->bmp->pitches[0];
		//			linesize[1] = vp->bmp->pitches[2];
		//			linesize[2] = vp->bmp->pitches[1];

		//			for (i = 0; i < sp->sub.num_rects; i++)
		//				blend_subrect(data, linesize, sp->subrects[i],
		//					vp->bmp->w, vp->bmp->h);

		//			SDL_UnlockYUVOverlay(vp->bmp);
		//		}
		//	}
		//}
		if (is->DecodeCB->NeedNewFrame(vp->Bmp.Width, vp->Bmp.Height, 61))
			is->DecodeCB->ProcessVideoFrame(vp->Bmp.Data, 0);

		//calculate_display_rect(&rect, is->xleft, is->ytop, is->width, is->height, vp->width, vp->height, vp->sar);

		//SDL_DisplayYUVOverlay(vp->bmp, &rect);

		//if (rect.x != is->last_display_rect.x || rect.y != is->last_display_rect.y || rect.w != is->last_display_rect.w || rect.h != is->last_display_rect.h || is->force_refresh) {
		//	int bgcolor = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
		//	fill_border(is->xleft, is->ytop, is->width, is->height, rect.x, rect.y, rect.w, rect.h, bgcolor, 1);
		//	is->LastBmp = rect;
		//}
	}
}

void LDecoder::FLMediaDecoderPrivate::VideoAudioDisplay(FVideoState * is)
{
}

void LDecoder::FLMediaDecoderPrivate::VideoDisplay(FVideoState * is)
{
	if (is->AudioStream && is->ShowMode != SHOW_MODE_VIDEO)
		VideoAudioDisplay(is);
	else if (is->VideoStream)
		VideoImageDisplay(is);
}

void LDecoder::FLMediaDecoderPrivate::StreamComponentClose(FVideoState * is, int32 streamIndex)
{
	AVFormatContext *ic = is->Ic;
	AVCodecContext *avctx;

	if (streamIndex < 0 || streamIndex >= ic->nb_streams)
		return;
	avctx = ic->streams[streamIndex]->codec;

	switch (avctx->codec_type) {
	case AVMEDIA_TYPE_AUDIO:
		DecoderAbort(&is->AudioDec, &is->SamQueue);
		SDL_CloseAudio();
		DecoderDestroy(&is->AudioDec);
		swr_free(&is->SwrCtx);
		av_freep(&is->AudioBuf1);
		is->AudioBuf1Size = 0;
		is->AudioBuf = nullptr;

		if (is->Rdft) {
			av_rdft_end(is->Rdft);
			av_freep(&is->RdftData);
			is->Rdft = nullptr;
			is->RdftBits = 0;
		}
		break;
	case AVMEDIA_TYPE_VIDEO:
		DecoderAbort(&is->VideoDec, &is->PicQueue);
		DecoderDestroy(&is->VideoDec);
		break;
	case AVMEDIA_TYPE_SUBTITLE:
		DecoderAbort(&is->SubDec, &is->SubQueue);
		DecoderDestroy(&is->SubDec);
		break;
	default:
		break;
	}

	ic->streams[streamIndex]->discard = AVDISCARD_ALL;
	avcodec_close(avctx);
	switch (avctx->codec_type) {
	case AVMEDIA_TYPE_AUDIO:
		is->AudioStream = nullptr;
		is->AudioStreamIndex = -1;
		break;
	case AVMEDIA_TYPE_VIDEO:
		is->VideoStream = nullptr;
		is->VideoStreamIndex = -1;
		break;
	case AVMEDIA_TYPE_SUBTITLE:
		is->SubtitleStream = nullptr;
		is->SubtitleStreamIndex = -1;
		break;
	default:
		break;
	}
}

void LDecoder::FLMediaDecoderPrivate::StreamClose(FVideoState * is)
{
	/* XXX: use a special url_shutdown call to abort parse cleanly */
	is->bAbort = true;
	if (is->ReadThreadId.joinable())
		is->ReadThreadId.join();

	if (is->LoopThreadId.joinable())
		is->LoopThreadId.join();

	if (is->AudioThreadId.joinable())
		is->AudioThreadId.join();

	/* close each stream */
	if (is->AudioStreamIndex >= 0)
		StreamComponentClose(is, is->AudioStreamIndex);
	if (is->VideoStreamIndex >= 0)
		StreamComponentClose(is, is->VideoStreamIndex);
	if (is->SubtitleStreamIndex >= 0)
		StreamComponentClose(is, is->SubtitleStreamIndex);

	avformat_close_input(&is->Ic);

	PacketQueueDestroy(&is->VideoQueue);
	PacketQueueDestroy(&is->AudioQueue);
	PacketQueueDestroy(&is->SubtitleQueue);

	/* free all pictures */
	FrameQueueDestroy(&is->PicQueue);
	FrameQueueDestroy(&is->SamQueue);
	FrameQueueDestroy(&is->SubQueue);
	SDL_DestroyCond(is->ContinueReadThread);
	sws_freeContext(is->SubConvertCtx);
	av_free(is->Filename);
	av_free(is);
}

void LDecoder::FLMediaDecoderPrivate::DoExit(FVideoState * is)
{
	if (is) 
	{
		StreamClose(is);
	}

	av_lockmgr_register(nullptr);
	//uninit_opts();
	avformat_network_deinit();
	//SDL_Quit();
	//av_log(NULL, AV_LOG_QUIET, "%s", "");
	//exit(0);
}

int32 LDecoder::FLMediaDecoderPrivate::VideoOpen(FVideoState * is, bool bForceSetVideoMode, FFrame * vp)
{
	int32 w = DefaultWidth;
	int32 h = DefaultHeight;

	w = FFMIN(16383, w);

	is->Width = w;
	is->Height = h;

	return 0;
}

double LDecoder::FLMediaDecoderPrivate::GetClock(FClock * c)
{
	if (*c->QueueSerial != c->Serial)
		return NAN;
	if (c->bPaused) {
		return c->Pts;
	}
	else {
		double time = av_gettime_relative() / 1000000.0;
		return c->PtsDrift + time - (time - c->LastUpdated) * (1.0 - c->Speed);
	}
}

void LDecoder::FLMediaDecoderPrivate::SetClockAt(FClock * c, double pts, int32 serial, double time)
{
	c->Pts = pts;
	c->LastUpdated = time;
	c->PtsDrift = c->Pts - time;
	c->Serial = serial;
}

void LDecoder::FLMediaDecoderPrivate::SetClock(FClock * c, double pts, int32 serial)
{
	double time = av_gettime_relative() / 1000000.0;
	SetClockAt(c, pts, serial, time);
}

void LDecoder::FLMediaDecoderPrivate::SetClockSpeed(FClock * c, double speed)
{
	SetClock(c, GetClock(c), c->Serial);
	c->Speed = speed;
}

void LDecoder::FLMediaDecoderPrivate::InitClock(FClock * c, int32 * queueSerial)
{
	c->Speed = 1.0;
	c->bPaused = false;
	c->QueueSerial = queueSerial;
	SetClock(c, NAN, -1);
}

void LDecoder::FLMediaDecoderPrivate::SyncClockToSlave(FClock * c, FClock * slave)
{
	double clock = GetClock(c);
	double slave_clock = GetClock(slave);
	if (!isnan(slave_clock) && (isnan(clock) || fabs(clock - slave_clock) > AV_NOSYNC_THRESHOLD))
		SetClock(c, slave_clock, slave->Serial);
}

int32 LDecoder::FLMediaDecoderPrivate::GetMasterSyncType(FVideoState * is)
{
	if (is->AvSyncType == AV_SYNC_VIDEO_MASTER) {
		if (is->VideoStream)
			return AV_SYNC_VIDEO_MASTER;
		else
			return AV_SYNC_AUDIO_MASTER;
	}
	else if (is->AvSyncType == AV_SYNC_AUDIO_MASTER) {
		if (is->AudioStream)
			return AV_SYNC_AUDIO_MASTER;
		else
			return AV_SYNC_EXTERNAL_CLOCK;
	}
	else {
		return AV_SYNC_EXTERNAL_CLOCK;
	}
}

double LDecoder::FLMediaDecoderPrivate::GetMasterClock(FVideoState * is)
{
	double val;

	switch (GetMasterSyncType(is)) {
	case AV_SYNC_VIDEO_MASTER:
		val =GetClock(&is->VideoClock);
		break;
	case AV_SYNC_AUDIO_MASTER:
		val = GetClock(&is->AudioClock);
		break;
	default:
		val = GetClock(&is->ExternClock);
		break;
	}
	return val;
}

void LDecoder::FLMediaDecoderPrivate::CheckExternalClockSpeed(FVideoState * is)
{
	if (is->VideoStreamIndex >= 0 && is->VideoQueue.Num <= EXTERNAL_CLOCK_MIN_FRAMES ||
		is->AudioStreamIndex >= 0 && is->AudioQueue.Num <= EXTERNAL_CLOCK_MIN_FRAMES) {
		SetClockSpeed(&is->ExternClock, FFMAX(EXTERNAL_CLOCK_SPEED_MIN, is->ExternClock.Speed - EXTERNAL_CLOCK_SPEED_STEP));
	}
	else if ((is->VideoStreamIndex < 0 || is->VideoQueue.Num > EXTERNAL_CLOCK_MAX_FRAMES) &&
		(is->AudioStreamIndex < 0 || is->AudioQueue.Num > EXTERNAL_CLOCK_MAX_FRAMES)) {
		SetClockSpeed(&is->ExternClock, FFMIN(EXTERNAL_CLOCK_SPEED_MAX, is->ExternClock.Speed + EXTERNAL_CLOCK_SPEED_STEP));
	}
	else {
		double speed = is->ExternClock.Speed;
		if (speed != 1.0)
			SetClockSpeed(&is->ExternClock, speed + EXTERNAL_CLOCK_SPEED_STEP * (1.0 - speed) / fabs(1.0 - speed));
	}
}

void LDecoder::FLMediaDecoderPrivate::StreamSeek(FVideoState * is, int64 pos, int64 rel, bool seekByBytes)
{
	if (!is->bSeekReq) {
		is->SeekPos = pos;
		is->SeekRel = rel;
		is->SeekFlags &= ~AVSEEK_FLAG_BYTE;
		if (seekByBytes)
			is->SeekFlags |= AVSEEK_FLAG_BYTE;
		is->bSeekReq = true;
		SDL_CondSignal(is->ContinueReadThread);
	}
}

void LDecoder::FLMediaDecoderPrivate::StreamTogglePause(FVideoState * is)
{
	if (is->bPaused) {
		is->FrameTimer += av_gettime_relative() / 1000000.0 - is->VideoClock.LastUpdated;
		if (is->ReadPauseReturn != AVERROR(ENOSYS)) {
			is->VideoClock.bPaused = false;
		}
		SetClock(&is->VideoClock, GetClock(&is->VideoClock), is->VideoClock.Serial);
	}
	SetClock(&is->ExternClock, GetClock(&is->ExternClock), is->ExternClock.Serial);
	is->bPaused = is->AudioClock.bPaused = is->VideoClock.bPaused = is->ExternClock.bPaused = !is->bPaused;
}

void LDecoder::FLMediaDecoderPrivate::TogglePause(FVideoState * is)
{
	StreamTogglePause(is);
	is->Step = 0;
}

void LDecoder::FLMediaDecoderPrivate::ToggleMute(FVideoState * is)
{
	is->bMuted = !is->bMuted;
}

void LDecoder::FLMediaDecoderPrivate::StepToNextFrame(FVideoState * is)
{
	/* if the stream is paused unpause it, then step */
	if (is->bPaused)
		StreamTogglePause(is);
	is->Step = 1;
}

double LDecoder::FLMediaDecoderPrivate::ComputeTargetDelay(double delay, FVideoState * is)
{    
	double sync_threshold, diff = 0;

	/* update delay to follow master synchronisation source */
	if (GetMasterSyncType(is) != AV_SYNC_VIDEO_MASTER) {
		/* if video is slave, we try to correct big delays by
		duplicating or deleting a frame */
		diff = GetClock(&is->VideoClock) - GetMasterClock(is);

		/* skip or repeat frame. We take into account the
		delay to compute the threshold. I still don't know
		if it is the best guess */
		sync_threshold = FFMAX(AV_SYNC_THRESHOLD_MIN, FFMIN(AV_SYNC_THRESHOLD_MAX, delay));
		if (!isnan(diff) && fabs(diff) < is->MaxFrameDuration) {
			if (diff <= -sync_threshold)
				delay = FFMAX(0, delay + diff);
			else if (diff >= sync_threshold && delay > AV_SYNC_FRAMEDUP_THRESHOLD)
				delay = delay + diff;
			else if (diff >= sync_threshold)
				delay = 2 * delay;
		}
	}

	av_log(NULL, AV_LOG_TRACE, "video: delay=%0.3f A-V=%f\n",
		delay, -diff);

	return delay;
}

double LDecoder::FLMediaDecoderPrivate::VpDuration(FVideoState * is, FFrame * vp, FFrame * nextVp)
{
	if (vp->Serial == nextVp->Serial) {
		double duration = nextVp->Pts - vp->Pts;
		if (isnan(duration) || duration <= 0 || duration > is->MaxFrameDuration)
			return vp->Duration;
		else
			return duration;
	}
	else {
		return 0.0;
	}
}

void LDecoder::FLMediaDecoderPrivate::UpdateVideoPts(FVideoState * is, double pts, int64 pos, int32 serial)
{    
	/* update current video pts */
	SetClock(&is->VideoClock, pts, serial);
	SyncClockToSlave(&is->ExternClock, &is->VideoClock);
}

void LDecoder::FLMediaDecoderPrivate::VideoRefresh(void * opaque, double * remainingTime)
{
	FVideoState *is = (FVideoState *)opaque;
	double time;

	FFrame *sp, *sp2;

	if (!is->bPaused && GetMasterSyncType(is) == AV_SYNC_EXTERNAL_CLOCK && is->bRealtime)
		CheckExternalClockSpeed(is);

	if (!bDisplayDisable && is->ShowMode != SHOW_MODE_VIDEO && is->AudioStream) {
		time = av_gettime_relative() / 1000000.0;
		if (is->bForceRefresh || is->LastVisTime + RdftSpeed < time) {
			VideoDisplay(is);
			is->LastVisTime = time;
		}
		*remainingTime = FFMIN(*remainingTime, is->LastVisTime + RdftSpeed - time);
	}

	if (is->VideoStream) {
		int redisplay = 0;
		if (is->bForceRefresh)
			redisplay = FrameQueuePrev(&is->PicQueue);
	retry:
		if (FrameQueueNBRemaining(&is->PicQueue) == 0) {
			// nothing to do, no picture to display in the queue
		}
		else {
			double last_duration, duration, delay;
			FFrame *vp, *lastvp;

			/* dequeue the picture */
			lastvp = FrameQueuePeekLast(&is->PicQueue);
			vp = FrameQueuePeek(&is->PicQueue);

			if (vp->Serial != is->VideoQueue.Serial) {
				FrameQueueNext(&is->PicQueue);
				redisplay = 0;
				goto retry;
			}

			if (lastvp->Serial != vp->Serial && !redisplay)
				is->FrameTimer = av_gettime_relative() / 1000000.0;

			if (is->bPaused)
				goto display;

			/* compute nominal last_duration */
			last_duration = VpDuration(is, lastvp, vp);
			if (redisplay)
				delay = 0.0;
			else
				delay = ComputeTargetDelay(last_duration, is);

			time = av_gettime_relative() / 1000000.0;
			if (time < is->FrameTimer + delay && !redisplay) {
				*remainingTime = FFMIN(is->FrameTimer + delay - time, *remainingTime);
				return;
			}

			is->FrameTimer += delay;
			if (delay > 0 && time - is->FrameTimer > AV_SYNC_THRESHOLD_MAX)
				is->FrameTimer = time;

			SDL_LockMutex(is->PicQueue.Mutex);
			if (!redisplay && !isnan(vp->Pts))
				UpdateVideoPts(is, vp->Pts, vp->Pos, vp->Serial);
			SDL_UnlockMutex(is->PicQueue.Mutex);

			if (FrameQueueNBRemaining(&is->PicQueue) > 1) {
				FFrame *nextvp = FrameQueuePeekNext(&is->PicQueue);
				duration = VpDuration(is, vp, nextvp);
				if (!is->Step && (redisplay || Framedrop>0 || (Framedrop && GetMasterSyncType(is) != AV_SYNC_VIDEO_MASTER)) && time > is->FrameTimer + duration) {
					if (!redisplay)
						is->FrameDropsLate++;
					FrameQueueNext(&is->PicQueue);
					redisplay = 0;
					goto retry;
				}
			}

			if (is->SubtitleStream) {
				while (FrameQueueNBRemaining(&is->SubQueue) > 0) {
					sp = FrameQueuePeek(&is->SubQueue);

					if (FrameQueueNBRemaining(&is->SubQueue) > 1)
						sp2 = FrameQueuePeekNext(&is->SubQueue);
					else
						sp2 = NULL;

					if (sp->Serial != is->SubtitleQueue.Serial
						|| (is->VideoClock.Pts > (sp->Pts + ((float)sp->Sub.end_display_time / 1000)))
						|| (sp2 && is->VideoClock.Pts > (sp2->Pts + ((float)sp2->Sub.start_display_time / 1000))))
					{
						FrameQueueNext(&is->SubQueue);
					}
					else {
						break;
					}
				}
			}

		display:
			/* display picture */
			if (!bDisplayDisable && is->ShowMode == SHOW_MODE_VIDEO)
				VideoDisplay(is);

			FrameQueueNext(&is->PicQueue);

			if (is->Step && !is->bPaused)
				StreamTogglePause(is);
		}
	}
	is->bForceRefresh = 0;
	//if (bShowStatus) {
	//	static int64_t last_time;
	//	int64_t cur_time;
	//	int aqsize, vqsize, sqsize;
	//	double av_diff;

	//	cur_time = av_gettime_relative();
	//	if (!last_time || (cur_time - last_time) >= 30000) {
	//		aqsize = 0;
	//		vqsize = 0;
	//		sqsize = 0;
	//		if (is->audio_st)
	//			aqsize = is->audioq.size;
	//		if (is->video_st)
	//			vqsize = is->videoq.size;
	//		if (is->subtitle_st)
	//			sqsize = is->subtitleq.size;
	//		av_diff = 0;
	//		if (is->audio_st && is->video_st)
	//			av_diff = get_clock(&is->audclk) - get_clock(&is->vidclk);
	//		else if (is->video_st)
	//			av_diff = get_master_clock(is) - get_clock(&is->vidclk);
	//		else if (is->audio_st)
	//			av_diff = get_master_clock(is) - get_clock(&is->audclk);
	//		av_log(NULL, AV_LOG_INFO,
	//			"%7.2f %s:%7.3f fd=%4d aq=%5dKB vq=%5dKB sq=%5dB f=%"PRId64"/%"PRId64"   \r",
	//			get_master_clock(is),
	//			(is->audio_st && is->video_st) ? "A-V" : (is->video_st ? "M-V" : (is->audio_st ? "M-A" : "   ")),
	//			av_diff,
	//			is->frame_drops_early + is->frame_drops_late,
	//			aqsize / 1024,
	//			vqsize / 1024,
	//			sqsize,
	//			is->video_st ? is->video_st->codec->pts_correction_num_faulty_dts : 0,
	//			is->video_st ? is->video_st->codec->pts_correction_num_faulty_pts : 0);
	//		fflush(stdout);
	//		last_time = cur_time;
	//	}
	//}
}

int32 LDecoder::FLMediaDecoderPrivate::QueuePicture(FVideoState * is, AVFrame * srcFrame, double pts, double duration, int64 pos, int32 serial)
{
	//char msg[16];
	//snprintf(msg, 16, "\nserial: %d", serial);
	//OutputDebugStringA(msg);

	FFrame *vp;

	if (!(vp = FrameQueuePeekWritable(&is->PicQueue)))
		return -1;

	vp->Sar = srcFrame->sample_aspect_ratio;

	/* alloc or resize hardware picture buffer */
	if (!vp->Bmp.Data || vp->bReallocate || !vp->bAllocated||
		vp->Width != srcFrame->width ||
		vp->Height != srcFrame->height) {
		//SDL_Event event;

		vp->bAllocated = false;
		vp->bReallocate = false;
		vp->Width = srcFrame->width;
		vp->Height = srcFrame->height;

		/* the allocation must be done in the main thread to avoid
		locking problems. */
		//event.type = FF_ALLOC_EVENT;
		//event.user.data1 = is;
		//SDL_PushEvent(&event);

		PushCommand([&]() {AllocPicture(is); });

		/* wait until the picture is allocated */
		SDL_LockMutex(is->PicQueue.Mutex);
		while (!vp->bAllocated && !is->VideoQueue.bAbort) {
			SDL_CondWait(is->PicQueue.Cond, is->PicQueue.Mutex);
		}
		/* if the queue is aborted, we have to pop the pending ALLOC event or wait for the allocation to complete */
		if (is->VideoQueue.bAbort) {
			while (!vp->bAllocated && !is->bAbort) {
				SDL_CondWait(is->PicQueue.Cond, is->PicQueue.Mutex);
			}
		}
		SDL_UnlockMutex(is->PicQueue.Mutex);

		if (is->VideoQueue.bAbort)
			return -1;
	}

	/* if the frame is not skipped, then display it */
	if (vp->Bmp.Data) {
		//uint8_t *data[4];
		//int linesize[4];

		/* get a pointer on the bitmap */
		//SDL_LockYUVOverlay(vp->bmp);

		//data[0] = vp->bmp->pixels[0];
		//data[1] = vp->bmp->pixels[2];
		//data[2] = vp->bmp->pixels[1];

		//linesize[0] = vp->bmp->pitches[0];
		//linesize[1] = vp->bmp->pitches[2];
		//linesize[2] = vp->bmp->pitches[1];

		//{
		//	AVDictionaryEntry *e = av_dict_get(SwsDict, "sws_flags", NULL, 0);
		//	if (e) {
		//		const AVClass *class = sws_get_class();
		//		const AVOption    *o = av_opt_find(&class, "sws_flags", NULL, 0,
		//			AV_OPT_SEARCH_FAKE_OBJ);
		//		int ret = av_opt_eval_flags(&class, o, e->value, &sws_flags);
		//		if (ret < 0)
		//			exit(1);
		//	}
		//}

		//is->img_convert_ctx = sws_getCachedContext(is->img_convert_ctx,
		//	vp->width, vp->height, srcFrame->format, vp->width, vp->height,
		//	AV_PIX_FMT_YUV420P, sws_flags, NULL, NULL, NULL);
		//if (!is->img_convert_ctx) {
		//	av_log(NULL, AV_LOG_FATAL, "Cannot initialize the conversion context\n");
		//	exit(1);
		//}
		//sws_scale(is->img_convert_ctx, srcFrame->data, srcFrame->linesize,
		//	0, vp->height, data, linesize);



		///* workaround SDL PITCH_WORKAROUND */
		//duplicate_right_border_pixels(vp->bmp);
		///* update the bitmap content */
		//SDL_UnlockYUVOverlay(vp->bmp);

		int32 w = vp->Bmp.Width;
		int32 w2 = w / 2;
		int32 h = vp->Bmp.Height;
		int32 h2 = h / 2;
		for (int i = 0; i < h; ++i)
		{
			memcpy(vp->Bmp.Data + i * w, srcFrame->data[0] + i * srcFrame->linesize[0], w);
		}

		for (int i = 0; i < h2; ++i)
		{
			memcpy(vp->Bmp.Data + (i + h) * w, srcFrame->data[1] + i * srcFrame->linesize[1], w2);
			memcpy(vp->Bmp.Data + (i + h) * w + w2, srcFrame->data[2] + i * srcFrame->linesize[2], w2);
		}

		vp->Pts = pts;
		vp->Duration = duration;
		vp->Pos = pos;
		vp->Serial = serial;

		/* now we can update the picture count */
		FrameQueuePush(&is->PicQueue);
	}
	return 0;
}

int32 LDecoder::FLMediaDecoderPrivate::GetVideoFrame(FVideoState * is, AVFrame * frame)
{
	int got_picture;

	if ((got_picture = DecoderDecodeFrame(&is->VideoDec, frame, nullptr)) < 0)
		return -1;

	if (got_picture) {
		double dpts = NAN;

		if (frame->pts != AV_NOPTS_VALUE)
			dpts = av_q2d(is->VideoStream->time_base) * frame->pts;

		frame->sample_aspect_ratio = av_guess_sample_aspect_ratio(is->Ic, is->VideoStream, frame);

		is->VideoDecWidth = frame->width;
		is->VideoDecHeight = frame->height;

		if (Framedrop>0 || (Framedrop && GetMasterSyncType(is) != AV_SYNC_VIDEO_MASTER)) 
		{
			if (frame->pts != AV_NOPTS_VALUE) 
			{
				double diff = dpts - GetMasterClock(is);
				if (!isnan(diff) && fabs(diff) < AV_NOSYNC_THRESHOLD &&
					diff - is->FrameLastFilterDelay < 0 &&
					is->VideoDec.PacketSerial == is->VideoClock.Serial &&
					is->VideoQueue.Num)
				{
					is->FrameDropsEarly++;
					av_frame_unref(frame);
					got_picture = 0;
				}
			}
		}
	}

	return got_picture;
}

int32 LDecoder::FLMediaDecoderPrivate::AudioThread(void * arg)
{
	FVideoState *is = (FVideoState *)arg;
	AVFrame *frame = av_frame_alloc();
	FFrame *af;

	int got_frame = 0;
	AVRational tb;
	int ret = 0;

	if (!frame)
		return AVERROR(ENOMEM);

	do {
		if ((got_frame = DecoderDecodeFrame(&is->AudioDec, frame, NULL)) < 0)
			goto the_end;

		if (got_frame) {
			tb = { 1, frame->sample_rate };

				if (!(af = FrameQueuePeekWritable(&is->SamQueue)))
					goto the_end;

				af->Pts = (frame->pts == AV_NOPTS_VALUE) ? NAN : frame->pts * av_q2d(tb);
				af->Pos = av_frame_get_pkt_pos(frame);
				af->Serial = is->AudioDec.PacketSerial;
				af->Duration = av_q2d({ frame->nb_samples, frame->sample_rate });

				av_frame_move_ref(af->Frame, frame);
				FrameQueuePush(&is->SamQueue);


		}
	} while (ret >= 0 || ret == AVERROR(EAGAIN) || ret == AVERROR_EOF);
the_end:
	av_frame_free(&frame);
	return ret;
}

int32 LDecoder::FLMediaDecoderPrivate::DecoderStart(FDecoder * d, std::function<void()> func)
{
	PacketQueueStart(d->Queue);
	d->DecoderTID = std::thread([&]() { func(); });
	return 0;
}

int32 LDecoder::FLMediaDecoderPrivate::VideoThread(void * arg)
{
	FVideoState *is = (FVideoState *)arg;
	AVFrame *frame = av_frame_alloc();
	double pts;
	double duration;
	int ret;
	AVRational tb = is->VideoStream->time_base;
	AVRational frame_rate = av_guess_frame_rate(is->Ic, is->VideoStream, nullptr);
	
	if (!frame) {
		return AVERROR(ENOMEM);
	}

	for (;;) {
		ret = GetVideoFrame(is, frame);
		if (ret < 0)
			goto the_end;
		if (!ret)
			continue;

			duration = (frame_rate.num && frame_rate.den ? av_q2d({ frame_rate.den, frame_rate.num }) : 0);
			pts = (frame->pts == AV_NOPTS_VALUE) ? NAN : frame->pts * av_q2d(tb);
			ret = QueuePicture(is, frame, pts, duration, av_frame_get_pkt_pos(frame), is->VideoDec.PacketSerial);
			av_frame_unref(frame);

		if (ret < 0)
			goto the_end;
	}
the_end:

	av_frame_free(&frame);
	return 0;
}

int32 LDecoder::FLMediaDecoderPrivate::SubtitleThread(void * arg)
{
	FVideoState *is = (FVideoState *)arg;
	FFrame *sp;
	int got_subtitle;
	double pts;
	int i;

	for (;;) {
		if (!(sp = FrameQueuePeekWritable(&is->SubQueue)))
			return 0;

		if ((got_subtitle = DecoderDecodeFrame(&is->SubDec, nullptr, &sp->Sub)) < 0)
			break;

		pts = 0;

		if (got_subtitle && sp->Sub.format == 0) {
			if (sp->Sub.pts != AV_NOPTS_VALUE)
				pts = sp->Sub.pts / (double)AV_TIME_BASE;
			sp->Pts = pts;
			sp->Serial = is->SubDec.PacketSerial;
			if (!(sp->SubRects = (AVSubtitleRect**)av_mallocz_array(sp->Sub.num_rects, sizeof(AVSubtitleRect*)))) {
				av_log(NULL, AV_LOG_FATAL, "Cannot allocate subrects\n");
				exit(1);
			}

			for (i = 0; i < sp->Sub.num_rects; i++)
			{
				int in_w = sp->Sub.rects[i]->w;
				int in_h = sp->Sub.rects[i]->h;
				int subw = is->SubDec.AvCtx->width ? is->SubDec.AvCtx->width : is->VideoDecWidth;
				int subh = is->SubDec.AvCtx->height ? is->SubDec.AvCtx->height : is->VideoDecHeight;
				int out_w = is->VideoDecWidth ? in_w * is->VideoDecWidth / subw : in_w;
				int out_h = is->VideoDecHeight ? in_h * is->VideoDecHeight / subh : in_h;

				if (!(sp->SubRects[i] = (AVSubtitleRect*)av_mallocz(sizeof(AVSubtitleRect))) ||
					av_image_alloc(sp->SubRects[i]->data, sp->SubRects[i]->linesize, out_w, out_h, AV_PIX_FMT_YUVA420P, 16) < 0) {
					av_log(NULL, AV_LOG_FATAL, "Cannot allocate subtitle data\n");
					exit(1);
				}

				is->SubConvertCtx = sws_getCachedContext(is->SubConvertCtx,
					in_w, in_h, AV_PIX_FMT_PAL8, out_w, out_h,
					AV_PIX_FMT_YUVA420P, SwsFlags, NULL, NULL, NULL);
				if (!is->SubConvertCtx) {
					av_log(NULL, AV_LOG_FATAL, "Cannot initialize the sub conversion context\n");
					exit(1);
				}
				sws_scale(is->SubConvertCtx,
					sp->Sub.rects[i]->data, sp->Sub.rects[i]->linesize,
					0, in_h, sp->SubRects[i]->data, sp->SubRects[i]->linesize);

				sp->SubRects[i]->w = out_w;
				sp->SubRects[i]->h = out_h;
				sp->SubRects[i]->x = sp->Sub.rects[i]->x * out_w / in_w;
				sp->SubRects[i]->y = sp->Sub.rects[i]->y * out_h / in_h;
			}

			/* now we can update the picture count */
			FrameQueuePush(&is->SubQueue);
		}
		else if (got_subtitle) {
			avsubtitle_free(&sp->Sub);
		}
	}
	return 0;
}

void LDecoder::FLMediaDecoderPrivate::UpdateSampleDisplay(FVideoState * is, short * samples, int32 samplesSize)
{
	int size, len;

	size = samplesSize / sizeof(short);
	while (size > 0) {
		len = SAMPLE_ARRAY_SIZE - is->SampleArrayIndex;
		if (len > size)
			len = size;
		memcpy(is->SampleArray + is->SampleArrayIndex, samples, len * sizeof(short));
		samples += len;
		is->SampleArrayIndex += len;
		if (is->SampleArrayIndex >= SAMPLE_ARRAY_SIZE)
			is->SampleArrayIndex = 0;
		size -= len;
	}
}

int32 LDecoder::FLMediaDecoderPrivate::SynchronizeAudio(FVideoState * is, int32 nbSamples)
{
	int wanted_nb_samples = nbSamples;

	/* if not master, then we try to remove or add samples to correct the clock */
	if (GetMasterSyncType(is) != AV_SYNC_AUDIO_MASTER) {
		double diff, avg_diff;
		int min_nb_samples, max_nb_samples;

		diff = GetClock(&is->AudioClock) - GetMasterClock(is);

		if (!isnan(diff) && fabs(diff) < AV_NOSYNC_THRESHOLD) {
			is->AudioDiffCum = diff + is->AudioDiffAvgCoef * is->AudioDiffCum;
			if (is->AudioDiffAvgCount < AUDIO_DIFF_AVG_NB) {
				/* not enough measures to have a correct estimate */
				is->AudioDiffAvgCount++;
			}
			else {
				/* estimate the A-V difference */
				avg_diff = is->AudioDiffCum * (1.0 - is->AudioDiffAvgCoef);

				if (fabs(avg_diff) >= is->AudioDiffThreshold) {
					wanted_nb_samples = nbSamples + (int)(diff * is->AudioSrc.Freq);
					min_nb_samples = ((nbSamples * (100 - SAMPLE_CORRECTION_PERCENT_MAX) / 100));
					max_nb_samples = ((nbSamples * (100 + SAMPLE_CORRECTION_PERCENT_MAX) / 100));
					wanted_nb_samples = av_clip(wanted_nb_samples, min_nb_samples, max_nb_samples);
				}
				av_log(NULL, AV_LOG_TRACE, "diff=%f adiff=%f sample_diff=%d apts=%0.3f %f\n",
					diff, avg_diff, wanted_nb_samples - nbSamples,
					is->AudioClock, is->AudioDiffThreshold);
			}
		}
		else {
			/* too big difference : may be initial PTS errors, so
			reset A-V filter */
			is->AudioDiffAvgCount = 0;
			is->AudioDiffCum = 0;
		}
	}

	return wanted_nb_samples;
}

int32 LDecoder::FLMediaDecoderPrivate::AudioDecodeFrame(FVideoState * is)
{
	int data_size, resampled_data_size;
	int64_t dec_channel_layout;
	av_unused double audio_clock0;
	int wanted_nb_samples;
	FFrame *af;

	if (is->bPaused)
		return -1;

	do {
#if defined(_WIN32)
		while (FrameQueueNBRemaining(&is->SamQueue) == 0) {
			if ((av_gettime_relative() - AudioCallbackTime) > 1000000LL * is->AudioHwBufSize / is->AudioTgt.BytesPerSec / 2)
				return -1;
			av_usleep(1000);
		}
#endif
		if (!(af = FrameQueuePeekReadable(&is->SamQueue)))
			return -1;
		FrameQueueNext(&is->SamQueue);
	} while (af->Serial != is->AudioQueue.Serial);

	data_size = av_samples_get_buffer_size(NULL, av_frame_get_channels(af->Frame),
		af->Frame->nb_samples,
		(AVSampleFormat)af->Frame->format, 1);

	dec_channel_layout =
		(af->Frame->channel_layout && av_frame_get_channels(af->Frame) == av_get_channel_layout_nb_channels(af->Frame->channel_layout)) ?
		af->Frame->channel_layout : av_get_default_channel_layout(av_frame_get_channels(af->Frame));
	wanted_nb_samples = SynchronizeAudio(is, af->Frame->nb_samples);

	if (af->Frame->format != is->AudioSrc.Format ||
		dec_channel_layout != is->AudioSrc.ChannelLayout ||
		af->Frame->sample_rate != is->AudioSrc.Freq ||
		(wanted_nb_samples != af->Frame->nb_samples && !is->SwrCtx)) {
		swr_free(&is->SwrCtx);
		is->SwrCtx = swr_alloc_set_opts(NULL,
			is->AudioTgt.ChannelLayout, is->AudioTgt.Format, is->AudioTgt.Freq,
			dec_channel_layout, (AVSampleFormat)af->Frame->format, af->Frame->sample_rate,
			0, NULL);
		if (!is->SwrCtx || swr_init(is->SwrCtx) < 0) {
			av_log(NULL, AV_LOG_ERROR,
				"Cannot create sample rate converter for conversion of %d Hz %s %d channels to %d Hz %s %d channels!\n",
				af->Frame->sample_rate, av_get_sample_fmt_name((AVSampleFormat)af->Frame->format), av_frame_get_channels(af->Frame),
				is->AudioTgt.Freq, av_get_sample_fmt_name(is->AudioTgt.Format), is->AudioTgt.Channels);
			swr_free(&is->SwrCtx);
			return -1;
		}
		is->AudioSrc.ChannelLayout = dec_channel_layout;
		is->AudioSrc.Channels = av_frame_get_channels(af->Frame);
		is->AudioSrc.Freq = af->Frame->sample_rate;
		is->AudioSrc.Format = (AVSampleFormat)af->Frame->format;
	}

	if (is->SwrCtx) {
		const uint8_t **in = (const uint8_t **)af->Frame->extended_data;
		uint8_t **out = &is->AudioBuf1;
		int out_count = (int64_t)wanted_nb_samples * is->AudioTgt.Freq / af->Frame->sample_rate + 256;
		int out_size = av_samples_get_buffer_size(NULL, is->AudioTgt.Channels, out_count, is->AudioTgt.Format, 0);
		int len2;
		if (out_size < 0) {
			av_log(NULL, AV_LOG_ERROR, "av_samples_get_buffer_size() failed\n");
			return -1;
		}
		if (wanted_nb_samples != af->Frame->nb_samples) {
			if (swr_set_compensation(is->SwrCtx, (wanted_nb_samples - af->Frame->nb_samples) * is->AudioTgt.Freq / af->Frame->sample_rate,
				wanted_nb_samples * is->AudioTgt.Freq / af->Frame->sample_rate) < 0) {
				av_log(NULL, AV_LOG_ERROR, "swr_set_compensation() failed\n");
				return -1;
			}
		}
		av_fast_malloc(&is->AudioBuf1, &is->AudioBuf1Size, out_size);
		if (!is->AudioBuf1)
			return AVERROR(ENOMEM);
		len2 = swr_convert(is->SwrCtx, out, out_count, in, af->Frame->nb_samples);
		if (len2 < 0) {
			av_log(NULL, AV_LOG_ERROR, "swr_convert() failed\n");
			return -1;
		}
		if (len2 == out_count) {
			av_log(NULL, AV_LOG_WARNING, "audio buffer is probably too small\n");
			if (swr_init(is->SwrCtx) < 0)
				swr_free(&is->SwrCtx);
		}
		is->AudioBuf = is->AudioBuf1;
		resampled_data_size = len2 * is->AudioTgt.Channels * av_get_bytes_per_sample(is->AudioTgt.Format);
	}
	else {
		is->AudioBuf = af->Frame->data[0];
		resampled_data_size = data_size;
	}

	audio_clock0 = is->AudioClockValue;
	/* update the audio clock with the pts */
	if (!isnan(af->Pts))
		is->AudioClockValue = af->Pts + (double)af->Frame->nb_samples / af->Frame->sample_rate;
	else
		is->AudioClockValue = NAN;
	is->AudioClockSerial = af->Serial;
#ifdef _DEBUG
	{
		static double last_clock;
		printf("audio: delay=%0.3f clock=%0.3f clock0=%0.3f\n",
			is->AudioClockValue - last_clock,
			is->AudioClockValue, audio_clock0);
		last_clock = is->AudioClockValue;
	}
#endif
	return resampled_data_size;
}

void LDecoder::FLMediaDecoderPrivate::Execute_AudioThread(void * arg)
{
	const int32 constlen = 512 * 1024;
	uint8 * stream = (uint8 *)av_mallocz(constlen);

	while (1)
	{
		FVideoState *is = (FVideoState *)arg;
		int32 len = constlen;
		int audio_size, len1;

		AudioCallbackTime = av_gettime_relative();

		while (len > 0 && is->AudioTgt.BytesPerSec > 0)
		{
			if (is->AudioBufIndex >= is->AudioBufSize)
			{
				audio_size = AudioDecodeFrame(is);
				if (audio_size < 0) 
				{
					/* if error, just output silence */
					is->AudioBuf = is->SilenceBuf;
					is->AudioBufSize = sizeof(is->SilenceBuf) / is->AudioTgt.FrameSize * is->AudioTgt.FrameSize;
				}
				else
				{
					if (is->ShowMode != SHOW_MODE_VIDEO)
						UpdateSampleDisplay(is, (int16_t *)is->AudioBuf, audio_size);
					is->AudioBufSize = audio_size;
				}

				is->AudioBufIndex = 0;
			}

			len1 = is->AudioBufSize - is->AudioBufIndex;

			if (len1 > len)
			{
				len1 = len;
			}

			if (!is->bMuted && is->AudioVolume == SDL_MIX_MAXVOLUME)
				memcpy(stream, (uint8_t *)is->AudioBuf + is->AudioBufIndex, len1);
			else {
				memset(stream, is->SilenceBuf[0], len1);
				if (!is->bMuted)
					SDL_MixAudio(stream, (uint8_t *)is->AudioBuf + is->AudioBufIndex, len1, is->AudioVolume);
			}

			len -= len1;
			stream += len1;
			is->AudioBufIndex += len1;
		}

		is->AudioWriteBufSize = is->AudioBufSize - is->AudioBufIndex;
		/* Let's assume the audio driver that is used by SDL has two periods. */
		if (!isnan(is->AudioClockValue)) {
			SetClockAt(&is->AudioClock, is->AudioClockValue - (double)(2 * is->AudioHwBufSize + is->AudioWriteBufSize) / is->AudioTgt.BytesPerSec, is->AudioClockSerial, AudioCallbackTime / 1000000.0);
			SyncClockToSlave(&is->ExternClock, &is->AudioClock);
		}

		is->DecodeCB->ProcessAudioFrame(stream, len, is->AudioTgt.BytesPerSec);
	}

	av_freep(stream);
}

int32 LDecoder::FLMediaDecoderPrivate::AudioOpen(void * opaque, int64 wantedChannelLayout, int32 wantedNbChannels, int32 wantedSampleRate, FAudioAttribute * audioHwParams)
{
	SDL_AudioSpec wanted_spec, spec;
	const char *env;
	static const int next_nb_channels[] = { 0, 0, 1, 6, 2, 6, 4, 6 };
	static const int next_sample_rates[] = { 0, 44100, 48000, 96000, 192000 };
	int next_sample_rate_idx = FF_ARRAY_ELEMS(next_sample_rates) - 1;

	env = SDL_getenv("SDL_AUDIO_CHANNELS");
	if (env) {
		wantedNbChannels = atoi(env);
		wantedChannelLayout = av_get_default_channel_layout(wantedNbChannels);
	}
	if (!wantedChannelLayout || wantedNbChannels != av_get_channel_layout_nb_channels(wantedChannelLayout)) {
		wantedChannelLayout = av_get_default_channel_layout(wantedNbChannels);
		wantedChannelLayout &= ~AV_CH_LAYOUT_STEREO_DOWNMIX;
	}
	//wantedNbChannels = av_get_channel_layout_nb_channels(wantedChannelLayout);
	//wanted_spec.channels = wantedNbChannels;
	//wanted_spec.freq = wantedSampleRate;
	//if (wanted_spec.freq <= 0 || wanted_spec.channels <= 0) {
	//	av_log(NULL, AV_LOG_ERROR, "Invalid sample rate or channel count!\n");
	//	return -1;
	//}

	//while (next_sample_rate_idx && next_sample_rates[next_sample_rate_idx] >= wanted_spec.freq)
	//	next_sample_rate_idx--;
	//wanted_spec.format = AUDIO_S16SYS;
	//wanted_spec.silence = 0;
	//wanted_spec.samples = FFMAX(SDL_AUDIO_MIN_BUFFER_SIZE, 2 << av_log2(wanted_spec.freq / SDL_AUDIO_MAX_CALLBACKS_PER_SEC));
	//wanted_spec.callback = sdl_audio_callback;
	//wanted_spec.userdata = opaque;
	//while (SDL_OpenAudio(&wanted_spec, &spec) < 0) {
	//	av_log(NULL, AV_LOG_WARNING, "SDL_OpenAudio (%d channels, %d Hz): %s\n",
	//		wanted_spec.channels, wanted_spec.freq, SDL_GetError());
	//	wanted_spec.channels = next_nb_channels[FFMIN(7, wanted_spec.channels)];
	//	if (!wanted_spec.channels) {
	//		wanted_spec.freq = next_sample_rates[next_sample_rate_idx--];
	//		wanted_spec.channels = wantedNbChannels;
	//		if (!wanted_spec.freq) {
	//			av_log(NULL, AV_LOG_ERROR,
	//				"No more combinations to try, audio open failed\n");
	//			return -1;
	//		}
	//	}
	//	wantedChannelLayout = av_get_default_channel_layout(wanted_spec.channels);
	//}
	//if (spec.format != AUDIO_S16SYS) {
	//	av_log(NULL, AV_LOG_ERROR,
	//		"SDL advised audio format %d is not supported!\n", spec.format);
	//	return -1;
	//}
	//if (spec.channels != wanted_spec.channels) {
	//	wantedChannelLayout = av_get_default_channel_layout(spec.channels);
	//	if (!wantedChannelLayout) {
	//		av_log(NULL, AV_LOG_ERROR,
	//			"SDL advised channel count %d is not supported!\n", spec.channels);
	//		return -1;
	//	}
	//}

	audioHwParams->Format = AV_SAMPLE_FMT_S16;
	audioHwParams->Freq = 44100;
	audioHwParams->ChannelLayout = wantedChannelLayout;
	audioHwParams->Channels = wantedNbChannels;
	audioHwParams->FrameSize = av_samples_get_buffer_size(NULL, audioHwParams->Channels, 1, audioHwParams->Format, 1);
	audioHwParams->BytesPerSec = av_samples_get_buffer_size(NULL, audioHwParams->Channels, audioHwParams->Freq, audioHwParams->Format, 1);
	if (audioHwParams->BytesPerSec <= 0 || audioHwParams->FrameSize <= 0) {
		av_log(NULL, AV_LOG_ERROR, "av_samples_get_buffer_size failed\n");
		return -1;
	}
	return wantedSampleRate;
}

int32 LDecoder::FLMediaDecoderPrivate::StreamComponentOpen(FVideoState * is, int32 streamIndex)
{
	AVFormatContext *ic = is->Ic;
	AVCodecContext *avctx;
	AVCodec *codec;
	const char *forced_codec_name = NULL;
	AVDictionary *opts;
	AVDictionaryEntry *t = NULL;
	int sample_rate, nb_channels;
	int64_t channel_layout;
	int ret = 0;
	int stream_lowres = false;

	if (streamIndex < 0 || streamIndex >= ic->nb_streams)
		return -1;
	avctx = ic->streams[streamIndex]->codec;

	codec = avcodec_find_decoder(avctx->codec_id);

	switch (avctx->codec_type) {
	case AVMEDIA_TYPE_AUDIO: is->LastAudioStreamIndex = streamIndex; forced_codec_name = AudioCodecName; break;
	case AVMEDIA_TYPE_SUBTITLE: is->LastSubtitleStreamIndex = streamIndex; forced_codec_name = SubtitleCodecName; break;
	case AVMEDIA_TYPE_VIDEO: is->LastVideoStreamIndex = streamIndex; forced_codec_name = VideoCodecName; break;
	}
	if (forced_codec_name)
		codec = avcodec_find_decoder_by_name(forced_codec_name);
	if (!codec) {
		if (forced_codec_name) av_log(NULL, AV_LOG_WARNING,
			"No codec could be found with name '%s'\n", forced_codec_name);
		else                   av_log(NULL, AV_LOG_WARNING,
			"No codec could be found with id %d\n", avctx->codec_id);
		return -1;
	}

	avctx->codec_id = codec->id;
	if (stream_lowres > av_codec_get_max_lowres(codec)) {
		av_log(avctx, AV_LOG_WARNING, "The maximum value for lowres supported by the decoder is %d\n",
			av_codec_get_max_lowres(codec));
		stream_lowres = av_codec_get_max_lowres(codec);
	}
	av_codec_set_lowres(avctx, stream_lowres);

#if FF_API_EMU_EDGE
	if (stream_lowres) avctx->flags |= CODEC_FLAG_EMU_EDGE;
#endif
	if (bFast)
		avctx->flags2 |= AV_CODEC_FLAG2_FAST;
#if FF_API_EMU_EDGE
	if (codec->capabilities & AV_CODEC_CAP_DR1)
		avctx->flags |= CODEC_FLAG_EMU_EDGE;
#endif

	opts = filter_codec_opts(CodecOpts, avctx->codec_id, ic, ic->streams[streamIndex], codec);
	if (!av_dict_get(opts, "threads", NULL, 0))
		av_dict_set(&opts, "threads", "auto", 0);
	if (stream_lowres)
		av_dict_set_int(&opts, "lowres", stream_lowres, 0);
	if (avctx->codec_type == AVMEDIA_TYPE_VIDEO || avctx->codec_type == AVMEDIA_TYPE_AUDIO)
		av_dict_set(&opts, "refcounted_frames", "1", 0);
	if ((ret = avcodec_open2(avctx, codec, &opts)) < 0) {
		goto fail;
	}
	if ((t = av_dict_get(opts, "", NULL, AV_DICT_IGNORE_SUFFIX))) {
		av_log(NULL, AV_LOG_ERROR, "Option %s not found.\n", t->key);
		ret = AVERROR_OPTION_NOT_FOUND;
		goto fail;
	}

	is->Eof = 0;
	ic->streams[streamIndex]->discard = AVDISCARD_DEFAULT;
	switch (avctx->codec_type) {
	case AVMEDIA_TYPE_AUDIO:
		sample_rate = avctx->sample_rate;
		nb_channels = avctx->channels;
		channel_layout = avctx->channel_layout;

		/* prepare audio output */
		if ((ret = AudioOpen(is, channel_layout, nb_channels, sample_rate, &is->AudioTgt)) < 0)
			goto fail;
		is->AudioHwBufSize = ret;
		is->AudioSrc = is->AudioTgt;
		is->AudioBufSize = 0;
		is->AudioBufIndex = 0;

		/* init averaging filter */
		is->AudioDiffAvgCoef = exp(log(0.01) / AUDIO_DIFF_AVG_NB);
		is->AudioDiffAvgCount = 0;
		/* since we do not have a precise anough audio fifo fullness,
		we correct audio sync only if larger than this threshold */
		is->AudioDiffThreshold = (double)(is->AudioHwBufSize) / is->AudioTgt.BytesPerSec;

		is->AudioStreamIndex = streamIndex;
		is->AudioStream = ic->streams[streamIndex];

		DecoderInit(&is->AudioDec, avctx, &is->AudioQueue, is->ContinueReadThread);
		if ((is->Ic->iformat->flags & (AVFMT_NOBINSEARCH | AVFMT_NOGENSEARCH | AVFMT_NO_BYTE_SEEK)) && !is->Ic->iformat->read_seek) {
			is->AudioDec.StartPts = is->AudioStream->start_time;
			is->AudioDec.StartPtsTb = is->AudioStream->time_base;
		}
		if ((ret = DecoderStart(&is->AudioDec, [&]() {return this->AudioThread(is); })) < 0)
			goto fail;
		//SDL_PauseAudio(0);

		//is->AudioThreadId = std::thread([&]() {return this->Execute_AudioThread(is); });
		break;
	case AVMEDIA_TYPE_VIDEO:
		is->VideoStreamIndex = streamIndex;
		is->VideoStream = ic->streams[streamIndex];

		is->VideoDecWidth = avctx->width;
		is->VideoDecHeight = avctx->height;

		DecoderInit(&is->VideoDec, avctx, &is->VideoQueue, is->ContinueReadThread);
		if ((ret = DecoderStart(&is->VideoDec, [&]() {return this->VideoThread(is); })) < 0)
			goto fail;
		is->bQueueAttachmentsReq = true;
		break;
	case AVMEDIA_TYPE_SUBTITLE:
		is->SubtitleStreamIndex = streamIndex;
		is->SubtitleStream = ic->streams[streamIndex];

		DecoderInit(&is->SubDec, avctx, &is->SubtitleQueue, is->ContinueReadThread);
		if ((ret = DecoderStart(&is->SubDec, [&]() {return this->SubtitleThread(is); })) < 0)
			goto fail;
		break;
	default:
		break;
	}

fail:
	av_dict_free(&opts);

	return ret;
}

int32 LDecoder::FLMediaDecoderPrivate::DecodeInterruptCb(void * ctx)
{
	FVideoState *is = (FVideoState *)ctx;
	return is->bAbort;
}

bool LDecoder::FLMediaDecoderPrivate::IsRealtime(AVFormatContext * s)
{
	if (!strcmp(s->iformat->name, "rtp")
		|| !strcmp(s->iformat->name, "rtsp")
		|| !strcmp(s->iformat->name, "sdp")
		)
		return 1;

	if (s->pb && (!strncmp(s->filename, "rtp:", 4)
		|| !strncmp(s->filename, "udp:", 4)
		)
		)
		return 1;
	return 0;
}

int32 LDecoder::FLMediaDecoderPrivate::Execute_ReadThread(void * arg)
{
	FVideoState *is = (FVideoState *)arg;
	AVFormatContext *ic = nullptr;
	int err, i, ret;
	int st_index[AVMEDIA_TYPE_NB];
	AVPacket pkt1, *pkt = &pkt1;
	int64_t stream_start_time;
	int pkt_in_play_range = 0;
	AVDictionaryEntry *t;
	AVDictionary **opts;
	int orig_nb_streams;
	SDL_mutex *wait_mutex = SDL_CreateMutex();
	int scan_all_pmts_set = 0;
	int64_t pkt_ts;

	if (!wait_mutex) {
		av_log(NULL, AV_LOG_FATAL, "SDL_CreateMutex(): %s\n", SDL_GetError());
		ret = AVERROR(ENOMEM);
		goto fail;
	}

	memset(st_index, -1, sizeof(st_index));
	is->LastVideoStreamIndex = is->VideoStreamIndex = -1;
	is->LastAudioStreamIndex = is->AudioStreamIndex = -1;
	is->LastSubtitleStreamIndex = is->SubtitleStreamIndex = -1;
	is->Eof = 0;

	ic = avformat_alloc_context();
	if (!ic) {
		av_log(NULL, AV_LOG_FATAL, "Could not allocate context.\n");
		ret = AVERROR(ENOMEM);
		goto fail;
	}
	//ic->interrupt_callback.callback = (this->FLMediaDecoderPrivate::DecodeInterruptCb);
	//ic->interrupt_callback.opaque = is;
	if (!av_dict_get(FmtOpts, "scan_all_pmts", NULL, AV_DICT_MATCH_CASE)) {
		av_dict_set(&FmtOpts, "scan_all_pmts", "1", AV_DICT_DONT_OVERWRITE);
		scan_all_pmts_set = 1;
	}
	err = avformat_open_input(&ic, is->Filename, is->InputFormat, &FmtOpts);

	//AVFormatContext* ic2 = nullptr;
	//int ret2 = avformat_open_input(&ic2, is->Filename, nullptr, nullptr);
	//if (ret2 < 0)
	//{
	//	av_log(NULL, AV_LOG_ERROR, "failed, %d", ret2);
	//}

	if (err < 0) {
		{
			char errbuf[128];

			if (av_strerror(err, errbuf, sizeof(errbuf)) < 0)
				strerror_s(errbuf, 128, AVUNERROR(err));

			av_log(NULL, AV_LOG_ERROR, "%s: %s\n", is->Filename, errbuf);
		}
		//print_error(is->Filename, err);
		ret = -1;
		goto fail;
	}
	if (scan_all_pmts_set)
		av_dict_set(&FmtOpts, "scan_all_pmts", NULL, AV_DICT_MATCH_CASE);

	if ((t = av_dict_get(FmtOpts, "", NULL, AV_DICT_IGNORE_SUFFIX))) {
		av_log(NULL, AV_LOG_ERROR, "Option %s not found.\n", t->key);
		ret = AVERROR_OPTION_NOT_FOUND;
		goto fail;
	}
	is->Ic = ic;

	//if (Genpts)
	//	ic->flags |= AVFMT_FLAG_GENPTS;

	av_format_inject_global_side_data(ic);

	opts = setup_find_stream_info_opts(ic, CodecOpts);
	orig_nb_streams = ic->nb_streams;

	err = avformat_find_stream_info(ic, opts);

	for (i = 0; i < orig_nb_streams; i++)
		av_dict_free(&opts[i]);
	av_freep(&opts);

	if (err < 0) {
		av_log(NULL, AV_LOG_WARNING,
			"%s: could not find codec parameters\n", is->Filename);
		ret = -1;
		goto fail;
	}

	if (ic->pb)
		ic->pb->eof_reached = 0; // FIXME hack, ffplay maybe should not use avio_feof() to test for the end

	if (!bSeekByBytes)
		bSeekByBytes = !!(ic->iformat->flags & AVFMT_TS_DISCONT) && strcmp("ogg", ic->iformat->name);

	is->MaxFrameDuration = (ic->iformat->flags & AVFMT_TS_DISCONT) ? 10.0 : 3600.0;

	//if (!window_title && (t = av_dict_get(ic->metadata, "title", NULL, 0)))
	//	window_title = av_asprintf("%s - %s", t->value, input_filename);

	/* if seeking requested, we execute it */
	if (StartTime != AV_NOPTS_VALUE) {
		int64_t timestamp;

		timestamp = StartTime;
		/* add the stream start time */
		if (ic->start_time != AV_NOPTS_VALUE)
			timestamp += ic->start_time;
		ret = avformat_seek_file(ic, -1, INT64_MIN, timestamp, INT64_MAX, 0);
		if (ret < 0) {
			av_log(NULL, AV_LOG_WARNING, "%s: could not seek to position %0.3f\n",
				is->Filename, (double)timestamp / AV_TIME_BASE);
		}
	}

	is->bRealtime = !!is_realtime(ic);

	av_dump_format(ic, 0, is->Filename, 0);

	for (i = 0; i < ic->nb_streams; i++) {
		AVStream *st = ic->streams[i];
		enum AVMediaType type = st->codec->codec_type;
		st->discard = AVDISCARD_ALL;
		if (type >= 0 && WantedStreamSpec[type] && st_index[type] == -1)
			if (avformat_match_stream_specifier(ic, st, WantedStreamSpec[type]) > 0)
				st_index[type] = i;
	}
	for (i = 0; i < AVMEDIA_TYPE_NB; i++) {
		if (WantedStreamSpec[i] && st_index[i] == -1) {
			av_log(NULL, AV_LOG_ERROR, "Stream specifier %s does not match any %s stream\n", WantedStreamSpec[i], av_get_media_type_string((AVMediaType)i));
			st_index[i] = INT_MAX;
		}
	}

	if (!bVideoDisable)
		st_index[AVMEDIA_TYPE_VIDEO] =
		av_find_best_stream(ic, AVMEDIA_TYPE_VIDEO,
			st_index[AVMEDIA_TYPE_VIDEO], -1, NULL, 0);
	if (!bAudioDisable)
		st_index[AVMEDIA_TYPE_AUDIO] =
		av_find_best_stream(ic, AVMEDIA_TYPE_AUDIO,
			st_index[AVMEDIA_TYPE_AUDIO],
			st_index[AVMEDIA_TYPE_VIDEO],
			NULL, 0);
	if (!bVideoDisable && !bSubtitleDisable)
		st_index[AVMEDIA_TYPE_SUBTITLE] =
		av_find_best_stream(ic, AVMEDIA_TYPE_SUBTITLE,
			st_index[AVMEDIA_TYPE_SUBTITLE],
			(st_index[AVMEDIA_TYPE_AUDIO] >= 0 ?
				st_index[AVMEDIA_TYPE_AUDIO] :
				st_index[AVMEDIA_TYPE_VIDEO]),
			NULL, 0);

	is->ShowMode = ShowMode;
	if (st_index[AVMEDIA_TYPE_VIDEO] >= 0) {
		AVStream *st = ic->streams[st_index[AVMEDIA_TYPE_VIDEO]];
		AVCodecContext *avctx = st->codec;
		AVRational sar = av_guess_sample_aspect_ratio(ic, st, NULL);
		//if (avctx->width)
			//set_default_window_size(avctx->width, avctx->height, sar);
	}

	/* open the streams */
	if (st_index[AVMEDIA_TYPE_AUDIO] >= 0) {
		StreamComponentOpen(is, st_index[AVMEDIA_TYPE_AUDIO]);
	}

	ret = -1;
	if (st_index[AVMEDIA_TYPE_VIDEO] >= 0) {
		ret = StreamComponentOpen(is, st_index[AVMEDIA_TYPE_VIDEO]);
	}
	if (is->ShowMode == SHOW_MODE_NONE)
		is->ShowMode = ret >= 0 ? SHOW_MODE_VIDEO : SHOW_MODE_RDFT;

	if (st_index[AVMEDIA_TYPE_SUBTITLE] >= 0) {
		StreamComponentOpen(is, st_index[AVMEDIA_TYPE_SUBTITLE]);
	}

	if (is->VideoStreamIndex < 0 && is->AudioStreamIndex < 0) {
		av_log(NULL, AV_LOG_FATAL, "Failed to open file '%s' or configure filtergraph\n",
			is->Filename);
		ret = -1;
		goto fail;
	}

	if (InfiniteBuffer < 0 && is->bRealtime)
		InfiniteBuffer = 1;

	for (;;) {
		if (is->bAbort)
			break;
		if (is->bPaused != is->bLastPaused) {
			is->bLastPaused = is->bPaused;
			if (is->bPaused)
				is->ReadPauseReturn = av_read_pause(ic);
			else
				av_read_play(ic);
		}
#if CONFIG_RTSP_DEMUXER || CONFIG_MMSH_PROTOCOL
		if (is->paused &&
			(!strcmp(ic->iformat->name, "rtsp") ||
			(ic->pb && !strncmp(input_filename, "mmsh:", 5)))) {
			/* wait 10 ms to avoid trying to get another packet */
			/* XXX: horrible */
			SDL_Delay(10);
			continue;
		}
#endif
		if (is->bSeekReq) {
			int64_t seek_target = is->SeekPos;
			int64_t seek_min = is->SeekRel > 0 ? seek_target - is->SeekRel + 2 : INT64_MIN;
			int64_t seek_max = is->SeekRel < 0 ? seek_target - is->SeekRel - 2 : INT64_MAX;
			// FIXME the +-2 is due to rounding being not done in the correct direction in generation
			//      of the seek_pos/seek_rel variables

			ret = avformat_seek_file(is->Ic, -1, seek_min, seek_target, seek_max, is->SeekFlags);
			if (ret < 0) {
				av_log(NULL, AV_LOG_ERROR,
					"%s: error while seeking\n", is->Ic->filename);
			}
			else {
				if (is->AudioStreamIndex >= 0) {
					PacketQueueFlush(&is->AudioQueue);
					PacketQueuePut(&is->AudioQueue, &FlushPacket);
				}
				if (is->SubtitleStreamIndex >= 0) {
					PacketQueueFlush(&is->SubtitleQueue);
					PacketQueuePut(&is->SubtitleQueue, &FlushPacket);
				}
				if (is->VideoStreamIndex >= 0) {
					PacketQueueFlush(&is->VideoQueue);
					PacketQueuePut(&is->VideoQueue, &FlushPacket);
				}
				if (is->SeekFlags & AVSEEK_FLAG_BYTE) {
					SetClock(&is->ExternClock, NAN, 0);
				}
				else {
					SetClock(&is->ExternClock, seek_target / (double)AV_TIME_BASE, 0);
				}
			}
			is->bSeekReq = false;
			is->bQueueAttachmentsReq = true;
			is->Eof = 0;
			if (is->bPaused)
				StepToNextFrame(is);
		}
		if (is->bQueueAttachmentsReq) {
			if (is->VideoStream && is->VideoStream->disposition & AV_DISPOSITION_ATTACHED_PIC) {
				AVPacket copy;
				if ((ret = av_copy_packet(&copy, &is->VideoStream->attached_pic)) < 0)
					goto fail;
				PacketQueuePut(&is->VideoQueue, &copy);
				PacketQueuePutNull(&is->VideoQueue, is->VideoStreamIndex);
			}
			is->bQueueAttachmentsReq = false;
		}

		/* if the queue are full, no need to read more */
		if (InfiniteBuffer<1 &&
			(is->AudioQueue.Size + is->VideoQueue.Size + is->SubtitleQueue.Size > MAX_QUEUE_SIZE
				|| ((is->AudioQueue.Num > MIN_FRAMES || is->AudioStreamIndex < 0 || is->AudioQueue.bAbort)
					&& (is->VideoQueue.Num > MIN_FRAMES || is->VideoStreamIndex < 0 || is->VideoQueue.bAbort
						|| (is->VideoStream->disposition & AV_DISPOSITION_ATTACHED_PIC))
					&& (is->SubtitleQueue.Num > MIN_FRAMES || is->SubtitleStreamIndex < 0 || is->SubtitleQueue.bAbort)))) {
			/* wait 10 ms */
			SDL_LockMutex(wait_mutex);
			SDL_CondWaitTimeout(is->ContinueReadThread, wait_mutex, 10);
			SDL_UnlockMutex(wait_mutex);
			continue;
		}
		if (!is->bPaused &&
			(!is->AudioStream || (is->AudioDec.Finished == is->AudioQueue.Serial && FrameQueueNBRemaining(&is->SamQueue) == 0)) &&
			(!is->VideoStream || (is->VideoDec.Finished == is->VideoQueue.Serial && FrameQueueNBRemaining(&is->PicQueue) == 0))) {
			if (Loop != 1 && (!Loop || --Loop)) {
				StreamSeek(is, StartTime != AV_NOPTS_VALUE ? StartTime : 0, 0, 0);
			}
			else if (bAutoexit) {
				ret = AVERROR_EOF;
				goto fail;
			}
		}
		ret = av_read_frame(ic, pkt);
		if (ret < 0) {
			if ((ret == AVERROR_EOF || avio_feof(ic->pb)) && !is->Eof) {
				if (is->VideoStreamIndex >= 0)
					PacketQueuePutNull(&is->VideoQueue, is->VideoStreamIndex);
				if (is->AudioStreamIndex >= 0)
					PacketQueuePutNull(&is->AudioQueue, is->AudioStreamIndex);
				if (is->SubtitleStreamIndex >= 0)
					PacketQueuePutNull(&is->SubtitleQueue, is->SubtitleStreamIndex);
				is->Eof = 1;
			}
			if (ic->pb && ic->pb->error)
				break;
			SDL_LockMutex(wait_mutex);
			SDL_CondWaitTimeout(is->ContinueReadThread, wait_mutex, 10);
			SDL_UnlockMutex(wait_mutex);
			continue;
		}
		else {
			is->Eof = 0;
		}
		/* check if packet is in play range specified by user, then queue, otherwise discard */
		stream_start_time = ic->streams[pkt->stream_index]->start_time;
		pkt_ts = pkt->pts == AV_NOPTS_VALUE ? pkt->dts : pkt->pts;
		pkt_in_play_range = Duration == AV_NOPTS_VALUE ||
			(pkt_ts - (stream_start_time != AV_NOPTS_VALUE ? stream_start_time : 0)) *
			av_q2d(ic->streams[pkt->stream_index]->time_base) -
			(double)(StartTime != AV_NOPTS_VALUE ? StartTime : 0) / 1000000
			<= ((double)Duration / 1000000);
		if (pkt->stream_index == is->AudioStreamIndex && pkt_in_play_range) {
			PacketQueuePut(&is->AudioQueue, pkt);
		}
		else if (pkt->stream_index == is->VideoStreamIndex && pkt_in_play_range
			&& !(is->VideoStream->disposition & AV_DISPOSITION_ATTACHED_PIC)) {
			PacketQueuePut(&is->VideoQueue, pkt);
		}
		else if (pkt->stream_index == is->SubtitleStreamIndex && pkt_in_play_range) {
			PacketQueuePut(&is->SubtitleQueue, pkt);
		}
		else {
			av_packet_unref(pkt);
		}
	}

	ret = 0;
fail:
	if (ic && !is->Ic)
		avformat_close_input(&ic);

	if (ret != 0) {
		//SDL_Event event;

		//event.type = FF_QUIT_EVENT;
		//event.user.data1 = is;
		//SDL_PushEvent(&event);
	}
	SDL_DestroyMutex(wait_mutex);
	return 0;
}

int32 LDecoder::FLMediaDecoderPrivate::Execute_LoopThread(void * arg)
{
	FVideoState *is = (FVideoState *)arg;
	double remaining = 0.0;
	while (!is->bAbort)
	{
		{
			std::lock_guard<std::mutex> guard(CmdsLock);
			auto it = Cmds.begin();
			while (it != Cmds.end())
			{
				(*it++)();
			}

			Cmds.clear();
		}

		Refresh_LoopThread(is, &remaining);
	}

	StreamClose(is);

	return 0;
}

void LDecoder::FLMediaDecoderPrivate::Refresh_LoopThread(FVideoState * is, double* remaining)
{
	if (*remaining > 0.0)
	{
		av_usleep(*remaining * 1000000.0);
	}

	*remaining = 0.008;
	if (is->ShowMode != SHOW_MODE_NONE && (!is->bPaused || is->bForceRefresh))
	{
		VideoRefresh(is, remaining);
		//Execute_AudioThread(is);
	}
}

FVideoState * LDecoder::FLMediaDecoderPrivate::StreamOpen(const char * filename, AVInputFormat * iformat)
{
	FVideoState *is;

	//auto ta = std::thread([]() {int ret = 0; int i = 10000; OutputDebugStringA("ta begin\n"); while (i-- > 0) ret += i; OutputDebugStringA(std::to_string(ret).c_str()); OutputDebugStringA("\nta end\n"); });
	//ta.detach();
	//auto tb = std::thread([]() {int ret = 0; int i = 5000; OutputDebugStringA("tb begin\n"); while (i-- > 0) ret += i; OutputDebugStringA(std::to_string(ret).c_str()); OutputDebugStringA("\ntb end\n"); });
	//tb.detach();
	//auto tc = std::thread([]() {int ret = 0; int i = 1000; OutputDebugStringA("tc begin\n"); while (i-- > 0) ret += i; OutputDebugStringA(std::to_string(ret).c_str()); OutputDebugStringA("\ntc end\n"); });
	//tc.detach();

	is = (FVideoState *)av_mallocz(sizeof(FVideoState));
	if (!is)
		return NULL;
	is->Filename = av_strdup(filename);
	if (!is->Filename)
		goto fail;
	is->InputFormat = iformat;
	is->YTop = 0;
	is->XLeft = 0;

	/* start video display */
	if (FrameQueueInit(&is->PicQueue, &is->VideoQueue, VIDEO_PICTURE_QUEUE_SIZE, 1) < 0)
		goto fail;
	if (FrameQueueInit(&is->SubQueue, &is->SubtitleQueue, SUBPICTURE_QUEUE_SIZE, 0) < 0)
		goto fail;
	if (FrameQueueInit(&is->SamQueue, &is->AudioQueue, SAMPLE_QUEUE_SIZE, 1) < 0)
		goto fail;

	if (PacketQueueInit(&is->VideoQueue) < 0 ||
		PacketQueueInit(&is->AudioQueue) < 0 ||
		PacketQueueInit(&is->SubtitleQueue) < 0)
		goto fail;

	if (!(is->ContinueReadThread = SDL_CreateCond())) {
		av_log(NULL, AV_LOG_FATAL, "SDL_CreateCond(): %s\n", SDL_GetError());
		goto fail;
	}

	InitClock(&is->VideoClock, &is->VideoQueue.Serial);
	InitClock(&is->AudioClock, &is->AudioQueue.Serial);
	InitClock(&is->ExternClock, &is->ExternClock.Serial);
	is->AudioClockSerial = -1;
	is->AudioVolume = SDL_MIX_MAXVOLUME;
	is->bMuted = 0;
	is->AvSyncType = AvSyncType;
	is->ReadThreadId = std::thread([&]() {return this->Execute_ReadThread(is); });
	is->LoopThreadId = std::thread([&]() {return this->Execute_LoopThread(is); });
	return is;

	fail:
		StreamClose(is);
		return nullptr;
}

void LDecoder::FLMediaDecoderPrivate::StreamCycleChannel(FVideoState * is, int32 codecType)
{
	AVFormatContext *ic = is->Ic;
	int start_index, stream_index;
	int old_index;
	AVStream *st;
	AVProgram *p = NULL;
	int nb_streams = is->Ic->nb_streams;

	if (codecType == AVMEDIA_TYPE_VIDEO) {
		start_index = is->LastVideoStreamIndex;
		old_index = is->VideoStreamIndex;
	}
	else if (codecType == AVMEDIA_TYPE_AUDIO) {
		start_index = is->LastAudioStreamIndex;
		old_index = is->AudioStreamIndex;
	}
	else {
		start_index = is->LastSubtitleStreamIndex;
		old_index = is->SubtitleStreamIndex;
	}
	stream_index = start_index;

	if (codecType != AVMEDIA_TYPE_VIDEO && is->VideoStreamIndex != -1) {
		p = av_find_program_from_stream(ic, NULL, is->VideoStreamIndex);
		if (p) {
			nb_streams = p->nb_stream_indexes;
			for (start_index = 0; start_index < nb_streams; start_index++)
				if (p->stream_index[start_index] == stream_index)
					break;
			if (start_index == nb_streams)
				start_index = -1;
			stream_index = start_index;
		}
	}

	for (;;) {
		if (++stream_index >= nb_streams)
		{
			if (codecType == AVMEDIA_TYPE_SUBTITLE)
			{
				stream_index = -1;
				is->LastSubtitleStreamIndex = -1;
				goto the_end;
			}
			if (start_index == -1)
				return;
			stream_index = 0;
		}
		if (stream_index == start_index)
			return;
		st = is->Ic->streams[p ? p->stream_index[stream_index] : stream_index];
		if (st->codec->codec_type == codecType) {
			/* check that parameters are OK */
			switch (codecType) {
			case AVMEDIA_TYPE_AUDIO:
				if (st->codec->sample_rate != 0 &&
					st->codec->channels != 0)
					goto the_end;
				break;
			case AVMEDIA_TYPE_VIDEO:
			case AVMEDIA_TYPE_SUBTITLE:
				goto the_end;
			default:
				break;
			}
		}
	}
the_end:
	if (p && stream_index != -1)
		stream_index = p->stream_index[stream_index];
	av_log(NULL, AV_LOG_INFO, "Switch %s stream from #%d to #%d\n",
		av_get_media_type_string((AVMediaType)codecType),
		old_index,
		stream_index);

	StreamComponentClose(is, old_index);
	StreamComponentOpen(is, stream_index);
}

void LDecoder::FLMediaDecoderPrivate::ToggleAudioDisplay(FVideoState * is)
{
	//int bgcolor = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
	int next = is->ShowMode;
	do {
		next = (next + 1) % SHOW_MODE_NB;
	} while (next != is->ShowMode && (next == SHOW_MODE_VIDEO && !is->VideoStream || next != SHOW_MODE_VIDEO && !is->AudioStream));
	if (is->ShowMode != next) {
		//fill_rectangle(screen,
		//	is->xleft, is->ytop, is->width, is->height,
		//	bgcolor, 1);
		is->bForceRefresh = 1;
		is->ShowMode = (EShowMode)next;
	}
}

void LDecoder::FLMediaDecoderPrivate::SeekChapter(FVideoState * is, int32 incr)
{
	int64_t pos = GetMasterClock(is) * AV_TIME_BASE;
	int i;

	if (!is->Ic->nb_chapters)
		return;

	/* find the current chapter */
	for (i = 0; i < is->Ic->nb_chapters; i++) {
		AVChapter *ch = is->Ic->chapters[i];
		if (av_compare_ts(pos, {1, AV_TIME_BASE }, ch->start, ch->time_base) < 0) {
			i--;
			break;
		}
	}

	i += incr;
	i = FFMAX(i, 0);
	if (i >= is->Ic->nb_chapters)
		return;

	av_log(NULL, AV_LOG_VERBOSE, "Seeking to chapter %d.\n", i);
	StreamSeek(is, av_rescale_q(is->Ic->chapters[i]->start, is->Ic->chapters[i]->time_base,
	{ 1, AV_TIME_BASE }), 0, 0);
}

void LDecoder::FLMediaDecoderPrivate::DoSeek(FVideoState * is, int32 incr)
{
	double pos;
	if (bSeekByBytes) {
		pos = -1;
		if (pos < 0 && is->VideoStreamIndex >= 0)
			pos = FrameQueueLastPos(&is->PicQueue);
		if (pos < 0 && is->AudioStreamIndex >= 0)
			pos = FrameQueueLastPos(&is->SamQueue);
		if (pos < 0)
			pos = avio_tell(is->Ic->pb);
		if (is->Ic->bit_rate)
			incr *= is->Ic->bit_rate / 8.0;
		else
			incr *= 180000.0;
		pos += incr;
		StreamSeek(is, pos, incr, 1);
	}
	else {
		pos = GetMasterClock(is);
		if (isnan(pos))
			pos = (double)is->SeekPos / AV_TIME_BASE;
		pos += incr;
		if (is->Ic->start_time != AV_NOPTS_VALUE && pos < is->Ic->start_time / (double)AV_TIME_BASE)
			pos = is->Ic->start_time / (double)AV_TIME_BASE;
		StreamSeek(is, (int64_t)(pos * AV_TIME_BASE), (int64_t)(incr * AV_TIME_BASE), 0);
	}
}

LDecoder::FLMediaDecoderPrivate::FLMediaDecoderPrivate()
	: SwsFlags(SWS_BICUBIC)
	, SwsDict(nullptr)
	, SwrOpts(nullptr)
	, FmtOpts(nullptr)
	, CodecOpts(nullptr)
	, ResampleOpts(nullptr)
	, InputFileName(nullptr)
	, WindowTitle(nullptr)
	, ScreenWidth(-1)
	, ScreenHeight(-1)
	, bAudioDisable(false)
	, bVideoDisable(false)
	, bSubtitleDisable(false)
	, bDisplayDisable(false)
	, bFast(false)
	, bLowres(false)
	, bAutoexit(false)
	, bSeekByBytes(false)
	, AvSyncType(AV_SYNC_AUDIO_MASTER)
	, StartTime(AV_NOPTS_VALUE)
	, Duration(AV_NOPTS_VALUE)
	, DecoderReorderPts(-1)
	, Loop(1)
	, Framedrop(-1)
	, InfiniteBuffer(-1)
	, ShowMode(SHOW_MODE_VIDEO)
	, AudioCodecName(nullptr)
	, SubtitleCodecName(nullptr)
	, VideoCodecName(nullptr)
	, RdftSpeed(0.02)
	, AudioCallbackTime(0)
{
	av_register_all();
	avformat_network_init();

	for (int32 i = 0; i < AVMEDIA_TYPE_NB; ++i)
	{
		WantedStreamSpec[i] = nullptr;
	}
}

LDecoder::FLMediaDecoderPrivate::~FLMediaDecoderPrivate()
{
}

FLMediaDecoder::FLMediaDecoder()
	: PrivateDecoder(nullptr)
	, DecodeCallback(nullptr)
{
	PrivateDecoder = new LDecoder::FLMediaDecoderPrivate;
}

FLMediaDecoder::~FLMediaDecoder()
{
	if (PrivateDecoder != nullptr)
	{
		delete PrivateDecoder;
		PrivateDecoder = nullptr;
	}
}

bool FLMediaDecoder::OpenUrl(const char * url)
{
	UrlToPlay = url;
	LDecoder::FVideoState* is = PrivateDecoder->StreamOpen(url, nullptr);
	is->DecodeCB = DecodeCallback;
	return true;
}

void FLMediaDecoder::EnqueueCommand(EDecodeCommand cmd)
{
}

void FLMediaDecoder::SetCallback(IDecodeCallbackWeakPtr callback)
{
	DecodeCallback = callback;
}

void FLMediaDecoder::SetRate(float rate)
{
	Rate = rate;
}

float FLMediaDecoder::GetRate() const
{
	return Rate;
}

void FLMediaDecoder::SetLooping(bool looping)
{
}

bool FLMediaDecoder::GetLooping() const
{
	return false;
}

void FLMediaDecoder::Seek(double pos)
{
}

double FLMediaDecoder::GetPos() const
{
	return 0.0;
}

EDecodeState FLMediaDecoder::GetState() const
{
	return EDecodeState();
}

FLostMediaInfo * FLMediaDecoder::GetMediaInfo()
{
	return nullptr;
}

void FLMediaDecoder::Update()
{
}

DecoderHandle DecodeMedia(IDecodeCallbackWeakPtr callback, const char* url)
{
	DecoderHandle handle(new FLMediaDecoder);
	handle->SetCallback(callback);
	handle->OpenUrl(url);
	return handle;
}

void ReleaseDecoder(DecoderHandle* handle)
{
	if (handle != nullptr)
	{
		*handle = nullptr;
	}
}