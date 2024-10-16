#include "simple_logger.h" // always log stuff

#include "player.h"

#include "gf3d_camera.h"
#include "gfc_vector.h"

// used to be named dino.c

void dino_think(Entity* self);
void dino_update(Entity* self);
void dino_free(Entity* self);

// this is unfinished with a lot of things wrong or missing but this is generally how you move a character and map keybinds
// to do stuff like abilities or something (mainly their animation)

typedef struct {
	Uint8 freeLook;
	float cameraPitch;
}DinoData;

Entity* dino_spawn(GFC_Vector3D position) {
	Entity* self;
	DinoData* data;

	self = entity_new();
	if (!self) {
		return NULL;
	}
	self->model = gf3d_model_load("models/dino.model");
	self->free = dino_free;
	self->think = dino_think;
	self->update = dino_update; 
	self->position = position;

	data = gfc_allocate_array(sizeof(DinoData), 1);
	if (data) {
		self->data = data;
	}
	return self;
}

void dino_free(Entity* self) { // frees up entity
	DinoData* data;
	if (!self) {
		return;
	}
	if (!self->data) {
		return;
	}
	data = (DinoData*) self->data;
	
	free(data);
	self->data = NULL;
}
// * IF YOU WANT THE DINOSAUR TO DO OTHER THINGS LIKE MOVE BACKWARD OR SOMETHING, JUST LOOK AT THE VECTOR FILES FOR PREMADE FUNCTIONS THEN USE THE PARAMTERS CORRECTLY
void dino_think(Entity* self) { // these are the actions the entity will do when the game loads
	int dx, dy;
	DinoData* data;
	GFC_Vector2D dir_x = { 0,-1 }; // will only rotate in 2 dimensions
	GFC_Vector2D dir_y = { -1, 0 };

	if (!self || !self->data) {
		return;
	}
	data = self->data;

	dir_x = gfc_vector2d_rotate(dir_x, self->rotation.z);
	dir_y = gfc_vector2d_rotate(dir_y, self->rotation.z);

	if (gfc_input_command_down("walkright")) { // this makes the dinosaur walk forward when the walk forward key (W) is pressed
		gfc_vector2d_add(self->position, self->position, dir_x); // go check function defintion to check the macros for this (the parameters)
	}

	if (gfc_input_command_down("walkleft")) {
		gfc_vector2d_sub(self->position, self->position, dir_x);
	}

	if (gfc_input_command_down("jump")) { // come back to this later
		gfc_vector2d_add(self->position, self->position, dir_y);
	}

	if (gfc_input_command_pressed("freelook")) { // if things like these where camera changes dont work, look in game.c for camera changes/removals/additions if you made any or need to make any
		data->freeLook = !data->freeLook;
		gf3d_camera_enable_free_look(data->freeLook); // if this isnt working, you need to add some cmaera data check thing in dino_update
	}

	if (data->freeLook) {
		gf3d_camera_controls_update();
	}
	SDL_GetRelativeMouseState(&dx, &dy); // calculus

	//data->cameraPitch += dy * 0.01;
	//self->rotation.z += 0.01; // will rotate the dinosaur every frame by 0.01
}
void dino_update(Entity* self) {
	GFC_Vector3D lookTarget, camera, dir = { 0 };
	DinoData* data;

	if (!self) {
		return;
	}
	data = self->data;
	
	gfc_vector3d_copy(lookTarget, self->position);
	lookTarget.z += 5;
	//gf3d_camera_look_at(lookTarget, const GFC_Vector3D *position);
	dir.x = 50.0; // not sure how this works still but this gives the 2d camera angle like sonic games
	// could change to 3d pov just by changing the values of this and the camera below
	gfc_vector3d_rotate_about_z(&dir, self->rotation.z); // the rotation the camera will go along with
	gfc_vector3d_sub(camera, self->position, dir);
	camera.z += 10;
	gf3d_camera_look_at(lookTarget, &camera);
}
// in the end, you can spawn multiple dinosaurs that rotate and stuff using the spawn function made here (dino_spawn)
// for other entities, make something very similar to this file for enemies and stuff and have all their actions happen in
// _update while being spawned from _spawn
