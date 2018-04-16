#include <concurrent_queue.h>
#include <atomic>
#include <d3d9.h>
#include <d3d11.h>
#include <vector>
#include <map>

extern "C" {
#include <libavformat\avformat.h>
}

#include "tgaimage.h"

#include "video.h"
#include "video_dxframe.h"
#include "dxva2decoder.h"

#define SAFE_RELEASE(punk) if((punk) != NULL) { (punk)->Release(); (punk) = NULL; }

//////////////////////////////////////////////////////////////////////////////////
// DX video frame output (windows)
//////////////////////////////////////////////////////////////////////////////////
class TDXVideoFrameOutput_Int : public TDXVideoFrameOutput {
public:
	static const int CMaxVideoTextureCount = 8;
public:
	class TVideoTexture {
	public:
		AVFrame* frame;
		IDirect3DSurface9* tex9;
		ID3D11Texture2D* tex11;
		void* dataptr;
		uint32_t datapitch;
		double pts;
		double duration;
		int serial;
	};
public:
	enum EInitState {
		isNone = 0,
		isWorkerRequestsInit,
		isRenderThreadInitComplete,
		isWorkerRequestsShutdown,
		isRenderThreadShutdownComplete
	};
public:
	bool isPassthrough;
	IDirect3DTexture9* renderTex9;
	HANDLE renderTexHandle;
	IDirect3DSurface9* renderSurf9;
	ID3D11Texture2D* renderTex11;
	ID3D11ShaderResourceView* renderRV11;
	TVideoTexture aTexture[CMaxVideoTextureCount];
	concurrency::concurrent_queue<TVideoTexture*> aTextureFree;
	concurrency::concurrent_queue<TVideoTexture*> aTextureReady;
	std::vector<TVideoTexture*> aTextureMapQueue;
	EInitState initState;
	TVideoTexture* pFillTexture;
	std::map<HANDLE, ID3D11Texture2D*> sharedTextures;
public:
	TVideoObject* pVideo;
	IDirect3DDevice9* pDevice9;
	IDirect3DQuery9* pQuery9;
	ID3D11Device* pDevice11;
	ID3D11DeviceContext* pDeviceCtx11;
	ID3D11Query* pQuery11;
	TVideoTexture* pCurrentFrame;
	TVideoTexture* pNextFrame;
	bool hasValidFrame;
	int textureReleaseDelay;
public:
	TDXVideoFrameOutput_Int();
	~TDXVideoFrameOutput_Int();
public:
	int Initialize(int width, int height, int decodeBuferFrames, bool passthrough);
	void Shutdown();
	int BeginOutput(void* &data, int &pitch, double pts, double duration, int serial);
	void EndOutput();
	int TryWaitShutdown();
	void FlushBuffers();
	double GetTime();
public:
	int RenderThread_InitTextures();
	void RenderThread_ReleaseTextures();
	void* GetFrame();
	void* GetRenderTexHandle() { return renderTexHandle; }
protected:
	void UpdateTextureClear();
	int GetTextureFromDecoder(TVideoTexture* vt);
	int ReturnTextureToDecoder(TVideoTexture* vt);
};

TVideoFrameOutput* TDXVideoFrameOutput::Create(TVideoObject* vo, IDirect3DDevice9* dev9, ID3D11Device* dev11)
{
	TDXVideoFrameOutput_Int* vfo = new TDXVideoFrameOutput_Int;
	vfo->pVideo = vo;
	vfo->pDevice9 = dev9;
	vfo->pDevice11 = dev11;
	if (dev11)
		dev11->GetImmediateContext(&vfo->pDeviceCtx11);
	return vfo;
}

void TDXVideoFrameOutput::Destroy(TVideoFrameOutput* vfo)
{
	TDXVideoFrameOutput_Int* dx11vfo = (TDXVideoFrameOutput_Int*)vfo;
	delete dx11vfo;
}

TDXVideoFrameOutput_Int::TDXVideoFrameOutput_Int() : isPassthrough(false), renderTex9(0), renderTexHandle(0), renderSurf9(0), renderTex11(0), renderRV11(0), initState(isNone), pFillTexture(0), pVideo(0), pDevice9(0), pDevice11(0), pDeviceCtx11(0), pCurrentFrame(0), pNextFrame(0), hasValidFrame(false), textureReleaseDelay(0)
{
	memset(aTexture, 0, sizeof(aTexture));
}

