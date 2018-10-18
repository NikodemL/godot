#ifndef CURVED_LABEL_H
#define CURVED_LABEL_H

#include "scene/gui/control.h"
/**
@author Marko Maretić <marko.maretic@interblockgaming.com>
*/
class CurvedLabel : public Control {

	GDCLASS(CurvedLabel, Control);

public:
	enum TEXT_ALIGN {
		TEXT_ALIGN_LEFT,
		TEXT_ALIGN_CENTER,
		TEXT_ALIGN_RIGHT
	};

private:
	String text;
	TEXT_ALIGN text_align;
	int radius;
	float space;
	float rotoffset;
	bool mirroredtext;
	bool expand;
	String loc_label;
	int max_font_size;

	bool update_translation();

protected:
	void _notification(int p_what);

	static void _bind_methods();
	// bind helpers
public:
	void set_radius(int p_radius);
	int get_radius() const;

	void set_space(float p_spacing);
	float get_space() const;

	void set_rotation_offset(float p_offset);
	float get_rotation_offset() const;

	void set_mirrored_text(bool p_mirrored);
	bool get_mirrored_text() const;

	void set_curved_text(const String &p_string);
	String get_curved_text() const;

	bool has_expand() const;
	void set_expand(bool p_expand);

	int get_max_font_size() const;
	void set_max_font_size(int p_max_font_size);

	void set_loc_label(String p_loc_label);
	String get_loc_label() const;

	void set_text_align(TEXT_ALIGN p_text_align);
	TEXT_ALIGN get_text_align() const;

	CurvedLabel(const String &p_text = String());
};

VARIANT_ENUM_CAST(CurvedLabel::TEXT_ALIGN);

#endif
