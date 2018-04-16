#include <memory>
#include <vector>
#include <concurrent_queue.h>
#include <thread>
#include <atomic>
#include <queue>

extern "C" {
#include <libavformat\avformat.h>
#include <libavcodec\avcodec.h>
#include <libswscale\swscale.h>
#include <libswresample\swresample.h>
}

#include "video.h"
#include "video_ffmpeg.h"
#include "dxva2decoder.h"

#pragma comment(lib,"avutil" )
#pragma comment(lib,"avcodec" )
#pragma comment(lib,"avformat" )
#pragma comment(lib,"swscale" )
#pragma comment(lib,"swresample" )

// TODO: Reset sound device (change fx in control panel will kill the device)

double invTimerFrequency = 1.0;
LARGE_INTEGER startTime = { 0 };
double totalTime = 0;
bool timerInited = false;

struct QueuePacket {
	AVPacket pkt;
	int serial;
	byte seek;
	byte loop;
	byte _pad[2];
};

void InitTimer()
{
	if (timerInited)
	{
		return;
	}
	timerInited = true;

	LARGE_INTEGER timerFrequency = { 0 };
	QueryPerformanceFrequency(&timerFrequency);
	invTimerFrequency = 1.0 / (double)timerFrequency.QuadPart;

	QueryPerformanceCounter(&startTime);
}

double GetPreciseTime()
{
	LARGE_INTEGER curTime = { 0 };
	QueryPerformanceCounter(&curTime);

	double time = (double)(curTime.QuadPart - startTime.QuadPart) * invTimerFrequency;
	return time;
}

class TFFMPEGVideoObject_Int : public TFFMPEGVideoObject {
public:
	concurrency::concurrent_queue<TVideoCommand> aCommand;
public:
	std::thread* streamThread;
	AVFormatContext* pFormatContext;
	std::atomic<bool> isEndOfStream;
	std::atomic<bool> isStreamThreadDone;
public:
	AVStream* pVideoStream;
	AVCodec* pVideoCodec;
	AVCodecContext* pVideoDecoderContext;
	concurrency::concurrent_queue<QueuePacket> aVideoPacket;
	SwsContext* pVideoRescalerContext;
	std::atomic<bool> isVideoThreadDone;
	double frameRate;
	double lastPts;
public:
	AVStream* pAudioStream;
	AVCodec* pAudioCodec;
	AVCodecContext* pAudioDecoderContext;
	concurrency::concurrent_queue<QueuePacket> aAudioPacket;
	SwrContext* pAudioResamplerContext;
	std::atomic<bool> isAudioThreadDone;
	std::atomic<double> audioSeekTime;
	HANDLE videoThreadEvent;
	HANDLE audioThreadEvent;
	HANDLE streamThreadEventA;
	HANDLE streamThreadEventV;
	HANDLE streamThreadEventCommand;
public:
	TFFMPEGVideoObject_Int() :
		pFormatContext(0), isEndOfStream(0),
		pVideoStream(0), pVideoCodec(0), pVideoDecoderContext(0), pVideoRescalerContext(0), frameRate(1.0), lastPts(NAN),
		pAudioStream(0), pAudioCodec(0), pAudioDecoderContext(0), pAudioResamplerContext(0), audioSeekTime(0.0f)
	{
		isEndOfStream = false;
		isVideoThreadDone = false;
		isAudioThreadDone = false;
		isStreamThreadDone = false;
	}
public:
	void Initialize();
	void Play();
	void Pause();
	void Seek(const float &time);
	void RequestShutdown();
	int TryWaitShutdown();
	void SignalFrameConsumed();
protected:
	void PutCommand(TVideoCommand vc);
	int StreamThread();
	int AudioDecoderThread();
	int VideoDecoderThread();
	void InternalShutdown();
};

TVideoObject* TFFMPEGVideoObject::Create()
{
	static int isInitialized = 0;
	if (!isInitialized)
	{
		av_register_all();
		avformat_network_init();
		isInitialized = 1;
	}
	return new TFFMPEGVideoObject_Int;
}

void TFFMPEGVideoObject::Destroy(TVideoObject* vo)
{
	delete (TFFMPEGVideoObject_Int*)vo;
}

