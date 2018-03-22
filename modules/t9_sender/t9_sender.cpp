
#include "t9_sender.h"
#include <windows.h>
#include "T9CommonDefine.h"
#include "t9sendapi.h"


T9Sender::T9Sender() : adapters() {
	is_init = false;
	is_created = false;
	is_running = false;
	is_destroyed = false;
	screen_number = 0;
	screen_width = 0;
	screen_height = 0;
	adapter_count = 0;
}

T9Sender::~T9Sender() {
	if (is_running) {
		WARN_PRINT("Destructing sender while still running");
		stop();
	}
	if (!is_destroyed) {
		WARN_PRINT("Destructing sender with created screen");
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

	is_init = true;
	return true;
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
	send_screen_data(texture->get_data());
}

void T9Sender::send_screen_data(Ref<Image> data) {

	ERR_FAIL_COND(!is_init);
	ERR_FAIL_COND(!is_created);
	ERR_FAIL_COND(!is_running);

	auto d = data->get_data();
	// TODO: check format
	T9SendScreenPicture(screen_number, (LPBYTE)d.read().ptr());
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

	unsigned char* data = new unsigned char[screen_width*screen_height * 4];
	for (int i = 0; i < screen_width; i++) {
		for (int j = 0; j < screen_height; j++) {
			data[(i*screen_height + j) * 4 + 0] = r_;
			data[(i*screen_height + j) * 4 + 1] = g_;
			data[(i*screen_height + j) * 4 + 2] = b_;
			data[(i*screen_height + j) * 4 + 3] = 255;
		}
	}
	// TODO: check format
	T9SendScreenPicture(screen_number, (LPBYTE)data);

	delete[] data;
}

void T9Sender::destroy_screen() {
	ERR_FAIL_COND(!is_init);
	ERR_FAIL_COND(!is_created);

	T9DestroyScreen(screen_number);
	is_created = false;
	screen_number = 0;
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
}
