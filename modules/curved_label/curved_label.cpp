#include "curved_label.h"
#include "project_settings.h"
#include "scene/gui/control.h"
#include "translation.h"


void CurvedLabel::_notification(int p_what) {

	RID ci = get_canvas_item();
	Ref<Font> font = get_font("font");
	Color font_color = get_color("font_color");
	Size2 size = get_size();

	if (p_what == NOTIFICATION_TRANSLATION_CHANGED) {
		if (!update_translation())
			return;
		update();
	}

	if (p_what == NOTIFICATION_DRAW) {

		VisualServer::get_singleton()->canvas_item_set_distance_field_mode(get_canvas_item(), font.is_valid() && font->is_distance_field_hint());

		int text_size_y = font->get_height();

		if (text_size_y < 1) {
			text_size_y = 1;
		}

		float text_size_x = 0;
		for (int i = 0; i < text.length(); i++) {
			CharType c = text[i];
			int char_width = font->get_char_size(c).width;
			text_size_x += char_width;
		}

		if (text_size_x < 1)
			text_size_x = 1;

		float expand_scale = 1.0f;

		if (expand) {
			expand_scale = fminf(size.x / text_size_x, size.y / text_size_y);

			// Maximum font size so we don't overscale text
			if (text_size_y * expand_scale > max_font_size) {
				expand_scale = (float)max_font_size / text_size_y;
			}
		}

		int font_h_expand = font->get_height() * expand_scale;
		int space_w_expand = font->get_char_size(' ').width * expand_scale;
		int lines_visible = size.y / font_h_expand;

		float rotation = 0;
		for (int i = 0; i < text.length(); i++) {
			CharType c = text[i];
			CharType n = text[i + 1];

			float t = (rotation - rotoffset) * 4.0 * atan(1.0) / 180.0;
			draw_set_transform(Vector2(get_rect().size.x / 2, get_rect().size.y / 2), t, Vector2(1, 1));

			float char_w;
			if (mirroredtext) {
				char_w = -font->draw_char(ci, Vector2(0, radius), c, n, font_color, Point2(expand_scale, 1));
			}
			else {
				char_w = font->draw_char(ci, Vector2(0, -radius), c, n, font_color, Point2(expand_scale, 1));
			}
			float char_angle = 2 * atan(char_w / (2 * radius)) * 180. / 3.1415 * space;
			rotation += char_angle;
		}
	}
}

bool CurvedLabel::update_translation() {
	String new_text;
	if (loc_label == "") {
		new_text = tr(text);
		if (new_text == text)
			return false; //nothing new
		text = new_text;
	}
	else {
		// If in editor or localization disabled
		if (!can_translate_messages() || !TranslationServer::get_singleton() || !TranslationServer::get_singleton()->is_enabled()) {
			new_text = text;
		}
		else {
			new_text = tr(loc_label);
			if (new_text == loc_label) {
				new_text = loc_label + " not found";
			}
		}
		if (new_text == text)
			return false;
		text = new_text;
	}
	return true;
}

void CurvedLabel::set_loc_label(String p_loc_text) {
	if (p_loc_text.find(" ", 0) >= 0) {
		WARN_PRINT("Localization label typically does not contain spaces");
	}

	loc_label = p_loc_text;
	if (!update_translation())
		return;
	update();
}

String CurvedLabel::get_loc_label() const {
	return loc_label;
}

void CurvedLabel::set_curved_text(const String &p_string) {

	if (text == p_string)
		return;
	text = p_string;
	update();
}

String CurvedLabel::get_curved_text() const {
	return text;
}

void CurvedLabel::set_radius(int p_radius) {
	radius = p_radius;
	update();
}

int CurvedLabel::get_radius() const {
	return radius;
}

void CurvedLabel::set_space(float p_spacing) {
	space = p_spacing;
	update();
}

float CurvedLabel::get_space() const {
	return space;
}

void CurvedLabel::set_rotation_offset(float p_offset) {
	rotoffset = p_offset;
	update();
}