void TFFMPEGVideoObject_Int::InternalShutdown()
{
	State = TVideoObject::sShutdown;

	// Release thread events
	CloseHandle(videoThreadEvent);
	CloseHandle(audioThreadEvent);
	CloseHandle(streamThreadEventA);
	CloseHandle(streamThreadEventV);
	CloseHandle(streamThreadEventCommand);

	// Release any queued up packets
	QueuePacket qp;
	while (aVideoPacket.try_pop(qp))
		av_packet_unref(&qp.pkt);
	while (aAudioPacket.try_pop(qp))
		av_packet_unref(&qp.pkt);

	// Shutdown video rescaler and audio resampler
	if (pVideoRescalerContext) { sws_freeContext(pVideoRescalerContext); pVideoRescalerContext = 0; }
	if (pAudioResamplerContext) { swr_free(&pAudioResamplerContext); }

	// Shutdown the codecs
	if (pVideoDecoderContext) { avcodec_close(pVideoDecoderContext); HWDecoderManager::Uninit(pVideoDecoderContext); av_free(pVideoDecoderContext); pVideoDecoderContext = 0; }
	if (pAudioDecoderContext) { avcodec_close(pAudioDecoderContext); av_free(pAudioDecoderContext); pAudioDecoderContext = 0; }
	pVideoCodec = 0;
	pAudioCodec = 0;

	// Shutdown the input stream s
	if (pFormatContext) { avformat_close_input(&pFormatContext); pFormatContext = 0; }
	pVideoStream = 0;
	pAudioStream = 0;

	isStreamThreadDone = true;
}

double audioThreadSleepTime = 0, audioCodecTime = 0, audioResampleTime = 0, audioThreadTotalTime = 0;

int TFFMPEGVideoObject_Int::AudioDecoderThread()
{
	int audioSerial = 0;
	std::queue<int> loopDecoderSerial;
	QueuePacket qp = { 0 };
	bool bHasFrame = false;
	AVFrame* audioFrame = av_frame_alloc();
	bool eofPacketSubmitted = false;
	bool isEof = false;

	if (pAudioStream)
	{
		if (pAudioOut->Initialize(decodeBufferAudioMs))
		{
			MError("Error initializing audio player\n");
			return 1;
		}
	}

	for (;;)
	{
		int noInputData = 0;
		int noOutputBuffer = 0;
		double loopStart = GetPreciseTime();

		// Don't do anything if paused
		if (State == TVideoObject::sPaused)
		{
			if (pAudioStream)
				pAudioOut->Stop();
			SleepEx(1, FALSE);
			continue;
		}
		else if (State == TVideoObject::sPlaying)
		{
			if (pAudioStream)
				pAudioOut->Start();
		}
		else if (State == TVideoObject::sShutdown)
		{
			break;
		}

		// Get time from audio, if we have a stream
		if (pAudioStream)
		{
			if (pAudioOut->GetTime(syncTime, syncSerial))
			{
				break;
			}
		}

		if (!isEof)
		{
			// Get new data from stream thread
			if (qp.pkt.size == 0)
			{
				if (aAudioPacket.try_pop(qp) == false)
				{
					if (isEndOfStream && !eofPacketSubmitted)
					{
						// Send EOF/zero packet to decoder
						avcodec_send_packet(pAudioDecoderContext, NULL);
						eofPacketSubmitted = true;
					}
					else
					{
						noInputData = 1;
					}
				}
				else
				{
					// Signal stream thread to continue filling our buffer
					SetEvent(streamThreadEventA);

					// If we got a seek packet dump everything and start again as soon as possible
					if (qp.seek)
					{
						// Flush the decoder
						avcodec_flush_buffers(pAudioDecoderContext);
						// Flush the output
						pAudioOut->FlushBuffers();
						// Release the currently processing frame
						if (bHasFrame)
							av_frame_unref(audioFrame);
						bHasFrame = false;

						// All frames coming out of the decoder from now on will have the new serial
						while (!loopDecoderSerial.empty())
							loopDecoderSerial.pop();
						audioSerial = qp.serial;
					}
					else if (qp.loop) // Loop packets are handled smoothly - don't drop any data!
					{
						// Send EOF/zero packet to decoder
						avcodec_send_packet(pAudioDecoderContext, NULL);

						// Add the serial to the queue (will get popped once decoder returns EOF)
						loopDecoderSerial.push(qp.serial);
					}
				}
			}

			// Submit a packet to the decoder (can fail if buffers are full)
			if (qp.pkt.size > 0)
			{
				int ret = avcodec_send_packet(pAudioDecoderContext, &qp.pkt);
				switch (ret) {
				case 0:
					av_packet_unref(&qp.pkt);
					break;
				case AVERROR(EAGAIN): // Input buffer full, must consume frames with avcodec_receive_frame first
				case AVERROR_EOF: // Null packet has been submitted and decoder hasn't been flushed yet
					break;
				default:
					MError("Error in audio avcodec_send_packet! Error: %d", ret);
					break;
				}
			}

			// Decode frame
			if (!bHasFrame)
			{
				double t0 = GetPreciseTime();
				int ret = avcodec_receive_frame(pAudioDecoderContext, audioFrame);
				audioCodecTime += GetPreciseTime() - t0;
				switch (ret) {
					case 0:
						bHasFrame = true;
						break;
					case AVERROR(EAGAIN):
						break;
					case AVERROR_EOF:
						// End of stream (coded consumed everything) - check if we're looping and advance the serial and flush the decoder to be able to resume decoding
						if (!loopDecoderSerial.empty())
						{
							audioSerial = loopDecoderSerial.front();
							loopDecoderSerial.pop();
							avcodec_flush_buffers(pAudioDecoderContext);
						}
						else
						{
							// Put EOF marker in audio output stream
							pAudioOut->BeginOutput(0, 0, 0);
							isEof = true;
						}
						break;
					default:
						MError("Error in audio avcodec_receive_frame! Error: %d", ret);
						break;
				}
			}

			// Output frame
			if (bHasFrame)
			{
				if (!pAudioResamplerContext)
				{
					AVSampleFormat outSampleFormat = (AVSampleFormat)pAudioOut->GetOutputFormat();
					int outSampleRate = pAudioOut->GetOutputSampleRate();
					int outChannelLayout = pAudioOut->GetOutputChannelLayout();

					if (audioFrame->channel_layout == 0)
					{
						switch (audioFrame->channels) {
						case 1: audioFrame->channel_layout = AV_CH_LAYOUT_MONO; break;
						case 2: audioFrame->channel_layout = AV_CH_LAYOUT_STEREO; break;
						case 3: audioFrame->channel_layout = AV_CH_LAYOUT_2_1; break;
						case 4: audioFrame->channel_layout = AV_CH_LAYOUT_QUAD; break;
						case 5: audioFrame->channel_layout = AV_CH_LAYOUT_5POINT0; break;
						case 6: audioFrame->channel_layout = AV_CH_LAYOUT_5POINT1; break;
						case 7: audioFrame->channel_layout = AV_CH_LAYOUT_7POINT0; break;
						case 8: audioFrame->channel_layout = AV_CH_LAYOUT_5POINT1; break;
						}

					}

					pAudioResamplerContext = swr_alloc_set_opts(0, outChannelLayout, outSampleFormat, outSampleRate, audioFrame->channel_layout, (AVSampleFormat)audioFrame->format, audioFrame->sample_rate, 0, 0);
					swr_init(pAudioResamplerContext);
				}

				AVRational tbq = { 1, AV_TIME_BASE };
				int64_t intpts = av_frame_get_best_effort_timestamp(audioFrame);
				double pts = intpts == AV_NOPTS_VALUE ? NAN : av_rescale_q(intpts, pAudioStream->time_base, tbq) / 1000000.0;

				int outSampleRate = pAudioOut->GetOutputSampleRate();
				int numOutSamples = (audioFrame->nb_samples * outSampleRate) / audioFrame->sample_rate;

				void* buf = pAudioOut->BeginOutput(numOutSamples, pts, audioSerial);
				if (buf)
				{
					double t0 = GetPreciseTime();
					swr_convert(pAudioResamplerContext, (uint8_t**)&buf, numOutSamples, (const uint8_t**)audioFrame->data, audioFrame->nb_samples);
					audioResampleTime += GetPreciseTime() - t0;
					pAudioOut->EndOutput(numOutSamples);
					av_frame_unref(audioFrame);
					bHasFrame = false;
				}
				else
				{
					noOutputBuffer = 1;
				}
			}
		}

		// Put audio decoder thread to sleep if it can't do any work
		int canGetNewData = qp.pkt.size == 0 && !noInputData;
		int canDecodePacket = qp.pkt.size > 0 && !bHasFrame;
		int canOutputFrame = bHasFrame && !noOutputBuffer;
		if (!canGetNewData && !canDecodePacket && !canOutputFrame)
		{
			double t0 = GetPreciseTime();
			WaitForSingleObject(audioThreadEvent, 10);
			audioThreadSleepTime += GetPreciseTime() - t0;
		}

		audioThreadTotalTime += GetPreciseTime() - loopStart;
	}

	av_packet_unref(&qp.pkt);
	av_frame_free(&audioFrame);
	if (pAudioStream)
	{
		pAudioOut->Shutdown();
		pAudioStream = 0;
	}
	isAudioThreadDone = true;
	return 0;
}

