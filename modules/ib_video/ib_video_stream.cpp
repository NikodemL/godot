#include "ib_video_stream.h"
#include <d3d9.h>
#include <d3dx9.h>
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
		//logBuffer.push(le);
		print_line(le.str);
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

//int nextVideoId = 1000;
//std::map<int, TVideoInstance> mVideoObject;
std::mutex videoMutex;
IDirect3DDevice9Ex* pDevice9 = 0;
ID3D11Device* pDevice11 = 0;

/*
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
}*/

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

	// Register the render
	VS::get_singleton()->request_frame_drawn_callback(this, "render", Variant(p_delta_time));
}

void MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	MLog("GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
}

// Test purpuse only
IDirect3DSurface9*  g_pSurfaceRenderTarget = NULL;
IDirect3DSurface9*  g_pSharedSurface = NULL;
HANDLE              g_hSharedSurface = NULL;

IDirect3DTexture9*  g_pSharedTexture = NULL;
HANDLE              g_hSharedTexture = NULL;
LPDIRECT3DVERTEXBUFFER9 g_pVB = NULL;

HANDLE              g_hDX9Device = NULL;
HANDLE              g_hGLSharedTexture = NULL;
GLuint              g_GLTexture = NULL;

#define SCREEN_WIDTH 300
#define SCREEN_HEIGHT 300

bool g_AnyRenders = false;
bool g_IsInit = false;

Ref<DirectXIBVideoTexture> h_SharedTextureWrapper = NULL;

struct CUSTOMVERTEX {
	float x, y, z;
	DWORD color;
	float u, v;
};
#define CUSTOMFVF (D3DFVF_XYZ | D3DFVF_DIFFUSE)

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_DESTROY)
	{
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

void VideoStreamIBManager::init() {
	std::lock_guard<std::mutex> scopeLock(videoMutex);

	VS::get_singleton()->request_frame_drawn_callback(this, "init_in_render", Variant(0.0f));
}

void VideoStreamIBManager::init_in_render(float p_unused) {
	if (g_IsInit)
		return;
	g_IsInit = true;

	glewInit();

	glDebugMessageCallback = (PFNGLDEBUGMESSAGECALLBACKPROC)wglGetProcAddress("glDebugMessageCallback");

	// During init, enable debug output
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback((GLDEBUGPROC)MessageCallback, 0);

	if (pDevice9) {
		print_error("Init already called, cannot create a new device");
		return;
	}

	/*
	// Init WGL
	wglDXOpenDeviceNV = (PFNWGLDXOPENDEVICENVPROC)wglGetProcAddress("wglDXOpenDeviceNV");
	wglDXCloseDeviceNV = (PFNWGLDXCLOSEDEVICENVPROC)wglGetProcAddress("wglDXCloseDeviceNV");

	wglDXRegisterObjectNV = (PFNWGLDXREGISTEROBJECTNVPROC)wglGetProcAddress("wglDXRegisterObjectNV");
	wglDXUnregisterObjectNV = (PFNWGLDXUNREGISTEROBJECTNVPROC)wglGetProcAddress("wglDXUnregisterObjectNV");

	wglDXLockObjectsNV = (PFNWGLDXLOCKOBJECTSNVPROC)wglGetProcAddress("wglDXLockObjectsNV");
	wglDXUnlockObjectsNV = (PFNWGLDXUNLOCKOBJECTSNVPROC)wglGetProcAddress("wglDXUnlockObjectsNV");

	wglDXSetResourceShareHandleNV = (PFNWGLDXSETRESOURCESHAREHANDLENVPROC)wglGetProcAddress("wglDXSetResourceShareHandleNV");
	
	InitTimer();
	*/

	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = "WindowClass";
	wc.lpfnWndProc = WindowProc;
	RegisterClassEx(&wc);


	// Create a dummy window for DX device
	hWndDX = CreateWindowEx(NULL, "WindowClass", "DX - Shared Resource",
		WS_OVERLAPPEDWINDOW, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
		NULL, NULL, GetModuleHandle(NULL), NULL);

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
		&pDevice9);

	hr = pDevice9->GetRenderTarget(0, &g_pSurfaceRenderTarget);


	D3DSURFACE_DESC rtDesc;
	g_pSurfaceRenderTarget->GetDesc(&rtDesc);

	// g_pSharedTexture should be able to be opened in OGL via the WGL_NV_DX_interop extension
	// Vendor support for various textures/surfaces may vary
	hr = pDevice9->CreateTexture(rtDesc.Width,
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

	hr = pDevice9->SetRenderState(D3DRS_LIGHTING, FALSE);
	hr = pDevice9->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);


	CUSTOMVERTEX vertices[] =
	{
		{ 2.0f, -2.0f, 0.0f, D3DCOLOR_XRGB(0, 0, 255) },
		{ 0.0f,  2.0f, 0.0f, D3DCOLOR_XRGB(0, 255, 0) },
		{ -2.0f, -2.0f, 0.0f, D3DCOLOR_XRGB(255, 0, 0) },
	};

	hr = pDevice9->CreateVertexBuffer(9 * sizeof(CUSTOMVERTEX),
		0,
		CUSTOMFVF,
		D3DPOOL_DEFAULT,
		&g_pVB,
		NULL);

	VOID* pVoid;
	hr = g_pVB->Lock(0, 0, (void**)&pVoid, 0);
	memcpy(pVoid, vertices, sizeof(vertices));
	hr = g_pVB->Unlock();


	// We now need to initialize OpenGL extensions
	g_hDX9Device = wglDXOpenDeviceNV(pDevice9);

	if (!g_hDX9Device) {
		print_error("Could not open DX device");
		return;
	}

	// Create a video texture that is linked
	Ref<DirectXIBVideoTexture> ibVideoTexture = memnew(DirectXIBVideoTexture);
	ibVideoTexture->register_shared_texture_DX(1);
	h_SharedTextureWrapper = ibVideoTexture;
	g_GLTexture = VS::get_singleton()->texture_get_texid(ibVideoTexture->get_rid());


	// This registers a resource that was created as shared in DX with its shared handle
	bool success = wglDXSetResourceShareHandleNV(g_pSharedTexture, g_hSharedTexture);

	// g_hGLSharedTexture is the shared texture data, now identified by the g_GLTexture name
	g_hGLSharedTexture = wglDXRegisterObjectNV(g_hDX9Device,
		g_pSharedTexture,
		g_GLTexture,
		GL_TEXTURE_2D,
		WGL_ACCESS_READ_ONLY_NV);

	// Lock the shared surface
	wglDXLockObjectsNV(g_hDX9Device, 1, &g_hGLSharedTexture);
}

