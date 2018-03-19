#include <d3d9.h>
#include <d3d11.h>
#include <map>
#include <mutex>
#include <thread>
#include <string>
#include <list>
#include <concurrent_queue.h>

#include "ib_video_stream.h"

#include "video.h"
#include "video_ffmpeg.h"
#include "video_dxframe.h"
#include "video_wasapiaudio.h"
#include "dxva2decoder.h"



int DirectXIBVideoTexture::get_width() const {
	return w;
}

int DirectXIBVideoTexture::get_height() const {
	return h;
}

RID DirectXIBVideoTexture::get_rid() const {
	return texture;
}

bool DirectXIBVideoTexture::has_alpha() const {
	return false;
}

void DirectXIBVideoTexture::set_flags(uint32_t p_flags) {
	ERR_FAIL(false && "Cannot set flags for DX texture");
}
uint32_t DirectXIBVideoTexture::get_flags() const {
	return FLAG_VIDEO_SURFACE;
}

DirectXIBVideoTexture::DirectXIBVideoTexture() {
	format = Image::TEXTURE_MAX;
	flags = 0;
	w = 0;
	h = 0;

	texture = VS::get_singleton()->texture_create();
}

DirectXIBVideoTexture::~DirectXIBVideoTexture() {
	VS::get_singleton()->texture_free();
}


DirectXIBVideoTexture::register_shared_texture_DX(LPDIRECT3DDEVICE9EX p_dx_device, HANDLE p_shared_dx_texture) {

	ERR_FAIL(p_shared_dx_texture != NULL);
	ERR_FAIL(dx_handle == NULL);

	dx_texture_handle = p_shared_dx_texture;

	bool success = wglDXSetResourceShareHandleNV(dx_texture_handle, dx_texture_handle);

	// gl_texture_handle is the shared texture data, now identified by the g_GLTexture name
	gl_texture_handle = wglDXRegisterObjectNV(p_dx_device,
		dx_texture_handle,
		texture->get_id(),
		GL_TEXTURE_2D,
		WGL_ACCESS_READ_ONLY_NV);
}


void VideoStreamPlaybackIB::clear() {

}

void VideoStreamPlaybackIB::play() {

}

bool VideoStreamPlaybackIB::is_playing() {

}

void VideoStreamPlaybackIB::set_paused(bool p_paused) {

}

bool VideoStreamPlaybackIB::is_paused() {

}

void VideoStreamPlaybackIB::set_loop(int p_enable) {

}

bool VideoStreamPlaybackIB::get_loop() {

}

float VideoStreamPlaybackIB::get_length() {

}

String VideoStreamPlaybackIB::get_stream_name() const {

}

int VideoStreamPlaybackIB::get_loop_count() const {

}

float VideoStreamPlaybackIB::get_playback_position() const {

}

void VideoStreamPlaybackIB::seek(float p_time) {

}

Ref<Texture> VideoStreamPlaybackIB::get_texture() {

}

void VideoStreamPlaybackIB::update(float p_delta) {

}

void VideoStreamPlaybackIB::set_mix_callback(AudioMixCallback p_callback, void *p_userdata) {

}

int VideoStreamPlaybackIB::get_channels() const {

}

int VideoStreamPlaybackIB::get_mix_rate() const {

}

void VideoStreamPlaybackIB::set_audio_track(int p_idx) {

}

VideoStreamPlaybackIB::VideoStreamPlaybackIB() {

}

VideoStreamPlaybackIB::~VideoStreamPlaybackIB() {

}



/////////////////////////////////////////////////////////////////
// Loggging
/////////////////////////////////////////////////////////////////
int logLevel = 1; // Log errors only by default

				  // We cache all log entries and dump them only when UpdateLog is called
				  // This is becuase we can't trigger callbacks into Unity outside of the main thread (it causes Mono cleanup to fail, at least in Editor)
struct LogEntry {
	int level;
	char str[1024];
};
concurrency::concurrent_queue<LogEntry> logBuffer;

// Timer funcs from video_ffmpeg.cpp
void InitTimer();
double GetPreciseTime();

// Unity callback
typedef void(*LogFuncPtr)(int, const char *);
LogFuncPtr LogFunc = 0;

// All log calls are rerouted through here
void LogHelperVA(int level, const char* fmt, va_list args)
{
	if (logLevel >= level)
	{
		// Fix fmt string if using %td that visual studio doesn't support (asserts printf in CRT)
		char localFmt[2048];
		sprintf_s(localFmt, "%5.03f | %s", GetPreciseTime(), fmt);
		//strcpy_s(localFmt, fmt);
		char* fmtFix = 0;
		if (fmtFix = strstr(localFmt, "%td"))
		{
			fmtFix[1] = 'l';
		}

		// Add new log entry
		LogEntry le;
		le.level = level;
		vsprintf_s(le.str, localFmt, args);
		logBuffer.push(le);
	}
}

