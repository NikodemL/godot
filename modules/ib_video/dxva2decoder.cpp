#ifdef WIN32
#include "Dxva2Decoder.h"
//#include <video.h>
#include <concurrent_queue.h>
#include <vector>

#include <initguid.h>
DEFINE_GUID(IID_IDirectXVideoDecoderService, 0xfc51a551,0xd5e7,0x11d9,0xaf,0x55,0x00,0x05,0x4e,0x43,0xff,0x02);

DEFINE_GUID(DXVA2_ModeMPEG2_VLD,		0xee27417f, 0x5e28,0x4e65,0xbe,0xea,0x1d,0x26,0xb5,0x08,0xad,0xc9);
DEFINE_GUID(DXVA2_ModeMPEG2and1_VLD,	0x86695f12, 0x340e,0x4f04,0x9f,0xd3,0x92,0x53,0xdd,0x32,0x74,0x60);
DEFINE_GUID(DXVA2_ModeH264_E,			0x1b81be68, 0xa0c7,0x11d3,0xb9,0x84,0x00,0xc0,0x4f,0x2e,0x73,0xc5);
DEFINE_GUID(DXVA2_ModeH264_F,			0x1b81be69, 0xa0c7,0x11d3,0xb9,0x84,0x00,0xc0,0x4f,0x2e,0x73,0xc5);
DEFINE_GUID(DXVADDI_Intel_ModeH264_E,	0x604F8E68, 0x4951,0x4C54,0x88,0xFE,0xAB,0xD2,0x5C,0x15,0xB3,0xD6);
DEFINE_GUID(DXVA2_ModeVC1_D,			0x1b81beA3, 0xa0c7,0x11d3,0xb9,0x84,0x00,0xc0,0x4f,0x2e,0x73,0xc5);
DEFINE_GUID(DXVA2_ModeVC1_D2010,		0x1b81beA4, 0xa0c7,0x11d3,0xb9,0x84,0x00,0xc0,0x4f,0x2e,0x73,0xc5);
DEFINE_GUID(DXVA2_ModeHEVC_VLD_Main,	0x5b11d51b, 0x2f4c,0x4452,0xbc,0xc3,0x09,0xf2,0xa1,0x16,0x0c,0xc0);
DEFINE_GUID(DXVA2_NoEncrypt,			0x1b81beD0, 0xa0c7,0x11d3,0xb9,0x84,0x00,0xc0,0x4f,0x2e,0x73,0xc5);
DEFINE_GUID(GUID_NULL,					0x00000000, 0x0000,0x0000,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);

static const dxva2_mode dxva2_modes[] = {
	 /* MPEG-2 */
	 { &DXVA2_ModeMPEG2_VLD,		AV_CODEC_ID_MPEG2VIDEO },
	 { &DXVA2_ModeMPEG2and1_VLD,	AV_CODEC_ID_MPEG2VIDEO },

	 /* H.264 */
	 { &DXVA2_ModeH264_F,			AV_CODEC_ID_H264 },
	 { &DXVA2_ModeH264_E,			AV_CODEC_ID_H264 },
	 /* Intel specific H.264 mode */
	 { &DXVADDI_Intel_ModeH264_E,	AV_CODEC_ID_H264 },

	 /* VC-1 / WMV3 */
	 { &DXVA2_ModeVC1_D2010,		AV_CODEC_ID_VC1  },
	 { &DXVA2_ModeVC1_D2010,		AV_CODEC_ID_WMV3 },
	 { &DXVA2_ModeVC1_D,			AV_CODEC_ID_VC1  },
	 { &DXVA2_ModeVC1_D,			AV_CODEC_ID_WMV3 },

	 /* HEVC/H.265 */
	 { &DXVA2_ModeHEVC_VLD_Main,	AV_CODEC_ID_HEVC },

	 { NULL,						(AVCodecID)0 },
};

std::mutex Dxva2Decoder::s_InitMutex;

Dxva2Decoder::Dxva2Decoder() : m_Dxva2Context(NULL)
{
	
}

extern IDirect3DDevice9* pDevice9;
extern ID3D11Device* pDevice11;
#define SAFE_RELEASE(x) if(x){x->Release();x=0;}

