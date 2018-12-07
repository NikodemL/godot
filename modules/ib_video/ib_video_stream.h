/*************************************************************************/
/* IB video library for video and camera streaming                       */
/*************************************************************************/
#ifndef IB_VIDEO_STREAM_H
#define IB_VIDEO_STREAM_H

#include "core/resource.h"
#include "scene/resources/texture.h"
#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <assert.h>
#include "GL/glew.h"
#include "GL/wglew.h"

class VideoStreamIBManager;

// A texture that is accessible as read-only to OpenGL but is rendered to by DirectX
class DirectXIBVideoTexture : public Texture {
	GDCLASS(DirectXIBVideoTexture, Texture);

	RID texture;
	Image::Format format;
	int w, h;

	int id;
public:
	virtual int get_width() const override;
	virtual int get_height() const override;

	// Must lock/unlock resource before using it
	virtual RID get_rid() const override;

	virtual bool has_alpha() const override;

	virtual void set_flags(uint32_t p_flags) override;
	virtual uint32_t get_flags() const override;

	int get_video_id() const { return id; }

	DirectXIBVideoTexture();
	~DirectXIBVideoTexture();
protected:
	static void _bind_methods();
private:
	void register_shared_texture_DX(int id);

	friend class VideoStreamIBManager;
};


// Manager used to handle all video textures. We skip the "video" structure from Godot
// as it is much harder to port to the existing system.
class VideoStreamIBManager : public Reference {
	GDCLASS(VideoStreamIBManager, Reference);

	static VideoStreamIBManager* singleton;

private:
	void render(float p_delta_time);
	void init_in_render(float p_unused);
protected:
	static void _bind_methods();
public:
	VideoStreamIBManager() {
		if (singleton != NULL) {
			print_error("Creating a second instance of video stream manager!");
			return;
		}
		singleton = this;
	}

	VideoStreamIBManager* get_singleton() { return singleton; }

	// Global update (from render thread)
	void update(float p_delta_time);
	

	int create_video();
	int open_video(int id, String file_name, String options);
	int configure_video(int id, bool presentImmediately, bool useHwAccel);
	int get_video_state(int id);
	int play_video(int id, int loopCount);
	int pause_video(int id);
	int seek_video(int id, float time);
	int close_video(int id);
	Size2 get_video_info_size(int id);
	float get_video_duration(int id);
	Ref<DirectXIBVideoTexture> get_video_texture(int id);

	void init();
	bool is_init();
	void set_log_level(int level);
	void release();

};

#endif