VideoStreamIBManager* VideoStreamIBManager::singleton = NULL;

void VideoStreamIBManager::release() {

	if (pDevice9) {
		pDevice9->Release();
		pDevice9 = NULL;
	}

	if (d3d) {
		d3d->Release();
		d3d = NULL;
	}
}

int VideoStreamIBManager::create_video()
{
	return 0;
	/*
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

	inst.pVideoObject->useHwAccel = true;
	return inst.id;*/
}

int VideoStreamIBManager::configure_video(int id, unsigned int streamBufferVideoChunks, unsigned int streamBufferAudioChunks,
	unsigned int decodeBufferVideoFrames, unsigned int decodeBufferAudioMs, bool presentImmediately, float avSyncOffset, int decodeThreads, bool useHwAccel) {
	return 1;
	/*
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
	return (int)inst.pVideoObject->State;*/
}

int VideoStreamIBManager::open_video(int id, String fileName, String options)
{
	return 1;
	/*
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
	return (int)inst.pVideoObject->State;*/
}

int VideoStreamIBManager::get_video_state(int id)
{
	return 1;
	/*
	//MDiagnostic("GetVideoState(id=%d)", id);
	std::lock_guard<std::mutex> scopeLock(videoMutex);
	auto it = mVideoObject.find(id);
	if (it == mVideoObject.end())
		return -1;
	TVideoInstance &inst = it->second;

	// If video is shutting down, report as if already destroyed
	if ((int)inst.pVideoObject->State == TVideoObject::sShutdown)
		return -1;

	return (int)inst.pVideoObject->State;*/
}

int VideoStreamIBManager::play_video(int id, int loopCount)
{
	return 1;
	/*
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
	return inst.pVideoObject->State;*/
}

int VideoStreamIBManager::pause_video(int id)
{
	return 1;/*
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
	return inst.pVideoObject->State;*/
}

int VideoStreamIBManager::seek_video(int id, float time)
{
	return 1;
	/*
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
	return inst.pVideoObject->State;*/
}

int VideoStreamIBManager::close_video(int id)
{
	return 1;
	/*
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
	return inst.pVideoObject->State;*/
}