int Dxva2Decoder::Init()
{
	std::lock_guard<std::mutex> scopeLock(s_InitMutex);

	if (m_Dxva2Context)
		return 0;

	m_Dxva2Context = (DXVA2Context*)av_mallocz(sizeof(*m_Dxva2Context));
	if (!m_Dxva2Context)
		return AVERROR(ENOMEM);

	if (pDevice11)
	{
		// Load D3D11 DLL and get D3D11CreateDevice funcptr
		m_Dxva2Context->hD3D11Lib = LoadLibrary("d3d11.dll");
		if (!m_Dxva2Context->hD3D11Lib) {
			av_log(NULL, AV_LOG_ERROR, "Failed to load D3D11 library\n");
			goto fail;
		}

		PFN_D3D11_CREATE_DEVICE pD3D11CreateDevice = (PFN_D3D11_CREATE_DEVICE)GetProcAddress(m_Dxva2Context->hD3D11Lib, "D3D11CreateDevice");
		if (!pD3D11CreateDevice) {
			av_log(NULL, AV_LOG_ERROR, "Failed to locate D3D11CreateDevice\n");
			goto fail;
		}

		// Create the device
		D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0 };

		// The VIDEO_SUPPORT flag force lowering feature level if current env not support video on 11_1
		UINT createDeviceFlags = D3D11_CREATE_DEVICE_VIDEO_SUPPORT;
		#ifdef _DEBUG
			createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
		#endif

		HRESULT hr = pD3D11CreateDevice(m_Dxva2Context->pD3D11Adapter, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevels, ARRAYSIZE(featureLevels),
										D3D11_SDK_VERSION, &m_Dxva2Context->pD3D11Device, &m_Dxva2Context->D3D11FeatureLevel, &m_Dxva2Context->pD3D11ImmediateContext);

		if (FAILED(hr))
		{
			// DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
			av_log(NULL, AV_LOG_DEBUG, "%s - First try to create device failed with error: %x.", __FUNCTION__, hr);
			av_log(NULL, AV_LOG_DEBUG, "%s - Trying to create device with lowest feature level: %#x.", __FUNCTION__, featureLevels[1]);

			hr = pD3D11CreateDevice(m_Dxva2Context->pD3D11Adapter, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, &featureLevels[1], ARRAYSIZE(featureLevels) - 1,
									D3D11_SDK_VERSION, &m_Dxva2Context->pD3D11Device, &m_Dxva2Context->D3D11FeatureLevel, &m_Dxva2Context->pD3D11ImmediateContext);
			if (FAILED(hr))
			{
				// still failed. seems driver doesn't support video API acceleration, try without VIDEO_SUPPORT flag
				av_log(NULL, AV_LOG_DEBUG, "%s - Next try to create device failed with error: %x.", __FUNCTION__, hr);
				av_log(NULL, AV_LOG_DEBUG, "%s - Trying to create device without video API support.", __FUNCTION__);

				createDeviceFlags &= ~D3D11_CREATE_DEVICE_VIDEO_SUPPORT;
				hr = pD3D11CreateDevice(m_Dxva2Context->pD3D11Adapter, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, &featureLevels[1], ARRAYSIZE(featureLevels) - 1,
										D3D11_SDK_VERSION, &m_Dxva2Context->pD3D11Device, &m_Dxva2Context->D3D11FeatureLevel, &m_Dxva2Context->pD3D11ImmediateContext);
				if (SUCCEEDED(hr))
					av_log(NULL, AV_LOG_ERROR, "%s - Your video driver doesn't support DirectX 11 Video Acceleration API. Application is not be able to use hardware video processing and decoding", __FUNCTION__);
			}
		}

		if (FAILED(hr))
		{
			av_log(NULL, AV_LOG_ERROR, "%s - D3D11 device creation failure with error %s.", __FUNCTION__, hr);
			return false;
		}

		// Get ID3D11VideoDevice and ID3D11VideoContext from the device
		m_Dxva2Context->pD3D11Device->QueryInterface(&m_Dxva2Context->pD3D11VideoDevice);
		m_Dxva2Context->pD3D11ImmediateContext = m_Dxva2Context->pD3D11ImmediateContext;
		hr = m_Dxva2Context->pD3D11ImmediateContext->QueryInterface(&m_Dxva2Context->pD3D11VideoContext);

		// Enable multithread protection
		ID3D10Multithread *pMultiThreading = nullptr;
		if (SUCCEEDED(m_Dxva2Context->pD3D11Device->QueryInterface(__uuidof(ID3D10Multithread), reinterpret_cast<void**>(&pMultiThreading))))
		{
			pMultiThreading->SetMultithreadProtected(true);
			pMultiThreading->Release();
		}
	}
	else
	{
		// Load DXVA2 DLL and get funcptrs
		typedef HRESULT WINAPI pCreateDeviceManager9(UINT *, IDirect3DDeviceManager9 **);
		pCreateDeviceManager9 *createDeviceManager = NULL;

		HRESULT hr;
		unsigned resetToken = 0;

		m_Dxva2Context->hD3D9Device = INVALID_HANDLE_VALUE;

		m_Dxva2Context->hDXVALib = LoadLibrary("dxva2.dll");
		if (!m_Dxva2Context->hDXVALib) {
			av_log(NULL, AV_LOG_ERROR, "Failed to load DXVA2 library\n");
			goto fail;
		}

		createDeviceManager = (pCreateDeviceManager9 *)GetProcAddress(m_Dxva2Context->hDXVALib, "DXVA2CreateDirect3DDeviceManager9");
		if (!createDeviceManager) {
			av_log(NULL, AV_LOG_ERROR, "Failed to locate DXVA2CreateDirect3DDeviceManager9\n");
			goto fail;
		}

		m_Dxva2Context->pD3D9Device = pDevice9;

		// Create DXVA device manager and get IDirectXVideoDecoderService
		hr = createDeviceManager(&resetToken, &m_Dxva2Context->pD3D9DeviceManager);
		if (FAILED(hr)) {
			av_log(NULL, AV_LOG_ERROR, "Failed to create Direct3D device manager\n");
			goto fail;
		}

		hr = m_Dxva2Context->pD3D9DeviceManager->ResetDevice(m_Dxva2Context->pD3D9Device, resetToken);
		if (FAILED(hr)) {
			av_log(NULL, AV_LOG_ERROR, "Failed to bind Direct3D device to device manager\n");
			goto fail;
		}

		hr = m_Dxva2Context->pD3D9DeviceManager->OpenDeviceHandle(&m_Dxva2Context->hD3D9Device);
		if (FAILED(hr)) {
			av_log(NULL, AV_LOG_ERROR, "Failed to open device handle\n");
			goto fail;
		}

		hr = m_Dxva2Context->pD3D9DeviceManager->GetVideoService(m_Dxva2Context->hD3D9Device, IID_IDirectXVideoDecoderService, (void **)&m_Dxva2Context->pD3D9DecoderService);
		if (FAILED(hr)) {
			av_log(NULL, AV_LOG_ERROR, "Failed to create IDirectXVideoDecoderService\n");
			goto fail;
		}

	}

	return 0;
fail:
	Uninit();
	return AVERROR(EINVAL);
}

void Dxva2Decoder::Uninit()
{
	if (m_Dxva2Context != NULL)
	{
		SAFE_RELEASE(m_Dxva2Context->pD3D9DecoderService);
		if (m_Dxva2Context->pD3D9DeviceManager && m_Dxva2Context->hD3D9Device != INVALID_HANDLE_VALUE)
			m_Dxva2Context->pD3D9DeviceManager->CloseDeviceHandle(m_Dxva2Context->hD3D9Device);
		SAFE_RELEASE(m_Dxva2Context->pD3D9DeviceManager);

		SAFE_RELEASE(m_Dxva2Context->pD3D11VideoContext);
		SAFE_RELEASE(m_Dxva2Context->pD3D11ImmediateContext);
		SAFE_RELEASE(m_Dxva2Context->pD3D11VideoDevice);
		SAFE_RELEASE(m_Dxva2Context->pD3D11Device);
		SAFE_RELEASE(m_Dxva2Context->pD3D11Adapter);

		if (m_Dxva2Context->hDXVALib)
			FreeLibrary(m_Dxva2Context->hDXVALib);

		if (m_Dxva2Context->hD3D11Lib)
			FreeLibrary(m_Dxva2Context->hD3D11Lib);

		av_freep(&m_Dxva2Context);
	}
}

