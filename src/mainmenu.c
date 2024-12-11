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