TDXVideoFrameOutput_Int::~TDXVideoFrameOutput_Int()
{
}

int TDXVideoFrameOutput_Int::Initialize(int width, int height, int decodeBufferFrames, bool passthrough)
{
	isPassthrough = passthrough;
	videoOutputFormat = vofRGBA;
	decodeBufferVideoFrames = decodeBufferFrames > CMaxVideoTextureCount ? CMaxVideoTextureCount : decodeBufferFrames;

	// Request texture init (from render thread) and wait for it to finish
	if(initState == isNone)
	{
		_InterlockedExchange((LONG volatile *)&initState, isWorkerRequestsInit);
		while (_InterlockedCompareExchange((LONG volatile *)&initState, isRenderThreadInitComplete, isRenderThreadInitComplete) == isWorkerRequestsInit)
			SleepEx(1, FALSE);
	}
	return 0;
}

void TDXVideoFrameOutput_Int::Shutdown()
{
	if( initState != isRenderThreadInitComplete )
		return;

	// Request texture shutdown from render thread
	_InterlockedExchange((LONG volatile *)&initState, isWorkerRequestsShutdown);
	while (_InterlockedCompareExchange((LONG volatile *)&initState, isRenderThreadShutdownComplete, isRenderThreadShutdownComplete) == isWorkerRequestsShutdown)
		SleepEx(1, FALSE);
}

int TDXVideoFrameOutput_Int::BeginOutput(void* &data, int &pitch, double pts, double duration, int serial)
{
	// Try to pop a free texture
	if(aTextureFree.try_pop(pFillTexture) == false)
		return 1;

	// Store PTS
	pFillTexture->pts = pts;
	pFillTexture->duration = duration;
	pFillTexture->serial = serial;

	// Set pointers
	if (isPassthrough)
	{
		data = &pFillTexture->frame;
		pitch = 0;
	}
	else
	{
		data = pFillTexture->dataptr;
		pitch = pFillTexture->datapitch;
	}
	
	return 0;
}

void TDXVideoFrameOutput_Int::EndOutput()
{
	aTextureReady.push(pFillTexture);
	pFillTexture = 0;
}

void TDXVideoFrameOutput_Int::FlushBuffers()
{
}

int TDXVideoFrameOutput_Int::RenderThread_InitTextures()
{
	HRESULT hr;
	if (pDevice9)
	{
		// Create primary output surface - this one gets passed to the renderer
		hr = pDevice9->CreateTexture(videoWidth, videoHeight, 1, isPassthrough ? D3DUSAGE_RENDERTARGET : NULL,
			D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &renderTex9, &renderTexHandle);
		if (FAILED(hr))
			return 1;

		hr = renderTex9->GetSurfaceLevel(0, &renderSurf9);
		if (FAILED(hr))
			return 1;

		hr = pDevice9->CreateQuery(D3DQUERYTYPE_EVENT, &pQuery9);
		if (FAILED(hr))
				return 1;

		// Create N frames for the decoder to fill (only when using software decode)
		for (int a = 0; a < decodeBufferVideoFrames; a++)
		{
			if (!isPassthrough)
			{
				hr = pDevice9->CreateOffscreenPlainSurface(videoWidth, videoHeight, D3DFMT_X8R8G8B8, D3DPOOL_SYSTEMMEM, &aTexture[a].tex9, NULL);
				if (FAILED(hr))
					return 1;
			}

			aTexture[a].pts = 0;
			aTextureReady.push(&aTexture[a]);
		}
	}
	else
	{
		D3D11_TEXTURE2D_DESC d;
		d.ArraySize = 1;
		d.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		d.CPUAccessFlags = 0;
		d.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		d.Height = videoHeight;
		d.Width = videoWidth;
		d.MipLevels = 1;
		d.MiscFlags = 0;
		d.SampleDesc.Count = 1;
		d.SampleDesc.Quality = 0;
		d.Usage = D3D11_USAGE_DEFAULT;

		hr = pDevice11->CreateTexture2D(&d, 0, &renderTex11);
		if (FAILED(hr))
			return 1;

		D3D11_SHADER_RESOURCE_VIEW_DESC rvd;
		rvd.Texture2D.MipLevels = 1;
		rvd.Texture2D.MostDetailedMip = 0;
		rvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		rvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		hr = pDevice11->CreateShaderResourceView(renderTex11, &rvd, &renderRV11);
		if (FAILED(hr))
			return 1;

		D3D11_QUERY_DESC qd;
		qd.Query = D3D11_QUERY_EVENT;
		qd.MiscFlags = 0;
		pDevice11->CreateQuery(&qd, &pQuery11);

		for (int a = 0; a < decodeBufferVideoFrames; a++)
		{
			if (!isPassthrough)
			{
				D3D11_TEXTURE2D_DESC d;
				d.ArraySize = 1;
				d.BindFlags = 0;
				d.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
				d.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				d.Height = videoHeight;
				d.Width = videoWidth;
				d.MipLevels = 1;
				d.MiscFlags = 0;
				d.SampleDesc.Count = 1;
				d.SampleDesc.Quality = 0;
				d.Usage = D3D11_USAGE_STAGING;

				hr = pDevice11->CreateTexture2D(&d, 0, &aTexture[a].tex11);
				if (FAILED(hr))
					return 1;
			}

			aTexture[a].pts = 0;
			aTextureReady.push(&aTexture[a]);
		}
	}

	return 0;
}