AVPixelFormat Dxva2Decoder::GetSupportedPixelFormat() const
{
	return pDevice11 ? AV_PIX_FMT_D3D11VA_VLD : AV_PIX_FMT_DXVA2_VLD;
}

const char *Dxva2Decoder::GetName() const
{
	return "Dxva2Decoder";
}

int Dxva2Decoder::CreateDecoderContext(AVCodecContext *ctx)
{
	GUID *guid_list = NULL;
	unsigned guid_count = 0, i, j;
	D3DFORMAT target_format = (D3DFORMAT)0;
	DXGI_FORMAT target_format_11 = DXGI_FORMAT_UNKNOWN;
	DXVA2_VideoDesc desc = { 0 };
	D3D11_VIDEO_DECODER_DESC desc11 = { 0 };
	HRESULT hr;
	int surface_alignment;
	int ret;

	if (ctx->codec_id == AV_CODEC_ID_H264 && (ctx->profile & ~FF_PROFILE_H264_CONSTRAINED) > FF_PROFILE_H264_HIGH)
	{
		av_log(NULL, AV_LOG_ERROR, "Unsupported H.264 profile for DXVA2 HWAccel: %d\n", ctx->profile);
		goto fail;
	}

	DXVA2DecoderContext* decoderContext = new DXVA2DecoderContext;
	if (!decoderContext)
		goto fail;

	decoderContext->parentHWAccel = this;
	ctx->opaque = decoderContext;
	decoderContext->m_Dxva2Context = m_Dxva2Context;

	decoderContext->tempFrame = av_frame_alloc();
	if (!decoderContext->tempFrame)
		goto fail;

	// Get HWAccel and alloc the proper hwaccel_context
	auto hwa = GetHWAccel(ctx, GetSupportedPixelFormat());
	av_assert0(hwa != NULL);
	ctx->hwaccel = hwa;
	ctx->hwaccel_context = pDevice11 ? av_d3d11va_alloc_context() : av_mallocz(sizeof(struct dxva_context));
	if (!ctx->hwaccel_context)
		goto fail;

	if (decoderContext->e_OldGetBuffer == NULL)
	{
		decoderContext->e_OldGetBuffer = ctx->get_buffer2;
	}
	ctx->get_buffer2 = GetBuffer;

	// Get decoder GUIDs
	if (pDevice11)
	{
		guid_count = m_Dxva2Context->pD3D11VideoDevice->GetVideoDecoderProfileCount();
		guid_list = (GUID*)av_malloc_array(sizeof(GUID), guid_count);
		for (unsigned int i = 0; i < guid_count; i++)
			m_Dxva2Context->pD3D11VideoDevice->GetVideoDecoderProfile(i, &guid_list[i]);
	}
	else
	{
		hr = m_Dxva2Context->pD3D9DecoderService->GetDecoderDeviceGuids(&guid_count, &guid_list);
		if (FAILED(hr))
		{
			av_log(NULL, AV_LOG_ERROR, "Failed to retrieve decoder device GUIDs\n");
			goto fail;
		}

	}
  
	// Select a decoder that can decode NV12 format
	for (i = 0; dxva2_modes[i].guid; i++) 
	{
		const dxva2_mode *mode = &dxva2_modes[i];
		if (mode->codec != ctx->codec_id)
			continue;

		for (j = 0; j < guid_count; j++) 
		{
			if (IsEqualGUID(*mode->guid, guid_list[j]))
					break;
		}
		if (j == guid_count)
			continue;

		if (pDevice11)
		{
			BOOL supported = FALSE;
			HRESULT res = m_Dxva2Context->pD3D11VideoDevice->CheckVideoDecoderFormat(mode->guid, DXGI_FORMAT_NV12, &supported);
			if (supported)
			{
				decoderContext->decoderGUID = *mode->guid;
				target_format_11 = DXGI_FORMAT_NV12;
				break;
			}
		}
		else
		{
			D3DFORMAT *target_list = NULL;
			unsigned target_count = 0;
			hr = m_Dxva2Context->pD3D9DecoderService->GetDecoderRenderTargets(*mode->guid, &target_count, &target_list);
			if (FAILED(hr))
			{
				continue;
			}
			for (j = 0; j < target_count; j++)
			{
				const D3DFORMAT format = target_list[j];
				if (format == MKTAG('N', 'V', '1', '2'))
				{
					target_format = format;
					break;
				}
			}
			CoTaskMemFree(target_list);
			if (target_format)
			{
				decoderContext->decoderGUID = *mode->guid;
				break;
			}
		}
	}

	if (pDevice11)
		av_free(guid_list);
	else
		CoTaskMemFree(guid_list);

	if (IsEqualGUID(decoderContext->decoderGUID, GUID_NULL))
	{
		av_log(NULL, AV_LOG_ERROR, "No decoder device for codec found\n");
		goto fail;
	}

	// Get decoder configuration
	if (pDevice11)
	{
		desc11.Guid = decoderContext->decoderGUID;
		desc11.SampleWidth = ctx->coded_width;
		desc11.SampleHeight = ctx->coded_height;
		desc11.OutputFormat = target_format_11;
		
		ret = GetDecoderConfiguration11(ctx, &desc11, &decoderContext->decoderConfig11);
	}
	else
	{
		desc.SampleWidth = ctx->coded_width;
		desc.SampleHeight = ctx->coded_height;
		desc.Format = target_format;

		ret = GetDecoderConfiguration(ctx, &decoderContext->decoderGUID, &desc, &decoderContext->decoderConfig9);
	} 
	if (ret < 0)
	{
		goto fail;
	}

	// Figure out the required surface alignment
	// Decoding MPEG-2 requires additional alignment on some Intel GPUs, but it causes issues for H.264 on certain AMD GPUs...
	if (ctx->codec_id == AV_CODEC_ID_MPEG2VIDEO)
		surface_alignment = 32;
	// The HEVC DXVA2 spec asks for 128 pixel aligned surfaces to ensure all coding features have enough room to work with
	else if  (ctx->codec_id == AV_CODEC_ID_HEVC)
		surface_alignment = 128;
	else
		surface_alignment = 16;

	// Create the decoder suraces
	// 4 base work surfaces
	decoderContext->numSurfaces = 4;
	decoderContext->numOutputs = decoderContext->numSurfaces;

	// Add surfaces based on number of possible refs
	if (ctx->codec_id == AV_CODEC_ID_H264 || ctx->codec_id == AV_CODEC_ID_HEVC)
		decoderContext->numSurfaces += 16;
	else
		decoderContext->numSurfaces += 2;

	// Add extra surfaces for frame threading
	if (ctx->active_thread_type & FF_THREAD_FRAME)
		decoderContext->numSurfaces += ctx->thread_count;

	if (pDevice11)
	{
		// Alloc surface info buffers
		decoderContext->aVPResult = (DXVAProcessorResult*)av_mallocz(decoderContext->numOutputs * sizeof(*decoderContext->aVPResult));
		decoderContext->aDecOutView11 = (ID3D11VideoDecoderOutputView**)av_mallocz(decoderContext->numSurfaces * sizeof(*decoderContext->aDecOutView11));
		decoderContext->aVPInView11 = (ID3D11VideoProcessorInputView**)av_mallocz(decoderContext->numSurfaces * sizeof(*decoderContext->aVPInView11));

		if (!decoderContext->aVPResult || !decoderContext->aDecOutView11 || !decoderContext->aVPInView11)
		{
			av_log(NULL, AV_LOG_ERROR, "Unable to allocate surface arrays\n");
			goto fail;
		}

		// Create a single array texture (required to be an array by FFMPEG/DXVA)
		D3D11_TEXTURE2D_DESC texDesc = { 0 };
		texDesc.Format = desc11.OutputFormat;
		texDesc.Width = FFALIGN(ctx->coded_width, surface_alignment);
		texDesc.Height = FFALIGN(ctx->coded_height, surface_alignment);
		texDesc.BindFlags = D3D11_BIND_DECODER;
		texDesc.ArraySize = decoderContext->numSurfaces;
		texDesc.MipLevels = 1;
		texDesc.SampleDesc.Count = 1;

		D3D11_VIDEO_DECODER_OUTPUT_VIEW_DESC vdovDesc = { 0 };
		vdovDesc.DecodeProfile = desc11.Guid;
		vdovDesc.Texture2D.ArraySlice = 0;
		vdovDesc.ViewDimension = D3D11_VDOV_DIMENSION_TEXTURE2D;

		hr = m_Dxva2Context->pD3D11Device->CreateTexture2D(&texDesc, NULL, &decoderContext->pDecTexture11);
		if (FAILED(hr))
		{
			av_log(NULL, AV_LOG_ERROR, "Failed creating decoder texture array\n");
			goto fail;
		}

		// Create decoder output views for each slice of the array
		for (uint32_t i = 0; i < decoderContext->numSurfaces; ++i)
		{
			vdovDesc.Texture2D.ArraySlice = D3D11CalcSubresource(0, i, texDesc.MipLevels);
			hr = m_Dxva2Context->pD3D11VideoDevice->CreateVideoDecoderOutputView(decoderContext->pDecTexture11, &vdovDesc, &decoderContext->aDecOutView11[i]);
			if (FAILED(hr))
			{
				av_log(NULL, AV_LOG_ERROR, "Failed to create %d video surfaces\n", decoderContext->numSurfaces);
				goto fail;
			}

			decoderContext->freeSurfaces.push(i);
		}

		// Create the decoder
		hr = m_Dxva2Context->pD3D11VideoDevice->CreateVideoDecoder(&desc11, &decoderContext->decoderConfig11, &decoderContext->pDecoder11);
		if (FAILED(hr))
		{
			av_log(NULL, AV_LOG_ERROR, "Failed to create DXVA2 video decoder\n");
			goto fail;
		}

		// Create the video processor (used for YUV -> RGB hardware conversion)
		D3D11_VIDEO_PROCESSOR_CAPS m_vcaps = { 0 };

		D3D11_VIDEO_PROCESSOR_CONTENT_DESC contentDesc;
		ZeroMemory(&contentDesc, sizeof(contentDesc));
		contentDesc.InputFrameFormat = D3D11_VIDEO_FRAME_FORMAT_PROGRESSIVE;
		contentDesc.InputWidth = texDesc.Width;
		contentDesc.InputHeight = texDesc.Height;
		contentDesc.InputFrameRate.Numerator = ctx->framerate.num;
		contentDesc.InputFrameRate.Denominator = ctx->framerate.den;
		contentDesc.OutputWidth = texDesc.Width;
		contentDesc.OutputHeight = texDesc.Height;
		contentDesc.OutputFrameRate.Numerator = ctx->framerate.num;
		contentDesc.OutputFrameRate.Denominator = ctx->framerate.den;
		contentDesc.Usage = D3D11_VIDEO_USAGE_PLAYBACK_NORMAL;

		if (FAILED(m_Dxva2Context->pD3D11VideoDevice->CreateVideoProcessorEnumerator(&contentDesc, &decoderContext->pVPEnumerator11)))
		{
			av_log(NULL, AV_LOG_ERROR, "%s: failed to init video enumerator with params: %dx%d.", __FUNCTION__, texDesc.Width, texDesc.Height);
			goto fail;
		}

		if (FAILED(decoderContext->pVPEnumerator11->GetVideoProcessorCaps(&m_vcaps)))
		{
			av_log(NULL, AV_LOG_ERROR, "%s - failed to get processor caps.", __FUNCTION__);
			goto fail;
		}

		av_log(NULL, AV_LOG_DEBUG, "%s: Video processor has %d rate conversion.", __FUNCTION__, m_vcaps.RateConversionCapsCount);
		av_log(NULL, AV_LOG_DEBUG, "%s: Video processor has %#x feature caps.", __FUNCTION__, m_vcaps.FeatureCaps);
		av_log(NULL, AV_LOG_DEBUG, "%s: Video processor has %#x device caps.", __FUNCTION__, m_vcaps.DeviceCaps);
		av_log(NULL, AV_LOG_DEBUG, "%s: Video processor has %#x input format caps.", __FUNCTION__, m_vcaps.InputFormatCaps);
		av_log(NULL, AV_LOG_DEBUG, "%s: Video processor has %d max input streams.", __FUNCTION__, m_vcaps.MaxInputStreams);
		av_log(NULL, AV_LOG_DEBUG, "%s: Video processor has %d max stream states.", __FUNCTION__, m_vcaps.MaxStreamStates);

		hr = m_Dxva2Context->pD3D11VideoDevice->CreateVideoProcessor(decoderContext->pVPEnumerator11, 0, &decoderContext->pVProcessor11);
		if (FAILED(hr))
		{
			av_log(NULL, AV_LOG_ERROR, "%s: Failed creating video processor with error %x.", __FUNCTION__, hr);
			goto fail;
		}
		
		// Setup video processor input color space
		int fullRangeInput = 0;
		D3D11_VIDEO_PROCESSOR_COLOR_SPACE cs
		{
			0,											// 0 - Playback, 1 - Processing
			0,											// 0 - Full (0-255), 1 - Limited (16-235)
			ctx->colorspace & AVCOL_SPC_BT709 ? 1 : 0,	// 0 - BT.601, 1 - BT.709
			fullRangeInput ? 1 : 0,						// 0 - Conventional YCbCr, 1 - xvYCC
			0,											// 2 - Full luminance range [0-255], 1 - Studio luminance range [16-235], 0 - driver defaults
		};
		
		if (m_vcaps.DeviceCaps & D3D11_VIDEO_PROCESSOR_DEVICE_CAPS_NOMINAL_RANGE)
			cs.Nominal_Range = fullRangeInput ? 2 : 1;

		m_Dxva2Context->pD3D11VideoContext->VideoProcessorSetStreamColorSpace(decoderContext->pVProcessor11, 0, &cs);

		// Setup video processor output color space
		int fullRangeOutput = 1;
		D3D11_VIDEO_PROCESSOR_COLOR_SPACE colorSpace = {};
		colorSpace.Usage = 0;								// 0 - playback, 1 - video processing
		colorSpace.RGB_Range = fullRangeOutput ? 0 : 1;		// 0 - 0-255, 1 - 16-235
		colorSpace.YCbCr_Matrix = 1;						// 0 - BT.601, 1 = BT.709
		colorSpace.YCbCr_xvYCC = 1;							// 0 - Conventional YCbCr, 1 - xvYCC
		colorSpace.Nominal_Range = 0;						// 2 - 0-255, 1 = 16-235, 0 - undefined

		m_Dxva2Context->pD3D11VideoContext->VideoProcessorSetOutputColorSpace(decoderContext->pVProcessor11, &colorSpace);

		// Set video processor to outuput a black background by default
		D3D11_VIDEO_COLOR color = {};
		color.RGBA = { 0, 0, 0, 1 };
		m_Dxva2Context->pD3D11VideoContext->VideoProcessorSetOutputBackgroundColor(decoderContext->pVProcessor11, FALSE, &color);
		
		// Disable auto processing (the driver might do funky stuff based on control panel settings - video smoothing, etc.)
		m_Dxva2Context->pD3D11VideoContext->VideoProcessorSetStreamAutoProcessingMode(decoderContext->pVProcessor11, 0, FALSE);

		// Setup video processor input stream and clipping rectangles (clip to actual video size instead of codec aligned size e.g. 1920x1088 -> 1920x1080)
		RECT rect = { 0, 0, ctx->width, ctx->height };
		m_Dxva2Context->pD3D11VideoContext->VideoProcessorSetStreamSourceRect(decoderContext->pVProcessor11, 0, true, &rect);
		m_Dxva2Context->pD3D11VideoContext->VideoProcessorSetOutputTargetRect(decoderContext->pVProcessor11, true, &rect);
		m_Dxva2Context->pD3D11VideoContext->VideoProcessorSetStreamFrameFormat(decoderContext->pVProcessor11, 0, D3D11_VIDEO_FRAME_FORMAT_PROGRESSIVE);

		// Create video processor input views for all codec outputs
		for (uint32_t i = 0; i < decoderContext->numSurfaces; i++)
		{
			D3D11_VIDEO_PROCESSOR_INPUT_VIEW_DESC pivd = { 0, D3D11_VPIV_DIMENSION_TEXTURE2D };
			pivd.Texture2D.ArraySlice = i;
			pivd.Texture2D.MipSlice = 0;

			hr = m_Dxva2Context->pD3D11VideoDevice->CreateVideoProcessorInputView(decoderContext->pDecTexture11, decoderContext->pVPEnumerator11, &pivd, &decoderContext->aVPInView11[i]);
			if (FAILED(hr))
				break;
		}

		// Allocate video processor output textures
		// These textures are shared between the decoder DX11 device and the render DX11 device!
		D3D11_TEXTURE2D_DESC outTexDesc = { 0 };
		outTexDesc.Width = ctx->width;
		outTexDesc.Height = ctx->height;
		outTexDesc.MipLevels = 1;
		outTexDesc.SampleDesc.Count = 1;
		outTexDesc.ArraySize = 1;
		outTexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		outTexDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;
		outTexDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

		D3D11_VIDEO_PROCESSOR_OUTPUT_VIEW_DESC pivd = { D3D11_VPOV_DIMENSION_TEXTURE2D, 0 };
		pivd.Texture2D.MipSlice = 0;

		// Make sure the video processor supports outputting RGBA
		UINT uiFlags;
		if (S_OK == decoderContext->pVPEnumerator11->CheckVideoProcessorFormat(outTexDesc.Format, &uiFlags))
		{
			if ((uiFlags & D3D11_VIDEO_PROCESSOR_FORMAT_SUPPORT_OUTPUT) == 0)
			{
				av_log(NULL, AV_LOG_ERROR, "%s: Unsupported output format %d.", __FUNCTION__, outTexDesc.Format);
				goto fail;
			}
		}

		for (uint32_t i = 0; i < decoderContext->numOutputs; i++)
		{		
			hr = m_Dxva2Context->pD3D11Device->CreateTexture2D(&outTexDesc, NULL, &decoderContext->aVPResult[i].pVPOutTex11);
			if (FAILED(hr))
				break;

			IDXGIResource* dxgiResource;
			hr = decoderContext->aVPResult[i].pVPOutTex11->QueryInterface(&dxgiResource);
			if FAILED(hr)
			{
				av_log(NULL, AV_LOG_ERROR, "Failed getting video processor texture dxgi resource\n");
			}

			hr = dxgiResource->GetSharedHandle(&decoderContext->aVPResult[i].hVPOutTexSharedHandle11);
			if FAILED(hr)
			{
				av_log(NULL, AV_LOG_ERROR, "Failed getting video processor texture shared handle\n");
			}
			dxgiResource->Release();

			hr = decoderContext->aVPResult[i].pVPOutTex11->QueryInterface(&decoderContext->aVPResult[i].pVPOutTexMutex11);
			if FAILED(hr)
			{
				av_log(NULL, AV_LOG_ERROR, "Failed getting video processor texture shared mutex\n");
			}

			hr = m_Dxva2Context->pD3D11VideoDevice->CreateVideoProcessorOutputView(decoderContext->aVPResult[i].pVPOutTex11, decoderContext->pVPEnumerator11, &pivd, &decoderContext->aVPResult[i].pVPOutView11);
			if (FAILED(hr))
				break;

			decoderContext->freeOutputs.push(i);
		}
	}
	else
	{
		// Alloc surface info buffers
		decoderContext->aDecOutSurface9 = (IDirect3DSurface9**)av_mallocz(decoderContext->numSurfaces * sizeof(*decoderContext->aDecOutSurface9));
		if (!decoderContext->aDecOutSurface9)
		{
			av_log(NULL, AV_LOG_ERROR, "Unable to allocate surface arrays\n");
			goto fail;
		}

		// Create all the surfaces in one go
		hr = m_Dxva2Context->pD3D9DecoderService->CreateSurface(
			FFALIGN(ctx->coded_width, surface_alignment),
			FFALIGN(ctx->coded_height, surface_alignment),
			decoderContext->numSurfaces - 1,
			target_format, D3DPOOL_DEFAULT, 0,
			DXVA2_VideoDecoderRenderTarget,
			decoderContext->aDecOutSurface9, NULL);
		if (FAILED(hr)) {
			av_log(NULL, AV_LOG_ERROR, "Failed to create %d video surfaces\n", decoderContext->numSurfaces);
			goto fail;
		}

		// Create the decoder with the surfaces
		hr = m_Dxva2Context->pD3D9DecoderService->CreateVideoDecoder(decoderContext->decoderGUID,
			&desc, &decoderContext->decoderConfig9, decoderContext->aDecOutSurface9,
			decoderContext->numSurfaces, &decoderContext->pDecoder9);
		if (FAILED(hr)) {
			av_log(NULL, AV_LOG_ERROR, "Failed to create DXVA2 video decoder\n");
			goto fail;
		}

		// Add all services to free list
		for (uint32_t i = 0; i < decoderContext->numSurfaces; ++i)
		{
			decoderContext->freeSurfaces.push(i);
		}
	}

	if (pDevice11)
	{
		struct AVD3D11VAContext *dxva_ctx = (AVD3D11VAContext*)ctx->hwaccel_context;
		dxva_ctx->cfg = &decoderContext->decoderConfig11;
		dxva_ctx->video_context = m_Dxva2Context->pD3D11VideoContext;
		dxva_ctx->decoder = decoderContext->pDecoder11;
		dxva_ctx->surface = decoderContext->aDecOutView11;
		dxva_ctx->surface_count = decoderContext->numSurfaces;

		if (IsEqualGUID(decoderContext->decoderGUID, DXVADDI_Intel_ModeH264_E))
			dxva_ctx->workaround |= FF_DXVA2_WORKAROUND_INTEL_CLEARVIDEO;
	}
	else
	{
		struct dxva_context *dxva_ctx = (dxva_context*)ctx->hwaccel_context;
		dxva_ctx->cfg = &decoderContext->decoderConfig9;
		dxva_ctx->decoder = decoderContext->pDecoder9;
		dxva_ctx->surface = decoderContext->aDecOutSurface9;
		dxva_ctx->surface_count = decoderContext->numSurfaces;

		if (IsEqualGUID(decoderContext->decoderGUID, DXVADDI_Intel_ModeH264_E))
			dxva_ctx->workaround |= FF_DXVA2_WORKAROUND_INTEL_CLEARVIDEO;
	}

	return 0;

fail:
	DestroyDecoderContext(ctx);
	return AVERROR(EINVAL);
}

