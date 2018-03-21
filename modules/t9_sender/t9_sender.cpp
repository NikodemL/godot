#include "t9sendapi.h"
#include "t9_sender.h"


T9Sender::T9Sender() : adapters(0) {
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
	ERR_FAIL_COND(is_init);

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

		hr = ::T9GetNetAdapterInfo(i,
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
}


int T9Sender::get_adapter_count() {
	ERR_FAIL_COND(!is_init);
	return adapter_count;
}

String T9Sender::get_adapter_name(int id) {
	ERR_FAIL_COND(!is_init);
	ERR_FAIL_COND(id >= 0 && is < adapter_count);

	return String(adapters[id].name);
}

String T9Sender::get_adapter_desc(int id) {
	ERR_FAIL_COND(!is_init);
	ERR_FAIL_COND(id >= 0 && is < adapter_count);

	return String(adapters[id].name);
}

void T9Sender::set_brightness(int bright, int r, int g, int b, int cold) {
	ERR_FAIL_COND(!is_init);

	T9SetBirghtness(bright, r, g, b, cold);
}

void T9Sender::create_screen(int p_screen_number, int p_screen_width, int p_screen_height, bool p_is_virtual) {
	ERR_FAIL_COND(!is_init);
	ERR_FAIL_COND(is_created);

	HRESULT hr = T9CreateScreen(p_screen_number,
		p_screen_width,
		p_screen_height,
		p_is_virtual,
		vpa_RA_G__B_RB,
		m_cableDir);

	if (FAILED(hr)) {
		ERR_PRINT("T9: Failed to create screen");
		return;
	}

	screen_width = p_screen_width;
	screen_height = p_screen_height;
	p_screen_number = p_screen_number;
	is_created = true;
	
}

void T9Sender::start(int adapter_id) {
	ERR_FAIL_COND(!is_init);
	ERR_FAIL_COND(!is_created);
	ERR_FAIL_COND(is_running);
	ERR_FAIL_COND(id >= 0 && is < adapter_count);

	HRESULT hr = T9SenderStart(COMUNICATION_TYPE_NET, adapters[adapter_id].name);
	if (FAILED(hr)) {
		ERR_PRINTS("T9: Failed to start sender adapter " + String(adapter_id));
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
	send_screen(texture.get_data());
}

void T9Sender::send_screen(Ref<Image> data) {


	T9SendScreenPicture(screen_number, data->)
}

void T9Sender::destroy_screen() {
	ERR_FAIL_COND(!is_init);
	ERR_FAIL_COND(!is_created);

	T9DestroyScreen(screen_number);
	is_created = false;
	screen_number = 0;
}
