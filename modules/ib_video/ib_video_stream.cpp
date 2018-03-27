#include "ib_video_stream.h"
#include <d3d9.h>
#include <d3d11.h>
#include <map>
#include <mutex>
#include <thread>
#include <string>
#include <list>
#include <concurrent_queue.h>

#include "image.h"
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
	if (is_locked == false)
	{
		print_error("You must explicitly lock texture before using it");
	}
	return texture;
}

bool DirectXIBVideoTexture::has_alpha() const {
	return false;
}

void DirectXIBVideoTexture::set_flags(uint32_t p_flags) {
	print_error("Cannot set flags for DX texture");
}
uint32_t DirectXIBVideoTexture::get_flags() const {
	return FLAG_VIDEO_SURFACE;
}

DirectXIBVideoTexture::DirectXIBVideoTexture() {
	format = Image::FORMAT_MAX;
	w = 0;
	h = 0;
	id = 0;
	is_locked = false;

	texture = VS::get_singleton()->texture_create();
}

DirectXIBVideoTexture::~DirectXIBVideoTexture() {
	VS::get_singleton()->free(texture);
}


void DirectXIBVideoTexture::register_shared_texture_DX(int p_id) {
	id = p_id;
}


// This is a port of Unity binding to Godot system

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
	int id;
	TVideoObject* pVideoObject;
	IDirect3DTexture9* pVideoTexture9;
	//ID3D11ShaderResourceView* pVideoTexture11;

	double currentTime;
	Ref<DirectXIBVideoTexture> pGLVideoTexture;

	HANDLE pRenderTexHandle;
	HANDLE pDXGLSharedHandle;


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

	if (inst.pDXGLSharedHandle) {
		wglDXUnregisterObjectNV(pDevice9, inst.pDXGLSharedHandle);
		inst.pDXGLSharedHandle = NULL;
	}
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

void VideoStreamIBManager::set_log_level(int level) {
	logLevel = (level < 0) ? (0) : ((level > 3) ? (3) : (level));
}