void TDXVideoFrameOutput_Int::RenderThread_ReleaseTextures()
{
	if (pCurrentFrame)
	{
		while (!ReturnTextureToDecoder(pCurrentFrame));
		pCurrentFrame = 0;
	}

	if (pNextFrame)
	{
		GetTextureFromDecoder(pNextFrame);
		while (!ReturnTextureToDecoder(pNextFrame));
		pNextFrame = 0;
	}

	TVideoTexture* vt = 0;
	while (aTextureReady.try_pop(vt))
	{
		GetTextureFromDecoder(vt);
		while (!ReturnTextureToDecoder(vt));
	}

	aTextureFree.clear();
	aTextureReady.clear();

	for (auto const &entry : sharedTextures) 
	{
		entry.second->Release();
	}
	sharedTextures.clear();

	SAFE_RELEASE(pQuery9);
	SAFE_RELEASE(pQuery11);

	SAFE_RELEASE(renderSurf9);
	SAFE_RELEASE(renderTex9);

	SAFE_RELEASE(renderTex11);
	SAFE_RELEASE(renderRV11);

	for (int a = 0; a < CMaxVideoTextureCount; a++)
	{
		TVideoTexture& vt = aTexture[a];

		if(vt.dataptr)
		{
			if (pDevice9)
				vt.tex9->UnlockRect();
			else
				pDeviceCtx11->Unmap(vt.tex11, 0);
			vt.dataptr = 0;
		}

		SAFE_RELEASE(vt.tex9);
		SAFE_RELEASE(vt.tex11);
	}
}

void TDXVideoFrameOutput_Int::UpdateTextureClear()
{
	if(_InterlockedCompareExchange((LONG volatile *)&initState, isWorkerRequestsInit, isWorkerRequestsInit) == isWorkerRequestsInit)
	{
		//MDiagnostic("RenderThread_InitTextures");
		RenderThread_InitTextures();
		_InterlockedExchange((LONG volatile *)&initState, isRenderThreadInitComplete);
	}

	if (_InterlockedCompareExchange((LONG volatile *)&initState, isWorkerRequestsShutdown, isWorkerRequestsShutdown) == isWorkerRequestsShutdown)
	{
		//MDiagnostic("RenderThread_ReleaseTextures");
		BOOL queryResult = FALSE;

		// Once we get here, issue an event query (=fence) so that we can check when the Unity render event that tirggered us is actually compelted on the GPU side
		// After the first entry, wait for the query to return TRUE and only then release the resources
		if (pDevice11)
		{
			if (textureReleaseDelay)
				pDeviceCtx11->GetData(pQuery11, &queryResult, sizeof(queryResult), NULL);
			else
				pDeviceCtx11->End(pQuery11);
		}
		else
		{
			if (textureReleaseDelay)
				pQuery9->GetData(&queryResult, sizeof(queryResult), D3DGETDATA_FLUSH);
			else
				pQuery9->Issue(D3DISSUE_END);
		}

		textureReleaseDelay += 1;
		
		if (queryResult)
		{
			RenderThread_ReleaseTextures();
			_InterlockedExchange((LONG volatile *)&initState, isRenderThreadShutdownComplete);
		}
	}
}