double videoThreadSleepTime = 0, videoDecoderSubmitPacketTime = 0, videoDecoderReceiveFrameTime = 0, videoScalerTime = 0, videoThreadTotalTime = 0, videoHWRetrieveTime = 0, videoWaitForOutputBufferTime = 0;

int TFFMPEGVideoObject_Int::VideoDecoderThread()
{
	int frameState = 0;
	bool bHadAudioSync = true;
	double prevTime = GetPreciseTime();
	double lastFrameOutputTime = 0;
	bool seekHappened = false;
	int videoSerial = 0;
	std::queue<int> loopDecoderSerial;
	std::queue<int> loopTimerSerial;
	AVFrame* videoFrame = av_frame_alloc();
	QueuePacket qp = { 0 };
	bool eofPacketSubmitted = false;
	bool isEof = false;
	double waitForBufferStartTime = 0;

	// Initialize timer
	if (!pAudioStream)
	{
		syncSerial = videoSerial;
		bHadAudioSync = false;
	}

	for (;;)
	{
		double loopStart = GetPreciseTime();
		int noInputData = 0;
		int noOutputBuffer = 0;

		// Don't do anything if paused
		if (State == TVideoObject::sPaused)
		{
			SleepEx(1, FALSE);
			continue;
		}
		else if (State == TVideoObject::sShutdown)
		{
			break;
		}

		// Update timer if needed
		if (!pAudioStream)
		{
			// Increment time (forever for streams or until end of video)	
			if (pFrameOut->videoLength == 0.0f || syncTime < pFrameOut->videoLength)
			{
				double newTime = GetPreciseTime();
				double dt;
				// handle playing after audio is over
				if (bHadAudioSync)
				{
					dt = newTime - lastFrameOutputTime;
					bHadAudioSync = false;
				}
				else
				{
					dt = newTime - prevTime;
				}
				prevTime = newTime;
				syncTime += dt < 0.1 ? dt : 0.1; // Max 0.1s time step
			}

			// Clamp to video length or loop
			if (pFrameOut->videoLength > 0 && syncTime > pFrameOut->videoLength)
			{
				while (!loopTimerSerial.empty() && syncTime > pFrameOut->videoLength)
				{
					syncTime = syncTime - pFrameOut->videoLength;
					syncSerial = loopTimerSerial.front();
					loopTimerSerial.pop();
					//MLog("V: Time loop %d", syncSerial);
				}
					
				if(syncTime > pFrameOut->videoLength)
				{
					syncTime = pFrameOut->videoLength;

					// If end of stream and no audio then terminate video thread
					if (isEof)
						break;
					
					//MLog("V: Time clamped!");
				}
			}	

			// For infinite streams, break out on eof here since it won't break out of the loop in the timing code above
			if (pFrameOut->videoLength == 0.0f && isEof)
			{
				break;
			}
		}

		if (!isEof)
		{
			// Get a data packet from the video packet queue (wait if not available)
			if (qp.pkt.size == 0)
			{
				if (aVideoPacket.try_pop(qp) == false)
				{
					if (isEndOfStream && !eofPacketSubmitted)
					{
						// Send EOF/zero packet to decoder
						avcodec_send_packet(pVideoDecoderContext, NULL);
						eofPacketSubmitted = true;
					}
					else
					{
						noInputData = 1;
					}
				}
				else
				{
					// Signal stream thread to continue filling our buffer
					SetEvent(streamThreadEventV);

					// If we got a seek packet dump everything and start again as soon as possible
					if (qp.seek)
					{
						// Remember this as we need to reset the sync PTS and serial to the first packet coming out of the decoder
						seekHappened = true;

						// Flush the decoder
						avcodec_flush_buffers(pVideoDecoderContext);
						// Flush the frame output
						pFrameOut->FlushBuffers();
						// Release the currently processing frame as well
						if (frameState)
						{
							frameState = 0;
							av_frame_unref(videoFrame);
						}
						// All frames coming out of the decoder from now on will have the new serial
						while (!loopDecoderSerial.empty())
							loopDecoderSerial.pop();
						while (!loopTimerSerial.empty())
							loopTimerSerial.pop();
						videoSerial = qp.serial;

						//MLog("V: Seek packet %d", qp.serial);
					}
					else if (qp.loop) // Loop packets are handled smoothly - don't drop any data!
					{
						// Send EOF/zero packet to decoder
						avcodec_send_packet(pVideoDecoderContext, NULL);

						// Add the serial to the queue (will get popped once decoder returns EOF)
						loopDecoderSerial.push(qp.serial);
						if (!pAudioStream)
							loopTimerSerial.push(qp.serial);

						//MLog("V: Loop packet %d", qp.serial);
					}
				}
			}

			// Submit a packet to the decoder (can fail if buffers are full)
			// Don't submit if we're waiting for a loop since the API spec says that once NULL is submitted via avcodec_send_packet:
			// "Before decoding can be resumed again, the codec has to be reset with avcodec_flush_buffers()."
			if (qp.pkt.size > 0)
			{
				double t0 = GetPreciseTime();
				int ret = avcodec_send_packet(pVideoDecoderContext, &qp.pkt);
				videoDecoderSubmitPacketTime += GetPreciseTime() - t0;
				switch (ret) {
				case 0:
					av_packet_unref(&qp.pkt);
					break;
				case AVERROR(EAGAIN): // Input buffer full, must consume frames with avcodec_receive_frame first
				case AVERROR_EOF: // Null packet has been submitted and decoder hasn't been flushed yet
					break;
				default:
					MError("Error in video avcodec_send_packet! Error: %d", ret);
					break;
				}
			}

			// Get a frame from the decoder
			if (frameState == 0)
			{
				// Decode the data chunk
				int isFrameAvailable = 0;
				double t0 = GetPreciseTime();
				int ret = avcodec_receive_frame(pVideoDecoderContext, videoFrame);
				videoDecoderReceiveFrameTime += GetPreciseTime() - t0;
				switch (ret) {
				case 0:
					// We got a frame! Post process if needed by HW decoder
					frameState = 1;
					break;
				case AVERROR(EAGAIN):
					// Frame not ready yet
					break;
				case AVERROR_EOF:
					// End of stream (coded consumed everything) - check if we're looping and advance the serial and flush the decoder to be able to resume decoding
					//MLog("V: AVCodec EOF");
					if (!loopDecoderSerial.empty())
					{
						videoSerial = loopDecoderSerial.front();
						loopDecoderSerial.pop();
						//MLog("V: AVCodec EOF loop %d", videoSerial);
						avcodec_flush_buffers(pVideoDecoderContext);
					}
					else
					{
						// No more data
						isEof = true;
					}
					break;
				default:
					// Something went wrong!!
					MError("Error in video avcodec_receive_frame! Error: %d", ret);
					break;
				}
			}

			// Get frame data from the hardware decoder if applicable (this will fall through when SW decoding)
			if (frameState == 1)
			{
				double t0 = GetPreciseTime();
				if (HWDecoderManager::RetrieveData(pVideoDecoderContext, videoFrame) == 0)
				{
					frameState = 2;
					waitForBufferStartTime = GetPreciseTime();
					videoHWRetrieveTime += waitForBufferStartTime - t0;
				}
			}

			// Output the decoded frame
			if (frameState == 2)
			{
				// Request frame output init
				pFrameOut->Initialize(videoFrame->width, videoFrame->height, decodeBufferVideoFrames, useHwAccel);

				// Get PTS (presentation time stamp) for this frame as a double value in seconds
				int64_t intpts = av_frame_get_best_effort_timestamp(videoFrame);
				AVRational tbq = { 1, AV_TIME_BASE };
				double pts = av_rescale_q(intpts, pVideoStream->time_base, tbq) / (double)AV_TIME_BASE;
				double duration = av_rescale_q(videoFrame->pkt_duration, pVideoStream->time_base, tbq) / (double)AV_TIME_BASE;
				pts += avSyncOffset;

				// Init video rescaler context when we get the first frame
				if (!useHwAccel && pVideoRescalerContext == 0)
				{
					AVPixelFormat outputFmt = AV_PIX_FMT_RGBA;
					switch (pFrameOut->videoOutputFormat) {
					case vofBGRA: outputFmt = AV_PIX_FMT_BGRA; break;
					case vofRGBA: outputFmt = AV_PIX_FMT_RGBA; break;
					};

					pVideoRescalerContext = sws_getCachedContext(pVideoRescalerContext, videoFrame->width, videoFrame->height, static_cast<AVPixelFormat>(videoFrame->format), videoFrame->width, videoFrame->height, outputFmt, SWS_POINT, nullptr, nullptr, nullptr);
					if (pVideoRescalerContext == nullptr)
					{
						MError("Error while calling sws_getContext");
						break;
					}
				}

				// Reset audio time after seek if no audio stream
				if (pAudioStream == 0 && seekHappened)
				{
					syncTime = pts;
					syncSerial = videoSerial;
					seekHappened = false;
					//MLog("V: Time reset after sync %d (pts %.2f)", syncSerial, syncTime);
				}

				// Try to get an output buffer
				void* outputBufferData = 0;
				int outputBufferPitch = 0;
				double t0 = GetPreciseTime();
				if (pFrameOut->BeginOutput(outputBufferData, outputBufferPitch, pts, duration, presentImmediately ? -1 : videoSerial))
				{
					noOutputBuffer = 1;
				}
				else
				{
					videoWaitForOutputBufferTime += GetPreciseTime() - waitForBufferStartTime;

					// Update last PTS when we actually get a frame
					lastPts = pts;

					if (useHwAccel)
					{
						// Pass through the entire AVFrame to the output system and let it handle the HW buffers
						*((AVFrame**)outputBufferData) = videoFrame;
						videoFrame = av_frame_alloc();
					}
					else
					{
						// Software YUV -> RGB conversion into the output texture
						AVFrame out = { 0 };
						out.data[0] = (uint8_t*)outputBufferData;
						out.linesize[0] = outputBufferPitch;
						t0 = GetPreciseTime();
						lastFrameOutputTime = t0;
						sws_scale(pVideoRescalerContext, videoFrame->data, videoFrame->linesize, 0, videoFrame->height, out.data, out.linesize);
						videoScalerTime += GetPreciseTime() - t0;

						// Unreference frame data
						av_frame_unref(videoFrame);
					}

					// Output the ready frame
					pFrameOut->EndOutput();

					// Process next frame
					frameState = 0;
				}
			}
		}

		// Put video decoder thread to sleep if it can't do any work
		int canGetNewData = qp.pkt.size == 0 && !noInputData;
		int canDecodePacket = qp.pkt.size > 0 && !frameState;
		int canOutputFrame = frameState == 2 && !noOutputBuffer;

		if (!canGetNewData && !canDecodePacket && !canOutputFrame)
		{
			double t0 = GetPreciseTime();
			WaitForSingleObject(videoThreadEvent, 10);
			videoThreadSleepTime += GetPreciseTime() - t0;
		}

		videoThreadTotalTime += GetPreciseTime() - loopStart;
	}

	av_frame_free(&videoFrame);
	av_packet_unref(&qp.pkt);
	pFrameOut->Shutdown();
	isVideoThreadDone = 1;
	return 0;
}

