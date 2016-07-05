#include "stdafx.h"
#include "MediaDecoder.h"
#include "MpegDecoder.h"

int InitializeMediaDecoder()
{
	VideoPlayer Player;
	Player.audio_play = false;
	Player.load("F:\\files\\hlh\\showcase.mp4");
	Player.decodeFrame(nullptr, nullptr);
	return 0;
}