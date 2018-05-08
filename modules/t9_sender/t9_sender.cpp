#include "t9_sender.h"
#include <windows.h>
#include "T9CommonDefine.h"
#include "t9sendapi.h"
#include <glad/glad.h>

T9Sender::T9Sender() : adapters() {
	is_init = false;
	is_created = false;
	is_running = false;
	is_destroyed = false;
	screen_number = 0;
	screen_width = 0;
	screen_height = 0;
	adapter_count = 0;
	buffer_data = NULL;

	pbo_current_index = 0;
}

T9Sender::~T9Sender() {
	if (!is_init)
		return;

	if (is_running) {
		stop();
	}
	if (!is_destroyed) {
		destroy_screen();
	}
}


bool T9Sender::init() {
	ERR_FAIL_COND_V(is_init, false);

	// First get number of adapters to local variable
	DWORD adp_count;
	HRESULT res = T9GetNetAdapterCount(&adp_count);

	if (FAILED(res)) {
		ERR_PRINT("T9: Failed to get net adapter count");
		return false;
	}

	// Filter all adapters and save them to local array. adapter_count
	// only counts succesfully added adapters
	adapter_count = 0;
	for (int i = 0; i < adp_count && adapter_count < 6; i++) {

		HRESULT hr = ::T9GetNetAdapterInfo(i,
			adapters[adapter_count].name,
			sizeof(adapters[adapter_count].name),
			adapters[adapter_count].desc,
			sizeof(adapters[adapter_count].desc));

		// We do not allow wireless adapters (from example)
		if (SUCCEEDED(hr))
		{
			// TODO: might need to filter wireless adapters out and split on " - "
			adapter_count++;
		}
	}

	VS::get_singleton()->request_frame_drawn_callback(this, "init_in_render", Variant(0.0f));

	is_init = true;
	return true;
}

void T9Sender::init_in_render(float unsued) {
	glGenBuffers(2, pbo_objects);

	glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo_objects[0]);
	glBufferData(GL_PIXEL_PACK_BUFFER, 256 * 256 * 4, 0, GL_STREAM_READ);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo_objects[1]);
	glBufferData(GL_PIXEL_PACK_BUFFER, 256 * 256 * 4, 0, GL_STREAM_READ);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
}

int T9Sender::get_adapter_count() {
	ERR_FAIL_COND_V(!is_init, 0);
	return adapter_count;
}

String T9Sender::get_adapter_name(int id) {
	ERR_FAIL_COND_V(!is_init, "");
	ERR_FAIL_COND_V(id >= 0 && id < adapter_count, "");

	return String(adapters[id].name);
}

String T9Sender::get_adapter_desc(int id) {
	ERR_FAIL_COND_V(!is_init, "");
	ERR_FAIL_COND_V(id >= 0 && id < adapter_count, "");

	return String(adapters[id].name);
}

void T9Sender::set_brightness(int bright, int r, int g, int b, int vr, int cold) {
	ERR_FAIL_COND(!is_init);

	T9SetBirghtness(bright, r, g, b, vr, cold);
}

void T9Sender::create_screen(int p_screen_number, int p_screen_width, int p_screen_height, bool p_is_virtual) {
	ERR_FAIL_COND(!is_init);
	ERR_FAIL_COND(is_created);

	HRESULT hr = T9CreateScreen(p_screen_number,
		p_screen_width,
		p_screen_height,
		p_is_virtual,
		vpa_RA_G__B_RB,
		cs_right_to_left);

	if (FAILED(hr)) {
		ERR_PRINT("T9: Failed to create screen");
		return;
	}

	screen_width = p_screen_width;
	screen_height = p_screen_height;
	screen_number = p_screen_number;

	buffer_data = new unsigned char[screen_width * screen_height * 4];
	is_created = true;
}

void T9Sender::start(int adapter_id) {
	ERR_FAIL_COND(!is_init);
	ERR_FAIL_COND(!is_created);
	ERR_FAIL_COND(is_running);
	ERR_FAIL_COND(adapter_id < 0 || adapter_id >= adapter_count);

	HRESULT hr = T9SenderStart(COMUNICATION_TYPE_NET, adapters[adapter_id].name);
	if (FAILED(hr)) {
		ERR_PRINTS("T9: Failed to start sender adapter " + itos(adapter_id));
		return;
	}

	is_running = true;
}

void T9Sender::stop() {
	ERR_FAIL_COND(!is_init);
	ERR_FAIL_COND(!is_running);

	HRESULT hr = T9SenderStop();
	if (FAILED(hr)) {
		ERR_PRINT("T9: Failed to stop sender adapter");
		return;
	}

	is_running = false;
}

void T9Sender::send_screen(Ref<ViewportTexture> texture) {
	Ref<Image> data = texture->get_data();
	if (data.is_valid()) {
		send_screen_data(data);
	}
	else {
		ERR_PRINT("Failed to send screen, invalid data");
	}
}

void flip_image(unsigned char* src_data, unsigned char* dst_data, int screen_width, int screen_height) {
	for (int i = 0; i < screen_width; i++) {
		for (int j = 0; j < screen_height; j++) {
			int row_dst_idx = (screen_height - i - 1) * screen_height;
			int row_src_idx = i * screen_height;

			dst_data[(row_dst_idx + j) * 4 + 0] = src_data[(row_src_idx + j) * 4 + 2];
			dst_data[(row_dst_idx + j) * 4 + 1] = src_data[(row_src_idx + j) * 4 + 1];
			dst_data[(row_dst_idx + j) * 4 + 2] = src_data[(row_src_idx + j) * 4 + 0];
			dst_data[(row_dst_idx + j) * 4 + 3] = src_data[(row_src_idx + j) * 4 + 3];
		}
	}
}

