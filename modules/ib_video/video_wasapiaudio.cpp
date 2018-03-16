#include "stdafx.h"

#include <Mmdeviceapi.h>
#include <Audioclient.h>
#include <concurrent_queue.h>
#include <thread>
#include <atomic>
#include <vector>

#include "video.h"
#include "video_wasapiaudio.h"

#define SAFE_RELEASE(punk) if((punk) != NULL) { (punk)->Release(); (punk) = NULL; }

//////////////////////////////////////////////////////////////////////////////////
// WASAPI based audio output (windows)
//////////////////////////////////////////////////////////////////////////////////
struct AudioFrame {
	uint64_t startSample;
	uint64_t endSample;
	double pts;
	int serial;
};

class TWASAPIVideoAudioOutput_Int : public TWASAPIVideoAudioOutput {
public:
	IMMDeviceEnumerator *pAudioEnumerator;
	IMMDevice *pAudioDevice;
	IAudioClient *pAudioClient;
	IAudioRenderClient *pAudioRenderClient;
	IAudioClock* pAudioClock;
	IAudioStreamVolume* pAudioVolume;
	WAVEFORMATEX* pAudioFormat;
	int started;
	float currentVolume;
	double lastPts;
	uint64_t posAtEndOfBuffer;
	std::vector<AudioFrame> audioFrames;
public:
	TWASAPIVideoAudioOutput_Int();
	~TWASAPIVideoAudioOutput_Int();
public:
	int Initialize(int bufferLengthMs);
	void Shutdown();
	void Start();
	void Stop();
	bool GetTime(double &time, int &serial);
	void* BeginOutput(int numSamplesRequested, double pts, int serial);
	void EndOutput(int numSamplesWritten);
	EAudioSampleFormat GetOutputFormat();
	int GetOutputChannelLayout();
	int GetOutputSampleRate();
	void SetVolume(float vol);
	int TryWaitShutdown();
	void FlushBuffers();
protected:
	uint64_t GetAudioPosInSamples();
};

TVideoAudioOutput* TWASAPIVideoAudioOutput::Create()
{
	return new TWASAPIVideoAudioOutput_Int;
}

void TWASAPIVideoAudioOutput::Destroy(TVideoAudioOutput* vao)
{
	delete (TWASAPIVideoAudioOutput_Int*)vao;
}

TWASAPIVideoAudioOutput_Int::TWASAPIVideoAudioOutput_Int() : pAudioEnumerator(0), pAudioDevice(0), pAudioClient(0), pAudioRenderClient(0), pAudioClock(0), pAudioFormat(0), pAudioVolume(0), started(0), currentVolume(1.0f), lastPts(0), posAtEndOfBuffer(0)
{
}

TWASAPIVideoAudioOutput_Int::~TWASAPIVideoAudioOutput_Int()
{
}

int TWASAPIVideoAudioOutput_Int::Initialize(int bufferLengthMs)
{
   CoInitialize(0);

   HRESULT hr;
   REFERENCE_TIME hnsRequestedDuration = 10000 * bufferLengthMs;

   hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pAudioEnumerator);
   if(FAILED(hr)) { Shutdown(); return 1; }

   hr = pAudioEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pAudioDevice);
   if(FAILED(hr)) { Shutdown(); return 2; }

   hr = pAudioDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&pAudioClient);
   if(FAILED(hr)) { Shutdown(); return 3; }

   hr = pAudioClient->GetMixFormat(&pAudioFormat);
   if(FAILED(hr)) { Shutdown(); return 4; }

   hr = pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, hnsRequestedDuration, 0, pAudioFormat, NULL);
   if(FAILED(hr)) { Shutdown(); return 5; }

   hr = pAudioClient->GetService(__uuidof(IAudioRenderClient), (void**)&pAudioRenderClient);
   if(FAILED(hr)) { Shutdown(); return 6; }

   hr = pAudioClient->GetService(__uuidof(IAudioClock), (void**)&pAudioClock);
   if(FAILED(hr)) { Shutdown(); return 7; }

	hr = pAudioClient->GetService(__uuidof(IAudioStreamVolume), (void**)&pAudioVolume);
	if(FAILED(hr)) { Shutdown(); return 7; }

	SetVolume(currentVolume);

   return 0;
}

