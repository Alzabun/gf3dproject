#include "simple_logger.h" // always log stuff

#include "player.h"
#include "terraintest.h"

#include "gf3d_camera.h"
#include "gfc_vector.h"

#include "gf2d_font.h"

// used to be named player.c

// for collision, ctrl+shift+f collision and something about gfc_primitives comes up 
// im pretty sure shape comparisons are how collision checks are done so go investigate that
// i want to check if im supposed to makae collisions occur with entities so start here

void player_think(Entity* self);
void player_update(Entity* self);
void player_free(Entity* self);
void player_touch(Entity* self, Entity* other);

// this is unfinished with a lot of things wrong or missing but this is generally how you move a character and map keybinds
// to do stuff like abilities or something (mainly their animation)

typedef struct {
	GFC_Vector3D position;
	//GFC_Box playerBox; idk if i need this here
}playerData;

Entity* player_spawn(GFC_Vector3D position) {
	Entity* self;
	playerData* data;

	self = entity_new();
	if (!self) {
		return NULL;
	}
	self->model = gf3d_model_load("models/dino.model");
	self->free = player_free;
	self->think = player_think;
	self->update = player_update; 
	self->position = position;
	self->touch = player_touch;

	self->BoundingBox.x = position.x;
	self->BoundingBox.y = position.y;
	self->BoundingBox.z = position.z;

	self->BoundingBox.w = 5;
	self->BoundingBox.d = 5;
	self->BoundingBox.h = 5;

	data = gfc_allocate_array(sizeof(playerData), 1);
	if (data) {
		self->data = data;
	}
	return self;
}

void player_free(Entity* self) { // frees up entity
	playerData* data;
	if (!self) {
		return;
	}
	if (!self->data) {
		return;
	}
	data = (playerData*) self->data;
	
	free(data);
	self->data = NULL;
}
// * IF YOU WANT THE playerSAUR TO DO OTHER THINGS LIKE MOVE BACKWARD OR SOMETHING, JUST LOOK AT THE VECTOR FILES FOR PREMADE FUNCTIONS THEN USE THE PARAMTERS CORRECTLY
void player_think(Entity* self) { // these are the actions the entity will do when the game loads
	playerData* data;
	GFC_Vector2D dir_x = { 0,-1 }; // will only rotate in 2 dimensions
	GFC_Vector2D dir_y = { -1, 0 };

	Uint8 collision;
	ObjData* obj;

	GFC_Vector3D forZ = { 0, 0, -1 };
	GFC_Edge3D whereFloor = { self->position, gfc_vector3d_added(self->position, forZ) };

	//collision = gf3d_obj_line_test(obj, e, contact)

	if (!self || !self->data) {
		return;
	}
	data = self->data;

	dir_x = gfc_vector2d_rotate(dir_x, self->rotation.z);
	dir_y = gfc_vector2d_rotate(dir_y, self->rotation.z);

	if (gfc_input_command_down("walkright")) { // this makes the playersaur walk forward when the walk forward key (W) is pressed
		gfc_vector2d_add(self->position, self->position, dir_x); // go check function defintion to check the macros for this (the parameters)
	}

	if (gfc_input_command_down("walkleft")) {
		gfc_vector2d_sub(self->position, self->position, dir_x);
	}

	if (gfc_input_command_down("jump")) { // come back to this later
		gfc_vector2d_add(self->position, self->position, dir_y);
	}

	//data->cameraPitch += dy * 0.01;
	//self->rotation.z += 0.01; // will rotate the playersaur every frame by 0.01
}
void player_update(Entity* self) {
	GFC_Vector3D lookTarget, camera, dir = { 0 };
	playerData* data;

	int ground; 

	if (!self) {
		return;
	}
	data = self->data;

	// gfc_box_overlap and point overlap thing in primitives.c for collision?
	// comparing box with a box is best
	// you can make the range of bounds using the def file
	// make a new	 entry in a  def file for a object thing and do like
	// "sphere" : { "c" : [0,1,2,3], "r" : [0,2] }
	// my problem is how do i get the positino of the other thing to be over here

	gfc_vector3d_copy(lookTarget, self->position); // keeps track of position
	self->BoundingBox.x = self->position.x;
	self->BoundingBox.y = self->position.y;
	self->BoundingBox.z = self->position.z;


	lookTarget.z += 5; // this changes the offset of the camera
	//gf3d_camera_look_at(lookTarget, const GFC_Vector3D *position);
	dir.x = 50.0; // not sure how this works still but this gives the 2d camera angle like sonic games
	// could change to 3d pov just by changing the values of this and the camera below
	gfc_vector3d_rotate_about_z(&dir, self->rotation.z); // the rotation the camera will go along with
	gfc_vector3d_sub(camera, self->position, dir);
	camera.z += 10;
	gf3d_camera_look_at(lookTarget, &camera);

	gf2d_font_draw_line_tag("", FT_H1, GFC_COLOR_WHITE, gfc_vector2d(0, 10));
}
// in the end, you can spawn multiple dinosaurs that rotate and stuff using the spawn function made here (player_spawn)
// for other entities, make something very similar to this file for enemies and stuff and have all their actions happen in
// _update while being spawned from _spawn

/*
// this thing is trash!

// shouldnt this not be void??
void player_collision(Entity* self, GFC_Vector3D target) {
	return gfc_edge3d_	from_vectors(self->position, target);
	// should be 1 or 0	
	//return gfc_edge3d_from_vectors(self->position, model position??);
	//how do i get the position of the terrain object?????
	// also these collision checks are done by subticks, which are configurable but i dont know where that is
}*/ 

void player_touch(Entity* self, Entity* other) {
	// but how other
	/*if (collisiontest(self, other) == 1) {
		// movement = 0
		slog("collision detected");
		return;
	}
	else {
		slog("collision not detected");
		return;
	}*/
	slog("touch is activtating from player\n");
}