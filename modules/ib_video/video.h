/////////////////////////////////////////////////////////////////
//
// Video
// (c) ZootFly
//
/////////////////////////////////////////////////////////////////
#ifndef __VIDEO__
#define __VIDEO__

//////////////////////////////////////////////////////////////////////////////////
// Video audio output
//////////////////////////////////////////////////////////////////////////////////
enum EAudioSampleFormat { // Copied from FFMPEG to remove dependency
	asfNONE = -1,
	asfU8,          ///< unsigned 8 bits
	asfS16,         ///< signed 16 bits
	asfS32,         ///< signed 32 bits
	asfFLT,         ///< float
	asfDBL,         ///< double

	asfU8P,         ///< unsigned 8 bits, planar
	asfS16P,        ///< signed 16 bits, planar
	asfS32P,        ///< signed 32 bits, planar
	asfFLTP,        ///< float, planar
	asfDBLP,        ///< double, planar

	asfNB           ///< Number of sample formats. DO NOT USE if linking dynamically
};

enum EVideoOutputFormat {
	vofRGBA = 1,
	vofBGRA
};

class TVideoAudioOutput {
public:
	virtual ~TVideoAudioOutput() {}
	virtual int Initialize(int bufferLengthMs) = 0;
	virtual void Shutdown() = 0;
	virtual void Start() = 0;
	virtual void Stop() = 0;
	virtual bool GetTime(double &time, int &serial) = 0;
	virtual void SetVolume(float vol)=0;
	virtual void* BeginOutput(int numSamplesRequested, double pts, int serial) = 0;
	virtual void EndOutput(int numSamplesWritten) = 0;
	virtual EAudioSampleFormat GetOutputFormat() = 0;
	virtual int GetOutputChannelLayout() = 0;
	virtual int GetOutputSampleRate() = 0;
	virtual int TryWaitShutdown() = 0;
	virtual void FlushBuffers() = 0;
};

//////////////////////////////////////////////////////////////////////////////////
// TVideoFrameOutput
//////////////////////////////////////////////////////////////////////////////////
class TVideoFrameOutput {
public:
	int videoWidth;
	int videoHeight;
	int decodeBufferVideoFrames;
	float videoLength;
	EVideoOutputFormat videoOutputFormat;
public:
	TVideoFrameOutput() : videoWidth(0), videoHeight(0), videoLength(0), videoOutputFormat(vofRGBA), decodeBufferVideoFrames(1) {}
	virtual ~TVideoFrameOutput() {}
public:
	virtual int Initialize(int width, int height, int decodeBufferFrames, bool passthrough) = 0;
	virtual void Shutdown() = 0;
	virtual int BeginOutput(void* &data, int &pitch, double pts, double duration, int serial) = 0;
	virtual void EndOutput() = 0;
	virtual int TryWaitShutdown() = 0;
	virtual void FlushBuffers() = 0;
	virtual double GetTime() = 0;
};

//////////////////////////////////////////////////////////////////////////////////
// Video command
//////////////////////////////////////////////////////////////////////////////////
class TVideoCommand {
public:
	enum ECommand {
		cPause = 0,
		cPlay = 1,
		cShutdown = 2,
		cSeek = 3,
	};
public:
	ECommand cmd;
	int i;
	float f;
public:
	TVideoCommand(): cmd(cPause), i(0), f(0.0f) {}
	TVideoCommand(ECommand _cmd, int _i = 0, float _f = 0.0f): cmd(_cmd), i(_i), f(_f) {}
};

//////////////////////////////////////////////////////////////////////////////////
// Main video object
//////////////////////////////////////////////////////////////////////////////////
class TVideoObject {
public:
	enum EState {
		sUninitialized = 0,
		sPaused,
		sPlaying,
		sShutdown,
		sSeek,
	};
public:
	char fileName[512];
	char options[512];
	TVideoAudioOutput* pAudioOut;
	TVideoFrameOutput* pFrameOut;
	double syncTime;
	int syncSerial;
	EState State;

	unsigned int streamBufferVideoChunks;
	unsigned int streamBufferAudioChunks;
	unsigned int decodeBufferVideoFrames;
	unsigned int decodeBufferAudioMs;
	bool presentImmediately;
	float avSyncOffset;
	int numDecodeThreads;
	bool useHwAccel;
	int loopCount;
public:
	TVideoObject() : pAudioOut(0), pFrameOut(0), syncTime(0), State(sUninitialized),
		streamBufferVideoChunks(100),
		streamBufferAudioChunks(100),
		decodeBufferVideoFrames(3),
		decodeBufferAudioMs(200),
		presentImmediately(false),
		avSyncOffset(0.0f),
		loopCount(0),
		syncSerial(-1),
		useHwAccel(true)
	{
		strcpy_s(fileName, "");
	}
	virtual ~TVideoObject() {}
public:
	virtual void Initialize() = 0;
	virtual void Play() = 0;
	virtual void Pause() = 0;
	virtual void Seek(const float &time) = 0;
	virtual void RequestShutdown() = 0;
	virtual int TryWaitShutdown() = 0;
	virtual void SignalFrameConsumed() = 0;
};

void LogHelper(int level, const char* fmt, ...);
void LogHelperVA(int level, const char* fmt, va_list args);

#define MError(fmt,...) { LogHelper(1,fmt,__VA_ARGS__); }
#define MLog(fmt,...) { LogHelper(2,fmt,__VA_ARGS__); }
#define MDiagnostic(fmt,...) { LogHelper(3,fmt,__VA_ARGS__); }

#endif
/////////////////////////////////////////////////////////////////
//
//
//
/////////////////////////////////////////////////////////////////
