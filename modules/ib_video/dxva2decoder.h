#ifdef WIN32
#ifndef _DXVA2DECODER_H_
#define _DXVA2DECODER_H_

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define DXVA2API_USE_BITFIELDS

#include <stdint.h>
#include <mutex>
#include <concurrent_queue.h>

#include <d3d9.h>
#include <d3d11.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "libavcodec/dxva2.h"
#include "libavcodec/d3d11va.h"
#include "libavcodec/avcodec.h"

#include "libavutil/avassert.h"
#include "libavutil/buffer.h"
#include "libavutil/frame.h"
#include "libavutil/imgutils.h"
#include "libavutil/pixfmt.h"
#ifdef __cplusplus
}
#endif

typedef struct dxva2_mode {
  const GUID     *guid;
  enum AVCodecID codec;
} dxva2_mode;

typedef struct DXVA2Context {
	HMODULE hD3D11Lib;
    HMODULE hDXVALib;

    IDirect3DDevice9*				pD3D9Device;
	HANDLE							hD3D9Device;
    IDirect3DDeviceManager9*		pD3D9DeviceManager;
    IDirectXVideoDecoderService*	pD3D9DecoderService;

	IDXGIAdapter1*					pD3D11Adapter;
	D3D_FEATURE_LEVEL				D3D11FeatureLevel;
	ID3D11Device*					pD3D11Device;
	ID3D11DeviceContext*			pD3D11ImmediateContext;
	ID3D11VideoDevice*				pD3D11VideoDevice;
	ID3D11VideoContext*				pD3D11VideoContext;
} DXVA2Context;

class HWDecoderBase {
public:
	virtual int Init() = 0;
	virtual void Uninit() = 0;
	virtual int CreateDecoderContext(AVCodecContext *ctx) = 0;
	virtual void DestroyDecoderContext(AVCodecContext *ctx) = 0;
	virtual AVPixelFormat GetSupportedPixelFormat() const = 0;
	virtual const char *GetName() const = 0;
};

class Dxva2Decoder : public HWDecoderBase {
protected:
	DXVA2Context *m_Dxva2Context;
public:
	Dxva2Decoder();

   int Init();
   void Uninit();
   int CreateDecoderContext(AVCodecContext *ctx);
   void DestroyDecoderContext(AVCodecContext *ctx);
   AVPixelFormat GetSupportedPixelFormat() const;
   const char *GetName() const;

protected:
   int GetDecoderConfiguration(AVCodecContext *ctx, const GUID *device_guid, const DXVA2_VideoDesc *desc, DXVA2_ConfigPictureDecode *config);
   int GetDecoderConfiguration11(AVCodecContext *ctx, const D3D11_VIDEO_DECODER_DESC *desc, D3D11_VIDEO_DECODER_CONFIG *config);
  
   static AVHWAccel *GetHWAccel(AVCodecContext *ctx, AVPixelFormat pixFormat);
   static int GetBuffer(AVCodecContext *ctx, AVFrame *frame, int flags);
   static void ReleaseBuffer(void *opaque, uint8_t *data);

private:
	static std::mutex s_InitMutex;
};

class HWDecoderContextBase {
public:
	virtual int RetrieveData(AVCodecContext *ctx, AVFrame *frame) = 0;
	virtual HWDecoderBase* GetHWDecoder() const = 0;
};

struct DXVAProcessorResult {
	ID3D11Texture2D*					pVPOutTex11;
	HANDLE								hVPOutTexSharedHandle11;
	IDXGIKeyedMutex*    				pVPOutTexMutex11;
	ID3D11VideoProcessorOutputView*		pVPOutView11;
};

class DXVA2DecoderContext : public HWDecoderContextBase {
public:
	int(*e_OldGetBuffer)(struct AVCodecContext *s, AVFrame *frame, int flags);

	Dxva2Decoder*						parentHWAccel;

	GUID								decoderGUID;

	IDirectXVideoDecoder*				pDecoder9;
	DXVA2_ConfigPictureDecode			decoderConfig9;
	LPDIRECT3DSURFACE9					*aDecOutSurface9;

	ID3D11VideoDecoder*					pDecoder11;
	ID3D11VideoProcessorEnumerator*		pVPEnumerator11;
	ID3D11VideoProcessor*				pVProcessor11;
	D3D11_VIDEO_DECODER_CONFIG			decoderConfig11;
	ID3D11Texture2D*					pDecTexture11;
	DXVAProcessorResult					*aVPResult;
	ID3D11VideoDecoderOutputView*		*aDecOutView11;
	ID3D11VideoProcessorInputView*		*aVPInView11;

	uint32_t							numSurfaces;
	uint32_t							numOutputs;

	DXVA2Context*						m_Dxva2Context;

	concurrency::concurrent_queue<int>	freeSurfaces;
	concurrency::concurrent_queue<int>	freeOutputs;

	AVFrame*							tempFrame;
public:
	DXVA2DecoderContext() : parentHWAccel(0), pDecoder9(0), pDecoder11(0), pVPEnumerator11(0), pVProcessor11(0), pDecTexture11(0), aVPInView11(0), aVPResult(0), numSurfaces(0), aDecOutSurface9(0), aDecOutView11(0), numOutputs(0), tempFrame(0), decoderGUID(GUID_NULL) {}
	int RetrieveData(AVCodecContext *ctx, AVFrame *frame);
	HWDecoderBase* GetHWDecoder() const { return parentHWAccel; }
public:
	static void ReleaseOutput(void *opaque, uint8_t *data);
};

class HWDecoderManager {
public:
	static enum AVPixelFormat GetFormat(AVCodecContext *ctx, const enum AVPixelFormat *pix_fmts);
	static void Init(AVCodecContext *ctx);
	static void Uninit(AVCodecContext *ctx);
	static int RetrieveData(AVCodecContext *ctx, AVFrame *frame);
	static void RenderThreadUpdate();
	static void Reset();
protected:
	static HWDecoderBase* s_Decoders[];
	static HWDecoderBase *GetHWDecoder(AVPixelFormat pixFormat);
};

#endif
#endif