int TDXVideoFrameOutput_Int::TryWaitShutdown()
{
	 UpdateTextureClear();
	 return 0;
}

/*
uint8_t clamp(int v, int min, int max)
{
	if (v < min) return min;
	if (v > max) return max;
	return v;
}

void yuv2rgb(uint8_t yValue, uint8_t uValue, uint8_t vValue, uint8_t *r, uint8_t *g, uint8_t *b) {
	float Y = 1.164f*yValue - 16.0f;
	float U = uValue - 128.0f;
	float V = vValue - 128.0f;

	// Do the YUV -> RGB conversion
	int R = (int)(Y + 1.596f*V);
	int G = (int)(Y - 0.813f*V - 0.391f*U);
	int B = (int)(Y + 2.018f*U);

	// Clip rgb values to 0-255
	*r = R < 0 ? 0 : R > 255 ? 255 : R;
	*g = G < 0 ? 0 : G > 255 ? 255 : G;
	*b = B < 0 ? 0 : B > 255 ? 255 : B;
}

void SaveImageYUV(uint8_t* data, uint32_t pitch, int w, int h, char* fn)
{
	uint8_t* yData = data;
	uint8_t* uvData = data + pitch * h;

	static TGAImage *img = new TGAImage(w, h);
	TGAImage::Color c;

	static int frameCount = 0;

	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++)
		{
			uint8_t Y = yData[y*pitch + x];
			uint8_t U = uvData[((y >> 1)*pitch) + (x & ~1) + 0];
			uint8_t V = uvData[((y >> 1)*pitch) + (x & ~1) + 1];

			yuv2rgb(Y, U, V, &c.r, &c.g, &c.b);
			c.a = 255;
			img->SetPixel(c, x, y);
		}
	}

	char buf[128];
	sprintf_s(buf, fn, frameCount++);
	img->WriteImage(buf);
}

void SaveImageRGB(uint8_t* data, uint32_t pitch, int w, int h, char* fn)
{
	static TGAImage *img = new TGAImage(w, h);
	TGAImage::Color c;

	static int frameCount = 0;

	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++)
		{
			c.r = data[y*pitch + x*4 + 0];
			c.g = data[y*pitch + x*4 + 1];
			c.b = data[y*pitch + x*4 + 2];
			c.a = 255;
			img->SetPixel(c, x, y);
		}
	}

	char buf[128];
	sprintf_s(buf, fn, frameCount++);
	img->WriteImage(buf);
}

void Dump(ID3D11Device* pDevice11, ID3D11DeviceContext* pDeviceCtx11, ID3D11Texture2D* texture)
{
	HRESULT hr = 0;

	D3D11_TEXTURE2D_DESC td = { 0 };
	texture->GetDesc(&td);

	static ID3D11Texture2D* staging = 0;
	if (!staging)
	{
		D3D11_TEXTURE2D_DESC d;
		d.ArraySize = 1;
		d.BindFlags = 0;
		d.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		d.Format = td.Format;
		d.Width = td.Width;
		d.Height = td.Height;
		d.MipLevels = 1;
		d.MiscFlags = 0;
		d.SampleDesc.Count = 1;
		d.SampleDesc.Quality = 0;
		d.Usage = D3D11_USAGE_STAGING;

		hr = pDevice11->CreateTexture2D(&d, 0, &staging);
	}

	D3D11_BOX box = { 0, 0, 0, td.Width, td.Height, 1 };
	pDeviceCtx11->CopySubresourceRegion(staging, 0, 0, 0, 0, texture, 0, &box);

	D3D11_MAPPED_SUBRESOURCE lock = { 0 };
	hr = pDeviceCtx11->Map(staging, 0, D3D11_MAP_READ, 0, &lock);

	SaveImageRGB((uint8_t*)lock.pData, lock.RowPitch, td.Width, td.Height, "dx11_out_%03d.tga");

	pDeviceCtx11->Unmap(staging, 0);
}*/

