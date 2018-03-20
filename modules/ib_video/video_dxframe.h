//////////////////////////////////////////////////////////////////////////////////
// DX11 video frame output (windows)
//////////////////////////////////////////////////////////////////////////////////
class TDXVideoFrameOutput : public TVideoFrameOutput {
public:
	static TVideoFrameOutput* Create(TVideoObject* vo, IDirect3DDevice9* dev9, ID3D11Device* dev11);
	static void Destroy(TVideoFrameOutput* vo);
public:
	virtual void* GetFrame()=0;
	virtual void* GetRenderTexHandle() = 0;
};
