#include "simple_logger.h" // always log stuff

#include "player.h"
#include "terraintest.h"

#include "gf3d_camera.h"
#include "gfc_vector.h"
#include "gfc_input.h"

#include "gf2d_font.h"

void player_think(Entity* self);
void player_update(Entity* self);
void player_free(Entity* self);
void player_touch(Entity* self, Entity* other);

const int maxHeight = 100;

typedef struct {
	GFC_Vector3D position;
	// (ive been adding these velocity and acceleration to entity.h to make it look neater with the rest of this
	// code but i may or may not change it to actually use the playerdata cus idk if it really matters)
	GFC_Vector3D velocity;
	GFC_Vector3D acceleration; 
	int airborne;
	int jumpHeight;
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
	self->velocity = gfc_vector3d(0, 0, 0);
	self->acceleration = gfc_vector3d(0, 0, 0);

	self->BoundingBox.x = position.x;
	self->BoundingBox.y = position.y;
	self->BoundingBox.z = position.z;

	self->BoundingBox.w = 20;
	self->BoundingBox.d = 20;
	self->BoundingBox.h = 30; // i dont think this is properly being detected?????

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

void player_think(Entity* self) { // these are the actions the entity will do when the game loads
	playerData* data;
	GFC_Vector2D dir_x = { 0,-1 }; // will only rotate in 2 dimensions
	GFC_Vector3D dir_z = { 0, 0, 1 };
	GFC_Vector2D dir_y = { -1, 0 };

	if (!self || !self->data) {
		return;
	}
	data = self->data;

	dir_x = gfc_vector2d_rotate(dir_x, self->rotation.z);
	dir_y = gfc_vector2d_rotate(dir_y, self->rotation.z);

	// walking right/left moves in the y-axis because of the camera direction
	// walking into the 3rd dimension is the x-axis
	// up/down should be the z-axis
	if (gfc_input_command_down("walkright")) { 
		gfc_vector2d_add(self->position, self->position, dir_x); // go check function defintion to check the macros for this (the parameters)
	}

	if (gfc_input_command_down("walkleft")) {
		gfc_vector2d_sub(self->position, self->position, dir_x);
	}

	if (gfc_input_command_down("jump") && data->airborne == 0) {
		data->jumpHeight = 0;
		data->airborne = 1;
		self->velocity.z = 10;
	}
	
	if (gfc_input_command_held("jump") && data->airborne == 1) {
		self->model = gf3d_model_load("models/dino_jump.model");
		gfc_vector3d_add(self->position, self->position, dir_z);
		if (data->jumpHeight <= maxHeight) {
			self->velocity.z += 1;
			data->jumpHeight += self->velocity.z;
		}
	}
	else {
		self->velocity.z = 0;
		data->jumpHeight = 0;
	}

	if (gfc_input_command_down("walkback")) { // for testing only, wont be in the final game
		gfc_vector3d_sub(self->position, self->position, dir_z);
	}

	//data->cameraPitch += dy * 0.01;
	//self->rotation.z += 0.01; // will rotate the playersaur every frame by 0.01
}
void player_update(Entity* self) {
	GFC_Vector3D lookTarget, camera, dir = { 0 };
	playerData* data;

	if (!self) {
		return;
	}
	data = self->data;

	// velocity process
	// i kind of want to make the x/y velocity go faster and faster the longer you run in one direction without interruption
	// like a normal sonic game (capped at a certain point obviously)
	// remember to do this later

	self->velocity.y += self->acceleration.y;

	self->position.x += self->velocity.x;
	self->position.y += self->velocity.y;
	self->position.z += self->velocity.z;


	// 100 = max speed
	if (self->velocity.y > 100 ) {
		self->velocity.y = 100;
	}
	else if (self->velocity.y < -100) {
		self->velocity.y = -100;
	}

	self->BoundingBox.x = self->position.x;
	self->BoundingBox.y = self->position.y;
	self->BoundingBox.z = self->position.z;

	if (data->airborne == 1) {
		self->velocity.z -= 0.1;
		// respawn player on top of the map if they fall into the void
		if (self->position.z <= -250) {
			self->position.z = 100;
			self->velocity.z = 0;
		}
		// this is obviously going to cause problems when there has to be collisions with walls and probably enemies
		// im pretty sure i can use the bounding box sides to fix this tho, but that means changing a lot of how this works right now
	}
	gfc_vector3d_copy(lookTarget, self->position); // keeps track of position (this is outdate dprobably)
	/*printf("Player box: x=%.2f, y=%.2f, z=%.2f, w=%.2f, d=%.2f, h=%.2f\n",
       self->BoundingBox.x, self->BoundingBox.y, self->BoundingBox.z,
       self->BoundingBox.w, self->BoundingBox.d, self->BoundingBox.h);*/
	

	// everything below here should probably be moved but i dont feel like figuring it out yet


	lookTarget.z += 5; // this changes the offset of the camera
	//gf3d_camera_look_at(lookTarget, const GFC_Vector3D *position);
	dir.x = 50.0; // not sure how this works still but this gives the 2d camera angle like sonic games
	// could change to 3d pov just by changing the values of this and the camera below
	gfc_vector3d_rotate_about_z(&dir, self->rotation.z); // the rotation the camera will go along with
	gfc_vector3d_sub(camera, self->position, dir);
	camera.z += 10;
	gf3d_camera_look_at(lookTarget, &camera);

}
	
void player_touch(Entity* self, Entity* other) {
	playerData* data;
	// this whole thing is kind of based on the fact that the terrain is the only other entity at this point
	// i think theres a way to like have a flag system for each separate entity (when i make them)
	// so change it later
	data = self->data;

	// if other->Terrain (implement this)
	self->rotation.y = 0;
	self->velocity.z = 0;
	data->airborne = 0;

	self->model = gf3d_model_load("models/dino.model");
	//slog("touch is activtating from player\n");
	printf("Player touched Terrain at: x=%.2f, y=%.2f, z=%.2f, w=%.2f, d=%.2f, h=%.2f\n",
		self->BoundingBox.x, self->BoundingBox.y, self->BoundingBox.z,
		self->BoundingBox.w, self->BoundingBox.d, self->BoundingBox.h);
}