int TDXVideoFrameOutput_Int::GetTextureFromDecoder(TVideoTexture* vt)
{
	// Unmap it and increment numFramesProcessed so that it gets copied to the target surface below
	if (vt->dataptr)
	{
		if (pDevice9)
			vt->tex9->UnlockRect();
		else
			pDeviceCtx11->Unmap(vt->tex11, 0);
		vt->dataptr = 0;
		
		return 1;
	}

	if (isPassthrough && vt->frame)
	{
		if (pDevice11)
		{
			// D3D11 only - Open the shared texture from the decoder device if not opened yet and acquire the mutex
			HANDLE h = (HANDLE)vt->frame->data[3];

			HRESULT hr = S_OK;
			ID3D11Texture2D* &texture = sharedTextures[h];
			if (texture == NULL)
			{
				hr = pDevice11->OpenSharedResource(h, __uuidof(ID3D11Texture2D), (void**)&texture);
			}

			IDXGIKeyedMutex* mutex = 0;
			hr = texture->QueryInterface(&mutex);
			while ((hr = mutex->AcquireSync(1, INFINITE)) != S_OK);
			mutex->Release();

			MLog("GetTextureFromDecoder %llx", h);
		}

		return 1;
	}

	return 0;
}

int TDXVideoFrameOutput_Int::ReturnTextureToDecoder(TVideoTexture* vt)
{
	bool isFree = false;
	if (isPassthrough)
	{
		if (vt->frame)
		{
			if (pDevice11)
			{		
				// On D3D11 we have to release the texture mutex so that the decoder device can use it
				HANDLE* h = (HANDLE*)vt->frame->data[3];

				ID3D11Texture2D* &texture = sharedTextures[h];

				IDXGIKeyedMutex* mutex = 0;
				HRESULT hr = texture->QueryInterface(&mutex);
				while ((hr = mutex->ReleaseSync(0)) != S_OK);
				mutex->Release();

				MLog("ReturnTextureToDecoder %llx", h);
			}

			av_frame_free(&vt->frame);
		}

		isFree = true;
	}
	else
	{
		if (pDevice9)
		{
			HRESULT hr;
			D3DLOCKED_RECT lr = { 0 };
			hr = vt->tex9->LockRect(&lr, NULL, D3DLOCK_DISCARD | D3DLOCK_DONOTWAIT);
			if (hr == S_OK)
			{
				isFree = true;
				vt->dataptr = lr.pBits;
				vt->datapitch = lr.Pitch;
			}
			else if (hr == D3DERR_WASSTILLDRAWING)
			{
				return 0;
			}
			else if (FAILED(hr))
			{
				MError("MAP FAILED!");
				return 0;
			}
		}
		else
		{
			D3D11_MAPPED_SUBRESOURCE mr = { 0 };
			HRESULT hr = pDeviceCtx11->Map(vt->tex11, 0, D3D11_MAP_WRITE, D3D11_MAP_FLAG_DO_NOT_WAIT, &mr);
			if (hr == S_OK)
			{
				isFree = true;
				vt->dataptr = mr.pData;
				vt->datapitch = mr.RowPitch;
			}
		}
	}

	if (!isFree)
		return 0;

	aTextureFree.push(vt);
	return 1;
}