void LogHelper(int level, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	LogHelperVA(level, fmt, args);
}

/////////////////////////////////////////////////////////////////
// Video globals
/////////////////////////////////////////////////////////////////
struct TVideoInstance {
	int id;;
	TVideoObject* pVideoObject;
	IDirect3DTexture9* pVideoTexture9;
	ID3D11ShaderResourceView* pVideoTexture11;
	double currentTime;
};

int nextVideoId = 1000;
std::map<int, TVideoInstance> mVideoObject;
std::mutex videoMutex;
IDirect3DDevice9* pDevice9 = 0;
ID3D11Device* pDevice11 = 0;

void InitVideoInstance(TVideoInstance &inst)
{
	inst.pVideoObject = TFFMPEGVideoObject::Create();
	inst.pVideoObject->pAudioOut = TWASAPIVideoAudioOutput::Create();
	inst.pVideoObject->pFrameOut = TDXVideoFrameOutput::Create(inst.pVideoObject, pDevice9, pDevice11);
}

void DestroyVideoInstance(TVideoInstance& inst)
{
	MLog("Destroying video %d (%s) at time %f", inst.id, inst.pVideoObject->fileName, inst.currentTime)
		TWASAPIVideoAudioOutput::Destroy(inst.pVideoObject->pAudioOut);
	TDXVideoFrameOutput::Destroy(inst.pVideoObject->pFrameOut);
	TFFMPEGVideoObject::Destroy(inst.pVideoObject);
}

void ForceShutdownVideos(int saveState = 0)
{
	auto it = mVideoObject.begin();
	while (it != mVideoObject.end())
	{
		TVideoInstance &inst = it->second;
		inst.pVideoObject->RequestShutdown();
		while (inst.pVideoObject->TryWaitShutdown() == 0)
			SleepEx(1, TRUE);
		DestroyVideoInstance(inst);
		it = mVideoObject.erase(it);
	}
}

/////////////////////////////////////////////////////////////////
// DLL interface
/////////////////////////////////////////////////////////////////
extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API SetLogCallback(LogFuncPtr ptr, int level)
{
	LogFunc = ptr;
	logLevel = (level < 0) ? (0) : ((level > 3) ? (3) : (level));
}

void VideoStreamIBManager::update() {
	std::lock_guard<std::mutex> scopeLock(videoMutex);

	// Dump log entries
	LogEntry le;
	while (logBuffer.try_pop(le))
	{
		LogFunc(le.level, le.str);
	}
}

void VideoStreamIBManager::init() {
	std::lock_guard<std::mutex> scopeLock(videoMutex);

	InitTimer();

	// Create a dummy window for DX device
	hWndDX = CreateWindowA("STATIC", "dummy", NULL, 0, 0, 100, 100, NULL, NULL, NULL, NULL);

	// Create D3D device
	D3DPRESENT_PARAMETERS d3dpp;

	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hWndDX;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;

	HRESULT hr = S_OK;

	// A D3D9EX device is required to create the g_hSharedSurface 
	Direct3DCreate9Ex(D3D_SDK_VERSION, &d3d);

	// The interop definition states D3DCREATE_MULTITHREADED is required, but it may vary by vendor
	hr = d3d->CreateDeviceEx(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWndDX,
		D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
		&d3dpp,
		NULL,
		&d9device);

	hr = d9device->GetRenderTarget(0, &g_pSurfaceRenderTarget);
	D3DSURFACE_DESC rtDesc;
	g_pSurfaceRenderTarget->GetDesc(&rtDesc);

	// g_pSharedTexture should be able to be opened in OGL via the WGL_NV_DX_interop extension
	// Vendor support for various textures/surfaces may vary
	hr = g_pDevice->CreateTexture(rtDesc.Width,
		rtDesc.Height,
		1,
		0,
		rtDesc.Format,
		D3DPOOL_DEFAULT,
		&g_pSharedTexture,
		&g_hSharedTexture);

	// We want access to the underlying surface of this texture
	if (g_pSharedTexture)
	{
		hr = g_pSharedTexture->GetSurfaceLevel(0, &g_pSharedSurface);
	}

	hr = g_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	hr = g_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

}

void VideoStreamIBManager::release() {

	if (d9device) {
		d9device->Release();
		d9device = NULL;
	}

	if(d3d) {
		d3d->Release();
		d3d = NULL;
	}
}