void av_log_callback(void* ptr, int level, const char* fmt, va_list vl)
{
	if (level >= 0 || ptr)
	{
		int ourLevel = 4;
		if (level <= AV_LOG_ERROR)
			ourLevel = 1;
		else if (level <= AV_LOG_INFO)
			ourLevel = 2;
		else if (level <= AV_LOG_VERBOSE)
			ourLevel = 3;
		LogHelperVA(ourLevel, fmt, vl);
	}
}

double streamThreadTotalTime = 0, streamThreadSleepTime = 0;

int TFFMPEGVideoObject_Int::StreamThread()
{
	// Uncomment to enable FFMPEG logging
	av_log_set_callback(av_log_callback);

	// Reset any variables left over from the previous run
	State = sUninitialized;
	syncTime = 0;
	isVideoThreadDone = false;
	isAudioThreadDone = false;
	isEndOfStream = false;

	videoThreadEvent = CreateEvent(0, FALSE, FALSE, NULL);
	audioThreadEvent = CreateEvent(0, FALSE, FALSE, NULL);
	streamThreadEventA = CreateEvent(0, FALSE, FALSE, NULL);
	streamThreadEventV = CreateEvent(0, FALSE, FALSE, NULL);
	streamThreadEventCommand = CreateEvent(0, FALSE, FALSE, NULL);

	// Parse options
	AVDictionary *opts = NULL;
	av_dict_parse_string(&opts, options, "=", ",", 0);

	////////////////////////////////////////
	// Open video
	////////////////////////////////////////
	int err = avformat_open_input(&pFormatContext, fileName, nullptr, &opts);

	// Release options
	if (opts)
		av_dict_free(&opts);

	if (err != 0)
	{
		MError("avformat_open_input failed. Probably invalid file name or format. Error: %d\n", err);
		InternalShutdown();
		return 1;
	}

	////////////////////////////////////////
	// Get stream info
	////////////////////////////////////////
	err = avformat_find_stream_info(pFormatContext, nullptr);
	if (err < 0)
	{
		MError("avformat_find_stream_info failed. Error: %d\n", err);
		InternalShutdown();
		return 2;
	}

	for (unsigned int i = 0; i < pFormatContext->nb_streams; ++i)
	{
		AVStream* stream = pFormatContext->streams[i];		// pointer to a structure describing the stream
		switch (stream->codecpar->codec_type)
		{
		case AVMEDIA_TYPE_VIDEO: pVideoStream = stream; break;
		case AVMEDIA_TYPE_AUDIO: pAudioStream = stream; break;
		};
	}

	frameRate = av_q2d(av_guess_frame_rate(pFormatContext, pVideoStream, NULL));

	////////////////////////////////////////
	// Init video codec
	////////////////////////////////////////
	pVideoCodec = avcodec_find_decoder(pVideoStream->codecpar->codec_id);
	if (pVideoCodec == nullptr)
	{
		MError("Video codec required by video file not available!");
		InternalShutdown();
		return 1;
	}

	pVideoDecoderContext = avcodec_alloc_context3(pVideoCodec);
	pVideoDecoderContext->thread_count = useHwAccel ? 1 : numDecodeThreads; // Force decoder to single thread if using HW decode (useless anyways)
	pVideoDecoderContext->thread_type = FF_THREAD_FRAME;
	pVideoDecoderContext->extradata = pVideoStream->codecpar->extradata;
	pVideoDecoderContext->extradata_size = pVideoStream->codecpar->extradata_size;
	pVideoDecoderContext->width = pVideoStream->codecpar->width;
	pVideoDecoderContext->height = pVideoStream->codecpar->height;
	pVideoDecoderContext->refcounted_frames = 1;

	// Get video length
	pFrameOut->videoWidth = pVideoDecoderContext->width;
	pFrameOut->videoHeight = pVideoDecoderContext->height;
	pFrameOut->videoLength = pFormatContext->duration == AV_NOPTS_VALUE ? 0.0f : (float)(pFormatContext->duration / (float)AV_TIME_BASE);

	// Setup HW decoder
	if (useHwAccel)
	{
		HWDecoderManager::Init(pVideoDecoderContext);
	}

	// initializing the structure by opening the codec
	err = avcodec_open2(pVideoDecoderContext, pVideoCodec, nullptr);
	if (err < 0)
	{
		MError("Could not open video codec! Error: %d", err);
		InternalShutdown();
		return 1;
	}

	////////////////////////////////////////
	// Init audio codec
	////////////////////////////////////////
	if (pAudioStream)
	{
		pAudioCodec = avcodec_find_decoder(pAudioStream->codecpar->codec_id);
		if (pAudioCodec == nullptr)
		{
			MError("Audio codec required by video file not available!");
			InternalShutdown();
			return 0;
		}

		pAudioDecoderContext = avcodec_alloc_context3(pAudioCodec);

		// we need to make a copy of audioStream->codec->extradata and give it to the context
		// make sure that this vector exists as long as the avVideoCodec exists
		pAudioDecoderContext->thread_count = 1;
		pAudioDecoderContext->thread_type = 0;
		pAudioDecoderContext->sample_rate = pAudioStream->codecpar->sample_rate;
		pAudioDecoderContext->channels = pAudioStream->codecpar->channels;
		pAudioDecoderContext->bit_rate = pAudioStream->codecpar->bit_rate;
		pAudioDecoderContext->bits_per_coded_sample = pAudioStream->codecpar->bits_per_coded_sample;
		pAudioDecoderContext->block_align = pAudioStream->codecpar->block_align;
		pAudioDecoderContext->err_recognition = 0;
		pAudioDecoderContext->refcounted_frames = 1;
		//pAudioDecoderContext->pkt_timebase.num      = 1;
		//pAudioDecoderContext->pkt_timebase.den      = 10000000;

		pAudioDecoderContext->extradata = pAudioStream->codecpar->extradata;
		pAudioDecoderContext->extradata_size = pAudioStream->codecpar->extradata_size;

		// initializing the structure by opening the codec
		err = avcodec_open2(pAudioDecoderContext, pAudioCodec, nullptr);
		if (err < 0)
		{
			MError("Could not open audio codec! Error: %d", err);
			InternalShutdown();
			return 0;
		}
	}

	State = TVideoObject::sPaused;

	////////////////////////////////////////
	// Start audio/video decode threads
	////////////////////////////////////////
	std::thread videoDecoderThread(&TFFMPEGVideoObject_Int::VideoDecoderThread, this);
	std::thread* audioDecoderThread = NULL;
	if (pAudioStream)
	{
		audioDecoderThread = new std::thread(&TFFMPEGVideoObject_Int::AudioDecoderThread, this);
	}

	////////////////////////////////////////
	// Start reading data packets
	////////////////////////////////////////
	QueuePacket qp = { { 0 }, 0, 0, 0, 0, 0 };

	for (;;)
	{
		double loopStart = GetPreciseTime();

		// Process a pending command
		TVideoCommand cmd;
		if (aCommand.try_pop(cmd))
		{
			switch (cmd.cmd)
			{
			case TVideoCommand::cPause:
			{
				if (State == sPlaying)
					State = sPaused;
				break;
			}
			case TVideoCommand::cPlay:
			{
				if (State == sPaused)
					State = sPlaying;
				break;
			}
			case TVideoCommand::cShutdown:
			{
				if (State < sShutdown)
				{
					State = sShutdown;
					isEndOfStream = true;
				}
				SetEvent(audioThreadEvent);
				SetEvent(videoThreadEvent);
				break;
			}
			case TVideoCommand::cSeek:
			{
				if (isEndOfStream)
					break;

				int64_t pts = (int64_t)(cmd.f * (float)AV_TIME_BASE);
				int err = avformat_seek_file(pFormatContext, -1, INT64_MIN, pts, INT64_MAX, 0);
				if (err < 0)
				{
					MLog("Seek Failed! Error: %d", err);
					return 0;
				}
				else
				{
					// Flush the packets queued up so far
					QueuePacket discardqp = { 0 };
					while (aVideoPacket.try_pop(discardqp))
					{
						av_packet_unref(&discardqp.pkt);
					}
					while (aAudioPacket.try_pop(discardqp))
					{
						av_packet_unref(&discardqp.pkt);
					}

					av_packet_unref(&qp.pkt);

					// Increment serial
					qp.serial++;

					// Insert flush packet
					QueuePacket fp = { { 0 }, qp.serial, 1, 0, 0, 0 };
					if (pAudioStream)
						aAudioPacket.push(fp);
					if (pVideoStream)
						aVideoPacket.push(fp);

					SetEvent(audioThreadEvent);
					SetEvent(videoThreadEvent);
				}
				break;
			}
			}
		}

		// If we're paused just loop
		if (State == sPaused)
		{
			SleepEx(1, FALSE);
			continue;
		}

		// Try to read more data from the stream
		if (!isEndOfStream)
		{
			if (qp.pkt.size == 0)
			{
				int err = av_read_frame(pFormatContext, &qp.pkt);
				if (err < 0)
				{
					//MDiagnostic("av_read_frame (%d)", err);
					if (err == AVERROR_EOF)
					{
						MDiagnostic("av_read_frame EOF");
						if (loopCount != 0)
						{
							if (loopCount > 0)
								loopCount -= 1;

							// Try to seek the stream to start
							int serr = avformat_seek_file(pFormatContext, -1, INT64_MIN, 0, INT64_MAX, 0);
							if (serr < 0)
							{
								isEndOfStream = true;
							}
							else
							{
								// Increment serial
								qp.serial++;

								// Insert loop notification packets after successful seek
								QueuePacket lp = { { 0 }, qp.serial, 0, 1, 0, 0 };
								if (pAudioStream)
									aAudioPacket.push(lp);
								if (pVideoStream)
									aVideoPacket.push(lp);

								SetEvent(audioThreadEvent);
								SetEvent(videoThreadEvent);
							}
						}
						else
						{
							// Signal A/V threads that we don't have any input data left
							isEndOfStream = true;
						}
					}
					else
					{
						// Unrecoverable error!
						MDiagnostic("av_read_frame unrecoverable error", err);
						isEndOfStream = true;
					}
				}
			}
			if (qp.pkt.size > 0)
			{
				if (pVideoStream && qp.pkt.stream_index == pVideoStream->index)
				{
					if (aVideoPacket.unsafe_size() < streamBufferVideoChunks)
					{
						//MDiagnostic("av_read_frame queue video packet %d", qp.pkt.pts);
						aVideoPacket.push(qp);
						SetEvent(videoThreadEvent);
						memset(&qp.pkt, 0, sizeof(qp.pkt));
					}
					else
					{
						//MDiagnostic("av_read_frame video queue full - sleep", qp.pkt.pts);
						double waitStart = GetPreciseTime();
						HANDLE aMultipleSignals[] = { streamThreadEventV, streamThreadEventCommand };
						WaitForMultipleObjects(sizeof(aMultipleSignals) / sizeof(aMultipleSignals[0]), aMultipleSignals, FALSE, 100);
						streamThreadSleepTime += GetPreciseTime() - waitStart;
					}
				}
				else if (pAudioStream && qp.pkt.stream_index == pAudioStream->index)
				{
					if (aAudioPacket.unsafe_size() < streamBufferAudioChunks)
					{
						//MDiagnostic("av_read_frame queue audio packet %d", qp.pkt.pts);
						aAudioPacket.push(qp);
						memset(&qp.pkt, 0, sizeof(qp.pkt));
					}
					else
					{
						//MDiagnostic("av_read_frame audio queue full - sleep", qp.pkt.pts);
						double waitStart = GetPreciseTime();
						HANDLE aMultipleSignals[] = { streamThreadEventA, streamThreadEventCommand };
						WaitForMultipleObjects(sizeof(aMultipleSignals) / sizeof(aMultipleSignals[0]), aMultipleSignals, FALSE, 100);
						streamThreadSleepTime += GetPreciseTime() - waitStart;
					}
				}
				else
				{
					av_packet_unref(&qp.pkt);
				}
			}
		}

		streamThreadTotalTime += GetPreciseTime() - loopStart;

		// Bail out of the loop when any of A/V threads finish (either done or error!)
		if (isVideoThreadDone || isAudioThreadDone)
			break;
	}

	av_packet_unref(&qp.pkt);

	////////////////////////////////////////
	// Wait for the threads to finish
	////////////////////////////////////////
	State = sShutdown;
	isEndOfStream = true;
	videoDecoderThread.join();
	if (audioDecoderThread)
	{
		audioDecoderThread->join();
		delete audioDecoderThread;
		audioDecoderThread = NULL;
	}
		

	////////////////////////////////////////
	// Cleanup
	////////////////////////////////////////
	InternalShutdown();
	return 0;
}

