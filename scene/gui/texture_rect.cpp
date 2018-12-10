/*************************************************************************/
/*  texture_rect.cpp                                                     */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2018 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2018 Godot Engine contributors (cf. AUTHORS.md)    */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#include "texture_rect.h"
#include "servers/visual_server.h"
#include "scene/scene_string_names.h"

void TextureRect::_notification(int p_what) {

	if (p_what == NOTIFICATION_DRAW) {

		if (texture.is_null())
			return;

		Rect2 src_rect = Rect2();
		src_rect.size = texture->get_size() / Size2(hframes, vframes);
		src_rect.position.x = float(frame % hframes) * src_rect.size.x;
		src_rect.position.y = float(frame / hframes) * src_rect.size.y;

		if (hflip)
			src_rect.size.x = -src_rect.size.x;
		if (vflip)
			src_rect.size.y = -src_rect.size.y;

		switch (stretch_mode) {
			case STRETCH_SCALE_ON_EXPAND: {

				Size2 s = expand ? get_size() : texture->get_size() / Size2(hframes, vframes);
				draw_texture_rect_region(texture, Rect2(Point2(), s), src_rect);
			} break;
			case STRETCH_SCALE: {
				draw_texture_rect_region(texture, Rect2(Point2(), get_size()), src_rect);
			} break;
			case STRETCH_TILE: {
				draw_texture_rect(texture, Rect2(Point2(), get_size()), true);
			} break;
			case STRETCH_KEEP: {

				Size2 s = texture->get_size() / Size2(hframes, vframes);
				draw_texture_rect_region(texture, Rect2(Point2(), s), src_rect);
			} break;
			case STRETCH_KEEP_CENTERED: {

				Size2 ofs = (get_size() - (texture->get_size() / Size2(hframes, vframes))) / 2;
				Size2 s = texture->get_size() / Size2(hframes, vframes);

				draw_texture_rect_region(texture, Rect2(ofs, s), src_rect);
			} break;
			case STRETCH_KEEP_ASPECT_CENTERED:
			case STRETCH_KEEP_ASPECT: {

				Size2 size = get_size();
				Size2 tex_size = texture->get_size() / Size2(hframes, vframes); 

				int tex_width = tex_size.x * size.height / tex_size.y;
				int tex_height = size.height;

				if (tex_width > size.width) {
					tex_width = size.width;
					tex_height = tex_size.y * tex_width / tex_size.x;
				}

				int ofs_x = 0;
				int ofs_y = 0;

				if (stretch_mode == STRETCH_KEEP_ASPECT_CENTERED) {
					ofs_x += (size.width - tex_width) / 2;
					ofs_y += (size.height - tex_height) / 2;
				}

				draw_texture_rect_region(texture, Rect2(ofs_x, ofs_y, tex_width, tex_height), src_rect);
			} break;
			case STRETCH_KEEP_ASPECT_COVERED: {

				Size2 size = get_size();
				Size2 tex_size = texture->get_size() / Size2(hframes, vframes);

				Size2 scaleSize(size.width / tex_size.width, size.height / tex_size.height);
				float scale = scaleSize.width > scaleSize.height ? scaleSize.width : scaleSize.height;
				Size2 scaledTexSize = tex_size * scale;
				Point2 ofs = ((scaledTexSize - size) / scale).abs() / 2.0f;

				Rect2 rect = Rect2(ofs, size / scale);

				if (hflip)
					rect.size.x = -rect.size.x;
				if (vflip)
					rect.size.y = -rect.size.y;

				draw_texture_rect_region(texture, Rect2(Point2(), size), rect);
			} break;
		}
	}
}