void TWASAPIVideoAudioOutput_Int::Shutdown()
{
   Stop();
   SAFE_RELEASE(pAudioVolume);
   SAFE_RELEASE(pAudioClock);
   SAFE_RELEASE(pAudioRenderClient);
   if(pAudioFormat) { CoTaskMemFree(pAudioFormat); pAudioFormat = 0; }
   SAFE_RELEASE(pAudioClient);
   SAFE_RELEASE(pAudioDevice);
   SAFE_RELEASE(pAudioEnumerator);

   CoUninitialize();
}

void TWASAPIVideoAudioOutput_Int::Start()
{
   if(started || !pAudioClient)
      return;
   HRESULT hr = pAudioClient->Start();
   started = 1;
	lastPts = 0;
   if(FAILED(hr)) { Shutdown(); return; }
}

void TWASAPIVideoAudioOutput_Int::Stop()
{
   if(!started || !pAudioClient)
      return;
   HRESULT hr = pAudioClient->Stop();
   started = 0;
   if(FAILED(hr)) { Shutdown(); return; }
}

uint64_t TWASAPIVideoAudioOutput_Int::GetAudioPosInSamples()
{
	UINT64 audioPos = 0, audioFreq = 0;
	pAudioClock->GetPosition(&audioPos, 0);
	pAudioClock->GetFrequency(&audioFreq);
	return (audioPos * pAudioFormat->nSamplesPerSec) / audioFreq;
}

bool TWASAPIVideoAudioOutput_Int::GetTime(double &time, int &serial)
{
	size_t numFrames = audioFrames.size();
	bool isEof = false;

	if (numFrames > 0)
	{
		uint64_t audioPos = GetAudioPosInSamples();
		int lastGoodFrame = -1;

		//MDiagnostic("GetTime - audioPos: %d, frames: %d (total: %d)", audioPos, numFrames, posAtEndOfBuffer);

		// Find which frame we're in right now and calculate time based on it's timing info
		for (int i = 0; i < (int)audioFrames.size(); i++)
		{
			AudioFrame &af = audioFrames[i];
			
			// Stop if the frame is in front of the current audio position
			if (audioPos < af.startSample)
			{
				break;
			}

			// EOF frame
			if (af.startSample == 0 && af.endSample == 0)
			{
				isEof = true;
				break;
			}

			// This is a good frame
			lastGoodFrame = i;
		}
		
		// If we have a good frame, get the current time (clamped to frame duration!)
		if (lastGoodFrame != -1)
		{
			AudioFrame &af = audioFrames[lastGoodFrame];
			uint64_t delta = (audioPos < af.endSample ? audioPos : af.endSample) - af.startSample;
			time = ((double)delta / (double)pAudioFormat->nSamplesPerSec) + af.pts;
			serial = af.serial;
			//MDiagnostic("GetTime - found frame  %d (start: %d end: %d pts:%f), time %f", a, af.startSample, af.endSample, af.pts, actualTime);
		}
		
		// Remove any frames that we've passed
		if (lastGoodFrame > 0)
		{
			//MDiagnostic("GetTime - del frames %d", frameIdx);
			audioFrames.erase(audioFrames.begin(), audioFrames.begin() + lastGoodFrame);
		}
	}
	
	if (isEof)
		audioFrames.clear();

	return isEof;
}

void* TWASAPIVideoAudioOutput_Int::BeginOutput(int numSamplesRequested, double pts, int serial)
{
	// EOF frame
	if (numSamplesRequested == 0)
	{
		AudioFrame &af = audioFrames.back();
		af.startSample = af.endSample = 0;
		af.serial = 0;
		af.pts = 0;
		return 0;
	}

	// Regular frame
	BYTE* pData = 0;
    HRESULT hr = pAudioRenderClient->GetBuffer(numSamplesRequested, &pData);
	if (SUCCEEDED(hr))
	{
		audioFrames.emplace_back();
		AudioFrame &af = audioFrames.back();
		if (pts != NAN)
		{
			lastPts = af.pts = pts;
		}
		else
		{
			af.pts = lastPts;
		}

		af.serial = serial;

		return pData;
	}

	return 0;
}

