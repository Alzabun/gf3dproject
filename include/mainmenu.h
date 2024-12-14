#ifndef __MENU_H__
#define __MENU_H__

#include "simple_logger.h"

#include "gf3d_camera.h"
#include "gfc_vector.h"
#include "gfc_input.h"

#include "gf2d_sprite.h"

typedef enum {
	NORMAL,
	DEBUG, // TOOL CHAIN
	OBJECTIVE, // aka mission
	DEFAULT //fallback
}menuState;

typedef struct {
	Uint8 select; // moving in the menu before actually picking a option
	Sprite* button;
	GFC_Vector2D generic;
	GFC_Vector2D menupos;
	menuState state;
}menuData;

void draw_menu();
void prepare_menu_UI();
menuState get_menu();
void file_menu(SJson* textentry);

// fade in and fade out to black screen could be here
// soething about while blackscren != alpha 1 , alpha += 0.1 then while blackscreen != alpha 0 , alpha -= 0.1

#endif