Size2 TextureRect::get_minimum_size() const {

	if (!expand && !texture.is_null())
		return texture->get_size() / Size2(hframes, vframes);
	else
		return Size2();
}
void TextureRect::_bind_methods() {

	ClassDB::bind_method(D_METHOD("set_texture", "texture"), &TextureRect::set_texture);
	ClassDB::bind_method(D_METHOD("get_texture"), &TextureRect::get_texture);

	ClassDB::bind_method(D_METHOD("set_expand", "enable"), &TextureRect::set_expand);
	ClassDB::bind_method(D_METHOD("has_expand"), &TextureRect::has_expand);

	ClassDB::bind_method(D_METHOD("set_stretch_mode", "stretch_mode"), &TextureRect::set_stretch_mode);
	ClassDB::bind_method(D_METHOD("get_stretch_mode"), &TextureRect::get_stretch_mode);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "texture", PROPERTY_HINT_RESOURCE_TYPE, "Texture"), "set_texture", "get_texture");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "expand"), "set_expand", "has_expand");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "stretch_mode", PROPERTY_HINT_ENUM, "Scale On Expand (Compat),Scale,Tile,Keep,Keep Centered,Keep Aspect,Keep Aspect Centered,Keep Aspect Covered"), "set_stretch_mode", "get_stretch_mode");
	ClassDB::bind_method(D_METHOD("set_vflip", "enable"), &TextureRect::set_vflip);
	ClassDB::bind_method(D_METHOD("has_vflip"), &TextureRect::has_vflip);

	ClassDB::bind_method(D_METHOD("set_hflip", "enable"), &TextureRect::set_hflip);
	ClassDB::bind_method(D_METHOD("has_hflip"), &TextureRect::has_hflip);

	ClassDB::bind_method(D_METHOD("set_frame", "frame"), &TextureRect::set_frame);
	ClassDB::bind_method(D_METHOD("get_frame"), &TextureRect::get_frame);

	ClassDB::bind_method(D_METHOD("set_vframes", "vframes"), &TextureRect::set_vframes);
	ClassDB::bind_method(D_METHOD("get_vframes"), &TextureRect::get_vframes);

	ClassDB::bind_method(D_METHOD("set_hframes", "hframes"), &TextureRect::set_hframes);
	ClassDB::bind_method(D_METHOD("get_hframes"), &TextureRect::get_hframes);
	
	ADD_GROUP("Flip", "");
   ADD_PROPERTY(PropertyInfo(Variant::BOOL, "vflip"), "set_vflip", "has_vflip");
   ADD_PROPERTY(PropertyInfo(Variant::BOOL, "hflip"), "set_hflip", "has_hflip");

	ADD_GROUP("Animation", "");
   ADD_PROPERTY(PropertyInfo(Variant::INT, "vframes", PROPERTY_HINT_RANGE, "1,16384,1"), "set_vframes", "get_vframes");
   ADD_PROPERTY(PropertyInfo(Variant::INT, "hframes", PROPERTY_HINT_RANGE, "1,16384,1"), "set_hframes", "get_hframes");
   ADD_PROPERTY(PropertyInfo(Variant::INT, "frame", PROPERTY_HINT_SPRITE_FRAME), "set_frame", "get_frame");

	BIND_ENUM_CONSTANT(STRETCH_SCALE_ON_EXPAND);
	BIND_ENUM_CONSTANT(STRETCH_SCALE);
	BIND_ENUM_CONSTANT(STRETCH_TILE);
	BIND_ENUM_CONSTANT(STRETCH_KEEP);
	BIND_ENUM_CONSTANT(STRETCH_KEEP_CENTERED);
	BIND_ENUM_CONSTANT(STRETCH_KEEP_ASPECT);
	BIND_ENUM_CONSTANT(STRETCH_KEEP_ASPECT_CENTERED);
	BIND_ENUM_CONSTANT(STRETCH_KEEP_ASPECT_COVERED);
}

void TextureRect::set_texture(const Ref<Texture> &p_tex) {

	texture = p_tex;
	update();
	/*
	if (texture.is_valid())
		texture->set_flags(texture->get_flags()&(~Texture::FLAG_REPEAT)); //remove repeat from texture, it looks bad in sprites
	*/
	minimum_size_changed();
}

Ref<Texture> TextureRect::get_texture() const {

	return texture;
}

void TextureRect::set_expand(bool p_expand) {

	expand = p_expand;
	update();
	minimum_size_changed();
}

bool TextureRect::has_expand() const {

	return expand;
}

void TextureRect::set_stretch_mode(StretchMode p_mode) {

	stretch_mode = p_mode;
	update();
}

TextureRect::StretchMode TextureRect::get_stretch_mode() const {

	return stretch_mode;
}

void TextureRect::set_vflip(bool p_vflip) {

	vflip = p_vflip;
	update();
}

bool TextureRect::has_vflip() const {

	return vflip;
}

void TextureRect::set_hflip(bool p_hflip) {

	hflip = p_hflip;
	update();
}


bool TextureRect::has_hflip() const {

	return hflip;
}

void TextureRect::set_frame(int p_frame) {

	ERR_FAIL_INDEX(p_frame, vframes * hframes);

	if (frame != p_frame)
		item_rect_changed();

	frame = p_frame;

	_change_notify("frame");
	emit_signal(SceneStringNames::get_singleton()->frame_changed);
}

int TextureRect::get_frame() const {

	return frame;
}

void TextureRect::set_vframes(int p_amount) {

	ERR_FAIL_COND(p_amount < 1);
	vframes = p_amount;

	update();
	item_rect_changed();
	_change_notify();
}
int TextureRect::get_vframes() const {

	return vframes;
}

void TextureRect::set_hframes(int p_amount) {

	ERR_FAIL_COND(p_amount < 1);
	hframes = p_amount;

	update();
	item_rect_changed();
	_change_notify();
}
int TextureRect::get_hframes() const {

	return hframes;
}

void TextureRect::_validate_property(PropertyInfo &property) const {

	if (property.name == "frame") {

		property.hint = PROPERTY_HINT_SPRITE_FRAME;

		property.hint_string = "0," + itos(vframes * hframes - 1) + ",1";
	}
}

TextureRect::TextureRect() {

	expand = false;
	set_mouse_filter(MOUSE_FILTER_PASS);
	stretch_mode = STRETCH_SCALE_ON_EXPAND;

	vflip = false;
	hflip = false;

	frame = 0;

	vframes = 1;
	hframes = 1;
}

TextureRect::~TextureRect() {
}