void TFFMPEGVideoObject_Int::Initialize()
{
	streamThread = new std::thread(&TFFMPEGVideoObject_Int::StreamThread, this);
}

void TFFMPEGVideoObject_Int::PutCommand(TVideoCommand vc)
{
	aCommand.push(vc);
	SetEvent(streamThreadEventCommand);
}

void TFFMPEGVideoObject_Int::Play()
{
	TVideoCommand vc(TVideoCommand::cPlay);
	PutCommand(vc);
}

void TFFMPEGVideoObject_Int::Pause()
{
	TVideoCommand vc(TVideoCommand::cPause);
	PutCommand(vc);
}

void TFFMPEGVideoObject_Int::Seek(const float &time)
{
	TVideoCommand vc(TVideoCommand::cSeek, 0, time);
	PutCommand(vc);
}

void TFFMPEGVideoObject_Int::RequestShutdown()
{
	TVideoCommand vc(TVideoCommand::cShutdown);
	PutCommand(vc);
}

int TFFMPEGVideoObject_Int::TryWaitShutdown()
{
	pFrameOut->TryWaitShutdown();
	pAudioOut->TryWaitShutdown();
	if (isStreamThreadDone == false)
		return 0;
	streamThread->join();
	delete streamThread;
	streamThread = 0;
	State = sUninitialized;
	return 1;
}

void TFFMPEGVideoObject_Int::SignalFrameConsumed()
{
	SetEvent(videoThreadEvent);
}
