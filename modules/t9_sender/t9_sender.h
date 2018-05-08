// This is T9 binder for Godot

#ifndef __T9__SENDER__H_
#define __T9__SENDER__H_

#include "reference.h"
#include "scene/main/node.h"
#include "scene/main/viewport.h"

class T9Sender : public Node {
	GDCLASS(T9Sender, Node);

	bool is_init;
	bool is_created;
	bool is_running;
	bool is_destroyed;
	int screen_number;
	int screen_width;
	int screen_height;

	struct AdapterDesc {
		char name[200];
		char desc[200];
	};

	int adapter_count;
	AdapterDesc adapters[6];

	unsigned char* buffer_data;

	// For fast framebuffer transfer
	unsigned int pbo_objects[2];
	int pbo_current_index;
protected:
	static void _bind_methods();

public:
	T9Sender();
	virtual ~T9Sender();

	// Initializes the managed, must be called before anything
	bool init();

	// Obtains number of adapters for sender; you can only choose one
	// adapter to send data. Adapter ids are 0, 1, ... get_adapter_count()-1
	int get_adapter_count();

	// Get the name of adapter/desc
	String get_adapter_name(int id);
	String get_adapter_desc(int id);

	// Sets the brigtness of display
	void set_brightness(int bright = 255, int r = 255, int g = 255, int b = 255, int vr = 255, int cold = 0);

	// Creates a sender data to a screen number. 1 is default screen number
	void create_screen(int p_screen_number, int p_screen_width, int p_screen_height, bool p_is_virtual);

	// Starts/end streaming to adapted id. Should be called after create_screen to start streaming it
	void start(int adapter_id);
	void stop();

	// Should be called on interval to send contents of viewport texture after screen is created
	void send_screen(Ref<ViewportTexture> texture);
	void send_screen_data(Ref<Image> data);
	void send_screen_color(float r, float g, float b);

	void init_in_render(float unused);
	void send_front_framebuffer_in_render(float unused);

	// Optimized send framebuffer methods
	void send_front_framebuffer();

	// Destroys 
	void destroy_screen();
};



#endif