void Dxva2Decoder::DestroyDecoderContext(AVCodecContext *ctx)
{
	HRESULT hr = S_OK;
	uint32_t i;

	DXVA2DecoderContext* decoderContext = (DXVA2DecoderContext*)ctx->opaque;
	if (decoderContext == 0)
		return;

	for (i = 0; i < decoderContext->numSurfaces; i++)
	{
		if (decoderContext->aDecOutSurface9)
			SAFE_RELEASE(decoderContext->aDecOutSurface9[i]);

		if (decoderContext->aDecOutView11)
			SAFE_RELEASE(decoderContext->aDecOutView11[i]);

		if (decoderContext->aVPInView11)
			SAFE_RELEASE(decoderContext->aVPInView11[i]);
	}

	av_freep(&decoderContext->aDecOutSurface9);
	av_freep(&decoderContext->aDecOutView11);
	av_freep(&decoderContext->aVPInView11);
	decoderContext->numSurfaces = 0;

	for (i = 0; i < decoderContext->numOutputs; i++)
	{
		if (decoderContext->aVPResult)
		{
			auto vpr = decoderContext->aVPResult[i];
			
			while ((hr = vpr.pVPOutTexMutex11->AcquireSync(0, INFINITE)) != S_OK);

			vpr.hVPOutTexSharedHandle11 = 0;
			SAFE_RELEASE(vpr.pVPOutTex11);
			SAFE_RELEASE(vpr.pVPOutTexMutex11);
			SAFE_RELEASE(vpr.pVPOutView11);
		}
	}

	av_freep(&decoderContext->aVPResult);
	decoderContext->numOutputs = 0;

	SAFE_RELEASE(decoderContext->pDecoder9);

	SAFE_RELEASE(decoderContext->pVProcessor11);
	SAFE_RELEASE(decoderContext->pVPEnumerator11);
	SAFE_RELEASE(decoderContext->pDecTexture11);
	SAFE_RELEASE(decoderContext->pDecoder11);

	av_frame_free(&decoderContext->tempFrame);

	av_freep(&ctx->hwaccel_context);

	ctx->get_buffer2 = decoderContext->e_OldGetBuffer;

	ctx->hwaccel = NULL;
	ctx->hwaccel_context = NULL;
	ctx->opaque = NULL;

	delete decoderContext;
}