float CurvedLabel::get_rotation_offset() const {
	return rotoffset;
}

void CurvedLabel::set_mirrored_text(bool p_mirrored) {
	mirroredtext = p_mirrored;
	update();
}

bool CurvedLabel::get_mirrored_text() const {
	return mirroredtext;
}

bool CurvedLabel::has_expand() const {
	return expand;
}

void CurvedLabel::set_expand(bool p_expand) {
	expand = p_expand;
	update();
}

int CurvedLabel::get_max_font_size() const {
	return max_font_size;
}

void CurvedLabel::set_max_font_size(int p_max_font_size) {
	max_font_size = p_max_font_size;
	update();
}

void CurvedLabel::_bind_methods() {

	ClassDB::bind_method(D_METHOD("set_curved_text", "text"), &CurvedLabel::set_curved_text);
	ClassDB::bind_method(D_METHOD("get_curved_text"), &CurvedLabel::get_curved_text);
	ClassDB::bind_method(D_METHOD("set_radius", "radius"), &CurvedLabel::set_radius);
	ClassDB::bind_method(D_METHOD("get_radius"), &CurvedLabel::get_radius);
	ClassDB::bind_method(D_METHOD("set_space", "space"), &CurvedLabel::set_space);
	ClassDB::bind_method(D_METHOD("get_space"), &CurvedLabel::get_space);
	ClassDB::bind_method(D_METHOD("set_rotation_offset", "rotoffset"), &CurvedLabel::set_rotation_offset);
	ClassDB::bind_method(D_METHOD("get_rotation_offset"), &CurvedLabel::get_rotation_offset);
	ClassDB::bind_method(D_METHOD("set_mirrored_text", "mirroredtext"), &CurvedLabel::set_mirrored_text);
	ClassDB::bind_method(D_METHOD("get_mirrored_text"), &CurvedLabel::get_mirrored_text);
	ClassDB::bind_method(D_METHOD("has_expand"), &CurvedLabel::has_expand);
	ClassDB::bind_method(D_METHOD("set_expand", "expand"), &CurvedLabel::set_expand);
	ClassDB::bind_method(D_METHOD("get_max_font_size"), &CurvedLabel::get_max_font_size);
	ClassDB::bind_method(D_METHOD("set_max_font_size", "max_size"), &CurvedLabel::set_max_font_size);
	ClassDB::bind_method(D_METHOD("get_loc_label"), &CurvedLabel::get_loc_label);
	ClassDB::bind_method(D_METHOD("set_loc_label", "loc_label"), &CurvedLabel::set_loc_label);

	ADD_PROPERTYNZ(PropertyInfo(Variant::STRING, "text", PROPERTY_HINT_MULTILINE_TEXT, "", PROPERTY_USAGE_DEFAULT_INTL), "set_curved_text", "get_curved_text");
	ADD_PROPERTYNZ(PropertyInfo(Variant::INT, "radius"), "set_radius", "get_radius");
	ADD_PROPERTYNZ(PropertyInfo(Variant::REAL, "space"), "set_space", "get_space");
	ADD_PROPERTYNZ(PropertyInfo(Variant::REAL, "rotoffset"), "set_rotation_offset", "get_rotation_offset");
	ADD_PROPERTYNZ(PropertyInfo(Variant::BOOL, "mirroredText"), "set_mirrored_text", "get_mirrored_text");
	ADD_PROPERTYNZ(PropertyInfo(Variant::BOOL, "expand"), "set_expand", "has_expand");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "max_font_size", PROPERTY_HINT_RANGE, "0,1000"), "set_max_font_size", "get_max_font_size");
	ADD_PROPERTYNZ(PropertyInfo(Variant::STRING, "loc_label"), "set_loc_label", "get_loc_label");
}

CurvedLabel::CurvedLabel(const String &p_text) {

	text = "";
	radius = 500;
	space = 1;
	rotoffset = 0;
	mirroredtext = false;
	loc_label = "";
	expand = false;
	max_font_size = 3000;
}