extern "C" int UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API CreateVideo()
{
	MDiagnostic("CreateVideo(), nextVideoId = %d", nextVideoId);
	std::lock_guard<std::mutex> scopeLock(videoMutex);
	// Fail if no valid GFX device is set
	if (!pDevice9 && !pDevice11)
		return 0;
	TVideoInstance inst = { 0 };
	InitVideoInstance(inst);
	inst.id = nextVideoId;
	nextVideoId += 1;
	mVideoObject.insert(std::pair<int, TVideoInstance>(inst.id, inst));
	return inst.id;
}

extern "C" int UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API  ConfigureVideo(int id, unsigned int streamBufferVideoChunks, unsigned int streamBufferAudioChunks, unsigned int decodeBufferVideoFrames, unsigned int decodeBufferAudioMs, bool presentImmediately, float avSyncOffset, int decodeThreads, bool useHwAccel) {
	MDiagnostic("ConfigureVideo(id=%d,streamBufferVideoChunks=%d,streamBufferAudioChunks=%d,decodeBufferVideoFrames=%d,decodeBufferAudioMs=%d, presentImmediately=%d, avSyncOffset=%f, decodeThreads=%d, useHwAccell=%d)", id, streamBufferVideoChunks, streamBufferAudioChunks, decodeBufferVideoFrames, decodeBufferAudioMs, presentImmediately, avSyncOffset, decodeThreads, useHwAccel);
	std::lock_guard<std::mutex> scopeLock(videoMutex);
	auto it = mVideoObject.find(id);
	if (it == mVideoObject.end())
	{
		MError("Video not found! ConfigureVideo(id=%d,streamBufferVideoChunks=%d,streamBufferAudioChunks=%d,decodeBufferVideoFrames=%d,decodeBufferAudioMs=%d)", id, streamBufferVideoChunks, streamBufferAudioChunks, decodeBufferVideoFrames, decodeBufferAudioMs);
		return -1;
	}
	TVideoInstance &inst = it->second;
	inst.pVideoObject->streamBufferVideoChunks = streamBufferVideoChunks;
	inst.pVideoObject->streamBufferAudioChunks = streamBufferAudioChunks;
	inst.pVideoObject->decodeBufferVideoFrames = decodeBufferVideoFrames;
	inst.pVideoObject->decodeBufferAudioMs = decodeBufferAudioMs;
	inst.pVideoObject->presentImmediately = presentImmediately;
	inst.pVideoObject->avSyncOffset = avSyncOffset;
	inst.pVideoObject->numDecodeThreads = decodeThreads;
	inst.pVideoObject->useHwAccel = useHwAccel;
	return (int)inst.pVideoObject->State;
}

extern "C" int UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API OpenVideo(int id, const char* fileName, const char* options)
{
	MDiagnostic("OpenVideo(id=%d,fileName=%s)", id, fileName);
	std::lock_guard<std::mutex> scopeLock(videoMutex);
	auto it = mVideoObject.find(id);
	if (it == mVideoObject.end())
	{
		MError("Video not found! OpenVideo(id=%d,fileName=%s,options=%s)", id, fileName, options);
		return -1;
	}
	TVideoInstance &inst = it->second;
	strcpy_s(inst.pVideoObject->fileName, fileName);
	strcpy_s(inst.pVideoObject->options, options);
	inst.pVideoObject->Initialize();
	return (int)inst.pVideoObject->State;
}

extern "C" int UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetVideoState(int id)
{
	//MDiagnostic("GetVideoState(id=%d)", id);
	std::lock_guard<std::mutex> scopeLock(videoMutex);
	auto it = mVideoObject.find(id);
	if (it == mVideoObject.end())
		return -1;
	TVideoInstance &inst = it->second;

	// If video is shutting down, report as if already destroyed
	if ((int)inst.pVideoObject->State == TVideoObject::sShutdown)
		return -1;

	return (int)inst.pVideoObject->State;
}

extern "C" int UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API PlayVideo(int id, int loopCount)
{
	MDiagnostic("PlayVideo(id=%d)", id);
	std::lock_guard<std::mutex> scopeLock(videoMutex);
	auto it = mVideoObject.find(id);
	if (it == mVideoObject.end())
	{
		MError("Video not found! PlayVideo(id=%d)", id);
		return -1;
	}
	TVideoInstance &inst = it->second;
	inst.pVideoObject->loopCount = loopCount;
	inst.pVideoObject->Play();
	return inst.pVideoObject->State;
}