int Dxva2Decoder::GetDecoderConfiguration(AVCodecContext *ctx, const GUID *device_guid, const DXVA2_VideoDesc *desc, DXVA2_ConfigPictureDecode *config)
{
	unsigned cfg_count = 0, best_score = 0;
	DXVA2_ConfigPictureDecode *cfg_list = NULL;
	DXVA2_ConfigPictureDecode best_cfg = {{0}};
	HRESULT hr;
	uint32_t i;

	hr = m_Dxva2Context->pD3D9DecoderService->GetDecoderConfigurations(*device_guid, desc, NULL, &cfg_count, &cfg_list);
	if (FAILED(hr)) 
	{
		av_log(NULL, AV_LOG_ERROR, "Unable to retrieve decoder configurations\n");
		return AVERROR(EINVAL);
	}

	for (i = 0; i < cfg_count; i++) {
		DXVA2_ConfigPictureDecode *cfg = &cfg_list[i];

		unsigned score;
		if (cfg->ConfigBitstreamRaw == 1)
			score = 1;
		else if (ctx->codec_id == AV_CODEC_ID_H264 && cfg->ConfigBitstreamRaw == 2)
			score = 2;
		else
			continue;
		if (IsEqualGUID(cfg->guidConfigBitstreamEncryption, DXVA2_NoEncrypt))
			score += 16;
		if (score > best_score) {
			best_score = score;
			best_cfg	= *cfg;
		}
	}
	CoTaskMemFree(cfg_list);

	if (!best_score)
	{
		av_log(NULL, AV_LOG_ERROR, "No valid decoder configuration available\n");
		return AVERROR(EINVAL);
	}

	*config = best_cfg;
	return 0;
}

