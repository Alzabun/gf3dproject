#include "mainmenu.h"

// STRUCT IN H

static menuData* data = NULL; 

void prepare_menu_UI() {
	data = gfc_allocate_array(sizeof(menuData), 1);
	if (!data) {
		//slog("no data");
		return;
	}
	data->button = gf2d_sprite_load_image("images/menu/menutest.png");
	data->generic = gfc_vector2d(1, 1); // testing
	if (!data->button) {
		slog("menu did not load");
	}
	data->state = DEFAULT;
	data->menupos = gfc_vector2d(data->button->frameWidth, data->button->frameHeight);
}

void draw_menu() {

	if (gfc_input_key_pressed("n")) {
		data->state = NORMAL;
		gf2d_sprite_free(data->button);
		slog("activated NORMAL");
	}
	else if (gfc_input_key_pressed("m")) {
		data->state = OBJECTIVE;
		gf2d_sprite_free(data->button);
		slog("activated OBJECTIVE");
	}
	else if (gfc_input_key_pressed("b")) {
		data->state = DEBUG;
		gf2d_sprite_free(data->button);
		slog("activated DEBUG");
	}
	else {
		gf2d_sprite_draw(data->button, gfc_vector2d(0, 0), &data->generic, &data->menupos, NULL, NULL, NULL, NULL, NULL);
	}
	//gf2d_sprite_draw(data->menu, gfc_vector2d(data->thisplayer->position.x, data->thisplayer->position.y), &data->generic, NULL, NULL, NULL, NULL, NULL, NULL);
}

menuState get_menu() {
	return data->state;
}

/*
* ignore this i guess
* 
void file_menu(SJson* textentry)
{
	SJson* boundaries = sj_object_get_value(textentry, "bounds");

	if (!boundaries) {
		slog("couldnt get the bounds from the json");
		return;
	}

	float x = 0, y = 0, xs = 0, ys = 0;
	SJson* x_pos = sj_array_get_nth(boundaries, 0);
	sj_get_float_value(x_pos, &x);

	SJson* y_pos = sj_array_get_nth(boundaries, 1);
	sj_get_float_value(y_pos, &y);

	SJson* x_scale = sj_array_get_nth(boundaries, 2);
	sj_get_float_value(x_scale, &xs);

	SJson* y_scale = sj_array_get_nth(boundaries, 3);
	sj_get_float_value(y_scale, &ys);

	GFC_Vector2D pos = gfc_vector2d(x, y);
	GFC_Vector2D scale = gfc_vector2d(xs, ys);

	Sprite* button = gf2d_sprite_load_image("images/ui/arrow_button.png");

	gf2d_sprite_draw(button, pos, &scale, &data->generic, &data->menupos, NULL, NULL, NULL, NULL);

}
*/
