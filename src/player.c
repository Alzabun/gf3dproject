#include "simple_logger.h" // always log stuff

#include "player.h"
#include "terraintest.h"

#include "gf3d_camera.h"
#include "gfc_vector.h"

#include "gf2d_font.h"

void player_think(Entity* self);
void player_update(Entity* self);
void player_free(Entity* self);
void player_touch(Entity* self, Entity* other);

typedef struct {
	GFC_Vector3D position;
	GFC_Vector3D velocity;
	int airborne;
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

	if (gfc_input_command_down("jump")) { // come back to this later
		if (data->airborne == 0) {
			for (int i = 0; i < 20; i++) {
				gfc_vector3d_add(self->position, self->position, dir_z);
			} // a dumb way to make the jump be held down
			 // ideally, it should go higher the longer you hold it but ill figure that out later surely it wont be hard to implement
		}
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

	int ground; 

	if (!self) {
		return;
	}
	data = self->data;

	self->BoundingBox.x = self->position.x;
	self->BoundingBox.y = self->position.y;
	self->BoundingBox.z = self->position.z;

	// velocity process
	// i kind of want to make the x/y velocity go faster and faster the longer you run in one direction without interruption
	// like a normal sonic game (capped at a certain point obviously)
	// remember to do this later
	data->airborne = 1;
	self->position.x += self->velocity.x;
	self->position.y += self->velocity.y;
	self->position.z += self->velocity.z;

	gfc_vector3d_copy(lookTarget, self->position); // keeps track of position

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
	
	self->velocity.z = 0;
	data->airborne = 0;
	//self->position.z = other->position.z; good idea but wrong execution
	//slog("touch is activtating from player\n");
	printf("Player touched Terrain at: x=%.2f, y=%.2f, z=%.2f, w=%.2f, d=%.2f, h=%.2f\n",
		self->BoundingBox.x, self->BoundingBox.y, self->BoundingBox.z,
		self->BoundingBox.w, self->BoundingBox.d, self->BoundingBox.h);
}

// gfc_box_overlap and point overlap thing in primitives.c for collision?
// comparing box with a box is best
// you can make the range of bounds using the def file
// make a new	 entry in a  def file for a object thing and do like
// "sphere" : { "c" : [0,1,2,3], "r" : [0,2] }
// my problem is how do i get the positino of the other thing to be over here

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