int Dxva2Decoder::GetDecoderConfiguration11(AVCodecContext *ctx, const D3D11_VIDEO_DECODER_DESC *desc, D3D11_VIDEO_DECODER_CONFIG *config)
{
	unsigned cfg_count = 0, best_score = 0;
	D3D11_VIDEO_DECODER_CONFIG best_cfg = { 0 };
	HRESULT hr;
	uint32_t i;

	hr = m_Dxva2Context->pD3D11VideoDevice->GetVideoDecoderConfigCount(desc, &cfg_count);
	if (FAILED(hr))
	{
		av_log(NULL, AV_LOG_ERROR, "Unable to retrieve decoder configurations\n");
		return AVERROR(EINVAL);
	}

	for (i = 0; i < cfg_count; i++) {
		D3D11_VIDEO_DECODER_CONFIG cfg = { 0 };
		hr = m_Dxva2Context->pD3D11VideoDevice->GetVideoDecoderConfig(desc, i, &cfg);

		unsigned score;
		if (cfg.ConfigBitstreamRaw == 1)
			score = 1;
		else if (ctx->codec_id == AV_CODEC_ID_H264 && cfg.ConfigBitstreamRaw == 2)
			score = 2;
		else
			continue;
		if (IsEqualGUID(cfg.guidConfigBitstreamEncryption, DXVA2_NoEncrypt))
			score += 16;
		if (score > best_score) {
			best_score = score;
			best_cfg = cfg;
		}
	}

	if (!best_score)
	{
		av_log(NULL, AV_LOG_ERROR, "No valid decoder configuration available\n");
		return AVERROR(EINVAL);
	}

	*config = best_cfg;
	return 0;
}