Size2 VideoStreamIBManager::get_video_info_size(int id)
{
	return Size2(1, 1);
	/*
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
	return s;*/
}

float VideoStreamIBManager::get_video_duration(int id)
{
	return 1;
	/*
	MDiagnostic("GetVideoInfo(id=%d)", id);
	std::lock_guard<std::mutex> scopeLock(videoMutex);
	auto it = mVideoObject.find(id);
	if (it == mVideoObject.end())
	{
		MError("Video not found! GetVideoInfo(id=%d)", id);
		return -1;
	}

	TVideoInstance &inst = it->second;
	return inst.pVideoObject->pFrameOut->videoLength;*/
}

Ref<DirectXIBVideoTexture> VideoStreamIBManager::get_video_texture(int id)
{
	if (!g_AnyRenders)
		return NULL;
	return h_SharedTextureWrapper;
	/*
	//MDiagnostic("GetVideoTexture(id=%d,texturePtr=%x)", id, texturePtr);
	std::lock_guard<std::mutex> scopeLock(videoMutex);
	auto it = mVideoObject.find(id);
	if (it == mVideoObject.end())
	{
		MError("Video not found! GetVideoTexture(id=%d)", id);
		return NULL;
	}
	TVideoInstance &inst = it->second;

	if (inst.pDXGLSharedHandle == NULL)
		return NULL;

	return inst.pGLVideoTexture;*/
}

// Called from render thread
void VideoStreamIBManager::render(float p_delta_tme)
{
	if (!g_IsInit)
		return;

	// Lock the shared surface
	wglDXUnlockObjectsNV(g_hDX9Device, 1, &g_hGLSharedTexture);

	// Set up transformations
	D3DXMATRIX matView;
	D3DXMatrixLookAtLH(&matView,
		&D3DXVECTOR3(0.0f, 0.0f, -10.0f),
		&D3DXVECTOR3(0.0f, 0.0f, 0.0f),
		&D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	pDevice9->SetTransform(D3DTS_VIEW, &matView);

	D3DXMATRIX matProjection;
	D3DXMatrixPerspectiveFovLH(&matProjection,
		D3DXToRadian(45),
		(FLOAT)SCREEN_WIDTH / (FLOAT)SCREEN_HEIGHT,
		1.0f,
		25.0f);
	pDevice9->SetTransform(D3DTS_PROJECTION, &matProjection);

	D3DXMATRIX matTranslate;
	D3DXMatrixTranslation(&matTranslate, 0.0f, 0.0f, 0.0f);

	D3DXMATRIX matRotate;
	static float rot = 0;
	rot += 0.01;
	D3DXMatrixRotationZ(&matRotate, rot);

	D3DXMATRIX matTransform = matRotate * matTranslate;

	HRESULT hr = S_OK;
	hr = pDevice9->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(40, 40, 60), 1.0f, 0);

	// Draw a spinning triangle
	hr = pDevice9->BeginScene();

	hr = pDevice9->SetStreamSource(0, g_pVB, 0, sizeof(CUSTOMVERTEX));
	hr = pDevice9->SetFVF(CUSTOMFVF);
	hr = pDevice9->SetTransform(D3DTS_WORLD, &matTransform);
	hr = pDevice9->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);


	// StretchRect between two D3DPOOL_DEFAULT surfaces will be a GPU Blt.
	// Note that GetRenderTargetData() cannot be used because it is intended to copy from GPU to CPU.
	hr = pDevice9->StretchRect(g_pSurfaceRenderTarget, NULL, g_pSharedSurface, NULL, D3DTEXF_NONE);

	hr = pDevice9->EndScene();

	// Lock the shared surface
	wglDXLockObjectsNV(g_hDX9Device, 1, &g_hGLSharedTexture);
	g_AnyRenders = true;

	/*
	std::lock_guard<std::mutex> scopeLock(videoMutex);

	
	// Unlock all videos so they can be used ny DX
	auto it2 = mVideoObject.begin();
	while (it2 != mVideoObject.end())
	{
		TVideoInstance &inst = it2->second;
		if (inst.pDXGLSharedHandle != NULL)
		{
			wglDXUnlockObjectsNV(glDXHandle, 1, &inst.pDXGLSharedHandle);
		}

		++it2;
	}

	// Update the render targets
	HWDecoderManager::RenderThreadUpdate();

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
				if (inst.pVideoTexture9 != NULL && inst.pRenderTexHandle == NULL) {
					inst.pRenderTexHandle = (HANDLE)((TDXVideoFrameOutput*)inst.pVideoObject->pFrameOut)->GetRenderTexHandle();

					bool success = wglDXSetResourceShareHandleNV(inst.pVideoTexture9, inst.pRenderTexHandle);

					// gl_texture_handle is the shared texture data, now identified by the g_GLTexture name
					uint32_t gl_id = inst.pGLVideoTexture->get_rid().get_id();
					inst.pDXGLSharedHandle = wglDXRegisterObjectNV(glDXHandle,
						inst.pVideoTexture9,
						gl_id,
						GL_TEXTURE_2D,
						WGL_ACCESS_READ_ONLY_NV);

					if (inst.pDXGLSharedHandle == NULL) {
						print_error("Failed to create DX GL handle");
					}
				}

			}
			else {
				MDiagnostic("Unsupported device");
				//inst.pVideoTexture11 = (ID3D11ShaderResourceView*)((TDXVideoFrameOutput*)inst.pVideoObject->pFrameOut)->GetFrame();
			}
			inst.currentTime = inst.pVideoObject->syncTime;
			break;
		case TVideoObject::sShutdown:
			MDiagnostic("UnityRenderEvent - Shutdown %d", it->first);
			if (inst.pVideoObject->TryWaitShutdown())
			{
				if (inst.pDXGLSharedHandle != NULL)
				{
					wglDXUnregisterObjectNV(glDXHandle, inst.pDXGLSharedHandle);
					inst.pDXGLSharedHandle = NULL;
				}

				DestroyVideoInstance(inst);
				it = mVideoObject.erase(it);
				continue;
			}
			break;
		};
		++it;
	}

	// Lock targets
	auto it3 = mVideoObject.begin();
	while (it3 != mVideoObject.end())
	{
		TVideoInstance &inst = it3->second;
		if (inst.pDXGLSharedHandle != NULL)
		{
			wglDXLockObjectsNV(glDXHandle, 1, &inst.pDXGLSharedHandle);
		}

		++it3;
	}*/

}