void T9Sender::send_screen_data(Ref<Image> data) {

	ERR_FAIL_COND(!is_init);
	ERR_FAIL_COND(!is_created);
	ERR_FAIL_COND(!is_running);

	auto d = data->get_data();

	// We need to reorder data to correct format
	auto read = d.read();
	unsigned char* src_data = (unsigned char*)read.ptr();
	unsigned char* dst_data = buffer_data;

	flip_image(src_data, dst_data, screen_width, screen_height);

	T9SendScreenPicture(screen_number, (LPBYTE)dst_data);
}

void checkError()
{
	GLenum error;
	while (true) {
		error = glGetError();

		if (error != GL_NO_ERROR) {
			ERR_PRINT("OpenGL error");
			continue;
		}
		break;
	}
}

void T9Sender::send_front_framebuffer()
{
	VS::get_singleton()->request_frame_drawn_callback(this, "send_front_framebuffer_in_render", Variant(0.0f));
}

void T9Sender::send_front_framebuffer_in_render(float unsued)
{
	// "index" is used to read pixels from framebuffer to a PBO
	// "nextIndex" is used to update pixels in the other PBO
	pbo_current_index = (pbo_current_index + 1) % 2;
	int next_index = (pbo_current_index + 1) % 2;

	checkError();

	// set the target framebuffer to read
	glReadBuffer(GL_FRONT);

	checkError();

	// read pixels from framebuffer to PBO
	// glReadPixels() should return immediately.
	glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo_objects[pbo_current_index]);
	checkError();

	glReadPixels(0, 0, screen_width, screen_width, GL_BGRA, GL_UNSIGNED_BYTE, 0);
	checkError();
	// map the PBO to process its data by CPU
	glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo_objects[next_index]);
	checkError();
	GLubyte* ptr = (GLubyte*)glMapBuffer(GL_PIXEL_PACK_BUFFER,
		GL_READ_ONLY);
	checkError();
	if (ptr)
	{
		// T9 expects top-down row format, we have it down-top
		flip_image(ptr, buffer_data, screen_width, screen_height);

		T9SendScreenPicture(screen_number, buffer_data);
		checkError();
		glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
		checkError();
	}


	// back to conventional pixel operation
	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
	checkError();
}

float clamp(float x) {
	x = x > 1 ? 1 : x;
	x = x < 0 ? 0 : x;
	return x;
}

void T9Sender::send_screen_color(float r, float g, float b) {
	ERR_FAIL_COND(!is_init);
	ERR_FAIL_COND(!is_created);
	ERR_FAIL_COND(!is_running);

	unsigned char r_ = (char)(clamp(r) * 255.0f);
	unsigned char g_ = (char)(clamp(g) * 255.0f);
	unsigned char b_ = (char)(clamp(b) * 255.0f);

	unsigned char* data = buffer_data;
	for (int i = 0; i < screen_width; i++) {
		for (int j = 0; j < screen_height; j++) {
			int row_idx = i * screen_height;
			data[(row_idx + j) * 4 + 0] = b_;
			data[(row_idx + j) * 4 + 1] = g_;
			data[(row_idx + j) * 4 + 2] = r_;
			data[(row_idx + j) * 4 + 3] = 255;
		}
	}
	// TODO: check format
	T9SendScreenPicture(screen_number, (LPBYTE)buffer_data);
}

void T9Sender::destroy_screen() {
	ERR_FAIL_COND(!is_init);
	ERR_FAIL_COND(!is_created);

	T9DestroyScreen(screen_number);
	is_created = false;
	delete[] buffer_data;
	screen_number = 0;

	glDeleteBuffers(2, pbo_objects);
}

void T9Sender::_bind_methods() {
	ClassDB::bind_method(D_METHOD("init"), &T9Sender::init);
	ClassDB::bind_method(D_METHOD("get_adapter_count"), &T9Sender::get_adapter_count);
	ClassDB::bind_method(D_METHOD("get_adapter_name", "id"), &T9Sender::get_adapter_name);
	ClassDB::bind_method(D_METHOD("get_adapter_desc", "id"), &T9Sender::get_adapter_desc);
	ClassDB::bind_method(D_METHOD("set_brightness", "bright", "r", "g", "b", "vr", "color"), &T9Sender::set_brightness);
	ClassDB::bind_method(D_METHOD("create_screen", "screen_number", "width", "height", "virtual"), &T9Sender::create_screen);
	ClassDB::bind_method(D_METHOD("start", "adapter_id"), &T9Sender::start);
	ClassDB::bind_method(D_METHOD("stop"), &T9Sender::stop);
	ClassDB::bind_method(D_METHOD("send_screen", "texture"), &T9Sender::send_screen);
	ClassDB::bind_method(D_METHOD("send_screen_data", "data"), &T9Sender::send_screen_data);
	ClassDB::bind_method(D_METHOD("send_screen_color", "r", "g", "b"), &T9Sender::send_screen_color);
	ClassDB::bind_method(D_METHOD("send_front_framebuffer"), &T9Sender::send_front_framebuffer);

	ClassDB::bind_method(D_METHOD("send_front_framebuffer_in_render", "unused"), &T9Sender::send_front_framebuffer_in_render);
	ClassDB::bind_method(D_METHOD("init_in_render", "unused"), &T9Sender::init_in_render);
}