void* TDXVideoFrameOutput_Int::GetFrame()
{
	UpdateTextureClear();

	int numFramesProcessed = 0;
	if(pVideo->State != TVideoObject::sShutdown)
	{
		for(;;)
		{
			bool shouldGoToNextFrame = false;

			// Try to get the next frame if don't have one queued already. Bail out if we don't get one since we can't do anything else.
			if (pNextFrame == 0)
			{
				if (aTextureReady.try_pop(pNextFrame) == false)
					break;
			}

			// If we have the next frame available see if we should use it
			if (pNextFrame)
			{
				// Always go to latest frame immediately if serial is -1
				if (pNextFrame->serial == -1)
				{
					shouldGoToNextFrame = true;
				}
				// If it's within the same serial check if it's time to show the frame
				else if (pNextFrame->serial == pVideo->syncSerial)
				{
					double timeDelta = pVideo->syncTime - pNextFrame->pts;
					if (timeDelta >= 0)
						shouldGoToNextFrame = true; // It's time to show this frame
				}
				// If it's behind the sync serial then it's definitely old - process it to get to newer frames
				else if (pNextFrame->serial < pVideo->syncSerial)
				{
					shouldGoToNextFrame = true;
				}

				// Go to next frame if requested
				if (shouldGoToNextFrame)
				{
					//MLog("V: DXFrame consume %d %.2f @ time %d %.2f", pNextFrame->serial, pNextFrame->pts, pVideo->syncSerial, pVideo->syncTime);
					//MDiagnostic("ADVANCING TO NEXT FRAME pts %03f serial %d (syncTime %03f, syncSerial %d)", pNextFrame->pts, pNextFrame->serial, pVideo->syncTime, pVideo->syncSerial);

					if (GetTextureFromDecoder(pNextFrame))
					{
						numFramesProcessed += 1;
					}

					// Add the currently active frame back to the map11 queue to get reused again
					if (pCurrentFrame)
					{
						aTextureMapQueue.push_back(pCurrentFrame);
					}

					pCurrentFrame = pNextFrame;
					pNextFrame = 0;
				}
				else
				{
					break;
				}
			}
		}

		// Copy the frame over to the render texture if there is one available
		if(numFramesProcessed>0)
		{
			HRESULT hr = 0;
			hasValidFrame = true;

			//MLog("V: DXFrame render %d %.2f @ time %d %.2f", pCurrentFrame->serial, pCurrentFrame->pts, pVideo->syncSerial, pVideo->syncTime);

			if (isPassthrough)
			{
				if (pDevice9)
				{
					// Use StretchRect as it will do the conversion from YUV to RGB for us for free here
					auto videoSurface = (IDirect3DSurface9*)pCurrentFrame->frame->data[3];
					RECT rect = { 0, 0, videoWidth, videoHeight };
					hr = pDevice9->StretchRect(videoSurface, &rect, renderSurf9, &rect, D3DTEXF_NONE);
				}
				else
				{
					// Use CopyResource to copy the output of VideoDecoder/Processor to the display texture
					HANDLE h = (HANDLE)pCurrentFrame->frame->data[3];
					ID3D11Texture2D* &texture = sharedTextures[h];
					pDeviceCtx11->CopyResource(renderTex11, texture);
				}
			}
			else
			{
				if (pDevice9)
				{
					// Use UpdateSurface to copy the CPU modified texture to the GPU
					hr = pDevice9->UpdateSurface(pCurrentFrame->tex9, NULL, renderSurf9, NULL);
				}
				else
				{
					// Use CopyResrouce to copy the CPU modified staging texture to the GPU
					pDeviceCtx11->CopyResource(renderTex11, pCurrentFrame->tex11);
				}
			}
		}

		// Try to map all the textures in the map queue and send them to the video decoder
		while(aTextureMapQueue.size()>0)
		{
			TVideoTexture *vt = aTextureMapQueue[0];

			if (!ReturnTextureToDecoder(vt))
				break;
			
			pVideo->SignalFrameConsumed();
			aTextureMapQueue.erase(aTextureMapQueue.begin());
		}
	}

	//MDiagnostic("FramePTS %03f FrameSerial %d AudioTime %03f AudioSerial %d Processed %02d TextureFree %02d TextureReady %02d TextureMap %02d", pCurrentFrame ? pCurrentFrame->pts : 0, pCurrentFrame ? pCurrentFrame->serial : 0, pVideo->syncTime, pVideo->syncSerial, numFramesProcessed, aTextureFree.unsafe_size(), aTextureReady.unsafe_size(), aTextureMapQueue.size());
	if (pDevice9)
		return (hasValidFrame ? renderTex9 : 0);
	else
		return (hasValidFrame ? renderRV11 : 0);
}

double TDXVideoFrameOutput_Int::GetTime()
{
	return pCurrentFrame ? pCurrentFrame->pts : 0.0;
}