void VideoStreamIBManager::update(float p_delta_time) {
	std::lock_guard<std::mutex> scopeLock(videoMutex);

	// Dump log entries
	LogEntry le;
	while (logBuffer.try_pop(le))
	{
		print_line(String(le.str));
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

	// A D3D9EX device is required to create the 
	Direct3DCreate9Ex(D3D_SDK_VERSION, &d3d);

	// The interop definition states D3DCREATE_MULTITHREADED is required, but it may vary by vendor
	hr = d3d->CreateDeviceEx(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWndDX,
		D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
		&d3dpp,
		NULL,
		&d9device);

	// TODO: Do we need to acquire render target like this ...
	hr = d9device->GetRenderTarget(0, &d3dSurface);


	// We now need to initialize OpenGL extensions
	glDXHandle = wglDXOpenDeviceNV(d9device);

	if (!glDXHandle) {
		print_error("Could not open DX device");
		return;
	}
}

VideoStreamIBManager* VideoStreamIBManager::singleton = NULL;

void VideoStreamIBManager::release() {

	if (d9device) {
		d9device->Release();
		d9device = NULL;
	}

	if (d3d) {
		d3d->Release();
		d3d = NULL;
	}
}

int VideoStreamIBManager::create_video()
{
	MDiagnostic("CreateVideo(), nextVideoId = %d", nextVideoId);
	std::lock_guard<std::mutex> scopeLock(videoMutex);
	// Fail if no valid GFX device is set
	if (!pDevice9 && !pDevice11)
		return 0;
	TVideoInstance inst = { 0 };
	InitVideoInstance(inst);
	inst.id = nextVideoId;

	// We have video instance, we need to link it to OpenGL texture (share mode)
	Ref<DirectXIBVideoTexture> ibVideoTexture = memnew(DirectXIBVideoTexture);
	inst.pGLVideoTexture = ibVideoTexture;
	ibVideoTexture->register_shared_texture_DX(inst.id);

	nextVideoId += 1;
	mVideoObject.insert(std::pair<int, TVideoInstance>(inst.id, inst));
	return inst.id;
}

int VideoStreamIBManager::configure_video(int id, unsigned int streamBufferVideoChunks, unsigned int streamBufferAudioChunks,
	unsigned int decodeBufferVideoFrames, unsigned int decodeBufferAudioMs, bool presentImmediately, float avSyncOffset, int decodeThreads, bool useHwAccel) {
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

int VideoStreamIBManager::open_video(int id, String fileName, String options)
{
	MDiagnostic("OpenVideo(id=%d,fileName=%s)", id, fileName.ascii());
	std::lock_guard<std::mutex> scopeLock(videoMutex);
	auto it = mVideoObject.find(id);
	if (it == mVideoObject.end())
	{
		MError("Video not found! OpenVideo(id=%d,fileName=%s,options=%s)", id, fileName, options);
		return -1;
	}
	TVideoInstance &inst = it->second;
	strcpy_s(inst.pVideoObject->fileName, fileName.ascii());
	strcpy_s(inst.pVideoObject->options, options.ascii());
	inst.pVideoObject->Initialize();
	return (int)inst.pVideoObject->State;
}

int VideoStreamIBManager::get_video_state(int id)
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

int VideoStreamIBManager::play_video(int id, int loopCount)
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

int VideoStreamIBManager::pause_video(int id)
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

int VideoStreamIBManager::seek_video(int id, float time)
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

int VideoStreamIBManager::close_video(int id)
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

Size2 VideoStreamIBManager::get_video_info_size(int id)
{
	MDiagnostic("GetVideoInfo(id=%d)", id);
	std::lock_guard<std::mutex> scopeLock(videoMutex);
	auto it = mVideoObject.find(id);
	if (it == mVideoObject.end())
	{
		MError("Video not found! GetVideoInfo(id=%d)", id);
		return Size2(-1, -1);
	}
	Size2 s;

	TVideoInstance &inst = it->second;
	s.x = inst.pVideoObject->pFrameOut->videoWidth;
	s.y = inst.pVideoObject->pFrameOut->videoHeight;
	return s;
}

float VideoStreamIBManager::get_video_duration(int id)
{
	MDiagnostic("GetVideoInfo(id=%d)", id);
	std::lock_guard<std::mutex> scopeLock(videoMutex);
	auto it = mVideoObject.find(id);
	if (it == mVideoObject.end())
	{
		MError("Video not found! GetVideoInfo(id=%d)", id);
		return -1;
	}

	TVideoInstance &inst = it->second;
	return inst.pVideoObject->pFrameOut->videoLength;
}

bool VideoStreamIBManager::lock_video(int id) {
	std::lock_guard<std::mutex> scopeLock(videoMutex);
	auto it = mVideoObject.find(id);
	if (it == mVideoObject.end())
	{
		MError("Video not found! GetVideoTexture(id=%d)", id);
		return NULL;
	}
	TVideoInstance &inst = it->second;

	if (inst.pGLVideoTexture->is_locked == false) {
		MError("Video not locked before accessed");
		return false;
	}

	if (FAILED(wglDXLockObjectsNV(glDXHandle, 1, &inst.pDXGLSharedHandle))) {
		MError("Failed to lock video");
		return false;
	}
	inst.pGLVideoTexture->is_locked = true;
	return true;
}

void VideoStreamIBManager::unlock_video(int id) {
	std::lock_guard<std::mutex> scopeLock(videoMutex);
	auto it = mVideoObject.find(id);
	if (it == mVideoObject.end())
	{
		MError("Video not found! GetVideoTexture(id=%d)", id);
		return;
	}
	TVideoInstance &inst = it->second;

	if (inst.pGLVideoTexture->is_locked == false) {
		MError("Video not locked before unlocked");
		return;
	}

	if (FAILED(wglDXUnlockObjectsNV(glDXHandle, 1, &inst.pDXGLSharedHandle))) {
		MError("Failed to lock video");
		return;
	}

	inst.pGLVideoTexture->is_locked = false;
}

Ref<DirectXIBVideoTexture> VideoStreamIBManager::get_video_texture(int id)
{
	//MDiagnostic("GetVideoTexture(id=%d,texturePtr=%x)", id, texturePtr);
	std::lock_guard<std::mutex> scopeLock(videoMutex);
	auto it = mVideoObject.find(id);
	if (it == mVideoObject.end())
	{
		MError("Video not found! GetVideoTexture(id=%d)", id);
		return NULL;
	}
	TVideoInstance &inst = it->second;

	if (inst.pGLVideoTexture->is_locked == false) {
		MError("Video not locked before accessed");
		return NULL;
	}
	return inst.pGLVideoTexture;
}

// Called from render thread
void VideoStreamIBManager::render(float p_delta_tme)
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
			if (pDevice9) {
				inst.pVideoTexture9 = (IDirect3DTexture9*)((TDXVideoFrameOutput*)inst.pVideoObject->pFrameOut)->GetFrame();

				// Make it shared now if not already
				if (inst.pRenderTexHandle == NULL) {
					inst.pRenderTexHandle = (HANDLE)((TDXVideoFrameOutput*)inst.pVideoObject->pFrameOut)->GetRenderTexHandle();

					bool success = wglDXSetResourceShareHandleNV(inst.pVideoTexture9, inst.pRenderTexHandle);

					// gl_texture_handle is the shared texture data, now identified by the g_GLTexture name
					inst.pDXGLSharedHandle = wglDXRegisterObjectNV(pDevice9,
						inst.pRenderTexHandle,
						inst.pGLVideoTexture->get_rid().get_id(),
						GL_TEXTURE_2D,
						WGL_ACCESS_READ_ONLY_NV);
				}

			}
			else {
				//inst.pVideoTexture11 = (ID3D11ShaderResourceView*)((TDXVideoFrameOutput*)inst.pVideoObject->pFrameOut)->GetFrame();
			}
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

void DirectXIBVideoTexture::_bind_methods() {

}