AVHWAccel *Dxva2Decoder::GetHWAccel(AVCodecContext *ctx, AVPixelFormat pixFormat)
{
	AVHWAccel *hwa = NULL;
	while((hwa = av_hwaccel_next(hwa)))
	{
		if (hwa->id == ctx->codec->id && hwa->pix_fmt == pixFormat)
		{
			break;
		}
	}

	return hwa;
}

concurrency::concurrent_queue<std::string> debugLog;

int Dxva2Decoder::GetBuffer(AVCodecContext *ctx, AVFrame *frame, int flags)
{
	DXVA2DecoderContext *decoderContext = (DXVA2DecoderContext*)ctx->opaque;

	int idx;
	while (decoderContext->freeSurfaces.try_pop(idx) == false)
		SleepEx(1, FALSE);

	if (pDevice11)
		frame->data[0] = frame->data[3] = (uint8_t*)decoderContext->aDecOutView11[idx];
	else
		frame->data[0] = frame->data[3] = (uint8_t*)decoderContext->aDecOutSurface9[idx];

	frame->buf[0] = av_buffer_create((uint8_t*)idx, 0, ReleaseBuffer, decoderContext, AV_BUFFER_FLAG_READONLY);
	if (!frame->buf[0]) 
	{
		return AVERROR(ENOMEM);
	}
		
	return 0;
}

void Dxva2Decoder::ReleaseBuffer(void *opaque, uint8_t *data)
{
	int idx = (int)data;
	DXVA2DecoderContext *decoderContext = (DXVA2DecoderContext*)opaque;
	decoderContext->freeSurfaces.push(idx);
}

void DXVA2DecoderContext::ReleaseOutput(void *opaque, uint8_t *data)
{
	DXVA2DecoderContext *pThis = (DXVA2DecoderContext*)opaque;
	int idx = (int)data;
	//MDiagnostic("ReleaseOutput %d", idx);
	pThis->freeOutputs.push(idx);
}

