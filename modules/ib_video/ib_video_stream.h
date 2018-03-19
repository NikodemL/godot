/*************************************************************************/
/* IB video library for video and camera streaming                       */
/*************************************************************************/

#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <assert.h>
#include "glew.h"
#include "wglew.h"

#ifndef IB_VIDEO_STREAM_H
#define IB_VIDEO_STREAM_H

class VideoStreamPlaybackIB;

// A texture that is accessible as read-only to OpenGL but is rendered to by DirectX
class DirectXIBVideoTexture : public Texture {
	GDCLass(DirectXIBVideoTexture);

	RID texture;
	Image::Format format;
	int w, h;

	HANDLE dx_surface_handle;
	HANDLE gl_texture_handle;

public:
	virtual int get_width() const override;
	virtual int get_height() const override;
	virtual RID get_rid() const override;

	virtual bool has_alpha() const override;

	virtual void set_flags(uint32_t p_flags) override;
	virtual uint32_t get_flags() const override;


	DirectXIBVideoTexture();
	~DirectXIBVideoTexture();

private:
	void register_shared_texture_DX(LPDIRECT3DDEVICE9EX p_dx_device, HANDLE p_shared_dx_texture);

	friend VideoStreamPlaybackIB;
};


class VideoStreamPlaybackIB : public VideoStreamPlayback {

	GDCLASS(VideoStreamPlaybackIB, VideoStreamPlayback);

	Ref<DirectXIBVideoTexture> texture;

protected:
	void clear();

public:
	virtual void play();
	virtual void stop();
	virtual bool is_playing() const;

	virtual void set_paused(bool p_paused);
	virtual bool is_paused() const;

	virtual void set_loop(bool p_enable);
	virtual bool has_loop() const;

	virtual float get_length() const;

	virtual String get_stream_name() const;

	virtual int get_loop_count() const;

	virtual float get_playback_position() const;
	virtual void seek(float p_time);

	void set_url(const String &p_url);

	virtual Ref<Texture> get_texture();
	virtual void update(float p_delta);

	virtual void set_mix_callback(AudioMixCallback p_callback, void *p_userdata);
	virtual int get_channels() const;
	virtual int get_mix_rate() const;

	virtual void set_audio_track(int p_idx);

	VideoStreamPlaybackIB();
	~VideoStreamPlaybackIB();
};

class VideoStreamIB : public VideoStream {

	GDCLASS(VideoStreamIB, VideoStream);

	String url;
	int audio_track;

protected:
	static void _bind_methods();

public:
	Ref<VideoStreamPlayback> instance_playback() {
		Ref<VideoStreamPlaybackIB> pb = memnew(VideoStreamPlaybackIB);
		pb->set_audio_track(audio_track);
		pb->set_url(url);
		return pb;
	}

	void set_url(const String &p_file) { url = p_file; }
	String get_url() { return url; }
	void set_audio_track(int p_track) { audio_track = p_track; }

	VideoStreamIB() { audio_track = 0; }
};


class VideoStreamIBManager {

	static VideoStreamIBManager* singleton;

	LPDIRECT3D9EX d3d;
	LPDIRECT3DDEVICE9EX d9device;
	HWND hWndDX;

public:
	VideoStreamIBManager() {
		singleton = this;
	}

	VideoStreamIBManager* get_singleton{ return singleton; }

	// Global update 
	void update();

	// Will initiali
	void init();
	void release();

};

#endif