void DirectXIBVideoTexture::_bind_methods() {

}

void VideoStreamIBManager::_bind_methods() {

	ClassDB::bind_method(D_METHOD("init"), &VideoStreamIBManager::init);
	ClassDB::bind_method(D_METHOD("init_in_render", "unused"), &VideoStreamIBManager::init_in_render);
	ClassDB::bind_method(D_METHOD("set_log_level", "level"), &VideoStreamIBManager::set_log_level);
	ClassDB::bind_method(D_METHOD("release"), &VideoStreamIBManager::release);

	ClassDB::bind_method(D_METHOD("update", "delta_time"), &VideoStreamIBManager::update);
	ClassDB::bind_method(D_METHOD("render", "delta_time"), &VideoStreamIBManager::render);
	ClassDB::bind_method(D_METHOD("create_video"), &VideoStreamIBManager::create_video);
	ClassDB::bind_method(D_METHOD("open_video", "id", "filename", "options"), &VideoStreamIBManager::open_video);

	// We do not bind as such, too many parameters
	//ClassDB::bind_method(D_METHOD("configure_video", "id", "filename", "options"), &VideoStreamIBManager::configure_video);
	ClassDB::bind_method(D_METHOD("get_video_state", "id"), &VideoStreamIBManager::get_video_state);
	ClassDB::bind_method(D_METHOD("play_video", "id", "loop"), &VideoStreamIBManager::play_video);
	ClassDB::bind_method(D_METHOD("pause_video", "id"), &VideoStreamIBManager::pause_video);
	ClassDB::bind_method(D_METHOD("seek_video", "id"), &VideoStreamIBManager::seek_video);
	ClassDB::bind_method(D_METHOD("close_video", "id"), &VideoStreamIBManager::close_video);
	ClassDB::bind_method(D_METHOD("get_video_info_size", "id"), &VideoStreamIBManager::get_video_info_size);
	ClassDB::bind_method(D_METHOD("get_video_duration", "id"), &VideoStreamIBManager::get_video_duration);
	ClassDB::bind_method(D_METHOD("get_video_texture", "id"), &VideoStreamIBManager::get_video_texture);
	//ClassDB::bind_method(D_METHOD("get_video_current_time", "id"), &VideoStreamIBManager::get_video_current_time);
}
