#include "register_types.h"
#include "core/class_db.h"
#include "scene/gui/control.h"
#include "curved_label.h"
#include "scene/resources/default_theme/default_theme.h"

void register_curved_label_types() {

	ClassDB::register_class<CurvedLabel>();

	Ref<Theme> t = Theme::get_default();

	// CurvedLabel

	t->set_stylebox("normal", "CurvedLabel", memnew(StyleBoxEmpty));
	t->set_font("font", "CurvedLabel", t->get_default_theme_font());

	t->set_color("font_color", "CurvedLabel", Color(1, 1, 1));
	t->set_color("font_color_shadow", "CurvedLabel", Color(0, 0, 0, 0));

	t->set_constant("shadow_offset_x", "CurvedLabel", 1);
	t->set_constant("shadow_offset_y", "CurvedLabel", 1);
	t->set_constant("shadow_as_outline", "CurvedLabel", 0);
	t->set_constant("line_spacing", "CurvedLabel", 3);
}

void unregister_curved_label_types() {
}