int DXVA2DecoderContext::RetrieveData(AVCodecContext *ctx, AVFrame *frame)
{
	tempFrame->width = frame->width;
	tempFrame->height = frame->height;
	tempFrame->format = frame->format;

	int ret = 0;
	if (pDevice11)
	{
		int outputIdx;
		if (freeOutputs.try_pop(outputIdx) == false)
			return 1;

		//MDiagnostic("AllocOutput %d", outputIdx);

		HRESULT hr = S_OK;
		DXVA2DecoderContext* pThis = (DXVA2DecoderContext*)av_buffer_get_opaque(frame->buf[0]);
		int surfaceIdx = (int)frame->buf[0]->data;

		D3D11_VIDEO_PROCESSOR_STREAM inputStreamInfo = { 0 };
		inputStreamInfo.Enable = TRUE;
		inputStreamInfo.pInputSurface = this->aVPInView11[surfaceIdx];

		while ((hr = aVPResult[outputIdx].pVPOutTexMutex11->AcquireSync(0, INFINITE)) != S_OK);
		hr = m_Dxva2Context->pD3D11VideoContext->VideoProcessorBlt(pVProcessor11, aVPResult[outputIdx].pVPOutView11, 0, 1, &inputStreamInfo);
		while ((hr = aVPResult[outputIdx].pVPOutTexMutex11->ReleaseSync(1)) != S_OK);		

		ret = av_frame_copy_props(tempFrame, frame);
		if (ret < 0)
			goto fail;

		tempFrame->data[0] = tempFrame->data[3] = (uint8_t*)aVPResult[outputIdx].hVPOutTexSharedHandle11;
		tempFrame->buf[0] = av_buffer_create((uint8_t*)outputIdx, 0, ReleaseOutput, this, AV_BUFFER_FLAG_READONLY);

		av_frame_unref(frame);
		av_frame_move_ref(frame, tempFrame);
	}

fail:
	av_frame_unref(tempFrame);
	return ret;

	/*LPDIRECT3DSURFACE9 surface =  (LPDIRECT3DSURFACE9)frame->data[3];
	D3DSURFACE_DESC	 surfaceDesc;
	D3DLOCKED_RECT	  LockedRect;
	HRESULT				hr;
	int					 ret;

	MDiagnostic("Retrieve:		%016llx for frame %016llx", surface, frame);

	surface->GetDesc(&surfaceDesc);

	tempFrame->width = frame->width;
	tempFrame->height = frame->height;
	tempFrame->format = AV_PIX_FMT_NV12;

	ret = av_frame_get_buffer(tempFrame, 32);
	if (ret < 0)
		return ret;
	double t0 = GetPreciseTime();
	hr = surface->LockRect(&LockedRect, NULL, D3DLOCK_READONLY);
	double t1 = GetPreciseTime();
	if (FAILED(hr)) 
	{
		av_log(NULL, AV_LOG_ERROR, "Unable to lock DXVA2 surface\n");
		return AVERROR_UNKNOWN;
	}

	av_image_copy_plane(tempFrame->data[0], tempFrame->linesize[0],
							(uint8_t*)LockedRect.pBits,
							LockedRect.Pitch, frame->width, frame->height);
	double t2 = GetPreciseTime();
	av_image_copy_plane(tempFrame->data[1], tempFrame->linesize[1],
							(uint8_t*)LockedRect.pBits + LockedRect.Pitch * surfaceDesc.Height,
							LockedRect.Pitch, frame->width, frame->height / 2);
	double t3 = GetPreciseTime();
	surface->UnlockRect();

	ret = av_frame_copy_props(tempFrame, frame);
	if (ret < 0)
		goto fail;

	av_frame_unref(frame);
	av_frame_move_ref(frame, tempFrame);

	double t4 = GetPreciseTime();
	MDiagnostic("Retrieve timings: %f %f %f %f", t1-t0, t2-t1, t3-t2, t4-t3);

	return 0;

fail:
	av_frame_unref(tempFrame);
	return ret;*/
}

HWDecoderBase* HWDecoderManager::s_Decoders[] = { new Dxva2Decoder };

HWDecoderBase *HWDecoderManager::GetHWDecoder(AVPixelFormat pixFormat)
{
	size_t arrSize = sizeof(s_Decoders) / sizeof(*s_Decoders);
	for (uint32_t i = 0; i < arrSize; ++i)
	{
		if (s_Decoders[i]->GetSupportedPixelFormat() == pixFormat)
		{
			return s_Decoders[i];
		}
	}

	return NULL;
}

struct GetFormatItem {
	AVCodecContext *ctx;
	const enum AVPixelFormat *pix_fmts;
	concurrency::event *waitEvent;
	AVPixelFormat* returnFormat;
};

concurrency::concurrent_queue<GetFormatItem> getFormatQueue;

void HWDecoderManager::RenderThreadUpdate()
{
	GetFormatItem gfi;
	if (getFormatQueue.try_pop(gfi))
	{
		const enum AVPixelFormat *p;
		int ret;

		for (p = gfi.pix_fmts; *p != -1; p++)
		{
			const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(*p);

			if (!(desc->flags & AV_PIX_FMT_FLAG_HWACCEL))
				break;

			auto hwdecoder = GetHWDecoder(*p);
			if (hwdecoder == NULL)
			{
				continue;
			}

			if (!gfi.ctx->opaque)
			{
				ret = hwdecoder->Init();
				if (ret < 0)
				{
					av_log(NULL, AV_LOG_ERROR, "%s hwdecoder cannot be initialized.\n", hwdecoder->GetName());
					continue;
				}

				ret = hwdecoder->CreateDecoderContext(gfi.ctx);
				if (ret < 0)
				{
					av_log(NULL, AV_LOG_ERROR, "Error creating the decoder context for %s!\n", hwdecoder->GetName());
					continue;
				}

				av_log(NULL, AV_LOG_INFO, "Using %s hwaccel with %s pixel format.\n", gfi.ctx->hwaccel->name, av_pix_fmt_desc_get(gfi.ctx->hwaccel->pix_fmt)->name);
			}

			break;
		}

		*gfi.returnFormat = *p;
		gfi.waitEvent->set();
	}
}

enum AVPixelFormat HWDecoderManager::GetFormat(AVCodecContext *ctx, const enum AVPixelFormat *pix_fmts)
{
	concurrency::event waitEvent;
	AVPixelFormat ret;

	GetFormatItem gfi;
	gfi.ctx = ctx;
	gfi.pix_fmts = pix_fmts;
	gfi.waitEvent = &waitEvent;
	gfi.returnFormat = &ret;

	getFormatQueue.push(gfi);
	waitEvent.wait();

	return ret;
}

void HWDecoderManager::Init(AVCodecContext *ctx)
{
	ctx->get_format = GetFormat;
}

void HWDecoderManager::Uninit(AVCodecContext *ctx)
{
	HWDecoderContextBase* hwDecoderContext = (HWDecoderContextBase*)ctx->opaque;
	if (hwDecoderContext)
	{
		HWDecoderBase* hwDecoder = hwDecoderContext->GetHWDecoder();
		hwDecoder->DestroyDecoderContext(ctx);
	}
}

int HWDecoderManager::RetrieveData(AVCodecContext *ctx, AVFrame *frame)
{
	HWDecoderContextBase* hwDecoderContext = (HWDecoderContextBase*)ctx->opaque;
	if (hwDecoderContext)
	{
		return hwDecoderContext->RetrieveData(ctx, frame);
	}
	return 0;
}

void HWDecoderManager::Reset()
{
	size_t arrSize = sizeof(s_Decoders) / sizeof(*s_Decoders);
	for (uint32_t i = 0; i < arrSize; ++i)
	{
		s_Decoders[i]->Uninit();
	}
}

#endif
