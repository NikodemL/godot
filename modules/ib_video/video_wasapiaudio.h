//////////////////////////////////////////////////////////////////////////////////
// WASAPI based audio output (windows)
//////////////////////////////////////////////////////////////////////////////////
class TWASAPIVideoAudioOutput : public TVideoAudioOutput {
public:
	static TVideoAudioOutput* Create();
	static void Destroy(TVideoAudioOutput* vo);
};