void TWASAPIVideoAudioOutput_Int::EndOutput(int numSamplesWritten)
{
	AudioFrame &af = audioFrames.back();
	af.startSample = posAtEndOfBuffer;
	posAtEndOfBuffer = af.endSample = af.startSample + numSamplesWritten;
	//MDiagnostic("EndOutput - start: %d end: %d pts: %f (total %d)", af.startSample, af.endSample, af.pts, posAtEndOfBuffer);

	DWORD audioFlags = 0;
	HRESULT hr = pAudioRenderClient->ReleaseBuffer(numSamplesWritten, audioFlags);
	if(FAILED(hr)) { Shutdown(); return; }
}

EAudioSampleFormat TWASAPIVideoAudioOutput_Int::GetOutputFormat()
{
   if(pAudioFormat->wFormatTag == WAVE_FORMAT_EXTENSIBLE)
   {
      WAVEFORMATEXTENSIBLE *wfx = (WAVEFORMATEXTENSIBLE*)pAudioFormat;
      if(wfx->SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT)
      {
         return asfFLT;
      }
      else if(wfx->SubFormat == KSDATAFORMAT_SUBTYPE_PCM)
      {
         if(pAudioFormat->wBitsPerSample == 8) return asfU8;
         else if(pAudioFormat->wBitsPerSample == 16) return asfS16;
      }
   }
   else if(pAudioFormat->wFormatTag == WAVE_FORMAT_PCM)
   {
      if(pAudioFormat->wBitsPerSample == 8) return asfU8;
      else if(pAudioFormat->wBitsPerSample == 16) return asfS16;
   }
   else
   {
      MError("Bad audio format"); 
   }
   return asfNONE;
}

int TWASAPIVideoAudioOutput_Int::GetOutputChannelLayout()
{
	#define AV_CH_FRONT_LEFT             0x00000001
	#define AV_CH_FRONT_RIGHT            0x00000002
	#define AV_CH_FRONT_CENTER           0x00000004
	
	if(pAudioFormat->wFormatTag == WAVE_FORMAT_EXTENSIBLE)
   {
      WAVEFORMATEXTENSIBLE *wfx = (WAVEFORMATEXTENSIBLE*)pAudioFormat;
      return wfx->dwChannelMask;
   }
   else if(pAudioFormat->wFormatTag == WAVE_FORMAT_PCM)
   {
      switch(pAudioFormat->nChannels)
      {
			case 1: return AV_CH_FRONT_CENTER;
			case 2: return AV_CH_FRONT_LEFT | AV_CH_FRONT_RIGHT;
         default: MError("Bad channel layout in audio format\n"); return 1;
      }
   }
   else
   {
      MError("Bad audio format"); return 1;
   }
}

int TWASAPIVideoAudioOutput_Int::GetOutputSampleRate()
{
   return pAudioFormat->nSamplesPerSec;
}

void TWASAPIVideoAudioOutput_Int::SetVolume(float vol)
{
	currentVolume = vol;
	if(pAudioVolume)
	{
		float expVol = vol*vol;
		unsigned int channelCount = 0;
		pAudioVolume->GetChannelCount( &channelCount );
		for(unsigned int ch = 0; ch<channelCount; ch++)
			pAudioVolume->SetChannelVolume(ch, expVol);
	}
}

int TWASAPIVideoAudioOutput_Int::TryWaitShutdown()
{
	return 1;
}

void TWASAPIVideoAudioOutput_Int::FlushBuffers()
{
	int wasStarted = started;
	Stop();
	HRESULT hr = pAudioClient->Reset();
	posAtEndOfBuffer = 0;
	audioFrames.clear();
	if (FAILED(hr)) { Shutdown(); return; }
	if (wasStarted)
		Start();
}