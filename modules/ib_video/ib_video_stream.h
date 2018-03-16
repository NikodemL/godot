/*************************************************************************/
/* IB video library for video and camera streaming                       */
/*************************************************************************/


#ifndef IB_VIDEO_STREAM_H
#define IB_VIDEO_STREAM_H

// A texture that is accessible as read-only to OpenGL but is rendered to by DirectX
class DirectXIBVideoTexture : public Texture {
	GDCLass(DirectXIBVideoTexture);

public:
	virtual int get_width() const = 0;
	virtual int get_height() const = 0;
	virtual Size2 get_size() const;
	virtual RID get_rid() const = 0;

	virtual bool has_alpha() const = 0;

	virtual void set_flags(uint32_t p_flags) = 0;
	virtual uint32_t get_flags() const = 0;

	virtual void draw(RID p_canvas_item, const Point2 &p_pos, const Color &p_modulate = Color(1, 1, 1), bool p_transpose = false, const Ref<Texture> &p_normal_map = Ref<Texture>()) const;
	virtual void draw_rect(RID p_canvas_item, const Rect2 &p_rect, bool p_tile = false, const Color &p_modulate = Color(1, 1, 1), bool p_transpose = false, const Ref<Texture> &p_normal_map = Ref<Texture>()) const;
	virtual void draw_rect_region(RID p_canvas_item, const Rect2 &p_rect, const Rect2 &p_src_rect, const Color &p_modulate = Color(1, 1, 1), bool p_transpose = false, const Ref<Texture> &p_normal_map = Ref<Texture>(), bool p_clip_uv = true) const;
	virtual bool get_rect_region(const Rect2 &p_rect, const Rect2 &p_src_rect, Rect2 &r_rect, Rect2 &r_src_rect) const;

	virtual Ref<Image> get_data() const { return Ref<Image>(); }

	Texture();
};


class VideoStreamPlaybackIB : public VideoStreamPlayback {

	GDCLASS(VideoStreamPlaybackIB, VideoStreamPlayback);

	Ref<ImageTexture> texture;

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

#endif