extern "C" int UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API PauseVideo(int id)
{
	MDiagnostic("PauseVideo(id=%d)", id);
	std::lock_guard<std::mutex> scopeLock(videoMutex);
	auto it = mVideoObject.find(id);
	if (it == mVideoObject.end())
	{
		MError("Video not found! PauseVideo(id=%d)", id);
		return -1;
	}
	TVideoInstance &inst = it->second;
	inst.pVideoObject->Play();
	return inst.pVideoObject->State;
}

extern "C" int UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API SeekVideo(int id, float time)
{
	MDiagnostic("SeekVideo(id=%d,time=%f)", id, time);
	std::lock_guard<std::mutex> scopeLock(videoMutex);
	auto it = mVideoObject.find(id);
	if (it == mVideoObject.end())
	{
		MError("Video not found! SeekVideo(id=%d,time=%f)", id, time);
		return -1;
	}
	TVideoInstance &inst = it->second;
	inst.pVideoObject->Seek(time);
	return inst.pVideoObject->State;
}

extern "C" int UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API CloseVideo(int id)
{
	MDiagnostic("CloseVideo(id=%d)", id);
	std::lock_guard<std::mutex> scopeLock(videoMutex);
	auto it = mVideoObject.find(id);
	if (it == mVideoObject.end())
	{
		MError("Video not found! CloseVideo(id=%d)", id);
		return -1;
	}
	TVideoInstance &inst = it->second;
	inst.pVideoObject->RequestShutdown();
	return inst.pVideoObject->State;
}

extern "C" int UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetVideoInfo(int id, int* width, int* height, float* duration)
{
	MDiagnostic("GetVideoInfo(id=%d,widthPtr=%x,heightPtr=%x,durationPtr=%x)", id, width, height, duration);
	std::lock_guard<std::mutex> scopeLock(videoMutex);
	auto it = mVideoObject.find(id);
	if (it == mVideoObject.end())
	{
		MError("Video not found! GetVideoInfo(id=%d,widthPtr=%x,heightPtr=%x,durationPtr=%x)", id, width, height, duration);
		return -1;
	}
	TVideoInstance &inst = it->second;
	if (width) *width = inst.pVideoObject->pFrameOut->videoWidth;
	if (height) *height = inst.pVideoObject->pFrameOut->videoHeight;
	if (duration) *duration = inst.pVideoObject->pFrameOut->videoLength;
	return inst.pVideoObject->State;
}

extern "C" int UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetVideoTexture(int id, void** texturePtr, float* currentTime)
{
	//MDiagnostic("GetVideoTexture(id=%d,texturePtr=%x)", id, texturePtr);
	std::lock_guard<std::mutex> scopeLock(videoMutex);
	auto it = mVideoObject.find(id);
	if (it == mVideoObject.end())
	{
		MError("Video not found! GetVideoTexture(id=%d,texturePtr=%x)", id, texturePtr);
		return -1;
	}
	TVideoInstance &inst = it->second;
	if (texturePtr) *texturePtr = pDevice9 ? (void*)inst.pVideoTexture9 : (void*)inst.pVideoTexture11;
	if (currentTime) *currentTime = (float)inst.currentTime;
	return inst.pVideoObject->State;
}

// Called from render thread
extern "C" void UNITY_INTERFACE_API UnityRenderEvent(int eventID)
{
	//MDiagnostic("UnityRenderEvent");
	if (eventID = 1337515)
	{
		HWDecoderManager::RenderThreadUpdate();

		std::lock_guard<std::mutex> scopeLock(videoMutex);

		auto it = mVideoObject.begin();
		while (it != mVideoObject.end())
		{
			TVideoInstance &inst = it->second;
			switch (inst.pVideoObject->State) {
			case TVideoObject::sPlaying:
			case TVideoObject::sPaused:
				if (pDevice9)
					inst.pVideoTexture9 = (IDirect3DTexture9*)((TDXVideoFrameOutput*)inst.pVideoObject->pFrameOut)->GetFrame();
				else
					inst.pVideoTexture11 = (ID3D11ShaderResourceView*)((TDXVideoFrameOutput*)inst.pVideoObject->pFrameOut)->GetFrame();
				inst.currentTime = inst.pVideoObject->syncTime;
				break;
			case TVideoObject::sShutdown:
				//MDiagnostic("UnityRenderEvent - Shutdown %d", it->first);
				if (inst.pVideoObject->TryWaitShutdown())
				{
					DestroyVideoInstance(inst);
					it = mVideoObject.erase(it);
					continue;
				}
				break;
			};
			++it;
		}
	}
}

extern "C" UnityRenderingEvent UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetRenderEventFunc()
{
	return UnityRenderEvent;
}
